// Rogue Rigs - flow field world subsystem: one grid BFS from the miner's cell gives every ground robot its direction.

#include "FlowField/RRFlowFieldSubsystem.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

namespace RRFlowField
{
	/** Neighbour offsets: 4 orthogonal, then 4 diagonal. */
	const FIntPoint Offsets[8] = {
		FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1),
		FIntPoint(1, 1), FIntPoint(1, -1), FIntPoint(-1, 1), FIntPoint(-1, -1)
	};

	TAutoConsoleVariable<int32> CVarDebug(TEXT("rr.FlowField.Debug"), 0,
		TEXT("1: draw the flow field directions around the goal."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarDebugRadius(TEXT("rr.FlowField.DebugRadius"), 15,
		TEXT("Radius in cells around the goal drawn by rr.FlowField.Debug."), ECVF_Cheat);
}

void URRFlowFieldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateField();
	if (RRFlowField::CVarDebug.GetValueOnGameThread() > 0)
	{
		DrawDebugField();
	}
}

TStatId URRFlowFieldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRFlowFieldSubsystem, STATGROUP_Tickables);
}

void URRFlowFieldSubsystem::Deinitialize()
{
	if (ARRArenaGrid* Bound = BoundGrid.Get())
	{
		Bound->OnCellDestroyed.RemoveDynamic(this, &URRFlowFieldSubsystem::HandleCellDestroyed);
	}
	BoundGrid.Reset();
	Super::Deinitialize();
}

void URRFlowFieldSubsystem::SetGrid(ARRArenaGrid* InGrid)
{
	Grid = InGrid;
	bDirty = true;
}

void URRFlowFieldSubsystem::SetGoalActor(AActor* Goal)
{
	GoalActor = Goal;
	bExplicitGoal = Goal != nullptr;
	bDirty = true;
}

bool URRFlowFieldSubsystem::ResolveGridAndGoal()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	if (!Grid.IsValid())
	{
		for (TActorIterator<ARRArenaGrid> It(World); It; ++It)
		{
			Grid = *It;
			bDirty = true;
			break;
		}
	}
	if (Grid.Get() != BoundGrid.Get())
	{
		if (ARRArenaGrid* Bound = BoundGrid.Get())
		{
			Bound->OnCellDestroyed.RemoveDynamic(this, &URRFlowFieldSubsystem::HandleCellDestroyed);
		}
		BoundGrid = Grid;
		if (ARRArenaGrid* NewGrid = Grid.Get())
		{
			NewGrid->OnCellDestroyed.AddUniqueDynamic(this, &URRFlowFieldSubsystem::HandleCellDestroyed);
		}
	}
	if (!GoalActor.IsValid() && !bExplicitGoal)
	{
		for (TActorIterator<ARRMiner> It(World); It; ++It)
		{
			if (!It->IsDead())
			{
				GoalActor = *It;
				bDirty = true;
				break;
			}
		}
	}
	return Grid.IsValid() && GoalActor.IsValid();
}

void URRFlowFieldSubsystem::UpdateField()
{
	if (LastComputeFrame == GFrameCounter || !ResolveGridAndGoal())
	{
		return;
	}
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	const FIntPoint GoalCell = CurrentGrid->GetCellAtWorld(GoalActor->GetActorLocation());
	const FIntPoint Size = CurrentGrid->GetGridSize();
	if (!bDirty && GoalCell == FieldGoalCell && Size == FieldSize)
	{
		return;
	}
	ComputeField(GoalCell, Size);
}

bool URRFlowFieldSubsystem::CanStep(FIntPoint Cell, int32 Index) const
{
	const FIntPoint Offset = RRFlowField::Offsets[Index];
	const FIntPoint Next = Cell + Offset;
	if (!IsInField(Next) || !Walkable[GetFieldIndex(Next)])
	{
		return false;
	}
	if (Offset.X != 0 && Offset.Y != 0)
	{
		// No corner cutting: both orthogonal cells crossed by the diagonal must be walkable.
		return Walkable[GetFieldIndex(FIntPoint(Next.X, Cell.Y))] && Walkable[GetFieldIndex(FIntPoint(Cell.X, Next.Y))];
	}
	return true;
}

void URRFlowFieldSubsystem::ComputeField(FIntPoint GoalCell, FIntPoint Size)
{
	const double StartSeconds = FPlatformTime::Seconds();
	const ARRArenaGrid* CurrentGrid = Grid.Get();

	FieldSize = Size;
	FieldGoalCell = GoalCell;
	bDirty = false;
	LastComputeFrame = GFrameCounter;
	++ComputeCount;

	const int32 CellCount = Size.X * Size.Y;
	Walkable.SetNumUninitialized(CellCount);
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const ERRCellType Type = CurrentGrid->GetCellType(FIntPoint(X, Y));
			Walkable[Y * Size.X + X] = Type == ERRCellType::Floor || Type == ERRCellType::Spawn;
		}
	}
	Distances.Init(INDEX_NONE, CellCount);
	Directions.Init(NoDirection, CellCount);

	if (IsInField(GoalCell))
	{
		// Breadth-first from the goal; every step (orthogonal or diagonal) costs 1.
		TArray<int32> Queue;
		Queue.Reserve(CellCount);
		Distances[GetFieldIndex(GoalCell)] = 0;
		Queue.Add(GetFieldIndex(GoalCell));
		for (int32 Head = 0; Head < Queue.Num(); ++Head)
		{
			const int32 Index = Queue[Head];
			const FIntPoint Cell(Index % Size.X, Index / Size.X);
			for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
			{
				if (!CanStep(Cell, Neighbour))
				{
					continue;
				}
				const int32 NextIndex = GetFieldIndex(Cell + RRFlowField::Offsets[Neighbour]);
				if (Distances[NextIndex] == INDEX_NONE)
				{
					Distances[NextIndex] = Distances[Index] + 1;
					Queue.Add(NextIndex);
				}
			}
		}

		// Direction: the reachable neighbour with the lowest distance, ties broken by straight-line closeness to the goal.
		for (const int32 Index : Queue)
		{
			if (Distances[Index] == 0)
			{
				continue;
			}
			const FIntPoint Cell(Index % Size.X, Index / Size.X);
			int32 BestDistance = MAX_int32;
			int32 BestStraight = MAX_int32;
			for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
			{
				if (!CanStep(Cell, Neighbour))
				{
					continue;
				}
				const FIntPoint Next = Cell + RRFlowField::Offsets[Neighbour];
				const int32 NextDistance = Distances[GetFieldIndex(Next)];
				const FIntPoint ToGoal = GoalCell - Next;
				const int32 Straight = ToGoal.X * ToGoal.X + ToGoal.Y * ToGoal.Y;
				if (NextDistance != INDEX_NONE && (NextDistance < BestDistance || (NextDistance == BestDistance && Straight < BestStraight)))
				{
					BestDistance = NextDistance;
					BestStraight = Straight;
					Directions[Index] = static_cast<uint8>(Neighbour);
				}
			}
		}
	}

	LastComputeMilliseconds = static_cast<float>((FPlatformTime::Seconds() - StartSeconds) * 1000.0);
	UE_LOG(LogRogueRigs, Verbose, TEXT("URRFlowFieldSubsystem: field %dx%d computed from (%d, %d) in %.3f ms."),
		Size.X, Size.Y, GoalCell.X, GoalCell.Y, LastComputeMilliseconds);
}

void URRFlowFieldSubsystem::HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source)
{
	bDirty = true;
}

bool URRFlowFieldSubsystem::IsWalkableCell(FIntPoint Cell) const
{
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	if (!CurrentGrid)
	{
		return false;
	}
	const ERRCellType Type = CurrentGrid->GetCellType(Cell);
	return Type == ERRCellType::Floor || Type == ERRCellType::Spawn;
}

void URRFlowFieldSubsystem::GatherReachableCells(TArray<FIntPoint>& OutCells, TFunctionRef<bool(FIntPoint Cell, int32 Steps)> Filter,
	bool bFloorOnly, const TSet<FIntPoint>* ExcludedCells)
{
	UpdateField();
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	if (!CurrentGrid)
	{
		return;
	}
	const FIntPoint Size = CurrentGrid->GetGridSize();
	// Y outer, X inner: callers drawing from the result with a seeded stream depend on this order.
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const FIntPoint Cell(X, Y);
			const ERRCellType Type = CurrentGrid->GetCellType(Cell);
			if (Type != ERRCellType::Floor && (bFloorOnly || Type != ERRCellType::Spawn))
			{
				continue;
			}
			const int32 Steps = IsInField(Cell) && Distances.IsValidIndex(GetFieldIndex(Cell)) ? Distances[GetFieldIndex(Cell)] : INDEX_NONE;
			if (Steps == INDEX_NONE || (ExcludedCells && ExcludedCells->Contains(Cell)))
			{
				continue;
			}
			if (Filter(Cell, Steps))
			{
				OutCells.Add(Cell);
			}
		}
	}
}

void URRFlowFieldSubsystem::GatherPawnCells(TSet<FIntPoint>& OutCells) const
{
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	UWorld* World = GetWorld();
	if (!CurrentGrid || !World)
	{
		return;
	}
	for (TActorIterator<APawn> It(World); It; ++It)
	{
		if (IsValid(*It))
		{
			OutCells.Add(CurrentGrid->GetCellAtWorld(It->GetActorLocation()));
		}
	}
}

int32 URRFlowFieldSubsystem::GetCellDistance(FIntPoint Cell)
{
	UpdateField();
	return IsInField(Cell) && Distances.IsValidIndex(GetFieldIndex(Cell)) ? Distances[GetFieldIndex(Cell)] : INDEX_NONE;
}

FIntPoint URRFlowFieldSubsystem::GetNextCell(FIntPoint Cell)
{
	UpdateField();
	if (!IsInField(Cell) || !Directions.IsValidIndex(GetFieldIndex(Cell)) || Directions[GetFieldIndex(Cell)] == NoDirection)
	{
		return FIntPoint(INDEX_NONE, INDEX_NONE);
	}
	return Cell + RRFlowField::Offsets[Directions[GetFieldIndex(Cell)]];
}

FVector URRFlowFieldSubsystem::GetMoveDirection(const FVector& From)
{
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	if (!CurrentGrid)
	{
		UpdateField();
		CurrentGrid = Grid.Get();
		if (!CurrentGrid)
		{
			return FVector::ZeroVector;
		}
	}
	const FIntPoint Next = GetNextCell(CurrentGrid->GetCellAtWorld(From));
	if (Next.X == INDEX_NONE)
	{
		return FVector::ZeroVector;
	}
	// Aim at the next cell center from the actual position: the heading turns smoothly as the robot crosses the cell.
	FVector Direction = CurrentGrid->GetCellCenter(Next.X, Next.Y) - From;
	Direction.Z = 0.f;
	return Direction.GetSafeNormal();
}

bool URRFlowFieldSubsystem::IsReachable(const FVector& From)
{
	return GetPathDistance(From) >= 0.f;
}

float URRFlowFieldSubsystem::GetPathDistance(const FVector& From)
{
	UpdateField();
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	if (!CurrentGrid)
	{
		return -1.f;
	}
	const int32 Steps = GetCellDistance(CurrentGrid->GetCellAtWorld(From));
	return Steps == INDEX_NONE ? -1.f : Steps * CurrentGrid->GetCellSize();
}

bool URRFlowFieldSubsystem::GetGoalLocation(FVector& OutLocation)
{
	ResolveGridAndGoal();
	if (const AActor* Goal = GoalActor.Get())
	{
		OutLocation = Goal->GetActorLocation();
		return true;
	}
	return false;
}

bool URRFlowFieldSubsystem::HasDirectPath(const FVector& From, const FVector& To)
{
	ResolveGridAndGoal();
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	if (!CurrentGrid)
	{
		return false;
	}
	FIntPoint Previous = CurrentGrid->GetCellAtWorld(From);
	if (!IsWalkableCell(Previous))
	{
		return false;
	}
	// Samples every quarter cell: consecutive samples change by at most one cell on each axis.
	const float Length = FVector::Dist2D(From, To);
	const int32 SampleCount = FMath::Max(1, FMath::CeilToInt32(Length / (CurrentGrid->GetCellSize() * 0.25f)));
	for (int32 Sample = 1; Sample <= SampleCount; ++Sample)
	{
		const FIntPoint Cell = CurrentGrid->GetCellAtWorld(FMath::Lerp(From, To, static_cast<float>(Sample) / SampleCount));
		if (Cell == Previous)
		{
			continue;
		}
		if (!IsWalkableCell(Cell))
		{
			return false;
		}
		if (Cell.X != Previous.X && Cell.Y != Previous.Y
			&& (!IsWalkableCell(FIntPoint(Cell.X, Previous.Y)) || !IsWalkableCell(FIntPoint(Previous.X, Cell.Y))))
		{
			return false;
		}
		Previous = Cell;
	}
	return true;
}

void URRFlowFieldSubsystem::DrawDebugField() const
{
#if ENABLE_DRAW_DEBUG
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	UWorld* World = GetWorld();
	if (!CurrentGrid || !World || !IsInField(FieldGoalCell) || Directions.Num() != FieldSize.X * FieldSize.Y)
	{
		return;
	}
	const int32 Radius = FMath::Max(0, RRFlowField::CVarDebugRadius.GetValueOnGameThread());
	const float CellSize = CurrentGrid->GetCellSize();
	const FVector Lift(0.f, 0.f, CurrentGrid->GetBlockHeight() * 0.5f);
	for (int32 Y = FMath::Max(0, FieldGoalCell.Y - Radius); Y <= FMath::Min(FieldSize.Y - 1, FieldGoalCell.Y + Radius); ++Y)
	{
		for (int32 X = FMath::Max(0, FieldGoalCell.X - Radius); X <= FMath::Min(FieldSize.X - 1, FieldGoalCell.X + Radius); ++X)
		{
			const int32 Index = GetFieldIndex(FIntPoint(X, Y));
			const FVector Center = CurrentGrid->GetCellCenter(X, Y) + Lift;
			if (Distances[Index] == 0)
			{
				DrawDebugBox(World, Center, FVector(CellSize * 0.3f), FColor::Yellow, false, -1.f, 0, 4.f);
			}
			else if (Directions[Index] != NoDirection)
			{
				const FIntPoint Offset = RRFlowField::Offsets[Directions[Index]];
				const FVector Tip = Center + FVector(Offset.X, Offset.Y, 0.f).GetSafeNormal() * CellSize * 0.4f;
				DrawDebugDirectionalArrow(World, Center, Tip, CellSize * 0.15f, FColor::Green, false, -1.f, 0, 3.f);
			}
			else if (Walkable[Index])
			{
				DrawDebugPoint(World, Center, 8.f, FColor::Red, false, -1.f);
			}
		}
	}
#endif
}
