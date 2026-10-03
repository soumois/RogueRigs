// Rogue Rigs - automation tests of the extraction shuttle (RogueRigs.Extraction.*: landing distance, countdown loss, boarding win, no landing while the console is pending, end screen over the level-up offer).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/CapsuleComponent.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRExtractionTests
{
	/** Walkable start cell of the miner (the run flow tests use it too). */
	const FIntPoint MinerCell(40, 40);

	/** Grid, miner (weapons stopped) and run flow of a test world. */
	struct FExtractionSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		URRRunFlowSubsystem* RunFlow = nullptr;
	};

	/** Spawns the grid and the miner, stops its weapons, sets Config and Seed on the run flow and binds Recorder to its run events. */
	bool SetUp(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, URRTestEventRecorder* Recorder, FExtractionSetup& Out,
		const FRRRunFlowConfig& Config = FRRRunFlowConfig(), int32 Seed = 20261003)
	{
		Out.Grid = RRTestHarness::SpawnGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Out.Grid, MinerCell) : nullptr;
		Out.RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
		if (!Out.Miner || !Test.TestNotNull(TEXT("run flow subsystem"), Out.RunFlow))
		{
			return false;
		}
		for (ARRWeapon* Weapon : Out.Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		Out.RunFlow->SetConfig(Config);
		Out.RunFlow->SetRandomSeed(Seed);
		Out.RunFlow->OnExtractionReady.AddDynamic(Recorder, &URRTestEventRecorder::HandleExtractionReady);
		Out.RunFlow->OnRunEnded.AddDynamic(Recorder, &URRTestEventRecorder::HandleRunEnded);
		Out.RunFlow->OnShuttleCountdown.AddDynamic(Recorder, &URRTestEventRecorder::HandleShuttleCountdown);
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExtractionLandingTest, "RogueRigs.Extraction.LandingDistance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExtractionLandingTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRExtractionTests;

	const FRRRunFlowConfig Defaults;
	TestEqual(TEXT("shuttle 25 cells min"), Defaults.ShuttleMinDistanceCells, 25.f);
	TestEqual(TEXT("countdown 45 s"), Defaults.ShuttleCountdownSeconds, 45.f);

	// Several seeds: every landing respects the distance and reachability rules.
	for (int32 Seed : { 1, 20261003, 777 })
	{
		FTestWorld TestWorld(TEXT("RRExtractionLandingWorld"));
		TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
		FExtractionSetup Setup;
		if (!SetUp(*this, TestWorld, Recorder.Get(), Setup, Defaults, Seed))
		{
			return false;
		}
		TestNull(TEXT("no shuttle before extraction"), Setup.RunFlow->GetShuttle());
		TestTrue(TEXT("skip to extraction"), Setup.RunFlow->SkipToExtraction());
		ARRShuttle* Shuttle = Setup.RunFlow->GetShuttle();
		if (!TestNotNull(TEXT("shuttle landed with the extraction event"), Shuttle))
		{
			return false;
		}
		const FIntPoint Cell = Shuttle->GetCell();
		const FIntPoint From = Setup.Grid->GetCellAtWorld(Setup.Miner->GetActorLocation());
		const float Distance = FMath::Sqrt(static_cast<float>(FMath::Square(Cell.X - From.X) + FMath::Square(Cell.Y - From.Y)));
		TestTrue(FString::Printf(TEXT("seed %d: at least 25 cells (%.1f)"), Seed, Distance), Distance >= 25.f);
		TestEqual(TEXT("on a floor cell"), Setup.Grid->GetCellType(Cell), ERRCellType::Floor);
		URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
		TestTrue(TEXT("reachable from the miner"), FlowField && FlowField->GetCellDistance(Cell) >= 0);
		TestTrue(TEXT("on the cell center"), Shuttle->GetActorLocation().Equals(Setup.Grid->GetCellCenter(Cell.X, Cell.Y), 1.f));
		TestEqual(TEXT("countdown 45 s"), Shuttle->GetRemainingSeconds(), 45.f);
		TestEqual(TEXT("extraction event once"), Recorder->GetExtractionReadyCount(), 1);
		TestTrue(TEXT("countdown pushed at landing"), Recorder->GetShuttleCountdownCount() >= 1);

		// One second later: 44 s left, still the same shuttle, the run goes on.
		TickFrames(TestWorld, 60);
		TestNearlyEqual(TEXT("44 s left after 60 frames"), Shuttle->GetRemainingSeconds(), 44.f, 1e-3f);
		TestNearlyEqual(TEXT("HUD countdown follows"), Recorder->GetLastShuttleCountdown(), 44.f, 1e-3f);
		TestEqual(TEXT("one shuttle"), Setup.RunFlow->GetShuttle(), Shuttle);
		TestFalse(TEXT("run not ended"), Setup.RunFlow->IsRunEnded());
		TestEqual(TEXT("phase extraction ready"), Setup.RunFlow->GetPhase(), ERRRunPhase::ExtractionReady);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExtractionCountdownLossTest, "RogueRigs.Extraction.CountdownLoss",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExtractionCountdownLossTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRExtractionTests;

	FTestWorld TestWorld(TEXT("RRExtractionLossWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FRRRunFlowConfig Config;
	Config.ShuttleCountdownSeconds = 1.f;
	FExtractionSetup Setup;
	if (!SetUp(*this, TestWorld, Recorder.Get(), Setup, Config))
	{
		return false;
	}
	Setup.RunFlow->SkipToExtraction();
	ARRShuttle* Shuttle = Setup.RunFlow->GetShuttle();
	if (!TestNotNull(TEXT("shuttle"), Shuttle))
	{
		return false;
	}

	TickFrames(TestWorld, 50);
	TestFalse(TEXT("not over before 1 s"), Setup.RunFlow->IsRunEnded());
	TickFrames(TestWorld, 20);
	TestTrue(TEXT("run ended after the countdown"), Setup.RunFlow->IsRunEnded());
	TestFalse(TEXT("run lost"), Setup.RunFlow->WasRunSuccessful());
	TestTrue(TEXT("shuttle finished"), Shuttle->IsFinished());
	TestFalse(TEXT("miner not extracted"), Shuttle->WasExtracted());
	TestEqual(TEXT("phase ended"), Setup.RunFlow->GetPhase(), ERRRunPhase::Ended);

	TickFrames(TestWorld, 30);
	TestEqual(TEXT("one run end event"), Recorder->GetRunEndedResults().Num(), 1);
	if (Recorder->GetRunEndedResults().Num() == 1)
	{
		TestFalse(TEXT("event says lost"), Recorder->GetRunEndedResults()[0]);
	}
	TestFalse(TEXT("a second end is refused"), Setup.RunFlow->EndRun(true));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExtractionBoardWinTest, "RogueRigs.Extraction.OverlapWin",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExtractionBoardWinTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRExtractionTests;

	FTestWorld TestWorld(TEXT("RRExtractionWinWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FExtractionSetup Setup;
	if (!SetUp(*this, TestWorld, Recorder.Get(), Setup))
	{
		return false;
	}
	Setup.RunFlow->SkipToExtraction();
	ARRShuttle* Shuttle = Setup.RunFlow->GetShuttle();
	if (!TestNotNull(TEXT("shuttle"), Shuttle))
	{
		return false;
	}
	TickFrames(TestWorld, 10);
	TestFalse(TEXT("far from the shuttle: no win"), Setup.RunFlow->IsRunEnded());

	// The miner reaches the shuttle cell.
	const float HalfHeight = Setup.Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	Setup.Miner->SetActorLocation(Shuttle->GetActorLocation() + FVector(50.f, 0.f, HalfHeight + 2.f), false, nullptr, ETeleportType::TeleportPhysics);
	TestWorld.Tick();
	TestTrue(TEXT("run ended on boarding"), Setup.RunFlow->IsRunEnded());
	TestTrue(TEXT("run won"), Setup.RunFlow->WasRunSuccessful());
	TestTrue(TEXT("miner extracted"), Shuttle->WasExtracted());
	TestTrue(TEXT("countdown not over"), Shuttle->GetRemainingSeconds() > 40.f);
	TestEqual(TEXT("one run end event"), Recorder->GetRunEndedResults().Num(), 1);
	if (Recorder->GetRunEndedResults().Num() == 1)
	{
		TestTrue(TEXT("event says won"), Recorder->GetRunEndedResults()[0]);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExtractionWaitsForConsoleTest, "RogueRigs.Extraction.NoLandingWhileConsolePending",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExtractionWaitsForConsoleTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRExtractionTests;

	FTestWorld TestWorld(TEXT("RRExtractionConsoleWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FExtractionSetup Setup;
	if (!SetUp(*this, TestWorld, Recorder.Get(), Setup))
	{
		return false;
	}
	ARRConsole* Console = Setup.RunFlow->SkipToConsole();
	if (!TestNotNull(TEXT("console dropped"), Console))
	{
		return false;
	}
	Setup.RunFlow->SetAlert(1.f);
	TickFrames(TestWorld, 30);
	TestNull(TEXT("no shuttle while the console is pending"), Setup.RunFlow->GetShuttle());
	TestEqual(TEXT("no countdown"), Recorder->GetShuttleCountdownCount(), 0);

	Console->ForceComplete();
	TestWorld.Tick();
	TestNotNull(TEXT("shuttle once the console is hacked"), Setup.RunFlow->GetShuttle());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExtractionEndClosesOfferTest, "RogueRigs.Extraction.EndClosesLevelUpOffer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExtractionEndClosesOfferTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRExtractionTests;

	FTestWorld TestWorld(TEXT("RRExtractionOfferWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FExtractionSetup Setup;
	if (!SetUp(*this, TestWorld, Recorder.Get(), Setup))
	{
		return false;
	}
	URRProgressionComponent* Progression = Setup.Miner->FindComponentByClass<URRProgressionComponent>();
	URRLevelUpComponent* LevelUp = Setup.Miner->FindComponentByClass<URRLevelUpComponent>();
	if (!TestNotNull(TEXT("progression"), Progression) || !TestNotNull(TEXT("level-up flow"), LevelUp))
	{
		return false;
	}
	LevelUp->OnUpgradeOfferClosed.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleUpgradeOfferClosed);

	// An open card screen and a second level-up queued when the run ends.
	Progression->AddXP(5.f);
	TickFrames(TestWorld, 60);
	TestTrue(TEXT("card screen open"), LevelUp->IsAwaitingChoice());
	TestTrue(TEXT("run ends"), Setup.RunFlow->EndRun(true));
	TestFalse(TEXT("card screen closed by the end"), LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("offer closed once"), Recorder->GetOfferClosedCount(), 1);
	TestTrue(TEXT("choices blocked"), LevelUp->AreChoicesBlocked());

	// A later level-up opens no screen.
	Progression->AddXP(50.f);
	TickFrames(TestWorld, 60);
	TestFalse(TEXT("no card screen after the end"), LevelUp->IsAwaitingChoice() || LevelUp->IsChoiceDelayActive());
	TestEqual(TEXT("one run end event"), Recorder->GetRunEndedResults().Num(), 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
