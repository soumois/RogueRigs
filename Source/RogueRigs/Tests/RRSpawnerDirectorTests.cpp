// Rogue Rigs - automation tests of the spawner wave director (RogueRigs.Spawner.Director*): phase times, alert trigger, pack homogeneity with tough robots in front, horde front size and side, alive and per-class caps.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Algo/Count.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShooter.h"
#include "Enemies/RRWaveDirector.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRSpawnerDirectorTests
{
	/** One scheduler order and the run time at which it came. */
	struct FTimedAction
	{
		float Time = 0.f;
		FRRDirectorAction Action;
	};

	/** Runs a fresh scheduler on Config for Seconds at 60 frames per second, the alert given by AlertOf(run time); returns every order. */
	TArray<FTimedAction> RunDirectorSchedule(const FRRWaveDirectorConfig& Config, float Seconds, TFunctionRef<float(float)> AlertOf, int32 Seed)
	{
		FRRWaveDirector Director;
		Director.Reset();
		FRandomStream Stream(Seed);
		TArray<FTimedAction> Result;
		TArray<FRRDirectorAction> Frame;
		const int32 Frames = FMath::RoundToInt(Seconds * 60.f);
		for (int32 Index = 0; Index < Frames; ++Index)
		{
			Frame.Reset();
			Director.Advance(Config, 1.f / 60.f, AlertOf(Director.GetRunTime()), Stream, Frame);
			for (const FRRDirectorAction& Action : Frame)
			{
				Result.Add({ Director.GetRunTime(), Action });
			}
		}
		return Result;
	}

	/** Times of the orders of Type (and Index when not INDEX_NONE). */
	TArray<float> DirectorTimesOf(const TArray<FTimedAction>& Actions, FRRDirectorAction::EType Type, int32 Index = INDEX_NONE)
	{
		TArray<float> Times;
		for (const FTimedAction& Timed : Actions)
		{
			if (Timed.Action.Type == Type && (Index == INDEX_NONE || Timed.Action.Index == Index))
			{
				Times.Add(Timed.Time);
			}
		}
		return Times;
	}

	/** Two frames of tolerance. */
	constexpr float TimeTolerance = 2.f / 60.f;

	/** Miner cell on arena_m1_fine (open floor), as in the spawner tests. */
	const FIntPoint DirectorMinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Half extent (uu) of the fixed view rectangle centered on the miner. */
	const FVector2D DirectorViewHalfExtent(1600.f, 1000.f);

	/** World with the grid, an unarmed miner, and a native spawner (chaser 65, kamikaze 12, shooter 15, heavy 8 capped at 5) on a fixed view. */
	struct FDirectorSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARREnemySpawner* Spawner = nullptr;
		FBox2D View = FBox2D(ForceInit);
	};

	FRRWeightedEnemyClass DirectorEntry(TSubclassOf<ARREnemy> Class, float Weight, int32 MaxAlive = 0)
	{
		FRRWeightedEnemyClass Entry;
		Entry.EnemyClass = Class;
		Entry.Weight = Weight;
		Entry.MaxAlive = MaxAlive;
		return Entry;
	}

	bool MakeDirectorSetup(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, int32 MaxAlive, FDirectorSetup& Out)
	{
		using namespace RRTestAccess;
		Out.Grid = RRTestHarness::SpawnFineGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Out.Grid, DirectorMinerCell) : nullptr;
		if (!Out.Miner)
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
		Out.Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
		if (!Test.TestNotNull(TEXT("spawner spawned"), Out.Spawner))
		{
			return false;
		}
		Test.TestTrue(TEXT("director on by default"), Out.Spawner->IsDirectorEnabled());
		Test.TestEqual(TEXT("default MaxAlive 150"), FSpawnerAccess::MaxAliveOf(Out.Spawner), 150);
		Out.Spawner->SetSpawningEnabled(false);
		FSpawnerAccess::MaxAliveOf(Out.Spawner) = MaxAlive;
		FSpawnerAccess::WeightsOf(Out.Spawner) = { DirectorEntry(ARRChaser::StaticClass(), 65.f), DirectorEntry(ARRKamikaze::StaticClass(), 12.f),
			DirectorEntry(ARRShooter::StaticClass(), 15.f), DirectorEntry(ARRHeavy::StaticClass(), 8.f, 5) };
		const FVector MinerLocation = Out.Miner->GetActorLocation();
		const FVector2D Center(MinerLocation.X, MinerLocation.Y);
		Out.View = FBox2D(Center - DirectorViewHalfExtent, Center + DirectorViewHalfExtent);
		Out.Spawner->SetViewRectangleOverride(Out.View);
		Out.Spawner->SeedRandomStream(20261005);
		return true;
	}

	/** Box distance (uu) from View to the center of the cell of Location. */
	float DirectorViewDistance(const FDirectorSetup& Setup, const FIntPoint& Cell)
	{
		const FVector Center = Setup.Grid->GetCellCenter(Cell.X, Cell.Y);
		return ARREnemySpawner::GetDistanceToRectangle(Setup.View, FVector2D(Center.X, Center.Y));
	}

	/** Side of View beyond the center of Cell. */
	ERRSpawnSide DirectorSideOf(const FDirectorSetup& Setup, const FIntPoint& Cell)
	{
		const FVector Center = Setup.Grid->GetCellCenter(Cell.X, Cell.Y);
		return ARREnemySpawner::GetSideOfRectangle(Setup.View, FVector2D(Center.X, Center.Y));
	}

	/** A config without phases and one horde whose lull starts at 1 s (banner 1.5 s, front 2 s, spread 1 s). */
	FRRWaveDirectorConfig ShortHordeConfig()
	{
		FRRWaveDirectorConfig Config = FRRWaveDirectorConfig::MakeDefault();
		Config.Phases.Reset();
		Config.Hordes.SetNum(1);
		FRRDirectorHorde& Horde = Config.Hordes[0];
		Horde.TriggerTime = 2.f;
		Horde.LullSeconds = 1.f;
		Horde.WarningSeconds = 0.5f;
		Horde.SpreadSeconds = 1.f;
		return Config;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorPhaseTimesTest, "RogueRigs.Spawner.DirectorPhaseTimes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorPhaseTimesTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;
	using EType = FRRDirectorAction::EType;

	const FRRWaveDirectorConfig Config = FRRWaveDirectorConfig::MakeDefault();
	const int32 FirstChasers = Config.FindPackType(TEXT("FirstChasers"));
	const int32 Chasers = Config.FindPackType(TEXT("Chasers"));
	const int32 Shooters = Config.FindPackType(TEXT("Shooters"));
	const int32 Heavies = Config.FindPackType(TEXT("Heavies"));
	if (!TestTrue(TEXT("default pack types"), FirstChasers != INDEX_NONE && Chasers != INDEX_NONE && Shooters != INDEX_NONE && Heavies != INDEX_NONE)
		|| !TestEqual(TEXT("two default hordes"), Config.Hordes.Num(), 2))
	{
		return false;
	}
	TestEqual(TEXT("horde trigger is run time"), Config.HordeTrigger, ERRHordeTrigger::RunTime);
	const TArray<FTimedAction> Actions = RunDirectorSchedule(Config, 240.f, [](float) { return 0.f; }, 20261005);

	// Phases at 0, 10, 22, 28 s.
	const TArray<float> Phases = DirectorTimesOf(Actions, EType::PhaseChanged);
	const float ExpectedPhases[] = { 0.f, 10.f, 22.f, 28.f };
	if (TestEqual(TEXT("4 phase changes"), Phases.Num(), 4))
	{
		for (int32 Index = 0; Index < 4; ++Index)
		{
			TestTrue(*FString::Printf(TEXT("phase %d at %.0f s (%.3f)"), Index, ExpectedPhases[Index], Phases[Index]), FMath::Abs(Phases[Index] - ExpectedPhases[Index]) <= TimeTolerance);
		}
	}

	// 0-10 s: nothing. 10-22 s: a lone chaser at 10, 13, 16, 19 s.
	const TArray<float> Singles = DirectorTimesOf(Actions, EType::SpawnSingle);
	const TArray<float> Waves = DirectorTimesOf(Actions, EType::SpawnWave);
	TestTrue(TEXT("nothing spawns before 10 s"), (Singles.IsEmpty() || Singles[0] >= 10.f - TimeTolerance) && (Waves.IsEmpty() || Waves[0] >= 10.f));
	const float ExpectedSingles[] = { 10.f, 13.f, 16.f, 19.f };
	if (TestEqual(TEXT("4 lone robots"), Singles.Num(), 4))
	{
		for (int32 Index = 0; Index < 4; ++Index)
		{
			TestTrue(*FString::Printf(TEXT("lone robot %d at %.0f s (%.3f)"), Index, ExpectedSingles[Index], Singles[Index]), FMath::Abs(Singles[Index] - ExpectedSingles[Index]) <= TimeTolerance);
		}
	}
	for (const FTimedAction& Timed : Actions)
	{
		if (Timed.Action.Type == EType::SpawnSingle)
		{
			TestTrue(TEXT("lone robots are chasers"), Timed.Action.SingleClass == ARRChaser::StaticClass());
		}
	}

	// Hordes: lull at T-5, banner at T-4 (4 s ahead), front at T, over at T+2; nothing else spawns from the lull to the end.
	const float FrontTimes[] = { 95.f, 175.f };
	for (int32 Horde = 0; Horde < 2; ++Horde)
	{
		const float Front = FrontTimes[Horde];
		const TArray<float> Lull = DirectorTimesOf(Actions, EType::HordeLull, Horde);
		const TArray<float> Announced = DirectorTimesOf(Actions, EType::HordeAnnounced, Horde);
		const TArray<float> Started = DirectorTimesOf(Actions, EType::HordeStarted, Horde);
		const TArray<float> Finished = DirectorTimesOf(Actions, EType::HordeFinished, Horde);
		if (!TestTrue(*FString::Printf(TEXT("horde %d steps once each"), Horde), Lull.Num() == 1 && Announced.Num() == 1 && Started.Num() == 1 && Finished.Num() == 1))
		{
			continue;
		}
		AddInfo(FString::Printf(TEXT("horde %d: lull %.2f, banner %.2f, front %.2f, over %.2f"), Horde, Lull[0], Announced[0], Started[0], Finished[0]));
		TestTrue(*FString::Printf(TEXT("horde %d lull 5 s before the front"), Horde), FMath::Abs(Lull[0] - (Front - 5.f)) <= TimeTolerance);
		TestTrue(*FString::Printf(TEXT("horde %d banner 4 s before the front"), Horde), FMath::Abs(Announced[0] - (Front - 4.f)) <= TimeTolerance);
		TestTrue(*FString::Printf(TEXT("horde %d front at %.0f s"), Horde, Front), FMath::Abs(Started[0] - Front) <= TimeTolerance);
		TestTrue(*FString::Printf(TEXT("horde %d front over 2 s"), Horde), FMath::Abs(Finished[0] - (Front + 2.f)) <= TimeTolerance);
		for (const FTimedAction& Timed : Actions)
		{
			if (Timed.Action.Type == EType::HordeAnnounced && Timed.Action.Index == Horde)
			{
				TestTrue(*FString::Printf(TEXT("horde %d banner says 4 s (%.2f)"), Horde, Timed.Action.Seconds), FMath::Abs(Timed.Action.Seconds - 4.f) <= TimeTolerance);
			}
			const bool bSpawn = Timed.Action.Type == EType::SpawnSingle || Timed.Action.Type == EType::SpawnWave;
			if (bSpawn && Timed.Time >= Lull[0] && Timed.Time <= Finished[0])
			{
				AddError(FString::Printf(TEXT("horde %d: spawn order at %.2f s during the lull or the front"), Horde, Timed.Time));
			}
		}
	}

	// Waves: first pack at 22 s (one pack of 8-10 chasers), then 2-3 packs per wave from 28 s, interval 6 s shrinking to 3 s.
	if (!TestTrue(TEXT("waves"), Waves.Num() > 20))
	{
		return false;
	}
	TestTrue(*FString::Printf(TEXT("first wave at 22 s (%.3f)"), Waves[0]), FMath::Abs(Waves[0] - 22.f) <= TimeTolerance);
	TestTrue(*FString::Printf(TEXT("flow starts at 28 s (%.3f)"), Waves[1]), FMath::Abs(Waves[1] - 28.f) <= TimeTolerance);
	TestTrue(*FString::Printf(TEXT("second flow wave 6 s later (%.3f)"), Waves[2] - Waves[1]), FMath::Abs(Waves[2] - Waves[1] - 6.f) <= 0.1f);
	TSet<int32> TypesSeen;
	int32 WaveIndex = 0;
	for (const FTimedAction& Timed : Actions)
	{
		if (Timed.Action.Type != EType::SpawnWave)
		{
			continue;
		}
		if (WaveIndex == 0)
		{
			TestTrue(TEXT("first wave is one FirstChasers pack"), Timed.Action.PackTypes.Num() == 1 && Timed.Action.PackTypes[0] == FirstChasers);
		}
		else
		{
			TestTrue(*FString::Printf(TEXT("wave at %.2f has 2-3 packs (%d)"), Timed.Time, Timed.Action.PackTypes.Num()), Timed.Action.PackTypes.Num() >= 2 && Timed.Action.PackTypes.Num() <= 3);
			for (const int32 Type : Timed.Action.PackTypes)
			{
				TestTrue(TEXT("flow packs are Chasers, Shooters or Heavies"), Type == Chasers || Type == Shooters || Type == Heavies);
				TypesSeen.Add(Type);
			}
		}
		++WaveIndex;
	}
	TestEqual(TEXT("the flow draws all three pack types"), TypesSeen.Num(), 3);
	FString GapLog;
	float PreviousGap = TNumericLimits<float>::Max();
	for (int32 Index = 2; Index < Waves.Num(); ++Index)
	{
		const float Gap = Waves[Index] - Waves[Index - 1];
		bool bAcrossHorde = false;
		for (const float Front : FrontTimes)
		{
			bAcrossHorde |= Waves[Index - 1] < Front && Waves[Index] > Front - 5.f;
		}
		GapLog += FString::Printf(TEXT(" %.2f%s"), Gap, bAcrossHorde ? TEXT("h") : TEXT(""));
		if (bAcrossHorde)
		{
			continue;
		}
		TestTrue(*FString::Printf(TEXT("gap at %.1f s within 3-6 s (%.2f)"), Waves[Index], Gap), Gap >= 3.f - TimeTolerance && Gap <= 6.f + TimeTolerance);
		TestTrue(*FString::Printf(TEXT("gap at %.1f s not longer than the previous"), Waves[Index]), Gap <= PreviousGap + TimeTolerance);
		if (Waves[Index - 1] >= 28.f + 180.f)
		{
			TestTrue(*FString::Printf(TEXT("gap after 3 min of flow is 3 s (%.2f)"), Gap), FMath::Abs(Gap - 3.f) <= TimeTolerance);
		}
		PreviousGap = Gap;
	}
	AddInfo(FString::Printf(TEXT("wave gaps (h = across a horde):%s"), *GapLog));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorAlertTriggerTest, "RogueRigs.Spawner.DirectorAlertTrigger",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorAlertTriggerTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;
	using EType = FRRDirectorAction::EType;

	// Alert trigger: the alert fills in 240 s; hordes at 40 % (96 s) and 75 % (180 s), front 5 s after each.
	FRRWaveDirectorConfig Config = FRRWaveDirectorConfig::MakeDefault();
	Config.HordeTrigger = ERRHordeTrigger::Alert;
	const TArray<FTimedAction> Actions = RunDirectorSchedule(Config, 200.f, [](float RunTime) { return RunTime / 240.f; }, 7);
	const float LullTimes[] = { 96.f, 180.f };
	for (int32 Horde = 0; Horde < 2; ++Horde)
	{
		const TArray<float> Lull = DirectorTimesOf(Actions, EType::HordeLull, Horde);
		const TArray<float> Started = DirectorTimesOf(Actions, EType::HordeStarted, Horde);
		if (TestTrue(*FString::Printf(TEXT("horde %d triggered once"), Horde), Lull.Num() == 1 && Started.Num() == 1))
		{
			TestTrue(*FString::Printf(TEXT("horde %d lull at alert %.2f (%.2f s)"), Horde, Config.Hordes[Horde].TriggerAlert, Lull[0]), FMath::Abs(Lull[0] - LullTimes[Horde]) <= TimeTolerance);
			TestTrue(*FString::Printf(TEXT("horde %d front 5 s later"), Horde), FMath::Abs(Started[0] - Lull[0] - 5.f) <= TimeTolerance);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorPackHomogeneityTest, "RogueRigs.Spawner.DirectorPackHomogeneity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorPackHomogeneityTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRDirectorPacks"));
	FDirectorSetup Setup;
	if (!MakeDirectorSetup(*this, TestWorld, 150, Setup))
	{
		return false;
	}
	const FRRWaveDirectorConfig& Config = Setup.Spawner->GetDirectorConfig();
	const float CellSize = Setup.Grid->GetCellSize();
	for (int32 Type = 0; Type < Config.PackTypes.Num(); ++Type)
	{
		const FRRDirectorPackType& PackType = Config.PackTypes[Type];
		for (int32 Repeat = 0; Repeat < 3; ++Repeat)
		{
			const FString Label = FString::Printf(TEXT("%s pack %d"), *PackType.Name.ToString(), Repeat);
			const TArray<FRRSpawnedPack> Packs = Setup.Spawner->TrySpawnDirectorWave({ Type });
			if (!TestEqual(*FString::Printf(TEXT("%s: one pack"), *Label), Packs.Num(), 1))
			{
				continue;
			}
			const FRRSpawnedPack& Pack = Packs[0];
			TestEqual(*FString::Printf(TEXT("%s: pack type"), *Label), Pack.PackType, Type);

			// Only the classes of the type, each group within its count range.
			int32 Listed = 0;
			float FrontFarthest = 0.f;
			float BackNearest = TNumericLimits<float>::Max();
			for (const FRRDirectorPackMember& Member : PackType.Members)
			{
				int32 Count = 0;
				for (int32 Index = 0; Index < Pack.Robots.Num(); ++Index)
				{
					if (Pack.Robots[Index]->GetClass() != Member.RobotClass.Get())
					{
						continue;
					}
					++Count;
					const float Distance = DirectorViewDistance(Setup, Pack.Cells[Index]);
					FrontFarthest = Member.bFront ? FMath::Max(FrontFarthest, Distance) : FrontFarthest;
					BackNearest = Member.bFront ? BackNearest : FMath::Min(BackNearest, Distance);
				}
				Listed += Count;
				TestTrue(*FString::Printf(TEXT("%s: %d %s within %d-%d"), *Label, Count, *Member.RobotClass->GetName(), Member.CountMin, Member.CountMax),
					Count >= Member.CountMin && Count <= Member.CountMax);
			}
			TestEqual(*FString::Printf(TEXT("%s: no robot of another type"), *Label), Listed, Pack.Robots.Num());
			const bool bHasFront = PackType.Members.ContainsByPredicate([](const FRRDirectorPackMember& Member) { return Member.bFront; });
			if (bHasFront)
			{
				TestTrue(*FString::Printf(TEXT("%s: front robots nearest to the view (%.0f <= %.0f)"), *Label, FrontFarthest, BackNearest), FrontFarthest <= BackNearest);
			}

			// One side, in the band, distinct spawn cells (a heavy may be nudged off its cell by its neighbours).
			TSet<FIntPoint> Cells;
			FIntRect Bounds(TNumericLimits<int32>::Max(), TNumericLimits<int32>::Max(), TNumericLimits<int32>::Min(), TNumericLimits<int32>::Min());
			TestEqual(*FString::Printf(TEXT("%s: one cell per robot"), *Label), Pack.Cells.Num(), Pack.Robots.Num());
			for (const FIntPoint& Cell : Pack.Cells)
			{
				TestEqual(*FString::Printf(TEXT("%s: (%d, %d) on the pack side"), *Label, Cell.X, Cell.Y), DirectorSideOf(Setup, Cell), Pack.Side);
				const float Distance = DirectorViewDistance(Setup, Cell);
				TestTrue(*FString::Printf(TEXT("%s: (%d, %d) in the spawn band"), *Label, Cell.X, Cell.Y), Distance >= 3.5f * CellSize && Distance <= 18.5f * CellSize);
				TestFalse(*FString::Printf(TEXT("%s: (%d, %d) distinct"), *Label, Cell.X, Cell.Y), Cells.Contains(Cell));
				Cells.Add(Cell);
				Bounds.Include(Cell);
			}
			AddInfo(FString::Printf(TEXT("%s: %d robots, side %d, cell bounds %d x %d"), *Label, Pack.Robots.Num(), static_cast<int32>(Pack.Side),
				Bounds.Width() + 1, Bounds.Height() + 1));
			for (ARREnemy* Robot : Pack.Robots)
			{
				Robot->Destroy();
			}
		}
	}

	// A wave of three packs: three distinct sides.
	const TArray<FRRSpawnedPack> Wave = Setup.Spawner->TrySpawnDirectorWave({ Config.FindPackType(TEXT("Chasers")), Config.FindPackType(TEXT("Shooters")),
		Config.FindPackType(TEXT("Heavies")) });
	if (TestEqual(TEXT("wave of three packs"), Wave.Num(), 3))
	{
		TSet<ERRSpawnSide> Sides;
		for (const FRRSpawnedPack& Pack : Wave)
		{
			Sides.Add(Pack.Side);
			TestTrue(TEXT("wave pack not empty"), !Pack.Robots.IsEmpty());
		}
		TestEqual(TEXT("three distinct sides"), Sides.Num(), 3);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorHordeTest, "RogueRigs.Spawner.DirectorHorde",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorHordeTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;
	using namespace RRTestAccess;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRDirectorHorde"));
	FDirectorSetup Setup;
	if (!MakeDirectorSetup(*this, TestWorld, 150, Setup))
	{
		return false;
	}
	FSpawnerAccess::DirectorOf(Setup.Spawner) = ShortHordeConfig();
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Setup.Spawner->OnHordeLull.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleHordeLull);
	Setup.Spawner->OnHordeIncoming.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleHordeIncoming);
	Setup.Spawner->OnHordeStarted.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleHordeStarted);
	Setup.Spawner->OnHordeFinished.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleHordeFinished);
	Setup.Spawner->SetSpawningEnabled(true);

	// Lull at 1 s (frame 60), banner at 1.5 s (90), front from 2 s (120) to 3 s (180).
	TMap<FString, int32> EventFrames;
	int32 SeenEvents = 0;
	int32 AliveAtHalf = 0;
	int32 AliveBeforeFront = 0;
	int32 AliveAtEnd = 0;
	for (int32 Frame = 1; Frame <= 200; ++Frame)
	{
		TestWorld.Tick();
		const TArray<FString>& Events = Recorder->GetDirectorEvents();
		for (; SeenEvents < Events.Num(); ++SeenEvents)
		{
			FString Key;
			Events[SeenEvents].Split(TEXT(":"), &Key, nullptr);
			EventFrames.Add(Key, Frame);
		}
		const int32 Alive = Setup.Spawner->GetAliveCount();
		AliveBeforeFront = Frame == 119 ? Alive : AliveBeforeFront;
		AliveAtHalf = Frame == 150 ? Alive : AliveAtHalf;
		AliveAtEnd = Frame == 181 ? Alive : AliveAtEnd;
	}
	AddInfo(FString::Printf(TEXT("events: %s"), *FString::Join(Recorder->GetDirectorEvents(), TEXT(", "))));
	auto CheckFrame = [this, &EventFrames](const TCHAR* Key, int32 Expected)
	{
		const int32* Frame = EventFrames.Find(Key);
		TestTrue(*FString::Printf(TEXT("%s at frame %d (%d)"), Key, Expected, Frame ? *Frame : -1), Frame && FMath::Abs(*Frame - Expected) <= 1);
	};
	CheckFrame(TEXT("Lull"), 60);
	CheckFrame(TEXT("Incoming"), 90);
	CheckFrame(TEXT("Started"), 120);
	CheckFrame(TEXT("Finished"), 180);
	if (Recorder->GetHordeIncomingSeconds().Num() == 1)
	{
		TestTrue(TEXT("banner announces 0.5 s"), FMath::Abs(Recorder->GetHordeIncomingSeconds()[0] - 0.5f) <= TimeTolerance);
	}
	TestEqual(TEXT("nothing before the front"), AliveBeforeFront, 0);
	TestEqual(TEXT("horde state done"), Setup.Spawner->GetHordeState(0), ERRHordeState::Done);

	// The front: 40-60 robots on one side, heavies first and nearest, kamikazes among a carpet of chasers.
	const TArray<FRRHordeSlot>& Plan = FSpawnerAccess::HordePlanOf(Setup.Spawner);
	const int32 Spawned = FSpawnerAccess::HordeSpawnedOf(Setup.Spawner);
	AddInfo(FString::Printf(TEXT("plan %d, spawned %d, alive at half %d, at end %d"), Plan.Num(), Spawned, AliveAtHalf, AliveAtEnd));
	TestTrue(*FString::Printf(TEXT("front of 40-60 robots (%d)"), Plan.Num()), Plan.Num() >= 40 && Plan.Num() <= 60);
	TestEqual(TEXT("every planned robot spawned"), Spawned, Plan.Num());
	TestEqual(TEXT("all alive after the front"), AliveAtEnd, Spawned);
	TestTrue(*FString::Printf(TEXT("spread over the second (%d at half)"), AliveAtHalf), AliveAtHalf > Plan.Num() / 4 && AliveAtHalf < Plan.Num() * 3 / 4);
	const FString* StartedEvent = Recorder->GetDirectorEvents().FindByPredicate([](const FString& Event) { return Event.StartsWith(TEXT("Started:")); });
	const ERRSpawnSide Side = Plan.IsEmpty() ? ERRSpawnSide::None : DirectorSideOf(Setup, Plan[0].Cell);
	TestTrue(TEXT("started event names the side"), StartedEvent && *StartedEvent == FString::Printf(TEXT("Started:0:%d"), static_cast<int32>(Side)));
	int32 Heavies = 0;
	int32 Kamikazes = 0;
	int32 Chasers = 0;
	bool bHeaviesFirst = true;
	float HeavyFarthest = 0.f;
	float OtherNearest = TNumericLimits<float>::Max();
	int32 MinAlong = TNumericLimits<int32>::Max();
	int32 MaxAlong = TNumericLimits<int32>::Min();
	const bool bAlongY = Side == ERRSpawnSide::NegativeX || Side == ERRSpawnSide::PositiveX;
	for (int32 Index = 0; Index < Plan.Num(); ++Index)
	{
		const FRRHordeSlot& Slot = Plan[Index];
		TestEqual(*FString::Printf(TEXT("slot %d on the horde side"), Index), DirectorSideOf(Setup, Slot.Cell), Side);
		const bool bHeavy = Slot.Class == ARRHeavy::StaticClass();
		bHeaviesFirst &= !bHeavy || Index == Heavies;
		Heavies += bHeavy ? 1 : 0;
		Kamikazes += Slot.Class == ARRKamikaze::StaticClass() ? 1 : 0;
		Chasers += Slot.Class == ARRChaser::StaticClass() ? 1 : 0;
		const float Distance = DirectorViewDistance(Setup, Slot.Cell);
		HeavyFarthest = bHeavy ? FMath::Max(HeavyFarthest, Distance) : HeavyFarthest;
		OtherNearest = bHeavy ? OtherNearest : FMath::Min(OtherNearest, Distance);
		const int32 Along = bAlongY ? Slot.Cell.Y : Slot.Cell.X;
		MinAlong = FMath::Min(MinAlong, Along);
		MaxAlong = FMath::Max(MaxAlong, Along);
	}
	AddInfo(FString::Printf(TEXT("side %d: %d heavies, %d kamikazes, %d chasers, front %d cells long"), static_cast<int32>(Side), Heavies, Kamikazes, Chasers, MaxAlong - MinAlong + 1));
	TestTrue(*FString::Printf(TEXT("3-5 heavies (%d)"), Heavies), Heavies >= 3 && Heavies <= 5);
	TestTrue(*FString::Printf(TEXT("3-5 kamikazes (%d)"), Kamikazes), Kamikazes >= 3 && Kamikazes <= 5);
	TestEqual(TEXT("the rest are chasers"), Heavies + Kamikazes + Chasers, Plan.Num());
	TestTrue(TEXT("heavies spawn first"), bHeaviesFirst);
	TestTrue(*FString::Printf(TEXT("heavies in the front row (%.0f <= %.0f)"), HeavyFarthest, OtherNearest), HeavyFarthest <= OtherNearest);
	TestTrue(*FString::Printf(TEXT("a front along the side (%d cells long)"), MaxAlong - MinAlong + 1), MaxAlong - MinAlong + 1 >= 15);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorCapTest, "RogueRigs.Spawner.DirectorCap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorCapTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;
	using namespace RRTestAccess;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRDirectorCap"));
	FDirectorSetup Setup;
	if (!MakeDirectorSetup(*this, TestWorld, 30, Setup))
	{
		return false;
	}
	// Per-class cap: 5 heavies, then none.
	for (int32 Index = 0; Index < 5; ++Index)
	{
		TestNotNull(*FString::Printf(TEXT("heavy %d"), Index), Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass()));
	}
	TestNull(TEXT("no 6th heavy (MaxAlive 5)"), Setup.Spawner->TrySpawnEnemyOfClass(ARRHeavy::StaticClass()));
	const TArray<FRRSpawnedPack> HeavyPack = Setup.Spawner->TrySpawnDirectorWave({ Setup.Spawner->GetDirectorConfig().FindPackType(TEXT("Heavies")) });
	int32 PackHeavies = 0;
	int32 PackRobots = 0;
	for (const FRRSpawnedPack& Pack : HeavyPack)
	{
		PackRobots += Pack.Robots.Num();
		for (const ARREnemy* Robot : Pack.Robots)
		{
			PackHeavies += Robot->IsA<ARRHeavy>() ? 1 : 0;
		}
	}
	TestEqual(TEXT("heavy pack without room for heavies has none"), PackHeavies, 0);
	for (const FRRSpawnedPack& Pack : HeavyPack)
	{
		for (ARREnemy* Robot : Pack.Robots)
		{
			Robot->Destroy();
		}
	}
	TestEqual(TEXT("5 heavies alive"), Setup.Spawner->GetAliveCount(), 5);

	// Horde clamped to the 25 free slots, no heavy (capped); the alive count never exceeds 30.
	FSpawnerAccess::DirectorOf(Setup.Spawner) = ShortHordeConfig();
	Setup.Spawner->SetSpawningEnabled(true);
	int32 MaxSeen = 0;
	for (int32 Frame = 1; Frame <= 190; ++Frame)
	{
		TestWorld.Tick();
		MaxSeen = FMath::Max(MaxSeen, Setup.Spawner->GetAliveCount());
	}
	const TArray<FRRHordeSlot>& Plan = FSpawnerAccess::HordePlanOf(Setup.Spawner);
	const int32 PlanHeavies = Algo::CountIf(Plan, [](const FRRHordeSlot& Slot) { return Slot.Class == ARRHeavy::StaticClass(); });
	AddInfo(FString::Printf(TEXT("plan %d (heavies %d), max alive %d"), Plan.Num(), PlanHeavies, MaxSeen));
	TestEqual(TEXT("horde clamped to the free slots"), Plan.Num(), 25);
	TestEqual(TEXT("no heavy over the class cap"), PlanHeavies, 0);
	TestEqual(TEXT("cap of 30 reached, never exceeded"), MaxSeen, 30);
	TestEqual(TEXT("no wave at the cap"), Setup.Spawner->TrySpawnDirectorWave({ 0 }).Num(), 0);
	TestNull(TEXT("no lone robot at the cap"), Setup.Spawner->TrySpawnEnemyOfClass(ARRChaser::StaticClass()));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
