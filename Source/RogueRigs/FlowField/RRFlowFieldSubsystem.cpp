// Rogue Rigs - flow field world subsystem: one grid BFS from the miner's cell gives every ground robot its direction,
// repaired incrementally when cells open and refreshed when the miner changes cell, on a worker thread (double buffer) or time-sliced.

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
	TAutoConsoleVariable<int32> CVarDebug(TEXT("rr.FlowField.Debug"), 0,
		TEXT("1: draw the flow field directions around the goal."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarDebugRadius(TEXT("rr.FlowField.DebugRadius"), 15,
		TEXT("Radius in cells around the goal drawn by rr.FlowField.Debug."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarBudgetMs(TEXT("rr.FlowField.BudgetMs"), 0.3f,
		TEXT("Per-frame time budget (ms) of the incremental flow-field work: opened cells and the goal rebuild. <= 0: no budget, all work done in the frame."),
		ECVF_Default);

	TAutoConsoleVariable<int32> CVarAsync(TEXT("rr.FlowField.Async"), 1,
		TEXT("1: openings and goal rebuilds computed on a worker thread over a copy of the field, swapped in when complete. 0: time-sliced on the game thread under rr.FlowField.BudgetMs (platforms without spare cores)."),
		ECVF_Default);
}

void URRFlowFieldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateField();
	if (LastWorkFrame != GFrameCounter)
	{
		LastWorkFrame = GFrameCounter;
		if (IsAsyncEnabled())
		{
			const double StartSeconds = FPlatformTime::Seconds();
			if (ProcessAsyncWork())
			{
				AddFrameWork((FPlatformTime::Seconds() - StartSeconds) * 1000.0);
			}
		}
		else if (!Job.IsValid() || JobTask.IsCompleted())
		{
			// Time-sliced fallback; a job left by a switch from the worker path is applied first (it never blocks).
			if (Job.IsValid())
			{
				ApplyCompletedJob();
			}
			const float BudgetMs = RRFlowField::CVarBudgetMs.GetValueOnGameThread();
			ProcessPendingWork(BudgetMs > 0.f ? FPlatformTime::Seconds() + BudgetMs / 1000.0 : 0.0);
		}
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
	if (Job.IsValid())
	{
		// The job owns its data, but no worker should outlive the world it was launched for.
		JobTask.Wait();
		Job.Reset();
	}
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
	if (!bDirty && Size == Field.Size && Field.IsComputed())
	{
		return;
	}
	ComputeField(CurrentGrid->GetCellAtWorld(GoalActor->GetActorLocation()), Size);
}

void URRFlowFieldSubsystem::FlushPendingWork()
{
	UpdateField();
	if (Job.IsValid())
	{
		// Explicit flush: the only place the game thread waits for the worker.
		JobTask.Wait();
		ApplyCompletedJob();
	}
	// A goal rebuild started for an older cell may finish while the goal sits elsewhere: a few passes settle it.
	for (int32 Pass = 0; Pass < 4 && IsUpdatePending(); ++Pass)
	{
		ProcessPendingWork(0.0);
	}
}

bool URRFlowFieldSubsystem::IsUpdatePending() const
{
	if (PendingOpenings.Num() > 0 || Field.RelaxHeap.Num() > 0 || bGoalRebuildActive || Job.IsValid())
	{
		return true;
	}
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	const AActor* Goal = GoalActor.Get();
	return CurrentGrid && Goal && CurrentGrid->GetCellAtWorld(Goal->GetActorLocation()) != Field.GoalCell;
}

void URRFlowFieldSubsystem::ComputeField(FIntPoint GoalCell, FIntPoint Size)
{
	const double StartSeconds = FPlatformTime::Seconds();
	const ARRArenaGrid* CurrentGrid = Grid.Get();

	Field.Size = Size;
	Field.GoalCell = GoalCell;
	bDirty = false;
	LastComputeFrame = GFrameCounter;
	++ComputeCount;

	// The walkability is read fresh from the grid: queued openings, a running goal rebuild and a job in flight are obsolete.
	++JobGeneration;
	PendingOpenings.Reset();
	Field.RelaxHeap.Reset();
	bGoalRebuildActive = false;
	OpenedDuringRebuild.Reset();

	const int32 CellCount = Size.X * Size.Y;
	Field.Walkable.SetNumUninitialized(CellCount);
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const ERRCellType Type = CurrentGrid->GetCellType(FIntPoint(X, Y));
			Field.Walkable[Y * Size.X + X] = Type == ERRCellType::Floor || Type == ERRCellType::Spawn;
		}
	}
	Field.ComputeFull(GoalCell);

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
	if (!CurrentGrid || !Goal || bDirty || CurrentGrid->GetGridSize() != Field.Size || !Field.IsComputed())
	{
		// No valid field: the next UpdateField rebuilds it in full.
		return;
	}
	const FIntPoint GoalCell = CurrentGrid->GetCellAtWorld(Goal->GetActorLocation());
	if (PendingOpenings.Num() == 0 && Field.RelaxHeap.Num() == 0 && !bGoalRebuildActive && GoalCell == Field.GoalCell)
	{
		return;
	}
	const double StartSeconds = FPlatformTime::Seconds();

	// Openings first: they only shorten paths of the live field.
	ApplyPendingOpenings();
	Field.RunRelaxation(DeadlineSeconds);

	// Goal rebuild: a BFS into the second buffer, sliced; it always advances a little, so constant digging cannot starve it.
	if (!bGoalRebuildActive && GoalCell != Field.GoalCell)
	{
		const int32 CellCount = Field.Size.X * Field.Size.Y;
		bGoalRebuildActive = true;
		RebuildGoalCell = GoalCell;
		RebuildDistances.Init(INDEX_NONE, CellCount);
		RebuildDirections.Init(FRRFlowFieldData::NoDirection, CellCount);
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
		const bool bDone = Field.AdvanceBfs(RebuildGoalCell, RebuildDistances, RebuildDirections, RebuildQueue, RebuildHead, RebuildDirectionHead,
			DeadlineSeconds);
		RebuildMilliseconds += (FPlatformTime::Seconds() - SliceStart) * 1000.0;
		if (bDone)
		{
			Swap(Field.Distances, RebuildDistances);
			Swap(Field.Directions, RebuildDirections);
			Field.GoalCell = RebuildGoalCell;
			bGoalRebuildActive = false;
			++ComputeCount;
			LastComputeMilliseconds = static_cast<float>(RebuildMilliseconds);
			// The old relaxation targeted the old field; cells opened during the rebuild may be missing from the new one.
			Field.RelaxHeap.Reset();
			for (const FIntPoint& Cell : OpenedDuringRebuild)
			{
				Field.SeedAround(Cell);
			}
			OpenedDuringRebuild.Reset();
			Field.RunRelaxation(DeadlineSeconds);
		}
	}
	AddFrameWork((FPlatformTime::Seconds() - StartSeconds) * 1000.0);
}

bool URRFlowFieldSubsystem::IsAsyncEnabled()
{
	return RRFlowField::CVarAsync.GetValueOnGameThread() != 0;
}

bool URRFlowFieldSubsystem::ProcessAsyncWork()
{
	bool bWorked = false;
	if (Job.IsValid())
	{
		if (!JobTask.IsCompleted())
		{
			return false; // Still computing: the live field keeps answering, openings stay queued.
		}
		ApplyCompletedJob();
		bWorked = true;
	}
	return LaunchJob() || bWorked;
}

void URRFlowFieldSubsystem::ApplyCompletedJob()
{
	TSharedPtr<FRRFlowFieldJob, ESPMode::ThreadSafe> Done = MoveTemp(Job);
	JobTask = UE::Tasks::FTask();
	if (Done->Generation == JobGeneration && Done->Field.Size == Field.Size)
	{
		// Double buffer: the complete field becomes live, the old one becomes the spare buffer of the next job.
		Swap(Field, Done->Field);
		OpenedCellCount += Done->OpenedIndices.Num();
		if (Done->bGoalRebuild)
		{
			++ComputeCount;
			LastComputeMilliseconds = static_cast<float>(Done->WorkerMilliseconds);
		}
	}
	SpareJob = MoveTemp(Done);
}

bool URRFlowFieldSubsystem::LaunchJob()
{
	const ARRArenaGrid* CurrentGrid = Grid.Get();
	const AActor* Goal = GoalActor.Get();
	if (Job.IsValid() || !CurrentGrid || !Goal || bDirty || CurrentGrid->GetGridSize() != Field.Size || !Field.IsComputed())
	{
		return false;
	}
	const FIntPoint GoalCell = CurrentGrid->GetCellAtWorld(Goal->GetActorLocation());
	const bool bGoalRebuild = GoalCell != Field.GoalCell;
	// A time-sliced goal rebuild left by a switch from the fallback is superseded (its openings are already walkable).
	bGoalRebuildActive = false;
	OpenedDuringRebuild.Reset();
	if (PendingOpenings.Num() == 0 && Field.RelaxHeap.Num() == 0 && !bGoalRebuild)
	{
		return false;
	}

	TSharedPtr<FRRFlowFieldJob, ESPMode::ThreadSafe> NewJob = SpareJob.IsValid() ? MoveTemp(SpareJob) : MakeShared<FRRFlowFieldJob, ESPMode::ThreadSafe>();
	FRRFlowFieldData& JobField = NewJob->Field;
	JobField.Size = Field.Size;
	JobField.Walkable = Field.Walkable;
	// Openings validated here, on the game thread (the worker never reads the grid); duplicates and stale cells skipped.
	NewJob->OpenedIndices.Reset();
	for (const FIntPoint& Cell : PendingOpenings)
	{
		if (!Field.IsInField(Cell))
		{
			continue;
		}
		const int32 Index = Field.GetIndex(Cell);
		const ERRCellType Type = CurrentGrid->GetCellType(Cell);
		if (!JobField.Walkable[Index] && (Type == ERRCellType::Floor || Type == ERRCellType::Spawn))
		{
			JobField.Walkable[Index] = true;
			NewJob->OpenedIndices.Add(Index);
		}
	}
	PendingOpenings.Reset();
	if (!bGoalRebuild && NewJob->OpenedIndices.Num() == 0 && Field.RelaxHeap.Num() == 0)
	{
		SpareJob = MoveTemp(NewJob);
		return true; // Only already-applied openings: consumed, nothing to compute.
	}
	NewJob->bGoalRebuild = bGoalRebuild;
	NewJob->RebuildGoalCell = GoalCell;
	if (bGoalRebuild)
	{
		JobField.GoalCell = GoalCell;
		JobField.RelaxHeap.Reset();
	}
	else
	{
		// Incremental update on a copy of the live distances; a relaxation left by the fallback continues there.
		JobField.GoalCell = Field.GoalCell;
		JobField.Distances = Field.Distances;
		JobField.Directions = Field.Directions;
		JobField.RelaxHeap = Field.RelaxHeap;
	}
	Field.RelaxHeap.Reset();
	NewJob->Generation = JobGeneration;
	NewJob->WorkerMilliseconds = 0.0;
	Job = NewJob;
	JobTask = UE::Tasks::Launch(TEXT("RRFlowFieldJob"), [RunningJob = MoveTemp(NewJob)]() { RunningJob->Run(); });
	return true;
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
		if (Field.Walkable[Index] || (Type != ERRCellType::Floor && Type != ERRCellType::Spawn))
		{
			continue;
		}
		Field.Walkable[Index] = true;
		++OpenedCellCount;
		if (bGoalRebuildActive)
		{
			OpenedDuringRebuild.Add(Cell);
		}
		Field.SeedAround(Cell);
	}
	PendingOpenings.Reset();
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
			const int32 Steps = IsInField(Cell) && Field.Distances.IsValidIndex(GetFieldIndex(Cell)) ? Field.Distances[GetFieldIndex(Cell)] : INDEX_NONE;
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
	return IsInField(Cell) && Field.Distances.IsValidIndex(GetFieldIndex(Cell)) ? Field.Distances[GetFieldIndex(Cell)] : INDEX_NONE;
}

FIntPoint URRFlowFieldSubsystem::GetNextCell(FIntPoint Cell)
{
	UpdateField();
	if (!IsInField(Cell) || !Field.Directions.IsValidIndex(GetFieldIndex(Cell)) || Field.Directions[GetFieldIndex(Cell)] == FRRFlowFieldData::NoDirection)
	{
		return FIntPoint(INDEX_NONE, INDEX_NONE);
	}
	return Cell + FRRFlowFieldData::Offsets[Field.Directions[GetFieldIndex(Cell)]];
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
	if (!CurrentGrid || !World || !IsInField(Field.GoalCell) || Field.Directions.Num() != Field.Size.X * Field.Size.Y)
	{
		return;
	}
	const int32 Radius = FMath::Max(0, RRFlowField::CVarDebugRadius.GetValueOnGameThread());
	const float CellSize = CurrentGrid->GetCellSize();
	const FVector Lift(0.f, 0.f, CurrentGrid->GetBlockHeight() * 0.5f);
	for (int32 Y = FMath::Max(0, Field.GoalCell.Y - Radius); Y <= FMath::Min(Field.Size.Y - 1, Field.GoalCell.Y + Radius); ++Y)
	{
		for (int32 X = FMath::Max(0, Field.GoalCell.X - Radius); X <= FMath::Min(Field.Size.X - 1, Field.GoalCell.X + Radius); ++X)
		{
			const int32 Index = GetFieldIndex(FIntPoint(X, Y));
			const FVector Center = CurrentGrid->GetCellCenter(X, Y) + Lift;
			if (Field.Distances[Index] == 0)
			{
				DrawDebugBox(World, Center, FVector(CellSize * 0.3f), FColor::Yellow, false, -1.f, 0, 4.f);
			}
			else if (Field.Directions[Index] != FRRFlowFieldData::NoDirection)
			{
				const FIntPoint Offset = FRRFlowFieldData::Offsets[Field.Directions[Index]];
				const FVector Tip = Center + FVector(Offset.X, Offset.Y, 0.f).GetSafeNormal() * CellSize * 0.4f;
				DrawDebugDirectionalArrow(World, Center, Tip, CellSize * 0.15f, FColor::Green, false, -1.f, 0, 3.f);
			}
			else if (Field.Walkable[Index])
			{
				DrawDebugPoint(World, Center, 8.f, FColor::Red, false, -1.f);
			}
		}
	}
#endif
}
