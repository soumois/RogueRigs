// Rogue Rigs - automation tests of the flow field and the generic enemy movement (RogueRigs.FlowField.*): paths, pockets, recompute triggers, BFS time, incremental updates and budget.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "Debug/RRGridStress.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "Math/RandomStream.h"
#include "RogueRigs.h"
#include "Tests/RRTestHarness.h"

namespace RRFlowFieldTests
{
	using namespace RRTestHarness;

	/** Spawn cell of arena_m1: the miner stands there in every flow field test. */
	const FIntPoint MinerCell(40, 40);

	/** Reachable floor cells of arena_m1 spread over the asteroid, several behind rocks. */
	const FIntPoint ReachableCells[] = { FIntPoint(55, 42), FIntPoint(38, 48), FIntPoint(45, 37), FIntPoint(25, 55), FIntPoint(20, 30) };

	/** Floor cell of a hidden pocket of arena_m1 (34 cells, 41-43 x 23-35), closed on the south by rock (39, 36). */
	const FIntPoint PocketCell(39, 35);
	/** Rock separating PocketCell from the main floor cell (39, 37). */
	const FIntPoint PocketRockCell(39, 36);
	/** Main-area floor cell south of the pocket rock. */
	const FIntPoint PocketSouthCell(39, 37);

	/** Floor cell of arena_m1 the movement test starts from: rocks (38-39, 45-47) stand between it and the miner. */
	const FIntPoint RobotStartCell(38, 48);

	/** Grid, miner and flow field of one test world. */
	struct FFlowFieldSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		URRFlowFieldSubsystem* FlowField = nullptr;
	};

	/** Spawns the grid and a miner on MinerCell, gets the subsystem and ticks one frame so the field is computed. */
	bool SetUp(FAutomationTestBase& Test, const FTestWorld& TestWorld, FFlowFieldSetup& Out)
	{
		Out.Grid = SpawnGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? SpawnMiner(Test, TestWorld, Out.Grid, MinerCell) : nullptr;
		Out.FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
		if (!Out.Miner || !Test.TestNotNull(TEXT("flow field subsystem exists"), Out.FlowField))
		{
			return false;
		}
		TestWorld.Tick();
		Test.TestEqual(TEXT("field grid is the spawned grid"), Out.FlowField->GetGrid(), Out.Grid);
		Test.TestEqual(TEXT("field goal is the miner"), Out.FlowField->GetGoalActor(), static_cast<AActor*>(Out.Miner));
		Test.TestEqual(TEXT("field computed from the miner cell"), Out.FlowField->GetGoalCell(), MinerCell);
		return true;
	}

	/**
	 * Walks from Start by GetMoveDirection, one cell per step (the cell one CellSize along the direction from the cell
	 * center). Every step must land on a walkable cell, without cutting a block corner, one BFS step closer.
	 * Returns the last cell reached.
	 */
	FIntPoint FollowField(FAutomationTestBase& Test, const FFlowFieldSetup& Setup, FIntPoint Start)
	{
		const float CellSize = Setup.Grid->GetCellSize();
		FIntPoint Cell = Start;
		for (int32 Step = 0; Step < 400; ++Step)
		{
			const FVector Center = Setup.Grid->GetCellCenter(Cell.X, Cell.Y);
			const FVector Direction = Setup.FlowField->GetMoveDirection(Center);
			if (Direction.IsNearlyZero())
			{
				break;
			}
			const FIntPoint Next = Setup.Grid->GetCellAtWorld(Center + Direction * CellSize);
			const bool bValid = Setup.FlowField->IsWalkableCell(Next)
				&& FMath::Abs(Next.X - Cell.X) <= 1 && FMath::Abs(Next.Y - Cell.Y) <= 1
				&& Setup.FlowField->IsWalkableCell(FIntPoint(Next.X, Cell.Y)) && Setup.FlowField->IsWalkableCell(FIntPoint(Cell.X, Next.Y))
				&& Setup.FlowField->GetCellDistance(Next) == Setup.FlowField->GetCellDistance(Cell) - 1;
			if (!Test.TestTrue(FString::Printf(TEXT("step (%d, %d) -> (%d, %d) is a valid path step"), Cell.X, Cell.Y, Next.X, Next.Y), bValid))
			{
				break;
			}
			Cell = Next;
		}
		return Cell;
	}
}

using namespace RRFlowFieldTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldFollowTest, "RogueRigs.FlowField.FollowReachesMiner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldFollowTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	TestTrue(TEXT("goal cell reachable"), Setup.FlowField->IsReachable(Setup.Grid->GetCellCenter(MinerCell.X, MinerCell.Y)));
	TestTrue(TEXT("zero direction in the goal cell"), Setup.FlowField->GetMoveDirection(Setup.Grid->GetCellCenter(MinerCell.X, MinerCell.Y)).IsNearlyZero());

	for (const FIntPoint& Start : ReachableCells)
	{
		const FVector Center = Setup.Grid->GetCellCenter(Start.X, Start.Y);
		TestTrue(FString::Printf(TEXT("(%d, %d) reachable"), Start.X, Start.Y), Setup.FlowField->IsReachable(Center));
		const FVector Direction = Setup.FlowField->GetMoveDirection(Center);
		TestTrue(FString::Printf(TEXT("(%d, %d) direction is unit and flat"), Start.X, Start.Y),
			FMath::IsNearlyEqual(Direction.Size(), 1.f, 1e-3f) && FMath::IsNearlyZero(Direction.Z));
		const int32 Steps = Setup.FlowField->GetCellDistance(Start);
		TestNearlyEqual(FString::Printf(TEXT("(%d, %d) path distance = steps x CellSize"), Start.X, Start.Y),
			Setup.FlowField->GetPathDistance(Center), Steps * Setup.Grid->GetCellSize());
		const FIntPoint End = FollowField(*this, Setup, Start);
		AddInfo(FString::Printf(TEXT("(%d, %d): %d steps to the miner"), Start.X, Start.Y, Steps));
		TestEqual(FString::Printf(TEXT("following from (%d, %d) reaches the miner cell"), Start.X, Start.Y), End, MinerCell);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldPocketTest, "RogueRigs.FlowField.EnclosedPocketUnreachable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldPocketTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	TestEqual(TEXT("(39, 35) is floor"), Setup.Grid->GetCellType(PocketCell), ERRCellType::Floor);
	TestEqual(TEXT("(39, 36) is rock"), Setup.Grid->GetCellType(PocketRockCell), ERRCellType::Rock);
	TestTrue(TEXT("(39, 37) reachable"), Setup.FlowField->GetCellDistance(PocketSouthCell) > 0);

	const FVector PocketCenter = Setup.Grid->GetCellCenter(PocketCell.X, PocketCell.Y);
	TestFalse(TEXT("pocket unreachable"), Setup.FlowField->IsReachable(PocketCenter));
	TestTrue(TEXT("pocket path distance negative"), Setup.FlowField->GetPathDistance(PocketCenter) < 0.f);
	TestTrue(TEXT("pocket direction zero"), Setup.FlowField->GetMoveDirection(PocketCenter).IsNearlyZero());
	TestFalse(TEXT("no direct path through the rock"), Setup.FlowField->HasDirectPath(PocketCenter, Setup.Grid->GetCellCenter(PocketSouthCell.X, PocketSouthCell.Y)));
	TestFalse(TEXT("void is not walkable"), Setup.FlowField->IsWalkableCell(FIntPoint(0, 0)));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldOpenPocketTest, "RogueRigs.FlowField.DestroyedRockOpensPocket",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldOpenPocketTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	const FVector PocketCenter = Setup.Grid->GetCellCenter(PocketCell.X, PocketCell.Y);
	TestFalse(TEXT("pocket unreachable before"), Setup.FlowField->IsReachable(PocketCenter));
	const int32 CountBefore = Setup.FlowField->GetComputeCount();
	const int32 OpenedBefore = Setup.FlowField->GetOpenedCellCount();

	// Two destructions in one frame: applied incrementally by the next tick, no full rebuild.
	TestTrue(TEXT("pocket rock destroyed"), Setup.Grid->DestroyCell(PocketRockCell, ERRTerrainDamageSource::Drone));
	TestTrue(TEXT("(43, 38) destroyed"), Setup.Grid->DestroyCell(RockCell, ERRTerrainDamageSource::Drone));
	TestWorld.Tick();
	TestEqual(TEXT("no full rebuild for two destructions"), Setup.FlowField->GetComputeCount(), CountBefore);
	TestEqual(TEXT("two cells opened incrementally"), Setup.FlowField->GetOpenedCellCount(), OpenedBefore + 2);
	TestFalse(TEXT("nothing pending after one tick"), Setup.FlowField->IsUpdatePending());

	TestTrue(TEXT("pocket reachable after"), Setup.FlowField->IsReachable(PocketCenter));
	TestTrue(TEXT("direct path through the opened cell"), Setup.FlowField->HasDirectPath(PocketCenter, Setup.Grid->GetCellCenter(PocketSouthCell.X, PocketSouthCell.Y)));
	TestEqual(TEXT("following from the pocket reaches the miner cell"), FollowField(*this, Setup, PocketCell), MinerCell);

	TestWorld.Tick();
	TestEqual(TEXT("no recompute without a trigger"), Setup.FlowField->GetComputeCount(), CountBefore);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldMinerMoveTest, "RogueRigs.FlowField.MinerMoveUpdates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldMinerMoveTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	const FIntPoint NewCell(MinerCell.X + 1, MinerCell.Y);
	TestEqual(TEXT("new cell one step away before"), Setup.FlowField->GetCellDistance(NewCell), 1);
	const int32 CountBefore = Setup.FlowField->GetComputeCount();

	const FVector Offset = Setup.Grid->GetCellCenter(NewCell.X, NewCell.Y) - Setup.Grid->GetCellCenter(MinerCell.X, MinerCell.Y);
	Setup.Miner->SetActorLocation(Setup.Miner->GetActorLocation() + Offset, false, nullptr, ETeleportType::TeleportPhysics);
	// The goal rebuild is time-sliced (rr.FlowField.BudgetMs): 80 x 80 takes one or two frames.
	int32 Frames = 0;
	do
	{
		TestWorld.Tick();
	}
	while (++Frames < 10 && Setup.FlowField->GetGoalCell() != NewCell);
	AddInfo(FString::Printf(TEXT("goal rebuild done in %d frame(s)"), Frames));

	TestEqual(TEXT("one recompute"), Setup.FlowField->GetComputeCount(), CountBefore + 1);
	TestEqual(TEXT("goal cell follows the miner"), Setup.FlowField->GetGoalCell(), NewCell);
	TestEqual(TEXT("new miner cell at distance 0"), Setup.FlowField->GetCellDistance(NewCell), 0);
	TestEqual(TEXT("old miner cell at distance 1"), Setup.FlowField->GetCellDistance(MinerCell), 1);
	TestEqual(TEXT("old miner cell points to the new one"), Setup.FlowField->GetNextCell(MinerCell), NewCell);
	const FVector Direction = Setup.FlowField->GetMoveDirection(Setup.Grid->GetCellCenter(MinerCell.X, MinerCell.Y));
	TestTrue(TEXT("direction from the old cell is +X"), Direction.Equals(FVector(1.f, 0.f, 0.f), 1e-3f));

	TestWorld.Tick();
	TestEqual(TEXT("no recompute while the miner stays in its cell"), Setup.FlowField->GetComputeCount(), CountBefore + 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldTimingTest, "RogueRigs.FlowField.BfsTime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldTimingTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	TestEqual(TEXT("grid is 80 x 80"), Setup.Grid->GetGridSize(), FIntPoint(80, 80));
	constexpr int32 Runs = 20;
	float Total = 0.f;
	float Worst = 0.f;
	for (int32 Run = 0; Run < Runs; ++Run)
	{
		Setup.FlowField->MarkDirty();
		TestWorld.Tick();
		Total += Setup.FlowField->GetLastComputeMilliseconds();
		Worst = FMath::Max(Worst, Setup.FlowField->GetLastComputeMilliseconds());
	}
	AddInfo(FString::Printf(TEXT("flow field BFS 80 x 80: mean %.3f ms, worst %.3f ms over %d runs"), Total / Runs, Worst, Runs));
	UE_LOG(LogRogueRigs, Display, TEXT("RogueRigs.FlowField.BfsTime: 80 x 80 BFS mean %.3f ms, worst %.3f ms over %d runs"), Total / Runs, Worst, Runs);
	TestTrue(TEXT("BFS time measured"), Total > 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldMovementTest, "RogueRigs.FlowField.MovementComponentReachesMiner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldMovementTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	// Plain character as a stand-in robot: capsule and CharacterMovement defaults, walking without a controller.
	const float HalfHeight = ACharacter::StaticClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ACharacter* Robot = TestWorld.Spawn<ACharacter>(ACharacter::StaticClass(),
		Setup.Grid->GetCellCenter(RobotStartCell.X, RobotStartCell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f));
	if (!TestNotNull(TEXT("robot spawned"), Robot))
	{
		return false;
	}
	Robot->GetCharacterMovement()->bRunPhysicsWithNoController = true;
	Robot->GetCharacterMovement()->SetDefaultMovementMode();
	URREnemyMovementComponent* Movement = NewObject<URREnemyMovementComponent>(Robot);
	Movement->RegisterComponent();
	TestFalse(TEXT("no direct path from the start"), Setup.FlowField->HasDirectPath(Robot->GetActorLocation(), Setup.Miner->GetActorLocation()));

	int32 ArrivalFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 900 && ArrivalFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (Movement->HasArrived())
		{
			ArrivalFrame = Frame;
		}
	}
	const float Distance = FVector::Dist2D(Robot->GetActorLocation(), Setup.Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("robot arrived at frame %d (%.2f s), %.1f uu from the miner"), ArrivalFrame, ArrivalFrame * FrameSeconds, Distance));
	TestTrue(TEXT("robot arrives within 15 s"), ArrivalFrame != INDEX_NONE);
	TestTrue(TEXT("robot within the arrival distance (+ one frame of travel)"), Distance <= Movement->GetArrivalDistance() + 20.f);
	return true;
}

namespace RRFlowFieldIncrementalTests
{
	using namespace RRFlowFieldTests;

	/** Sets rr.FlowField.BudgetMs for the scope of a test and restores the previous value. */
	struct FScopedBudget
	{
		explicit FScopedBudget(float Milliseconds)
		{
			Variable = IConsoleManager::Get().FindConsoleVariable(TEXT("rr.FlowField.BudgetMs"));
			if (Variable)
			{
				Previous = Variable->GetFloat();
				Variable->Set(Milliseconds, ECVF_SetByCode);
			}
		}
		~FScopedBudget()
		{
			if (Variable)
			{
				Variable->Set(Previous, ECVF_SetByCode);
			}
		}
		IConsoleVariable* Variable = nullptr;
		float Previous = 0.f;
	};

	/** Current budget of the incremental work in ms (rr.FlowField.BudgetMs). */
	float GetBudgetMs()
	{
		const IConsoleVariable* Variable = IConsoleManager::Get().FindConsoleVariable(TEXT("rr.FlowField.BudgetMs"));
		return Variable ? Variable->GetFloat() : 0.f;
	}

	/** Rock cells of Grid (no ore, no barrel), sorted by distance to Center when bSortFromCenter, else shuffled with Seed. */
	TArray<FIntPoint> ListRockCells(const ARRArenaGrid* Grid, FIntPoint Center, bool bSortFromCenter, int32 Seed)
	{
		TArray<FIntPoint> Rocks;
		const FIntPoint Size = Grid->GetGridSize();
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				if (Grid->GetCellType(FIntPoint(X, Y)) == ERRCellType::Rock)
				{
					Rocks.Add(FIntPoint(X, Y));
				}
			}
		}
		if (bSortFromCenter)
		{
			Rocks.StableSort([Center](const FIntPoint& A, const FIntPoint& B)
			{
				return (A - Center).SizeSquared() < (B - Center).SizeSquared();
			});
		}
		else
		{
			FRandomStream Stream(Seed);
			for (int32 Index = Rocks.Num() - 1; Index > 0; --Index)
			{
				Rocks.Swap(Index, Stream.RandRange(0, Index));
			}
		}
		return Rocks;
	}

	/** Ticks until the flow field has no pending work (at most MaxFrames); returns the frames ticked. */
	int32 TickUntilSettled(const FTestWorld& TestWorld, URRFlowFieldSubsystem* FlowField, int32 MaxFrames)
	{
		int32 Frames = 0;
		while (FlowField->IsUpdatePending() && Frames < MaxFrames)
		{
			TestWorld.Tick();
			++Frames;
		}
		return Frames;
	}

	/**
	 * Snapshots the distance and next cell of every cell, forces a synchronous full rebuild and checks the rebuild gives the
	 * same values everywhere (the incremental result must equal a full rebuild, tie-breaks included).
	 */
	void CheckMatchesFullRebuild(FAutomationTestBase& Test, const FFlowFieldSetup& Setup)
	{
		const FIntPoint Size = Setup.Grid->GetGridSize();
		TArray<int32> IncrementalDistances;
		TArray<FIntPoint> IncrementalNext;
		IncrementalDistances.Reserve(Size.X * Size.Y);
		IncrementalNext.Reserve(Size.X * Size.Y);
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				IncrementalDistances.Add(Setup.FlowField->GetCellDistance(FIntPoint(X, Y)));
				IncrementalNext.Add(Setup.FlowField->GetNextCell(FIntPoint(X, Y)));
			}
		}
		const FIntPoint IncrementalGoal = Setup.FlowField->GetGoalCell();
		const int32 CountBefore = Setup.FlowField->GetComputeCount();
		Setup.FlowField->MarkDirty();
		Setup.FlowField->UpdateField();
		Test.TestEqual(TEXT("full rebuild forced"), Setup.FlowField->GetComputeCount(), CountBefore + 1);
		Test.TestEqual(TEXT("same goal cell as the rebuild"), IncrementalGoal, Setup.FlowField->GetGoalCell());

		int32 DistanceMismatches = 0;
		int32 DirectionMismatches = 0;
		int32 Reachable = 0;
		for (int32 Y = 0; Y < Size.Y; ++Y)
		{
			for (int32 X = 0; X < Size.X; ++X)
			{
				const int32 Index = Y * Size.X + X;
				const int32 Distance = Setup.FlowField->GetCellDistance(FIntPoint(X, Y));
				const FIntPoint Next = Setup.FlowField->GetNextCell(FIntPoint(X, Y));
				Reachable += Distance != INDEX_NONE ? 1 : 0;
				if (Distance != IncrementalDistances[Index] && ++DistanceMismatches <= 5)
				{
					Test.AddError(FString::Printf(TEXT("(%d, %d): incremental distance %d, rebuild %d"), X, Y, IncrementalDistances[Index], Distance));
				}
				if (Next != IncrementalNext[Index] && ++DirectionMismatches <= 5)
				{
					Test.AddError(FString::Printf(TEXT("(%d, %d): incremental next (%d, %d), rebuild (%d, %d)"), X, Y,
						IncrementalNext[Index].X, IncrementalNext[Index].Y, Next.X, Next.Y));
				}
			}
		}
		Test.AddInfo(FString::Printf(TEXT("%d reachable cells compared, %d distance and %d direction mismatches"), Reachable, DistanceMismatches, DirectionMismatches));
		Test.TestEqual(TEXT("distances equal a full rebuild"), DistanceMismatches, 0);
		Test.TestEqual(TEXT("directions equal a full rebuild"), DirectionMismatches, 0);
	}

	/** Teleports the miner to Location when its cell is walkable; true when moved. */
	bool MoveMinerTo(const FFlowFieldSetup& Setup, const FVector& Location)
	{
		if (!Setup.FlowField->IsWalkableCell(Setup.Grid->GetCellAtWorld(Location)))
		{
			return false;
		}
		Setup.Miner->SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
		return true;
	}
}

using namespace RRFlowFieldIncrementalTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldIncrementalMatchesTest, "RogueRigs.FlowField.IncrementalMatchesRebuild",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldIncrementalMatchesTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	// A tiny budget slices everything over many frames, so openings land while relaxations and goal rebuilds are half done.
	FScopedBudget Budget(0.001f);
	const TArray<FIntPoint> Rocks = ListRockCells(Setup.Grid, MinerCell, /*bSortFromCenter*/ false, 2902);
	if (!TestTrue(TEXT("enough rock cells"), Rocks.Num() >= 150))
	{
		return false;
	}
	const int32 ComputesBefore = Setup.FlowField->GetComputeCount();
	const int32 OpenedBefore = Setup.FlowField->GetOpenedCellCount();
	const float CellSize = Setup.Grid->GetCellSize();
	int32 Destroyed = 0;
	int32 GoalMoves = 0;
	for (int32 Frame = 0; Frame < 50; ++Frame)
	{
		for (int32 Count = 0; Count < 3; ++Count)
		{
			Destroyed += Setup.Grid->DestroyCell(Rocks[Frame * 3 + Count], ERRTerrainDamageSource::Drone) ? 1 : 0;
		}
		if (Frame == 10 || Frame == 20 || Frame == 35)
		{
			// The goal changes cell while openings keep coming: rebuild and relaxation interleave.
			GoalMoves += MoveMinerTo(Setup, Setup.Miner->GetActorLocation() + FVector(CellSize, (Frame == 20 ? CellSize : 0.f), 0.f)) ? 1 : 0;
		}
		TestWorld.Tick();
	}
	const int32 SettleFrames = TickUntilSettled(TestWorld, Setup.FlowField, 5000);
	AddInfo(FString::Printf(TEXT("%d rocks destroyed, %d goal moves, settled %d frames after the last opening"), Destroyed, GoalMoves, SettleFrames));
	TestEqual(TEXT("150 rocks destroyed"), Destroyed, 150);
	TestTrue(TEXT("the goal moved"), GoalMoves >= 2);
	TestFalse(TEXT("settled"), Setup.FlowField->IsUpdatePending());
	TestEqual(TEXT("every opening applied incrementally"), Setup.FlowField->GetOpenedCellCount() - OpenedBefore, Destroyed);
	TestTrue(TEXT("only goal rebuilds, no fallback rebuild"), Setup.FlowField->GetComputeCount() - ComputesBefore <= GoalMoves);
	CheckMatchesFullRebuild(*this, Setup);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldBudgetTest, "RogueRigs.FlowField.BudgetAt67uu",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldBudgetTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	// 67 uu cells: arena_m1 subdivided by 3 (240 x 240), the size task-029 moves to.
	if (!TestTrue(TEXT("grid subdivided"), RRGridStress::SubdivideGrid(Setup.Grid, 3)))
	{
		return false;
	}
	TestWorld.Tick();
	TestEqual(TEXT("grid is 240 x 240"), Setup.Grid->GetGridSize(), FIntPoint(240, 240));
	const float BudgetMs = GetBudgetMs();
	TestTrue(TEXT("default budget set"), BudgetMs > 0.f);

	// Reference: synchronous full rebuild at this size.
	float RebuildSum = 0.f;
	constexpr int32 RebuildRuns = 5;
	for (int32 Run = 0; Run < RebuildRuns; ++Run)
	{
		RebuildSum += RRGridStress::ForceFlowFieldCompute(TestWorld.Get());
	}

	// 6 s: the miner walks a circle of 800 uu at 600 uu/s (teleport, walkable cells only) while the rock nearest its start
	// is dug every frame (60 cells/s of 67 uu, the stress dig rate is 72).
	const FVector Center = Setup.Miner->GetActorLocation();
	const TArray<FIntPoint> Rocks = ListRockCells(Setup.Grid, Setup.Grid->GetCellAtWorld(Center), /*bSortFromCenter*/ true, 0);
	constexpr int32 Frames = 360;
	constexpr float Radius = 800.f;
	constexpr float Speed = 600.f;
	TArray<float> WorkMs;
	int32 GoalLagFrames = 0;
	int32 MaxGoalAge = 0;
	float MaxGoalOffset = 0.f;
	TArray<FIntPoint> MinerCells;
	int32 GoalCellChanges = 0;
	FIntPoint LastMinerCell = Setup.Grid->GetCellAtWorld(Center);
	const int32 ComputesBefore = Setup.FlowField->GetComputeCount();
	for (int32 Frame = 0; Frame < Frames; ++Frame)
	{
		const float Angle = Frame * FrameSeconds * Speed / Radius;
		MoveMinerTo(Setup, FVector(Center.X + Radius * FMath::Cos(Angle), Center.Y + Radius * FMath::Sin(Angle), Center.Z));
		if (Rocks.IsValidIndex(Frame))
		{
			Setup.Grid->DestroyCell(Rocks[Frame], ERRTerrainDamageSource::Drone);
		}
		const int32 WorkFramesBefore = Setup.FlowField->GetWorkFrameCount();
		TestWorld.Tick();
		if (Setup.FlowField->GetWorkFrameCount() != WorkFramesBefore)
		{
			WorkMs.Add(Setup.FlowField->GetLastFrameWorkMilliseconds());
		}
		const FIntPoint MinerCellNow = Setup.Grid->GetCellAtWorld(Setup.Miner->GetActorLocation());
		GoalCellChanges += MinerCellNow != LastMinerCell ? 1 : 0;
		LastMinerCell = MinerCellNow;
		// Goal age: frames since the miner last stood in the cell the field leads to (0 when it is there now).
		MinerCells.Add(MinerCellNow);
		const FIntPoint FieldGoal = Setup.FlowField->GetGoalCell();
		int32 Age = 0;
		while (Age < MinerCells.Num() && MinerCells[MinerCells.Num() - 1 - Age] != FieldGoal)
		{
			++Age;
		}
		GoalLagFrames += Age > 0 ? 1 : 0;
		MaxGoalAge = FMath::Max(MaxGoalAge, Age);
		MaxGoalOffset = FMath::Max(MaxGoalOffset, FVector::Dist2D(Setup.Miner->GetActorLocation(), Setup.Grid->GetCellCenter(FieldGoal.X, FieldGoal.Y)));
	}
	WorkMs.Sort();
	float WorkSum = 0.f;
	for (const float Ms : WorkMs)
	{
		WorkSum += Ms;
	}
	const float P95 = WorkMs.IsEmpty() ? 0.f : WorkMs[FMath::Min(WorkMs.Num() - 1, FMath::FloorToInt32(WorkMs.Num() * 0.95f))];
	const float Max = WorkMs.IsEmpty() ? 0.f : WorkMs.Last();
	const int32 GoalRebuilds = Setup.FlowField->GetComputeCount() - ComputesBefore;
	const FString Summary = FString::Printf(
		TEXT("RogueRigs.FlowField.BudgetAt67uu: 240 x 240, budget %.2f ms; full rebuild %.3f ms; walk 600 uu/s + dig 60 cells/s over %d frames: ")
		TEXT("%d work frames avg %.3f ms, p95 %.3f ms, max %.3f ms; %d goal cell changes, %d goal rebuilds, field goal behind the miner on %d frames, goal age max %d frames, miner up to %.0f uu from the goal cell center (teleports skip rock)"),
		BudgetMs, RebuildSum / RebuildRuns, Frames, WorkMs.Num(), WorkMs.IsEmpty() ? 0.f : WorkSum / WorkMs.Num(), P95, Max, GoalCellChanges,
		GoalRebuilds, GoalLagFrames, MaxGoalAge, MaxGoalOffset);
	AddInfo(Summary);
	UE_LOG(LogRogueRigs, Display, TEXT("%s"), *Summary);
	TestTrue(TEXT("work done on most frames"), WorkMs.Num() > Frames / 2);
	TestTrue(TEXT("p95 of the per-frame work within the budget (+0.15 ms clock granularity and noise)"), P95 <= BudgetMs + 0.15f);
	TestTrue(TEXT("the goal rebuilds keep up (field goal at most 30 frames old)"), MaxGoalAge <= 30);
	TestTrue(TEXT("goal rebuilds completed"), GoalRebuilds > 0);

	const int32 SettleFrames = TickUntilSettled(TestWorld, Setup.FlowField, 600);
	AddInfo(FString::Printf(TEXT("settled %d frames after the run"), SettleFrames));
	TestFalse(TEXT("settled"), Setup.FlowField->IsUpdatePending());
	CheckMatchesFullRebuild(*this, Setup);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRFlowFieldGapTest, "RogueRigs.FlowField.RobotPathsThroughNewGap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRFlowFieldGapTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld;
	FFlowFieldSetup Setup;
	if (!SetUp(*this, TestWorld, Setup))
	{
		return false;
	}
	// A robot shut in the pocket: no path until its rock opens.
	const float HalfHeight = ACharacter::StaticClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ACharacter* Robot = TestWorld.Spawn<ACharacter>(ACharacter::StaticClass(),
		Setup.Grid->GetCellCenter(PocketCell.X, PocketCell.Y) + FVector(0.f, 0.f, HalfHeight + 2.f));
	if (!TestNotNull(TEXT("robot spawned"), Robot))
	{
		return false;
	}
	Robot->GetCharacterMovement()->bRunPhysicsWithNoController = true;
	Robot->GetCharacterMovement()->SetDefaultMovementMode();
	URREnemyMovementComponent* Movement = NewObject<URREnemyMovementComponent>(Robot);
	Movement->RegisterComponent();
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestFalse(TEXT("pocket unreachable before"), Setup.FlowField->IsReachable(Robot->GetActorLocation()));
	const FVector StartLocation = Robot->GetActorLocation();

	TestTrue(TEXT("pocket rock destroyed"), Setup.Grid->DestroyCell(PocketRockCell, ERRTerrainDamageSource::Drone));
	constexpr int32 MaxFieldFrames = 3;
	int32 ReachableFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= MaxFieldFrames && ReachableFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		ReachableFrame = Setup.FlowField->IsReachable(Robot->GetActorLocation()) ? Frame : INDEX_NONE;
	}
	TestTrue(FString::Printf(TEXT("path through the gap within %d frames"), MaxFieldFrames), ReachableFrame != INDEX_NONE);
	TestEqual(TEXT("no full rebuild needed"), Setup.FlowField->GetGoalCell(), MinerCell);

	int32 ArrivalFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 900 && ArrivalFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		ArrivalFrame = Movement->HasArrived() ? Frame : INDEX_NONE;
	}
	AddInfo(FString::Printf(TEXT("path ready %d frame(s) after the opening; robot left the pocket and arrived at frame %d, %.0f uu from its start"),
		ReachableFrame, ArrivalFrame, FVector::Dist2D(StartLocation, Robot->GetActorLocation())));
	TestTrue(TEXT("robot reaches the miner through the gap within 15 s"), ArrivalFrame != INDEX_NONE);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
