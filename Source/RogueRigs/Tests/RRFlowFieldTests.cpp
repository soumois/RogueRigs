// Rogue Rigs - automation tests of the flow field and the generic enemy movement (RogueRigs.FlowField.*): paths, pockets, recompute triggers, BFS time.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "FlowField/RRFlowFieldSubsystem.h"
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

	// Two destructions in one frame: one recompute.
	TestTrue(TEXT("pocket rock destroyed"), Setup.Grid->DestroyCell(PocketRockCell, ERRTerrainDamageSource::Drone));
	TestTrue(TEXT("(43, 38) destroyed"), Setup.Grid->DestroyCell(RockCell, ERRTerrainDamageSource::Drone));
	TestWorld.Tick();
	TestEqual(TEXT("one recompute for two destructions in a frame"), Setup.FlowField->GetComputeCount(), CountBefore + 1);

	TestTrue(TEXT("pocket reachable after"), Setup.FlowField->IsReachable(PocketCenter));
	TestTrue(TEXT("direct path through the opened cell"), Setup.FlowField->HasDirectPath(PocketCenter, Setup.Grid->GetCellCenter(PocketSouthCell.X, PocketSouthCell.Y)));
	TestEqual(TEXT("following from the pocket reaches the miner cell"), FollowField(*this, Setup, PocketCell), MinerCell);

	TestWorld.Tick();
	TestEqual(TEXT("no recompute without a trigger"), Setup.FlowField->GetComputeCount(), CountBefore + 1);
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
	TestWorld.Tick();

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

#endif // WITH_DEV_AUTOMATION_TESTS
