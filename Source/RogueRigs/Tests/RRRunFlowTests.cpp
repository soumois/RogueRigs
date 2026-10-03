// Rogue Rigs - automation tests of the run flow (RogueRigs.RunFlow.*: alert, pause, extraction) and the console objective (RogueRigs.Console.*: drop, marked rocks, hack, drain).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Drone/RRDrone.h"
#include "Enemies/RRChaser.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GAS/RRGameplayTags.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRRunFlowTests
{
	/** Walkable start cell of the miner (flow-field tests use it too). */
	const FIntPoint RunMinerCell(40, 40);

	/** Floor cell with rock (43, 38) east of it; 2.83 cells from RunMinerCell, so inside a 3-cell zone. */
	const FIntPoint RunHackConsoleCell(42, 38);

	/** Grid, miner (weapons stopped) and run flow of a test world. */
	struct FRunSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		URRRunFlowSubsystem* RunFlow = nullptr;
	};

	/** Spawns the grid and the miner on Cell, stops its weapons and sets the run flow to the default config with Seed. */
	bool SetUpRun(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, FRunSetup& Out, FIntPoint Cell = RunMinerCell, int32 Seed = 20261003)
	{
		Out.Grid = RRTestHarness::SpawnGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Out.Grid, Cell) : nullptr;
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
		Out.RunFlow->SetConfig(FRRRunFlowConfig());
		Out.RunFlow->SetRandomSeed(Seed);
		return true;
	}

	/** Ticks until the run flow has a console (at most MaxFrames). Returns the frames ticked, -1 on timeout. */
	int32 TickUntilConsole(const RRTestHarness::FTestWorld& TestWorld, const URRRunFlowSubsystem* RunFlow, int32 MaxFrames)
	{
		for (int32 Frame = 1; Frame <= MaxFrames; ++Frame)
		{
			TestWorld.Tick();
			if (RunFlow->GetConsole())
			{
				return Frame;
			}
		}
		return -1;
	}

	/** Read access to the grid's block components and instance indices. Never instantiated. */
	struct FGridAccess : public ARRArenaGrid
	{
		/** Marked custom data float of the rendering instance of the block in Cell; -1 when unreadable. */
		static float GetMarkedData(const ARRArenaGrid* Grid, FIntPoint Cell)
		{
			UHierarchicalInstancedStaticMeshComponent* Component = (Grid->*(&FGridAccess::GetBlockComponent))(Grid->GetCellType(Cell));
			const TArray<int32>& Indices = Grid->*(&FGridAccess::CellInstanceIndex);
			const int32 CellIndex = Cell.Y * Grid->GetGridSize().X + Cell.X;
			if (!Component || !Indices.IsValidIndex(CellIndex) || Indices[CellIndex] == INDEX_NONE)
			{
				return -1.f;
			}
			const int32 DataIndex = Indices[CellIndex] * Component->NumCustomDataFloats + Grid->GetMarkedCustomDataIndex();
			return Component->PerInstanceSMCustomData.IsValidIndex(DataIndex) ? Component->PerInstanceSMCustomData[DataIndex] : -1.f;
		}
	};

	/** True for the block types a console marks by default. */
	bool IsMarkableType(ERRCellType Type)
	{
		return Type == ERRCellType::Rock || Type == ERRCellType::Ore || Type == ERRCellType::HealOre || Type == ERRCellType::SpecialOre;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRRunFlowAlertTest, "RogueRigs.RunFlow.AlertFromTimeAndKills",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRRunFlowAlertTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRRunFlowTests;

	FTestWorld TestWorld(TEXT("RRRunFlowAlertWorld"));
	FRunSetup Setup;
	if (!SetUpRun(*this, TestWorld, Setup))
	{
		return false;
	}
	URRRunFlowSubsystem* RunFlow = Setup.RunFlow;
	const FRRRunFlowConfig& Config = RunFlow->GetConfig();
	TestEqual(TEXT("fill 240 s"), Config.AlertFillSeconds, 240.f);
	TestEqual(TEXT("kill 0.5 s"), Config.KillAlertSeconds, 0.5f);
	TestEqual(TEXT("console at 0.4"), Config.ConsoleAlertThreshold, 0.4f);
	TestEqual(TEXT("console 12 cells min"), Config.ConsoleMinDistanceCells, 12.f);
	TestEqual(TEXT("console 20 cells max"), Config.ConsoleMaxDistanceCells, 20.f);
	TestEqual(TEXT("alert starts at 0"), RunFlow->GetAlert(), 0.f);

	// Time alone: 60 frames = 1 s of alert.
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestNearlyEqual(TEXT("1 s after 60 frames"), RunFlow->GetAlertSeconds(), 1.f, 1e-3f);
	TestNearlyEqual(TEXT("alert = 1 / 240"), RunFlow->GetAlert(), 1.f / 240.f, 1e-5f);
	TestEqual(TEXT("filling"), RunFlow->GetPhase(), ERRRunPhase::Filling);

	// A real kill: a chaser killed by a flat hit from the miner adds 0.5 s.
	UClass* ChaserClass = ARRChaser::StaticClass();
	ARRChaser* Chaser = TestWorld.Spawn<ARRChaser>(ChaserClass, Setup.Grid->GetCellCenter(44, 41) + FVector(0.f, 0.f, 100.f));
	if (!TestNotNull(TEXT("chaser spawned"), Chaser))
	{
		return false;
	}
	Chaser->GetEnemyMovement()->SetMovementEnabled(false);
	const float BeforeKill = RunFlow->GetAlertSeconds();
	UAbilitySystemComponent* MinerASC = Setup.Miner->GetAbilitySystemComponent();
	FGameplayEffectSpecHandle Spec = MinerASC->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, MinerASC->MakeEffectContext());
	Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, 1000.f);
	MinerASC->ApplyGameplayEffectSpecToTarget(*Spec.Data, Chaser->GetAbilitySystemComponent());
	TestTrue(TEXT("chaser dead"), Chaser->IsDead());
	TestEqual(TEXT("one kill"), RunFlow->GetKillCount(), 1);
	TestNearlyEqual(TEXT("kill adds 0.5 s"), RunFlow->GetAlertSeconds() - BeforeKill, 0.5f, 1e-4f);

	// A self-instigated death (kamikaze fuse) is not a kill.
	RunFlow->NotifyEnemyKilled(Chaser, Chaser);
	TestEqual(TEXT("own fuse: no kill"), RunFlow->GetKillCount(), 1);
	TestNearlyEqual(TEXT("own fuse: no alert"), RunFlow->GetAlertSeconds() - BeforeKill, 0.5f, 1e-4f);

	// A kill never jumps over the console threshold; the console drops on the next tick.
	RunFlow->SetAlert(0.399f);
	RunFlow->NotifyEnemyKilled(nullptr, nullptr);
	TestNearlyEqual(TEXT("kill clamped to the threshold"), RunFlow->GetAlert(), 0.4f, 1e-6f);
	TestNull(TEXT("no console before the tick"), RunFlow->GetConsole());
	TestWorld.Tick();
	TestNotNull(TEXT("console dropped on the next tick"), RunFlow->GetConsole());
	TestEqual(TEXT("objective pending"), RunFlow->GetPhase(), ERRRunPhase::ObjectivePending);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRRunFlowPauseTest, "RogueRigs.RunFlow.PausedWhileConsolePending",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRRunFlowPauseTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRRunFlowTests;

	FTestWorld TestWorld(TEXT("RRRunFlowPauseWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FRunSetup Setup;
	if (!SetUpRun(*this, TestWorld, Setup))
	{
		return false;
	}
	URRRunFlowSubsystem* RunFlow = Setup.RunFlow;
	RunFlow->OnExtractionReady.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleExtractionReady);

	// 0.399 -> 0.4 takes 0.24 s of fill time (14.4 frames).
	RunFlow->SetAlert(0.399f);
	const int32 DropFrame = TickUntilConsole(TestWorld, RunFlow, 60);
	TestTrue(TEXT("console dropped within 15 frames"), DropFrame >= 14 && DropFrame <= 16);
	ARRConsole* Console = RunFlow->GetConsole();
	if (!TestNotNull(TEXT("console"), Console))
	{
		return false;
	}
	TestNearlyEqual(TEXT("alert clamped at 0.4"), RunFlow->GetAlert(), 0.4f, 1e-6f);
	TestTrue(TEXT("objective pending"), RunFlow->HasPendingObjective());

	// Paused: neither time nor kills move the bar.
	for (int32 Frame = 0; Frame < 120; ++Frame)
	{
		TestWorld.Tick();
	}
	RunFlow->NotifyEnemyKilled(nullptr, nullptr);
	TestNearlyEqual(TEXT("paused 2 s and a kill: still 0.4"), RunFlow->GetAlert(), 0.4f, 1e-6f);
	TestEqual(TEXT("the kill is still counted"), RunFlow->GetKillCount(), 1);

	// Completion unpauses: 60 frames later the bar moved 1 s.
	Console->ForceComplete();
	TestTrue(TEXT("console complete"), Console->IsComplete());
	TestFalse(TEXT("nothing pending"), RunFlow->HasPendingObjective());
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestNearlyEqual(TEXT("alert resumed: 96 + 1 s"), RunFlow->GetAlertSeconds(), 97.f, 1e-3f);
	TestEqual(TEXT("filling again"), RunFlow->GetPhase(), ERRRunPhase::Filling);

	// Full bar with nothing pending: extraction ready once.
	RunFlow->SetAlert(0.999f);
	int32 ReadyFrame = -1;
	for (int32 Frame = 1; Frame <= 60 && ReadyFrame < 0; ++Frame)
	{
		TestWorld.Tick();
		if (RunFlow->IsExtractionReady())
		{
			ReadyFrame = Frame;
		}
	}
	TestTrue(TEXT("extraction ready within 15 frames"), ReadyFrame >= 14 && ReadyFrame <= 16);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("extraction event once"), Recorder->GetExtractionReadyCount(), 1);
	TestEqual(TEXT("phase extraction ready"), RunFlow->GetPhase(), ERRRunPhase::ExtractionReady);
	TestEqual(TEXT("alert 1"), RunFlow->GetAlert(), 1.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRRunFlowExtractionWaitsTest, "RogueRigs.RunFlow.ExtractionWaitsForObjective",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRRunFlowExtractionWaitsTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRRunFlowTests;

	FTestWorld TestWorld(TEXT("RRRunFlowExtractionWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FRunSetup Setup;
	if (!SetUpRun(*this, TestWorld, Setup))
	{
		return false;
	}
	URRRunFlowSubsystem* RunFlow = Setup.RunFlow;
	RunFlow->OnExtractionReady.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleExtractionReady);
	RunFlow->SetAlert(0.399f);
	if (!TestTrue(TEXT("console dropped"), TickUntilConsole(TestWorld, RunFlow, 60) > 0))
	{
		return false;
	}

	// Bar forced full while the console is pending: no extraction.
	RunFlow->SetAlert(1.f);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestFalse(TEXT("no extraction while the console is pending"), RunFlow->IsExtractionReady());
	TestEqual(TEXT("no extraction event"), Recorder->GetExtractionReadyCount(), 0);
	TestEqual(TEXT("still pending"), RunFlow->GetPhase(), ERRRunPhase::ObjectivePending);

	RunFlow->GetConsole()->ForceComplete();
	TestWorld.Tick();
	TestTrue(TEXT("extraction once the console is hacked"), RunFlow->IsExtractionReady());
	TestEqual(TEXT("extraction event once"), Recorder->GetExtractionReadyCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRConsoleDropTest, "RogueRigs.Console.DropPlacementAndMarks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRConsoleDropTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRRunFlowTests;

	const int32 Seeds[] = { 1, 20261003, 777 };
	for (const int32 Seed : Seeds)
	{
		FTestWorld TestWorld(TEXT("RRConsoleDropWorld"));
		FRunSetup Setup;
		if (!SetUpRun(*this, TestWorld, Setup, RunMinerCell, Seed))
		{
			return false;
		}
		ARRConsole* Console = Setup.RunFlow->SkipToConsole();
		if (!TestNotNull(*FString::Printf(TEXT("seed %d: console"), Seed), Console))
		{
			return false;
		}
		ARRArenaGrid* Grid = Setup.Grid;
		const FIntPoint Cell = Console->GetCell();
		const float Distance = FMath::Sqrt(static_cast<float>(FMath::Square(Cell.X - RunMinerCell.X) + FMath::Square(Cell.Y - RunMinerCell.Y)));
		AddInfo(FString::Printf(TEXT("seed %d: console on (%d, %d), %.2f cells, %d marked rocks."), Seed, Cell.X, Cell.Y, Distance, Console->GetMarkedRockCount()));
		TestEqual(*FString::Printf(TEXT("seed %d: floor cell"), Seed), Grid->GetCellType(Cell), ERRCellType::Floor);
		TestTrue(*FString::Printf(TEXT("seed %d: 12 to 20 cells away"), Seed), Distance >= 12.f && Distance <= 20.f);
		URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
		TestTrue(*FString::Printf(TEXT("seed %d: reachable"), Seed), FlowField && FlowField->GetCellDistance(Cell) >= 0);
		TestTrue(*FString::Printf(TEXT("seed %d: at the cell center"), Seed), Console->GetActorLocation().Equals(Grid->GetCellCenter(Cell.X, Cell.Y), 0.1));
		TestNearlyEqual(*FString::Printf(TEXT("seed %d: alert at 0.4"), Seed), Setup.RunFlow->GetAlert(), 0.4f, 1e-6f);

		// Marked = every rock-type block within 2 cells, nothing else.
		int32 Expected = 0;
		for (int32 DY = -3; DY <= 3; ++DY)
		{
			for (int32 DX = -3; DX <= 3; ++DX)
			{
				const FIntPoint Near(Cell.X + DX, Cell.Y + DY);
				const bool bShouldMark = DX * DX + DY * DY <= 4 && IsMarkableType(Grid->GetCellType(Near));
				Expected += bShouldMark ? 1 : 0;
				TestEqual(*FString::Printf(TEXT("seed %d: (%d, %d) marked"), Seed, Near.X, Near.Y), Grid->IsCellMarked(Near), bShouldMark);
				if (bShouldMark)
				{
					TestEqual(*FString::Printf(TEXT("seed %d: (%d, %d) outline data"), Seed, Near.X, Near.Y), FGridAccess::GetMarkedData(Grid, Near), 1.f);
				}
			}
		}
		TestEqual(*FString::Printf(TEXT("seed %d: marked count"), Seed), Console->GetMarkedRockCount(), Expected);
		TestEqual(*FString::Printf(TEXT("seed %d: grid marked count"), Seed), Grid->GetMarkedCells().Num(), Expected);
		TestTrue(*FString::Printf(TEXT("seed %d: rocks to clear (cells with rocks preferred)"), Seed), Expected > 0);
		TestEqual(*FString::Printf(TEXT("seed %d: locked"), Seed), Console->GetConsoleState(), ERRConsoleState::Locked);

		// A second skip returns the same console.
		TestTrue(*FString::Printf(TEXT("seed %d: one console"), Seed), Setup.RunFlow->SkipToConsole() == Console);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRConsoleHackTest, "RogueRigs.Console.HackRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRConsoleHackTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRRunFlowTests;

	FTestWorld TestWorld(TEXT("RRConsoleHackWorld"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	FRunSetup Setup;
	if (!SetUpRun(*this, TestWorld, Setup))
	{
		return false;
	}
	Setup.RunFlow->SetRunFlowEnabled(false);
	ARRArenaGrid* Grid = Setup.Grid;
	ARRMiner* Miner = Setup.Miner;
	ARRDrone* Drone = Miner->GetDrone();
	if (!TestNotNull(TEXT("drone"), Drone))
	{
		return false;
	}

	ARRConsole* Console = TestWorld.Spawn<ARRConsole>(ARRConsole::StaticClass(), Grid->GetCellCenter(RunHackConsoleCell.X, RunHackConsoleCell.Y));
	if (!TestNotNull(TEXT("console spawned"), Console))
	{
		return false;
	}
	Console->OnConsoleStateChanged.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleConsoleStateChanged);
	Console->InitializeConsole(Grid, Miner, RunHackConsoleCell);
	TestEqual(TEXT("hack 20 s"), Console->GetHackDuration(), 20.f);
	TestEqual(TEXT("drain 5 %/s"), Console->GetDrainPerSecond(), 0.05f);
	TestEqual(TEXT("zone 3 cells"), Console->GetZoneRadius(), 3.f * Grid->GetCellSize());
	TestTrue(TEXT("rock (43, 38) marked"), Grid->IsCellMarked(RockCell));
	const TArray<FIntPoint> Marked = Console->GetMarkedRocks();
	AddInfo(FString::Printf(TEXT("%d marked rocks around (42, 38)."), Marked.Num()));
	if (!TestTrue(TEXT("rocks marked"), Marked.Num() > 0))
	{
		return false;
	}

	// Locked: the miner is in the zone but the drone does not hack.
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestTrue(TEXT("miner in the zone"), Console->IsMinerInZone());
	TestEqual(TEXT("locked"), Console->GetConsoleState(), ERRConsoleState::Locked);
	TestTrue(TEXT("drone not hacking while locked"), Drone->GetState() != ERRDroneState::Hack);
	TestEqual(TEXT("no progress while locked"), Console->GetHackProgress(), 0.f);

	// Clearing the rocks: the count follows, the marks leave the grid.
	for (int32 Index = 0; Index < Marked.Num() - 1; ++Index)
	{
		Grid->DestroyCell(Marked[Index], ERRTerrainDamageSource::Drone);
		TestFalse(TEXT("destroyed rock unmarked"), Grid->IsCellMarked(Marked[Index]));
	}
	TestWorld.Tick();
	TestEqual(TEXT("one marked rock left"), Console->GetMarkedRockCount(), 1);
	TestEqual(TEXT("still locked"), Console->GetConsoleState(), ERRConsoleState::Locked);
	Grid->DestroyCell(Marked.Last(), ERRTerrainDamageSource::Drone);
	TestWorld.Tick();
	TestEqual(TEXT("no marked rock left"), Console->GetMarkedRockCount(), 0);
	TestEqual(TEXT("hacking"), Console->GetConsoleState(), ERRConsoleState::Hacking);
	TestEqual(TEXT("drone in Hack"), Drone->GetState(), ERRDroneState::Hack);
	TestTrue(TEXT("drone linked to the console"), Drone->GetHackTarget() == Console);

	// One target at a time, no dig meanwhile.
	FIntPoint BlockCell(-1, -1);
	for (int32 Y = 0; Y < Grid->GetGridSize().Y && BlockCell.X < 0; ++Y)
	{
		for (int32 X = 0; X < Grid->GetGridSize().X; ++X)
		{
			if (Grid->IsBlockCell(FIntPoint(X, Y)))
			{
				BlockCell = FIntPoint(X, Y);
				break;
			}
		}
	}
	TestFalse(TEXT("no dig while hacking"), Drone->RequestDig(Grid, BlockCell, FIntPoint(-1, 0)));
	TestFalse(TEXT("no second hack target"), Drone->StartHack(Grid));
	TestEqual(TEXT("still in Hack"), Drone->GetState(), ERRDroneState::Hack);

	// 600 frames of hacking (the first was the frame the rocks cleared) = 10 s = half the gauge.
	for (int32 Frame = 1; Frame < 600; ++Frame)
	{
		TestWorld.Tick();
	}
	TestNearlyEqual(TEXT("half after 10 s"), Console->GetHackProgress(), 0.5f, 0.002f);

	// Miner leaves: the drone follows him, progress drains 5 %/s.
	const FVector InZone = Miner->GetActorLocation();
	Miner->SetActorLocation(InZone + FVector(0.f, 8.f * Grid->GetCellSize(), 0.f));
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestFalse(TEXT("miner out of the zone"), Console->IsMinerInZone());
	TestEqual(TEXT("idle"), Console->GetConsoleState(), ERRConsoleState::Idle);
	TestEqual(TEXT("drone back to Orbit"), Drone->GetState(), ERRDroneState::Orbit);
	TestNull(TEXT("drone unlinked"), Drone->GetHackTarget());
	TestNearlyEqual(TEXT("drained 5 % in 1 s"), Console->GetHackProgress(), 0.45f, 0.002f);

	// Back in: 0.55 of the gauge left = 11 s = 660 frames.
	Miner->SetActorLocation(InZone);
	int32 CompleteFrame = -1;
	for (int32 Frame = 1; Frame <= 800 && CompleteFrame < 0; ++Frame)
	{
		TestWorld.Tick();
		if (Console->IsComplete())
		{
			CompleteFrame = Frame;
		}
	}
	AddInfo(FString::Printf(TEXT("complete %d frames after coming back."), CompleteFrame));
	TestTrue(TEXT("complete after 660 frames (+-2)"), FMath::Abs(CompleteFrame - 660) <= 2);
	TestEqual(TEXT("progress 1"), Console->GetHackProgress(), 1.f);
	TestEqual(TEXT("drone released"), Drone->GetState(), ERRDroneState::Orbit);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("complete is final"), Console->GetConsoleState(), ERRConsoleState::Complete);
	TestEqual(TEXT("drone stays free"), Drone->GetState(), ERRDroneState::Orbit);

	const TArray<ERRConsoleState> ExpectedStates = { ERRConsoleState::Locked, ERRConsoleState::Hacking, ERRConsoleState::Idle, ERRConsoleState::Hacking, ERRConsoleState::Complete };
	TestTrue(TEXT("state sequence Locked, Hacking, Idle, Hacking, Complete"), Recorder->GetConsoleStates() == ExpectedStates);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
