// Rogue Rigs - automation tests of the extraction shuttle (RogueRigs.Extraction.*: landing distance, countdown loss, boarding win, no landing while the console is pending, end screen over the level-up offer).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Drone/RRDrone.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/WorldSettings.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRLandingFootprint.h"
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
	/** Walkable start cell of the miner on arena_m1_fine (the run flow tests use it too). */
	const FIntPoint MinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

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
		Out.Grid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
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
	TestEqual(TEXT("shuttle 37 cells min"), Defaults.ShuttleMinDistanceCells, 37.f);
	TestEqual(TEXT("shuttle 50 cells max"), Defaults.ShuttleMaxDistanceCells, 50.f);
	TestEqual(TEXT("countdown 45 s"), Defaults.ShuttleCountdownSeconds, 45.f);
	// task-377 Q2: board zone 1608 x 1608 x 400 uu on the native shuttle, bottom on the floor, query only, Pawn overlap only, hidden in game.
	const UBoxComponent* DefaultZone = GetDefault<ARRShuttle>()->GetBoardZone();
	if (TestNotNull(TEXT("board zone"), DefaultZone))
	{
		TestTrue(TEXT("board zone extent 804 x 804 x 200"), DefaultZone->GetUnscaledBoxExtent().Equals(FVector(804.f, 804.f, 200.f), 0.01f));
		TestTrue(TEXT("board zone bottom on the floor"), DefaultZone->GetRelativeLocation().Equals(FVector(0.f, 0.f, 200.f), 0.01f));
		TestTrue(TEXT("board zone query only"), DefaultZone->GetCollisionEnabled() == ECollisionEnabled::QueryOnly);
		TestTrue(TEXT("board zone overlaps pawns"), DefaultZone->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Overlap);
		TestTrue(TEXT("board zone ignores the world"), DefaultZone->GetCollisionResponseToChannel(ECC_WorldStatic) == ECR_Ignore);
		TestTrue(TEXT("board zone overlap events"), DefaultZone->GetGenerateOverlapEvents());
		TestTrue(TEXT("board zone hidden in game"), DefaultZone->bHiddenInGame != 0);
	}

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
		TestTrue(FString::Printf(TEXT("seed %d: 37 to 50 cells (%.1f)"), Seed, Distance), Distance >= 37.f && Distance <= 50.f);
		TestEqual(TEXT("on a floor cell"), Setup.Grid->GetCellType(Cell), ERRCellType::Floor);
		URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
		TestTrue(TEXT("reachable from the miner"), FlowField && FlowField->GetCellDistance(Cell) >= 0);
		TestTrue(TEXT("at the footprint centre"), Shuttle->GetActorLocation().Equals(RRLandingFootprint::GetCenter(Setup.Grid, Cell, Shuttle->GetFootprintCells()), 1.f));
		TArray<FIntPoint> FootprintCells;
		RRLandingFootprint::GetCells(Cell, Shuttle->GetFootprintCells(), FootprintCells);
		TestEqual(TEXT("16 x 12 footprint (task-375)"), FootprintCells.Num(), 192);
		for (const FIntPoint& FootprintCell : FootprintCells)
		{
			TestTrue(FString::Printf(TEXT("footprint cell (%d, %d) is reachable floor"), FootprintCell.X, FootprintCell.Y),
				Setup.Grid->GetCellType(FootprintCell) == ERRCellType::Floor && FlowField && FlowField->GetCellDistance(FootprintCell) >= 0);
		}
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

	// The miner capsule enters the board zone (begin overlap, task-377).
	const float HalfHeight = Setup.Miner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	Setup.Miner->SetActorLocation(Shuttle->GetActorLocation() + FVector(50.f, 0.f, HalfHeight + 2.f), false, nullptr, ETeleportType::TeleportPhysics);
	TestWorld.Tick();
	TestTrue(TEXT("run ended on boarding"), Setup.RunFlow->IsRunEnded());
	TestTrue(TEXT("run won"), Setup.RunFlow->WasRunSuccessful());
	TestTrue(TEXT("miner extracted"), Shuttle->WasExtracted());
	TestTrue(TEXT("countdown not over"), Shuttle->GetRemainingSeconds() > 40.f);
	TestTrue(TEXT("takeoff started on the extraction"), Shuttle->IsTakingOff());
	TestTrue(TEXT("miner hidden"), Setup.Miner->IsHidden() && Setup.Miner->IsHiddenForExtraction());
	TestTrue(TEXT("drone hidden"), !Setup.Miner->GetDrone() || Setup.Miner->GetDrone()->IsHidden());
	TestFalse(TEXT("miner collision off"), Setup.Miner->GetActorEnableCollision());
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
	Progression->AddXP(Progression->GetXPToNextLevel() - Progression->GetXP());
	TickFrames(TestWorld, 60);
	TestTrue(TEXT("card screen open"), LevelUp->IsAwaitingChoice());
	TestTrue(TEXT("run ends"), Setup.RunFlow->EndRun(true));
	TestFalse(TEXT("card screen closed by the end"), LevelUp->IsAwaitingChoice());
	TestEqual(TEXT("offer closed once"), Recorder->GetOfferClosedCount(), 1);
	TestTrue(TEXT("choices blocked"), LevelUp->AreChoicesBlocked());

	// A later level-up opens no screen.
	Progression->AddXP(Progression->GetXPToNextLevel() - Progression->GetXP());
	TickFrames(TestWorld, 60);
	TestFalse(TEXT("no card screen after the end"), LevelUp->IsAwaitingChoice() || LevelUp->IsChoiceDelayActive());
	TestEqual(TEXT("one run end event"), Recorder->GetRunEndedResults().Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExtractionTakeoffTest, "RogueRigs.Extraction.Takeoff",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExtractionTakeoffTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRExtractionTests;

	FTestWorld TestWorld(TEXT("RRExtractionTakeoffWorld"));
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
	const TWeakObjectPtr<ARRShuttle> WeakShuttle(Shuttle);
	const FVector Start = Shuttle->GetActorLocation();
	Shuttle->StartTakeoff();
	TestTrue(TEXT("taking off"), Shuttle->IsTakingOff());

	// Spin-up 0.5 s: still on the ground; end of the rise (2.0 s): 400 uu up, level; then the nose goes down and it flies forward.
	TickFrames(TestWorld, 15);
	TestTrue(TEXT("still on the ground during the spin-up"), Shuttle->GetActorLocation().Equals(Start, 1.f));
	TickFrames(TestWorld, 105);
	TestTrue(TEXT("risen 400 uu"), FMath::IsNearlyEqual(Shuttle->GetActorLocation().Z - Start.Z, 400.f, 5.f));
	TickFrames(TestWorld, 60);
	TestTrue(TEXT("nose down"), Shuttle->GetActorRotation().Pitch < -19.f);
	TestTrue(TEXT("flying forward"), Shuttle->GetActorLocation().X - Start.X > 100.f);

	// No viewport in a test world: removed by the safety time.
	TickFrames(TestWorld, 700);
	TestFalse(TEXT("shuttle removed"), WeakShuttle.IsValid());
	return true;
}

namespace RRExtractionTests
{
	/**
	 * Ticks Frames frames and returns, per rotor (FL, FR, RL, RR), the yaw turned in revolutions (positive = clockwise seen from above),
	 * unwrapped frame by frame from the rotor relative rotation.
	 */
	TArray<float> SampleRotorRevolutions(const RRTestHarness::FTestWorld& TestWorld, const ARRShuttle* Shuttle, int32 Frames)
	{
		const TArray<UStaticMeshComponent*> Rotors = Shuttle->GetRotors();
		TArray<float> Previous;
		TArray<float> Turned;
		for (const UStaticMeshComponent* Rotor : Rotors)
		{
			Previous.Add(Rotor ? Rotor->GetRelativeRotation().Yaw : 0.f);
			Turned.Add(0.f);
		}
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
			for (int32 Index = 0; Index < Rotors.Num(); ++Index)
			{
				const float Yaw = Rotors[Index] ? Rotors[Index]->GetRelativeRotation().Yaw : 0.f;
				Turned[Index] += FMath::FindDeltaAngleDegrees(Previous[Index], Yaw) / 360.f;
				Previous[Index] = Yaw;
			}
		}
		return Turned;
	}

	/** Checks the four rotors turned Expected revolutions (within 5 %) in the Q6 directions: FL and RR clockwise, FR and RL counter-clockwise. */
	void TestRotorRevolutions(FAutomationTestBase& Test, const TCHAR* Phase, const TArray<float>& Turned, float Expected)
	{
		static const TCHAR* Names[4] = { TEXT("FL"), TEXT("FR"), TEXT("RL"), TEXT("RR") };
		static const float Signs[4] = { 1.f, -1.f, -1.f, 1.f };
		for (int32 Index = 0; Index < 4 && Index < Turned.Num(); ++Index)
		{
			const float Signed = Turned[Index] * Signs[Index];
			UE_LOG(LogTemp, Display, TEXT("RotorSpin %s %s: %.3f rev in 1 s (expected %.1f, %s)."), Phase, Names[Index], Turned[Index], Expected * Signs[Index],
				Signs[Index] > 0.f ? TEXT("clockwise") : TEXT("counter-clockwise"));
			Test.TestTrue(FString::Printf(TEXT("%s: rotor %s turns %.1f rev/s %s within 5 %% (%.3f)"), Phase, Names[Index], Expected,
				Signs[Index] > 0.f ? TEXT("clockwise") : TEXT("counter-clockwise"), Turned[Index]), FMath::Abs(Signed - Expected) <= 0.05f * Expected);
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRExtractionRotorSpinTest, "RogueRigs.Extraction.RotorSpin",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRExtractionRotorSpinTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRExtractionTests;

	FTestWorld TestWorld(TEXT("RRExtractionRotorSpinWorld"));
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
	const TWeakObjectPtr<ARRShuttle> WeakShuttle(Shuttle);
	TestNearlyEqual(TEXT("ground speed 3 rev/s"), Shuttle->GetRotorRevPerSecond(), 3.f, 1e-4f);
	TestNearlyEqual(TEXT("ground alpha 0.6"), Shuttle->GetRotorSpeedAlpha(), 0.6f, 1e-4f);

	// Ground: 1 s.
	TickFrames(TestWorld, 5);
	TestRotorRevolutions(*this, TEXT("ground"), SampleRotorRevolutions(TestWorld, Shuttle, 60), 3.f);

	// Takeoff: 0.5 s ramp to 5 rev/s (half-way after 15 frames), then 1 s in flight.
	Shuttle->StartTakeoff();
	TickFrames(TestWorld, 15);
	TestTrue(FString::Printf(TEXT("ramp half-way (%.2f rev/s)"), Shuttle->GetRotorRevPerSecond()), FMath::IsNearlyEqual(Shuttle->GetRotorRevPerSecond(), 4.f, 0.1f));
	TickFrames(TestWorld, 20);
	TestNearlyEqual(TEXT("flight speed 5 rev/s after the ramp"), Shuttle->GetRotorRevPerSecond(), 5.f, 1e-4f);
	TestNearlyEqual(TEXT("flight alpha 1"), Shuttle->GetRotorSpeedAlpha(), 1.f, 1e-4f);
	TestRotorRevolutions(*this, TEXT("flight"), SampleRotorRevolutions(TestWorld, Shuttle, 60), 5.f);

	// The end screen pauses the world: the rotors keep turning (1 s) and the takeoff goes on.
	APlayerState* Pauser = TestWorld.Get()->SpawnActor<APlayerState>();
	if (!TestNotNull(TEXT("pauser"), Pauser))
	{
		return false;
	}
	TestWorld.Get()->GetWorldSettings()->SetPauserPlayerState(Pauser);
	TestTrue(TEXT("world paused"), TestWorld.Get()->IsPaused());
	const FVector BeforePause = Shuttle->GetActorLocation();
	TestRotorRevolutions(*this, TEXT("paused"), SampleRotorRevolutions(TestWorld, Shuttle, 60), 5.f);
	TestTrue(TEXT("still flying while paused"), !Shuttle->GetActorLocation().Equals(BeforePause, 1.f));
	TestWorld.Get()->GetWorldSettings()->SetPauserPlayerState(nullptr);

	TickFrames(TestWorld, 700);
	TestFalse(TEXT("shuttle removed"), WeakShuttle.IsValid());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
