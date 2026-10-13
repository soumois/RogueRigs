// Rogue Rigs - automation tests of the hack zone ground telegraph of the console and the bonus chest (RogueRigs.Console.ZoneTelegraph*).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Objectives/RRBonusChest.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRMiner.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRConsoleTelegraphTests
{
	/** Miner start cell on arena_m1_fine (same as the RunFlow tests). */
	const FIntPoint MinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Console cell with marked rocks around it, 8.49 cells from MinerCell (inside the 9-cell console zone). */
	const FIntPoint ConsoleCell = RRTestHarness::FineCell(FIntPoint(42, 38));

	/** Spawns the fine grid and the miner on MinerCell (weapons stopped) and turns the run flow off. */
	bool SetUp(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner)
	{
		OutGrid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
		OutMiner = OutGrid ? RRTestHarness::SpawnMiner(Test, TestWorld, OutGrid, MinerCell) : nullptr;
		if (!OutMiner)
		{
			return false;
		}
		for (ARRWeapon* Weapon : OutMiner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		if (URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>())
		{
			RunFlow->SetRunFlowEnabled(false);
		}
		return true;
	}

	/** Ticks Frames frames. */
	void TickFrames(const RRTestHarness::FTestWorld& TestWorld, int32 Frames)
	{
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRConsoleZoneTelegraphTest, "RogueRigs.Console.ZoneTelegraph",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRConsoleZoneTelegraphTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRConsoleTelegraphTests;

	FTestWorld TestWorld(TEXT("RRConsoleZoneTelegraphWorld"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!SetUp(*this, TestWorld, Grid, Miner))
	{
		return false;
	}

	ARRConsole* Console = TestWorld.Spawn<ARRConsole>(ARRConsole::StaticClass(), Grid->GetCellCenter(ConsoleCell.X, ConsoleCell.Y));
	if (!TestNotNull(TEXT("console spawned"), Console))
	{
		return false;
	}
	URRGroundTelegraphComponent* Telegraph = Console->GetGroundTelegraph();
	if (!TestNotNull(TEXT("console has a ground telegraph"), Telegraph))
	{
		return false;
	}
	TestFalse(TEXT("hidden before the landing"), Telegraph->IsShown());

	Console->InitializeConsole(Grid, Miner, ConsoleCell);
	TestTrue(TEXT("shown at the landing"), Telegraph->IsShown());
	TestEqual(TEXT("radius = zone radius"), Telegraph->GetRadius(), Console->GetZoneRadius());
	TestNearlyEqual(TEXT("radius = 9 cells x cell size"), Telegraph->GetRadius(), 9.f * Grid->GetCellSize(), 0.01f);
	TestEqual(TEXT("progress 0"), Telegraph->GetProgress(), 0.f);
	const TArray<FIntPoint> Marked = Console->GetMarkedRocks();
	if (!TestTrue(TEXT("rocks marked"), Marked.Num() > 1))
	{
		return false;
	}
	TestEqual(TEXT("ZoneBlocked while marked rocks remain"), Telegraph->GetMode(), ERRGroundTelegraphMode::ZoneBlocked);

	// All rocks but one destroyed: still blocked.
	for (int32 Index = 0; Index < Marked.Num() - 1; ++Index)
	{
		Grid->DestroyCell(Marked[Index], ERRTerrainDamageSource::Drone);
	}
	TestWorld.Tick();
	TestEqual(TEXT("one rock left"), Console->GetMarkedRockCount(), 1);
	TestEqual(TEXT("still ZoneBlocked"), Telegraph->GetMode(), ERRGroundTelegraphMode::ZoneBlocked);

	// Last rock: active, the hack starts.
	Grid->DestroyCell(Marked.Last(), ERRTerrainDamageSource::Drone);
	TestWorld.Tick();
	TestEqual(TEXT("hacking"), Console->GetConsoleState(), ERRConsoleState::Hacking);
	TestEqual(TEXT("ZoneActive once the last marked rock is gone"), Telegraph->GetMode(), ERRGroundTelegraphMode::ZoneActive);

	// Progress follows the hack.
	TickFrames(TestWorld, 479);
	TestNearlyEqual(TEXT("half after 8 s"), Console->GetHackProgress(), 0.5f, 0.002f);
	TestEqual(TEXT("telegraph progress = hack progress"), Telegraph->GetProgress(), Console->GetHackProgress());

	// Miner out of the zone: the progress circle shrinks with the drain.
	const FVector InZone = Miner->GetActorLocation();
	Miner->SetActorLocation(InZone + FVector(0.f, 8.f * Grid->GetCellSize(), 0.f));
	TickFrames(TestWorld, 60);
	TestEqual(TEXT("idle"), Console->GetConsoleState(), ERRConsoleState::Idle);
	TestNearlyEqual(TEXT("drained 5 % in 1 s"), Console->GetHackProgress(), 0.45f, 0.002f);
	TestEqual(TEXT("telegraph progress drains with the hack"), Telegraph->GetProgress(), Console->GetHackProgress());
	TestTrue(TEXT("still shown while draining"), Telegraph->IsShown());
	TestEqual(TEXT("still ZoneActive while draining"), Telegraph->GetMode(), ERRGroundTelegraphMode::ZoneActive);

	// Back in until complete: hidden.
	Miner->SetActorLocation(InZone);
	for (int32 Frame = 0; Frame < 800 && !Console->IsComplete(); ++Frame)
	{
		TestWorld.Tick();
	}
	TestTrue(TEXT("complete"), Console->IsComplete());
	TestFalse(TEXT("hidden at completion"), Telegraph->IsShown());
	TickFrames(TestWorld, 10);
	TestFalse(TEXT("stays hidden"), Telegraph->IsShown());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRBonusChestZoneTelegraphTest, "RogueRigs.Console.ZoneTelegraphBonusChest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRBonusChestZoneTelegraphTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRConsoleTelegraphTests;

	FTestWorld TestWorld(TEXT("RRBonusChestZoneTelegraphWorld"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!SetUp(*this, TestWorld, Grid, Miner))
	{
		return false;
	}

	// Chest away from the miner first (outside its 6-cell zone).
	ARRBonusChest* Chest = TestWorld.Spawn<ARRBonusChest>(ARRBonusChest::StaticClass(), Grid->GetCellCenter(ConsoleCell.X, ConsoleCell.Y));
	if (!TestNotNull(TEXT("chest spawned"), Chest))
	{
		return false;
	}
	URRGroundTelegraphComponent* Telegraph = Chest->GetGroundTelegraph();
	if (!TestNotNull(TEXT("chest has a ground telegraph"), Telegraph))
	{
		return false;
	}
	Chest->InitializeConsole(Grid, Miner, ConsoleCell);
	TestTrue(TEXT("shown at the landing"), Telegraph->IsShown());
	TestEqual(TEXT("no marked rock"), Chest->GetMarkedRockCount(), 0);
	TestEqual(TEXT("ZoneActive at once"), Telegraph->GetMode(), ERRGroundTelegraphMode::ZoneActive);
	TestEqual(TEXT("radius = zone radius"), Telegraph->GetRadius(), Chest->GetZoneRadius());
	TestNearlyEqual(TEXT("radius = 6 cells x cell size"), Telegraph->GetRadius(), 6.f * Grid->GetCellSize(), 0.01f);

	// Miner on the chest: the progress circle grows with the hack.
	Miner->SetActorLocation(Chest->GetActorLocation() + FVector(0.f, 0.f, Miner->GetActorLocation().Z - Chest->GetActorLocation().Z));
	TickFrames(TestWorld, 120);
	TestTrue(TEXT("hack progressing"), Chest->GetHackProgress() > 0.f);
	TestEqual(TEXT("telegraph progress = hack progress"), Telegraph->GetProgress(), Chest->GetHackProgress());

	Chest->ForceComplete();
	TestFalse(TEXT("hidden at completion"), Telegraph->IsShown());

	// A second chest destroyed while shown hides its telegraph.
	ARRBonusChest* Other = TestWorld.Spawn<ARRBonusChest>(ARRBonusChest::StaticClass(), Grid->GetCellCenter(ConsoleCell.X, ConsoleCell.Y));
	if (!TestNotNull(TEXT("second chest spawned"), Other))
	{
		return false;
	}
	Other->InitializeConsole(Grid, Miner, ConsoleCell);
	URRGroundTelegraphComponent* OtherTelegraph = Other->GetGroundTelegraph();
	TestTrue(TEXT("second shown"), OtherTelegraph->IsShown());
	Other->Destroy();
	TestFalse(TEXT("hidden when destroyed"), OtherTelegraph->IsShown());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
