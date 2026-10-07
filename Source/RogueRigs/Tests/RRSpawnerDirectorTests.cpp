// Rogue Rigs - automation tests of the spawner wave director (RogueRigs.Spawner.Director*): phase times, shooter share over a run, alert trigger, pack composition with tough robots in front and shooters behind, horde front size and side, alive and per-class caps.

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

	// The phase rule before overtime (overtime from 3:30 has its own test, DirectorOvertime).
	FRRWaveDirectorConfig Config = FRRWaveDirectorConfig::MakeDefault();
	Config.Overtime.bEnabled = false;
	const int32 FirstChasers = Config.FindPackType(TEXT("FirstChasers"));
	const int32 Chasers = Config.FindPackType(TEXT("Chasers"));
	const int32 Heavies = Config.FindPackType(TEXT("Heavies"));
	const int32 ShieldChasers = Config.FindPackType(TEXT("ShieldChasers"));
	if (!TestTrue(TEXT("default pack types"), FirstChasers != INDEX_NONE && Chasers != INDEX_NONE && Heavies != INDEX_NONE && ShieldChasers != INDEX_NONE)
		|| !TestEqual(TEXT("two default hordes"), Config.Hordes.Num(), 2))
	{
		return false;
	}
	TestEqual(TEXT("no shooter-only pack type"), Config.FindPackType(TEXT("Shooters")), INDEX_NONE);
	TestEqual(TEXT("horde trigger is run time"), Config.HordeTrigger, ERRHordeTrigger::RunTime);
	const TArray<FTimedAction> Actions = RunDirectorSchedule(Config, 240.f, [](float) { return 0.f; }, 20261005);

	// Phases at 1, 15, 21, 60, 90 s (no calm; FlowShields and FlowWide keep the wave timer).
	const TArray<float> Phases = DirectorTimesOf(Actions, EType::PhaseChanged);
	const float ExpectedPhases[] = { 1.f, 15.f, 21.f, 60.f, 90.f };
	if (TestEqual(TEXT("5 phase changes"), Phases.Num(), 5))
	{
		for (int32 Index = 0; Index < 5; ++Index)
		{
			TestTrue(*FString::Printf(TEXT("phase %d at %.0f s (%.3f)"), Index, ExpectedPhases[Index], Phases[Index]), FMath::Abs(Phases[Index] - ExpectedPhases[Index]) <= TimeTolerance);
		}
	}

	// From 1 s: a lone chaser every 2 s (1, 3 ... 13 s) until the first pack at 15 s.
	const TArray<float> Singles = DirectorTimesOf(Actions, EType::SpawnSingle);
	const TArray<float> Waves = DirectorTimesOf(Actions, EType::SpawnWave);
	const float ExpectedSingles[] = { 1.f, 3.f, 5.f, 7.f, 9.f, 11.f, 13.f };
	if (TestEqual(TEXT("7 lone robots"), Singles.Num(), 7))
	{
		for (int32 Index = 0; Index < 7; ++Index)
		{
			// Three frames: the 1/60 s steps summed in float reach a 2 s interval one frame late now and then.
			TestTrue(*FString::Printf(TEXT("lone robot %d at %.0f s (%.3f)"), Index, ExpectedSingles[Index], Singles[Index]), FMath::Abs(Singles[Index] - ExpectedSingles[Index]) <= 3.f / 60.f);
		}
	}
	TestTrue(TEXT("no wave before the first pack"), Waves.IsEmpty() || Waves[0] >= 15.f - TimeTolerance);
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

	// Waves: first pack at 15 s (one pack of 6-8 chasers), flow from 21 s: 1-2 packs per wave until 1:30, 2-3 after, the
	// interval 6 s shrinking to 3.5 s over 210 s.
	constexpr float FlowStart = 21.f;
	constexpr float WideStart = 90.f;
	if (!TestTrue(TEXT("waves"), Waves.Num() > 20))
	{
		return false;
	}
	TestTrue(*FString::Printf(TEXT("first wave at 15 s (%.3f)"), Waves[0]), FMath::Abs(Waves[0] - 15.f) <= TimeTolerance);
	TestTrue(*FString::Printf(TEXT("flow starts at 21 s (%.3f)"), Waves[1]), FMath::Abs(Waves[1] - FlowStart) <= TimeTolerance);
	TestTrue(*FString::Printf(TEXT("second flow wave 6 s later (%.3f)"), Waves[2] - Waves[1]), FMath::Abs(Waves[2] - Waves[1] - 6.f) <= 0.1f);
	TSet<int32> TypesSeen;
	TSet<int32> PackCountsBeforeWide;
	TSet<int32> PackCountsAfterWide;
	int32 WaveIndex = 0;
	for (const FTimedAction& Timed : Actions)
	{
		if (Timed.Action.Type != EType::SpawnWave)
		{
			continue;
		}
		const int32 PackCount = Timed.Action.PackTypes.Num();
		if (WaveIndex == 0)
		{
			TestTrue(TEXT("first wave is one FirstChasers pack"), PackCount == 1 && Timed.Action.PackTypes[0] == FirstChasers);
		}
		else
		{
			const bool bWide = Timed.Time >= WideStart - TimeTolerance;
			(bWide ? PackCountsAfterWide : PackCountsBeforeWide).Add(PackCount);
			TestTrue(*FString::Printf(TEXT("wave at %.2f has %s packs (%d)"), Timed.Time, bWide ? TEXT("2-3") : TEXT("1-2"), PackCount),
				bWide ? PackCount >= 2 && PackCount <= 3 : PackCount >= 1 && PackCount <= 2);
			for (const int32 Type : Timed.Action.PackTypes)
			{
				TestTrue(TEXT("flow packs are Chasers or ShieldChasers, or Heavies"), Type == Chasers || Type == ShieldChasers || Type == Heavies);
				if (Type == Chasers || Type == ShieldChasers)
				{
					TestEqual(*FString::Printf(TEXT("chaser pack at %.2f s is %s"), Timed.Time, Timed.Time < 60.f ? TEXT("Chasers before 1:00") : TEXT("ShieldChasers from 1:00")),
						Type, Timed.Time < 60.f ? Chasers : ShieldChasers);
				}
				TypesSeen.Add(Type);
			}
		}
		++WaveIndex;
	}
	TestEqual(TEXT("the flow draws all three pack types (Chasers, ShieldChasers, Heavies)"), TypesSeen.Num(), 3);
	TestTrue(TEXT("waves of 1 and of 2 packs before 1:30"), PackCountsBeforeWide.Contains(1) && PackCountsBeforeWide.Contains(2));
	TestTrue(TEXT("waves of 2 and of 3 packs after 1:30"), PackCountsAfterWide.Contains(2) && PackCountsAfterWide.Contains(3));
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
		TestTrue(*FString::Printf(TEXT("gap at %.1f s within 3.5-6 s (%.2f)"), Waves[Index], Gap), Gap >= 3.5f - TimeTolerance && Gap <= 6.f + TimeTolerance);
		TestTrue(*FString::Printf(TEXT("gap at %.1f s not longer than the previous"), Waves[Index]), Gap <= PreviousGap + TimeTolerance);
		if (Waves[Index - 1] >= FlowStart + 210.f)
		{
			TestTrue(*FString::Printf(TEXT("gap after 3 min 30 of flow is 3.5 s (%.2f)"), Gap), FMath::Abs(Gap - 3.5f) <= TimeTolerance);
		}
		PreviousGap = Gap;
	}
	AddInfo(FString::Printf(TEXT("wave gaps (h = across a horde):%s"), *GapLog));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorShooterShareTest, "RogueRigs.Spawner.DirectorShooterShare",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorShooterShareTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;
	using EType = FRRDirectorAction::EType;

	// Shooters never form a pack of their own: every pack type holding shooters holds 1-2 of them, behind, among other robots.
	const FRRWaveDirectorConfig Config = FRRWaveDirectorConfig::MakeDefault();
	for (const FRRDirectorPackType& Type : Config.PackTypes)
	{
		int32 ShooterMax = 0;
		int32 OtherMin = 0;
		for (const FRRDirectorPackMember& Member : Type.Members)
		{
			const bool bShooter = Member.RobotClass && Member.RobotClass->IsChildOf(ARRShooter::StaticClass());
			if (bShooter)
			{
				ShooterMax += Member.CountMax;
				TestTrue(*FString::Printf(TEXT("%s: 1-2 shooters (%d-%d)"), *Type.Name.ToString(), Member.CountMin, Member.CountMax), Member.CountMin >= 1 && Member.CountMax <= 2);
				TestTrue(*FString::Printf(TEXT("%s: shooters ride behind, not in front"), *Type.Name.ToString()), Member.bBehind && !Member.bFront);
			}
			else
			{
				OtherMin += Member.CountMin;
			}
		}
		TestTrue(*FString::Printf(TEXT("%s: not a shooter-only pack"), *Type.Name.ToString()), ShooterMax == 0 || OtherMin > ShooterMax);
	}

	// Over simulated runs of 3 min 30 (the alert bar fills in about 4 min, kills shorten it), the drawn robots: lone robots,
	// pack members drawn within their ranges, horde fronts (40-60, no shooter). Caps are ignored: no world here.
	constexpr float RunSeconds = 210.f;
	const int32 Seeds[] = { 20261005, 7, 424242 };
	for (const int32 Seed : Seeds)
	{
		const TArray<FTimedAction> Actions = RunDirectorSchedule(Config, RunSeconds, [](float) { return 0.f; }, Seed);
		FRandomStream Draw(Seed + 1);
		int32 Shooters = 0;
		int32 NonHorde = 0;
		int32 Horde = 0;
		int32 Packs = 0;
		int32 PacksWithShooters = 0;
		for (const FTimedAction& Timed : Actions)
		{
			if (Timed.Action.Type == EType::SpawnSingle)
			{
				++NonHorde;
				Shooters += Timed.Action.SingleClass && Timed.Action.SingleClass->IsChildOf(ARRShooter::StaticClass()) ? 1 : 0;
			}
			else if (Timed.Action.Type == EType::SpawnWave)
			{
				for (const int32 TypeIndex : Timed.Action.PackTypes)
				{
					int32 PackShooters = 0;
					for (const FRRDirectorPackMember& Member : Config.PackTypes[TypeIndex].Members)
					{
						const int32 Count = Draw.RandRange(Member.CountMin, FMath::Max(Member.CountMin, Member.CountMax));
						NonHorde += Count;
						PackShooters += Member.RobotClass && Member.RobotClass->IsChildOf(ARRShooter::StaticClass()) ? Count : 0;
					}
					Shooters += PackShooters;
					++Packs;
					PacksWithShooters += PackShooters > 0 ? 1 : 0;
				}
			}
			else if (Timed.Action.Type == EType::HordeStarted)
			{
				const FRRDirectorHorde& Front = Config.Hordes[Timed.Action.Index];
				Horde += Draw.RandRange(Front.SizeMin, FMath::Max(Front.SizeMin, Front.SizeMax));
			}
		}
		const float Share = NonHorde > 0 ? static_cast<float>(Shooters) / NonHorde : 0.f;
		const float ShareWithHordes = NonHorde + Horde > 0 ? static_cast<float>(Shooters) / (NonHorde + Horde) : 0.f;
		AddInfo(FString::Printf(TEXT("DIRECTORSHARE seed %d: %d shooters of %d robots outside hordes (%.1f %%), %.1f %% with the %d horde robots; %d of %d packs carry shooters"),
			Seed, Shooters, NonHorde, Share * 100.f, ShareWithHordes * 100.f, Horde, PacksWithShooters, Packs));
		TestTrue(*FString::Printf(TEXT("seed %d: shooters about 10 %% of the robots outside hordes (%.1f %%)"), Seed, Share * 100.f), Share >= 0.08f && Share <= 0.12f);
	}
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

			// Behind members (shooters riding in a pack): in the back half of the mixed robots, never in the front row.
			TArray<float> Distances;
			int32 FrontRobots = 0;
			for (int32 Index = 0; Index < Pack.Robots.Num(); ++Index)
			{
				Distances.Add(DirectorViewDistance(Setup, Pack.Cells[Index]));
				FrontRobots += PackType.Members.ContainsByPredicate([Robot = Pack.Robots[Index]](const FRRDirectorPackMember& Member)
					{ return Member.bFront && Robot->GetClass() == Member.RobotClass.Get(); }) ? 1 : 0;
			}
			Distances.Sort();
			const int32 BackHalfIndex = Distances.IsEmpty() ? 0 : FMath::Min(FrontRobots + (Pack.Robots.Num() - FrontRobots) / 2, Distances.Num() - 1);
			for (int32 Index = 0; Index < Pack.Robots.Num(); ++Index)
			{
				const bool bBehind = PackType.Members.ContainsByPredicate([Robot = Pack.Robots[Index]](const FRRDirectorPackMember& Member)
					{ return Member.bBehind && Robot->GetClass() == Member.RobotClass.Get(); });
				if (!bBehind)
				{
					continue;
				}
				const float Distance = Distances.IsEmpty() ? 0.f : DirectorViewDistance(Setup, Pack.Cells[Index]);
				TestTrue(*FString::Printf(TEXT("%s: %s behind the front row (%.0f > %.0f)"), *Label, *Pack.Robots[Index]->GetClass()->GetName(), Distance, Distances[0]),
					Distance > Distances[0]);
				TestTrue(*FString::Printf(TEXT("%s: %s in the back half (%.0f >= %.0f)"), *Label, *Pack.Robots[Index]->GetClass()->GetName(), Distance, Distances[BackHalfIndex]),
					Distance >= Distances[BackHalfIndex]);
			}

			// One side, in the band, distinct spawn cells (a heavy may be nudged off its cell by its neighbours).
			TSet<FIntPoint> Cells;
			FIntRect Bounds(TNumericLimits<int32>::Max(), TNumericLimits<int32>::Max(), TNumericLimits<int32>::Min(), TNumericLimits<int32>::Min());
			TestEqual(*FString::Printf(TEXT("%s: one cell per robot"), *Label), Pack.Cells.Num(), Pack.Robots.Num());
			for (const FIntPoint& Cell : Pack.Cells)
			{
				TestEqual(*FString::Printf(TEXT("%s: (%d, %d) on the pack side"), *Label, Cell.X, Cell.Y), DirectorSideOf(Setup, Cell), Pack.Side);
				const float Distance = DirectorViewDistance(Setup, Cell);
				TestTrue(*FString::Printf(TEXT("%s: (%d, %d) in the spawn band"), *Label, Cell.X, Cell.Y), Distance >= 100.f && Distance <= 500.f);
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
	const TArray<FRRSpawnedPack> Wave = Setup.Spawner->TrySpawnDirectorWave({ Config.FindPackType(TEXT("Chasers")), Config.FindPackType(TEXT("ShieldChasers")),
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
	// Recycling off: this test counts spawns through the alive count; far frozen robots would be recycled (task-034-01 margin 0).
	RRTestAccess::FSpawnerAccess::RecycleEnabledOf(Setup.Spawner) = false;
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorShooterCapTest, "RogueRigs.Spawner.DirectorShooterCap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorShooterCapTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;
	using namespace RRTestAccess;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRDirectorShooterCap"));
	FDirectorSetup Setup;
	if (!MakeDirectorSetup(*this, TestWorld, 150, Setup))
	{
		return false;
	}
	FRRWaveDirectorConfig& Config = FSpawnerAccess::DirectorOf(Setup.Spawner);
	TestEqual(TEXT("default shooter cap 6"), Config.ShooterMaxAlive, 6);
	TestTrue(TEXT("default replacement is the chaser"), Config.ShooterCapReplacement.Get() == ARRChaser::StaticClass());
	const int32 ChasersType = Config.FindPackType(TEXT("Chasers"));
	if (!TestTrue(TEXT("Chasers pack type"), ChasersType != INDEX_NONE))
	{
		return false;
	}

	// Counts by class of one Chasers pack drawn with the stream seeded with Seed; the pack is destroyed afterwards.
	struct FPackCounts
	{
		int32 Total = 0;
		int32 Shooters = 0;
		int32 Chasers = 0;
		int32 ShootersInBackHalf = 0;
	};
	auto SpawnPack = [this, &Setup, ChasersType](int32 Seed, bool bDestroy)
	{
		FPackCounts Counts;
		Setup.Spawner->SeedRandomStream(Seed);
		const TArray<FRRSpawnedPack> Packs = Setup.Spawner->TrySpawnDirectorWave({ ChasersType });
		if (!TestEqual(TEXT("one pack"), Packs.Num(), 1))
		{
			return Counts;
		}
		const FRRSpawnedPack& Pack = Packs[0];
		Counts.Total = Pack.Robots.Num();
		for (int32 Index = 0; Index < Pack.Robots.Num(); ++Index)
		{
			const bool bShooter = Pack.Robots[Index]->IsA<ARRShooter>();
			Counts.Shooters += bShooter ? 1 : 0;
			Counts.Chasers += Pack.Robots[Index]->GetClass() == ARRChaser::StaticClass() ? 1 : 0;
			Counts.ShootersInBackHalf += bShooter && Index >= Pack.Robots.Num() / 2 ? 1 : 0;
		}
		if (bDestroy)
		{
			for (ARREnemy* Robot : Pack.Robots)
			{
				Robot->Destroy();
			}
		}
		return Counts;
	};

	// Same seed, cap off then cap reached: the same robots, the shooters turned into chasers.
	const int32 Seeds[] = { 20261006, 11, 4242 };
	for (const int32 Seed : Seeds)
	{
		Config.ShooterMaxAlive = 100;
		const FPackCounts Free = SpawnPack(Seed, true);
		Config.ShooterMaxAlive = 0;
		const FPackCounts Capped = SpawnPack(Seed, true);
		AddInfo(FString::Printf(TEXT("SHOOTERCAP seed %d: free %d robots (%d shooters, %d chasers); at the cap %d robots (%d shooters, %d chasers)"),
			Seed, Free.Total, Free.Shooters, Free.Chasers, Capped.Total, Capped.Shooters, Capped.Chasers));
		TestTrue(*FString::Printf(TEXT("seed %d: 1-2 shooters without the cap"), Seed), Free.Shooters >= 1 && Free.Shooters <= 2);
		TestEqual(*FString::Printf(TEXT("seed %d: shooters in the back half"), Seed), Free.ShootersInBackHalf, Free.Shooters);
		TestEqual(*FString::Printf(TEXT("seed %d: no shooter at the cap"), Seed), Capped.Shooters, 0);
		TestEqual(*FString::Printf(TEXT("seed %d: same pack size"), Seed), Capped.Total, Free.Total);
		TestEqual(*FString::Printf(TEXT("seed %d: chasers in place of the shooters"), Seed), Capped.Chasers, Free.Chasers + Free.Shooters);
	}

	// Default cap 6: 5 shooters alive leave room for one; packs kept alive never raise the count above 6.
	Config.ShooterMaxAlive = 6;
	for (int32 Index = 0; Index < 5; ++Index)
	{
		TestNotNull(*FString::Printf(TEXT("lone shooter %d"), Index), Setup.Spawner->TrySpawnEnemyOfClass(ARRShooter::StaticClass()));
	}
	int32 PackShooters = 0;
	for (int32 Repeat = 0; Repeat < 4; ++Repeat)
	{
		PackShooters += SpawnPack(777 + Repeat, false).Shooters;
		TestTrue(*FString::Printf(TEXT("pack %d: at most 6 shooters alive"), Repeat), Setup.Spawner->GetAliveCountOfClass(ARRShooter::StaticClass()) <= 6);
	}
	TestEqual(TEXT("the packs add one shooter in all"), PackShooters, 1);
	TestEqual(TEXT("6 shooters alive"), Setup.Spawner->GetAliveCountOfClass(ARRShooter::StaticClass()), 6);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerDirectorOvertimeTest, "RogueRigs.Spawner.DirectorOvertime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerDirectorOvertimeTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerDirectorTests;
	using EType = FRRDirectorAction::EType;

	// Defaults (difficulty gdd, owner 2026-10-06 third test, provisional): from 3:30 a horde every 30 s, the wave interval
	// halved at each overtime minute begun.
	const FRRWaveDirectorConfig Config = FRRWaveDirectorConfig::MakeDefault();
	TestTrue(TEXT("overtime on"), Config.Overtime.bEnabled);
	TestEqual(TEXT("overtime from 210 s (3:30)"), Config.Overtime.StartTime, 210.f);
	TestEqual(TEXT("a horde every 30 s"), Config.Overtime.HordeInterval, 30.f);
	TestEqual(TEXT("steps of 60 s"), Config.Overtime.StepSeconds, 60.f);
	TestEqual(TEXT("interval x 0.5 per step"), Config.Overtime.WaveIntervalMultiplier, 0.5f);
	TestEqual(TEXT("overtime horde = the timed horde front (40-60)"), Config.Overtime.Horde.SizeMin, Config.Hordes[0].SizeMin);
	TestEqual(TEXT("first overtime horde front at 215 s (lull from 210 s)"), Config.GetHordeFrontTime(Config.Hordes.Num()), 215.f);
	TestEqual(TEXT("second overtime horde front at 245 s"), Config.GetHordeFrontTime(Config.Hordes.Num() + 1), 245.f);

	// Wave interval along the run (one scheduler, sampled each frame).
	FRRWaveDirector Director;
	Director.Reset();
	FRandomStream Stream(20261006);
	TArray<FRRDirectorAction> Frame;
	TArray<FTimedAction> Actions;
	TMap<int32, float> IntervalAt;
	int32 LastSecond = -1;
	const int32 Frames = FMath::RoundToInt(345.f * 60.f);
	for (int32 Index = 0; Index < Frames; ++Index)
	{
		Frame.Reset();
		Director.Advance(Config, 1.f / 60.f, 0.f, Stream, Frame);
		for (const FRRDirectorAction& Action : Frame)
		{
			Actions.Add({ Director.GetRunTime(), Action });
		}
		// First frame of each second of run time.
		const int32 Second = FMath::FloorToInt(Director.GetRunTime());
		if (Second != LastSecond)
		{
			LastSecond = Second;
			IntervalAt.Add(Second, Director.GetCurrentWaveInterval(Config));
		}
	}
	const float Before = IntervalAt.FindRef(209);
	const float FirstMinute = IntervalAt.FindRef(211);
	const float SecondMinute = IntervalAt.FindRef(271);
	const float ThirdMinute = IntervalAt.FindRef(331);
	AddInfo(FString::Printf(TEXT("wave interval 209 s %.3f, 211 s %.3f, 271 s %.3f, 331 s %.3f"), Before, FirstMinute, SecondMinute, ThirdMinute));
	TestNearlyEqual(TEXT("unchanged at 209 s (before overtime)"), Before, FRRWaveDirector::GetWaveInterval(Config.Phases.Last(), 209.f - Config.Phases.Last().StartTime), 0.02f);
	TestNearlyEqual(TEXT("halved at 3:30"), FirstMinute, FRRWaveDirector::GetWaveInterval(Config.Phases.Last(), 211.f - Config.Phases.Last().StartTime) * 0.5f, 0.02f);
	TestNearlyEqual(TEXT("quartered at 4:30"), SecondMinute, FRRWaveDirector::GetWaveInterval(Config.Phases.Last(), 271.f - Config.Phases.Last().StartTime) * 0.25f, 0.02f);
	TestNearlyEqual(TEXT("an eighth at 5:30"), ThirdMinute, FRRWaveDirector::GetWaveInterval(Config.Phases.Last(), 331.f - Config.Phases.Last().StartTime) * 0.125f, 0.02f);

	const TArray<float> Overtime = DirectorTimesOf(Actions, EType::OvertimeStarted);
	TestTrue(TEXT("one OvertimeStarted at 210 s"), Overtime.Num() == 1 && FMath::Abs(Overtime[0] - 210.f) <= TimeTolerance);
	// Hordes: the two timed ones, then overtime fronts at 215, 245, 275, 305, 335 s.
	const TArray<float> Fronts = DirectorTimesOf(Actions, EType::HordeStarted);
	const TArray<float> Lulls = DirectorTimesOf(Actions, EType::HordeLull);
	AddInfo(FString::Printf(TEXT("horde lulls at %s; fronts at %s"),
		*FString::JoinBy(Lulls, TEXT(", "), [](float Time) { return FString::Printf(TEXT("%.2f"), Time); }),
		*FString::JoinBy(Fronts, TEXT(", "), [](float Time) { return FString::Printf(TEXT("%.2f"), Time); })));
	const float Expected[] = { 95.f, 175.f, 215.f, 245.f, 275.f, 305.f, 335.f };
	if (TestEqual(TEXT("seven fronts in 345 s"), Fronts.Num(), 7))
	{
		for (int32 Index = 0; Index < 7; ++Index)
		{
			TestTrue(*FString::Printf(TEXT("front %d at %.0f s"), Index, Expected[Index]), FMath::Abs(Fronts[Index] - Expected[Index]) <= TimeTolerance);
		}
	}
	TestTrue(TEXT("announced (banner) for every overtime horde"), DirectorTimesOf(Actions, EType::HordeAnnounced).Num() == 7);

	// Waves: more per minute in overtime than in the minute before it, despite the horde lulls.
	auto CountWaves = [&Actions](float From, float To)
	{
		return Algo::CountIf(Actions, [From, To](const FTimedAction& Timed) { return Timed.Action.Type == EType::SpawnWave && Timed.Time >= From && Timed.Time < To; });
	};
	const int32 WavesBefore = CountWaves(150.f, 210.f);
	const int32 WavesFirst = CountWaves(210.f, 270.f);
	const int32 WavesSecond = CountWaves(270.f, 330.f);
	AddInfo(FString::Printf(TEXT("waves 150-210 s: %d, 210-270 s: %d, 270-330 s: %d"), WavesBefore, WavesFirst, WavesSecond));
	TestTrue(TEXT("more waves in the first overtime minute"), WavesFirst > WavesBefore);
	TestTrue(TEXT("more waves again in the second"), WavesSecond > WavesFirst);

	// Off: no overtime action, no extra horde.
	FRRWaveDirectorConfig Off = Config;
	Off.Overtime.bEnabled = false;
	const TArray<FTimedAction> OffActions = RunDirectorSchedule(Off, 300.f, [](float) { return 0.f; }, 20261006);
	TestEqual(TEXT("overtime off: no OvertimeStarted"), DirectorTimesOf(OffActions, EType::OvertimeStarted).Num(), 0);
	TestEqual(TEXT("overtime off: two fronts"), DirectorTimesOf(OffActions, EType::HordeStarted).Num(), 2);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
