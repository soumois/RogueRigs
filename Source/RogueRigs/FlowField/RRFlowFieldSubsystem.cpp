// Rogue Rigs - flow field world subsystem: one grid BFS from the miner's cell gives every ground robot its direction,
// repaired incrementally when cells open and refreshed over several frames when the miner changes cell.

#include "FlowField/RRFlowFieldSubsystem.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "World/RRWorld.h"

namespace RRFlowField
{
	/** Neighbour offsets: 4 orthogonal, then 4 diagonal. */
	const FIntPoint Offsets[8] = {
		FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1),
		FIntPoint(1, 1), FIntPoint(1, -1), FIntPoint(-1, 1), FIntPoint(-1, -1)
	};

	/** Index of the opposite offset of each entry of Offsets (the step back). */
	const uint8 Opposite[8] = { 1, 0, 3, 2, 7, 6, 5, 4 };

	TAutoConsoleVariable<int32> CVarDebug(TEXT("rr.FlowField.Debug"), 0,
		TEXT("1: draw the flow field directions around the goal."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarDebugRadius(TEXT("rr.FlowField.DebugRadius"), 15,
		TEXT("Radius in cells around the goal drawn by rr.FlowField.Debug."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarBudgetMs(TEXT("rr.FlowField.BudgetMs"), 0.3f,
		TEXT("Per-frame time budget (ms) of the incremental flow-field work: opened cells and the goal rebuild. <= 0: no budget, all work done in the frame."),
		ECVF_Default);

	/** Packs a relaxation heap entry: distance in the high half (heap order), cell index in the low half. */
	uint64 PackEntry(int32 Distance, int32 Index)
	{
		return (static_cast<uint64>(static_cast<uint32>(Distance)) << 32) | static_cast<uint32>(Index);
	}

	/** True when the deadline (FPlatformTime::Seconds, <= 0 for none) has passed. */
	bool IsPastDeadline(double DeadlineSeconds)
	{
		return DeadlineSeconds > 0.0 && FPlatformTime::Seconds() >= DeadlineSeconds;
	}

	/** Squared straight-line distance in cells between two cells (direction tie-break). */
	int32 StraightSquared(FIntPoint From, FIntPoint To)
	{
		const FIntPoint Delta = To - From;
		return Delta.X * Delta.X + Delta.Y * Delta.Y;
	}
}

void URRFlowFieldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateField();
	if (LastWorkFrame != GFrameCounter)
	{
		LastWorkFrame = GFrameCounter;
		const float BudgetMs = RRFlowField::CVarBudgetMs.GetValueOnGameThread();
		ProcessPendingWork(BudgetMs > 0.f ? FPlatformTime::Seconds() + BudgetMs / 1000.0 : 0.0);
	}
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
		if (ARRArenaGrid* FoundGrid = RRWorld::FindArenaGrid(World))
		{
			Grid = FoundGrid;
			bDirty = true;
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
		if (ARRMiner* FoundMiner = RRWorld::FindLivingMiner(World))
		{
			GoalActor = FoundMiner;
			bDirty = true;
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
	const FIntPoint Size = CurrentGrid->GetGridSize();
	if (!bDirty && Size == FieldSize && Distances.Num() == Size.X * Size.Y)
	{
		return;
	}
	ComputeField(CurrentGrid->GetCellAtWorld(GoalActor->GetActorLocation()), Size);
}

void URRFlowFieldSubsystem::FlushPendingWork()
{
	UpdateField();
	// A goal rebuild started for an older cell may finish while the goal sits elsewhere: a few passes settle it.
	for (int32 Pass = 0; Pass < 4 && IsUpdatePending(); ++Pass)
	{
		ProcessPendingWork(0.0);
	}
}

bool URRFlowFieldSubsystem::IsUpdatePending() const
{
	if (PendingOpenings.Num() > 0 || RelaxHeap.Num() > 0 || bGoalRebuildActive)
	{
		return true;
	}
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	const AActor* Goal = GoalActor.Get();
	return CurrentGrid && Goal && CurrentGrid->GetCellAtWorld(Goal->GetActorLocation()) != FieldGoalCell;
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

uint8 URRFlowFieldSubsystem::ChooseDirection(FIntPoint Cell, const TArray<int32>& InDistances, FIntPoint Goal) const
{
	uint8 Best = NoDirection;
	int32 BestDistance = MAX_int32;
	int32 BestStraight = MAX_int32;
	for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
	{
		if (!CanStep(Cell, Neighbour))
		{
			continue;
		}
		const FIntPoint Next = Cell + RRFlowField::Offsets[Neighbour];
		const int32 NextDistance = InDistances[GetFieldIndex(Next)];
		const int32 Straight = RRFlowField::StraightSquared(Next, Goal);
		if (NextDistance != INDEX_NONE && (NextDistance < BestDistance || (NextDistance == BestDistance && Straight < BestStraight)))
		{
			BestDistance = NextDistance;
			BestStraight = Straight;
			Best = static_cast<uint8>(Neighbour);
		}
	}
	return Best;
}

bool URRFlowFieldSubsystem::IsBetterTie(FIntPoint Cell, int32 Candidate, int32 Current, FIntPoint Goal)
{
	const int32 CandidateStraight = RRFlowField::StraightSquared(Cell + RRFlowField::Offsets[Candidate], Goal);
	const int32 CurrentStraight = RRFlowField::StraightSquared(Cell + RRFlowField::Offsets[Current], Goal);
	return CandidateStraight < CurrentStraight || (CandidateStraight == CurrentStraight && Candidate < Current);
}

bool URRFlowFieldSubsystem::AdvanceBfs(FIntPoint Goal, TArray<int32>& OutDistances, TArray<uint8>& OutDirections, TArray<int32>& Queue,
	int32& Head, int32& DirectionHead, double DeadlineSeconds) const
{
	int32 Steps = 0;
	// Breadth-first from the goal; every step (orthogonal or diagonal) costs 1.
	while (Head < Queue.Num())
	{
		if (++Steps % BudgetCheckInterval == 0 && RRFlowField::IsPastDeadline(DeadlineSeconds))
		{
			return false;
		}
		const int32 Index = Queue[Head++];
		const FIntPoint Cell = GetFieldCell(Index);
		for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
		{
			if (!CanStep(Cell, Neighbour))
			{
				continue;
			}
			const int32 NextIndex = GetFieldIndex(Cell + RRFlowField::Offsets[Neighbour]);
			if (OutDistances[NextIndex] == INDEX_NONE)
			{
				OutDistances[NextIndex] = OutDistances[Index] + 1;
				Queue.Add(NextIndex);
			}
		}
	}
	// Directions once every distance is known.
	while (DirectionHead < Queue.Num())
	{
		if (++Steps % BudgetCheckInterval == 0 && RRFlowField::IsPastDeadline(DeadlineSeconds))
		{
			return false;
		}
		const int32 Index = Queue[DirectionHead++];
		if (OutDistances[Index] > 0)
		{
			OutDirections[Index] = ChooseDirection(GetFieldCell(Index), OutDistances, Goal);
		}
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

	// The walkability is read fresh from the grid: queued openings and a running goal rebuild are obsolete.
	PendingOpenings.Reset();
	RelaxHeap.Reset();
	bGoalRebuildActive = false;
	OpenedDuringRebuild.Reset();

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

	TArray<int32> Queue;
	Queue.Reserve(CellCount);
	if (IsInField(GoalCell))
	{
		Distances[GetFieldIndex(GoalCell)] = 0;
		Queue.Add(GetFieldIndex(GoalCell));
	}
	int32 Head = 0;
	int32 DirectionHead = 0;
	AdvanceBfs(GoalCell, Distances, Directions, Queue, Head, DirectionHead, 0.0);

	const double Milliseconds = (FPlatformTime::Seconds() - StartSeconds) * 1000.0;
	LastComputeMilliseconds = static_cast<float>(Milliseconds);
	AddFrameWork(Milliseconds);
	UE_LOG(LogRogueRigs, Verbose, TEXT("URRFlowFieldSubsystem: field %dx%d computed from (%d, %d) in %.3f ms."),
		Size.X, Size.Y, GoalCell.X, GoalCell.Y, LastComputeMilliseconds);
}

void URRFlowFieldSubsystem::ProcessPendingWork(double DeadlineSeconds)
{
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	const AActor* Goal = GoalActor.Get();
	if (!CurrentGrid || !Goal || bDirty || CurrentGrid->GetGridSize() != FieldSize || Distances.Num() != FieldSize.X * FieldSize.Y)
	{
		// No valid field: the next UpdateField rebuilds it in full.
		return;
	}
	const FIntPoint GoalCell = CurrentGrid->GetCellAtWorld(Goal->GetActorLocation());
	if (PendingOpenings.Num() == 0 && RelaxHeap.Num() == 0 && !bGoalRebuildActive && GoalCell == FieldGoalCell)
	{
		return;
	}
	const double StartSeconds = FPlatformTime::Seconds();

	// Openings first: they only shorten paths of the live field.
	ApplyPendingOpenings();
	RunRelaxation(DeadlineSeconds);

	// Goal rebuild: a BFS into the second buffer, sliced; it always advances a little, so constant digging cannot starve it.
	if (!bGoalRebuildActive && GoalCell != FieldGoalCell)
	{
		const int32 CellCount = FieldSize.X * FieldSize.Y;
		bGoalRebuildActive = true;
		RebuildGoalCell = GoalCell;
		RebuildDistances.Init(INDEX_NONE, CellCount);
		RebuildDirections.Init(NoDirection, CellCount);
		RebuildQueue.Reset(CellCount);
		RebuildHead = 0;
		RebuildDirectionHead = 0;
		RebuildMilliseconds = 0.0;
		OpenedDuringRebuild.Reset();
		if (IsInField(GoalCell))
		{
			RebuildDistances[GetFieldIndex(GoalCell)] = 0;
			RebuildQueue.Add(GetFieldIndex(GoalCell));
		}
	}
	if (bGoalRebuildActive)
	{
		const double SliceStart = FPlatformTime::Seconds();
		const bool bDone = AdvanceBfs(RebuildGoalCell, RebuildDistances, RebuildDirections, RebuildQueue, RebuildHead, RebuildDirectionHead,
			DeadlineSeconds);
		RebuildMilliseconds += (FPlatformTime::Seconds() - SliceStart) * 1000.0;
		if (bDone)
		{
			Swap(Distances, RebuildDistances);
			Swap(Directions, RebuildDirections);
			FieldGoalCell = RebuildGoalCell;
			bGoalRebuildActive = false;
			++ComputeCount;
			LastComputeMilliseconds = static_cast<float>(RebuildMilliseconds);
			// The old relaxation targeted the old field; cells opened during the rebuild may be missing from the new one.
			RelaxHeap.Reset();
			for (const FIntPoint& Cell : OpenedDuringRebuild)
			{
				SeedAround(Cell);
			}
			OpenedDuringRebuild.Reset();
			RunRelaxation(DeadlineSeconds);
		}
	}
	AddFrameWork((FPlatformTime::Seconds() - StartSeconds) * 1000.0);
}

void URRFlowFieldSubsystem::ApplyPendingOpenings()
{
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	for (const FIntPoint& Cell : PendingOpenings)
	{
		if (!IsInField(Cell))
		{
			continue;
		}
		const int32 Index = GetFieldIndex(Cell);
		const ERRCellType Type = CurrentGrid->GetCellType(Cell);
		if (Walkable[Index] || (Type != ERRCellType::Floor && Type != ERRCellType::Spawn))
		{
			continue;
		}
		Walkable[Index] = true;
		++OpenedCellCount;
		if (bGoalRebuildActive)
		{
			OpenedDuringRebuild.Add(Cell);
		}
		SeedAround(Cell);
	}
	PendingOpenings.Reset();
}

void URRFlowFieldSubsystem::SeedAround(FIntPoint Cell)
{
	// The opened cell itself: one step more than its best neighbour, direction set now so robots never meet a gap.
	const int32 Index = GetFieldIndex(Cell);
	const uint8 Best = ChooseDirection(Cell, Distances, FieldGoalCell);
	if (Best != NoDirection && Distances[Index] != 0)
	{
		const int32 Candidate = Distances[GetFieldIndex(Cell + RRFlowField::Offsets[Best])] + 1;
		if (Distances[Index] == INDEX_NONE || Candidate < Distances[Index])
		{
			Distances[Index] = Candidate;
			Directions[Index] = Best;
		}
	}
	PushRelax(Index);
	// Its neighbours gain edges (to the cell, and diagonals that no longer cut its corner): relax from each of them.
	for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
	{
		const FIntPoint Next = Cell + RRFlowField::Offsets[Neighbour];
		if (IsInField(Next) && Walkable[GetFieldIndex(Next)])
		{
			PushRelax(GetFieldIndex(Next));
		}
	}
}

void URRFlowFieldSubsystem::PushRelax(int32 Index)
{
	if (Distances[Index] != INDEX_NONE)
	{
		RelaxHeap.HeapPush(RRFlowField::PackEntry(Distances[Index], Index));
	}
}

bool URRFlowFieldSubsystem::RunRelaxation(double DeadlineSeconds)
{
	int32 Steps = 0;
	while (RelaxHeap.Num() > 0)
	{
		if (++Steps % BudgetCheckInterval == 0 && RRFlowField::IsPastDeadline(DeadlineSeconds))
		{
			return false;
		}
		uint64 Entry = 0;
		RelaxHeap.HeapPop(Entry, EAllowShrinking::No);
		const int32 Index = static_cast<int32>(Entry & 0xFFFFFFFFull);
		const int32 Distance = static_cast<int32>(Entry >> 32);
		if (Distances[Index] != Distance)
		{
			continue; // Stale: the cell got shorter since this entry was pushed.
		}
		const FIntPoint Cell = GetFieldCell(Index);
		if (Distance > 0)
		{
			// Lowest distance first: the neighbours closer than this cell are final, so the full rule applies.
			Directions[Index] = ChooseDirection(Cell, Distances, FieldGoalCell);
		}
		for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
		{
			if (!CanStep(Cell, Neighbour))
			{
				continue;
			}
			const FIntPoint Next = Cell + RRFlowField::Offsets[Neighbour];
			const int32 NextIndex = GetFieldIndex(Next);
			const int32 NextDistance = Distances[NextIndex];
			const uint8 Back = RRFlowField::Opposite[Neighbour];
			if (NextDistance == INDEX_NONE || Distance + 1 < NextDistance)
			{
				// Shorter through this cell: valid direction now, refined when Next is popped.
				Distances[NextIndex] = Distance + 1;
				Directions[NextIndex] = Back;
				PushRelax(NextIndex);
			}
			else if (Distance + 1 == NextDistance
				&& (Directions[NextIndex] == NoDirection || IsBetterTie(Next, Back, Directions[NextIndex], FieldGoalCell)))
			{
				// Same distance through this cell, better tie-break: what a full rebuild would pick.
				Directions[NextIndex] = Back;
			}
		}
	}
	return true;
}

void URRFlowFieldSubsystem::AddFrameWork(double Milliseconds)
{
	if (FrameWorkFrame != GFrameCounter)
	{
		FrameWorkFrame = GFrameCounter;
		FrameWorkMilliseconds = 0.f;
		++WorkFrameCount;
	}
	FrameWorkMilliseconds += static_cast<float>(Milliseconds);
}

void URRFlowFieldSubsystem::HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source)
{
	// Destruction only opens cells (the grid sets them to Floor): repaired incrementally by the next budgeted pass.
	PendingOpenings.Add(Cell);
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
