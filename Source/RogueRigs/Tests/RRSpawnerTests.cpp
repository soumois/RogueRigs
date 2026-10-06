// Rogue Rigs - automation tests of the enemy spawner (RogueRigs.Spawner.*): spawn cells in the band outside the view and reachable, alive cap, freed slot, trickle then packs, packs from one side, kamikaze share, run-heading sector split, heading from movement, first contact of a still miner.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRKamikaze.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GAS/RRGameplayTags.h"
#include "HAL/IConsoleManager.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRSpawnerTests
{
	using namespace RRTestHarness;

	/** Miner cell on arena_m1_fine (open floor; fine cell of the legacy (40, 40)). The spawn band is counted in cells, so these tests run on the game grid. */
	const FIntPoint MinerCell = FineCell(FIntPoint(40, 40));

	/** Spawn band beyond the view edge, cell center (uu): SpawnBandMinDistance to SpawnBandMaxDistance (task-031-02). */
	constexpr float BandMin = 100.f;
	constexpr float BandMax = 500.f;

	/** Half extent (uu) of the fixed view rectangle centered on the miner. */
	const FVector2D ViewHalfExtent(1600.f, 1000.f);

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** World with the grid, an unarmed miner on MinerCell and a native-chaser spawner using the fixed view rectangle. */
	struct FSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARREnemySpawner* Spawner = nullptr;
		FBox2D View = FBox2D(ForceInit);
	};

	bool MakeSetup(FAutomationTestBase& Test, const FTestWorld& TestWorld, int32 MaxAlive, float Interval, bool bEnabled, FSetup& Out)
	{
		Out.Grid = SpawnFineGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? SpawnMiner(Test, TestWorld, Out.Grid, MinerCell) : nullptr;
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
		Test.TestEqual(TEXT("default SpawnInterval 2 s"), FSpawnerAccess::SpawnIntervalOf(Out.Spawner), 2.f);
		Test.TestEqual(TEXT("default MaxAlive 150"), FSpawnerAccess::MaxAliveOf(Out.Spawner), 150);
		Test.TestEqual(TEXT("default TrickleDuration 30 s"), FSpawnerAccess::TrickleDurationOf(Out.Spawner), 30.f);
		Test.TestEqual(TEXT("default PackInterval 6 s"), FSpawnerAccess::PackIntervalOf(Out.Spawner), 6.f);
		Test.TestEqual(TEXT("default PackSizeMin 8"), FSpawnerAccess::PackSizeMinOf(Out.Spawner), 8);
		Test.TestEqual(TEXT("default PackSizeMax 15"), FSpawnerAccess::PackSizeMaxOf(Out.Spawner), 15);
		Test.TestEqual(TEXT("default LoneSpawnChance 0.1"), FSpawnerAccess::LoneSpawnChanceOf(Out.Spawner), 0.1f);
		Test.TestEqual(TEXT("default SpawnBandMinDistance 100 uu"), FSpawnerAccess::BandMinOf(Out.Spawner), BandMin);
		Test.TestEqual(TEXT("default SpawnBandMaxDistance 500 uu"), FSpawnerAccess::BandMaxOf(Out.Spawner), BandMax);
		FSpawnerAccess::EnemyClassOf(Out.Spawner) = ARRChaser::StaticClass();
		FSpawnerAccess::MaxAliveOf(Out.Spawner) = MaxAlive;
		FSpawnerAccess::SpawnIntervalOf(Out.Spawner) = Interval;
		Out.Spawner->SetSpawningEnabled(bEnabled);
		const FVector MinerLocation = Out.Miner->GetActorLocation();
		const FVector2D Center(MinerLocation.X, MinerLocation.Y);
		Out.View = FBox2D(Center - ViewHalfExtent, Center + ViewHalfExtent);
		Out.Spawner->SetViewRectangleOverride(Out.View);
		return true;
	}

	/** Checks that every robot of Pack stands on a distinct candidate-band cell beyond the same side of View; returns that side. */
	ERRSpawnSide CheckPackOnOneSide(FAutomationTestBase& Test, const FSetup& Setup, const TArray<ARREnemy*>& Pack, const TCHAR* Label)
	{
		ERRSpawnSide PackSide = ERRSpawnSide::None;
		TSet<FIntPoint> Cells;
		for (const ARREnemy* Enemy : Pack)
		{
			const FIntPoint Cell = Setup.Grid->GetCellAtWorld(Enemy->GetActorLocation());
			const FVector Center = Setup.Grid->GetCellCenter(Cell.X, Cell.Y);
			const FVector2D Point(Center.X, Center.Y);
			const ERRSpawnSide Side = ARREnemySpawner::GetSideOfRectangle(Setup.View, Point);
			PackSide = PackSide == ERRSpawnSide::None ? Side : PackSide;
			Test.TestTrue(*FString::Printf(TEXT("%s: (%d, %d) on the pack side"), Label, Cell.X, Cell.Y), Side == PackSide && Side != ERRSpawnSide::None);
			// The cell really lies beyond that edge.
			const bool bBeyond = (Side == ERRSpawnSide::NegativeX && Point.X < Setup.View.Min.X) || (Side == ERRSpawnSide::PositiveX && Point.X > Setup.View.Max.X)
				|| (Side == ERRSpawnSide::NegativeY && Point.Y < Setup.View.Min.Y) || (Side == ERRSpawnSide::PositiveY && Point.Y > Setup.View.Max.Y);
			Test.TestTrue(*FString::Printf(TEXT("%s: (%d, %d) beyond its edge"), Label, Cell.X, Cell.Y), bBeyond);
			const float Distance = ARREnemySpawner::GetDistanceToRectangle(Setup.View, Point);
			Test.TestTrue(*FString::Printf(TEXT("%s: (%d, %d) in the spawn band"), Label, Cell.X, Cell.Y), Distance >= BandMin && Distance <= BandMax);
			Test.TestFalse(*FString::Printf(TEXT("%s: (%d, %d) distinct"), Label, Cell.X, Cell.Y), Cells.Contains(Cell));
			Cells.Add(Cell);
		}
		return PackSide;
	}

	/** Applies a URRGE_DamageFlat of Damage made by Source to Target. */
	void ApplyFlatDamage(UAbilitySystemComponent* Source, UAbilitySystemComponent* Target, float Damage)
	{
		FGameplayEffectSpecHandle Spec = Source->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, Source->MakeEffectContext());
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Damage);
		Source->ApplyGameplayEffectSpecToTarget(*Spec.Data, Target);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerOutsideViewTest, "RogueRigs.Spawner.SpawnsOutsideViewAndReachable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerOutsideViewTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerOutsideView"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 20, 2.f, false, Setup))
	{
		return false;
	}
	URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
	const float FloorZ = Setup.Grid->GetActorLocation().Z;

	// The headless view: the miner's camera projected on the floor (no player controller here).
	Setup.Spawner->ClearViewRectangleOverride();
	FBox2D CameraView;
	if (TestTrue(TEXT("view rectangle from the miner camera"), Setup.Spawner->GetViewRectangle(CameraView)))
	{
		const FVector MinerLocation = Setup.Miner->GetActorLocation();
		AddInfo(FString::Printf(TEXT("miner camera floor rectangle X [%.0f, %.0f] Y [%.0f, %.0f] (miner at %.0f, %.0f)"),
			CameraView.Min.X, CameraView.Max.X, CameraView.Min.Y, CameraView.Max.Y, MinerLocation.X, MinerLocation.Y));
		TestTrue(TEXT("camera rectangle contains the miner"), CameraView.IsInside(FVector2D(MinerLocation.X, MinerLocation.Y)));
	}
	Setup.Spawner->SetViewRectangleOverride(Setup.View);

	TArray<FIntPoint> Candidates;
	Setup.Spawner->GetSpawnCandidates(Candidates);
	AddInfo(FString::Printf(TEXT("%d candidate cells around the fixed rectangle"), Candidates.Num()));
	TestTrue(TEXT("candidates exist"), Candidates.Num() >= 20);

	TSet<FIntPoint> Used;
	for (int32 Index = 0; Index < 20; ++Index)
	{
		ARREnemy* Enemy = Setup.Spawner->TrySpawnEnemy();
		if (!TestNotNull(*FString::Printf(TEXT("spawn %d"), Index), Enemy))
		{
			return false;
		}
		const FVector Location = Enemy->GetActorLocation();
		const FIntPoint Cell = Setup.Grid->GetCellAtWorld(Location);
		const FVector Center = Setup.Grid->GetCellCenter(Cell.X, Cell.Y);
		// The cell center lies 100 to 500 uu beyond the rectangle edge.
		const float Distance = ARREnemySpawner::GetDistanceToRectangle(Setup.View, FVector2D(Center.X, Center.Y));
		TestTrue(*FString::Printf(TEXT("spawn %d (%d, %d) at least 100 uu outside the view (%.0f uu)"), Index, Cell.X, Cell.Y, Distance),
			Distance >= BandMin);
		TestTrue(*FString::Printf(TEXT("spawn %d within 500 uu of the view edge"), Index), Distance <= BandMax);
		TestTrue(*FString::Printf(TEXT("spawn %d on a walkable cell"), Index), FlowField->IsWalkableCell(Cell));
		TestTrue(*FString::Printf(TEXT("spawn %d reachable"), Index), FlowField->GetCellDistance(Cell) > 0);
		TestFalse(*FString::Printf(TEXT("spawn %d on a free cell"), Index), Used.Contains(Cell));
		Used.Add(Cell);
		TestTrue(*FString::Printf(TEXT("spawn %d at the cell center"), Index), FVector2D(Location - Center).IsNearlyZero(1.f));
		const float Feet = Location.Z - Enemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		TestTrue(*FString::Printf(TEXT("spawn %d capsule rests on the floor (feet %.1f)"), Index, Feet - FloorZ),
			Feet >= FloorZ && Feet <= FloorZ + 5.f);
	}
	TestEqual(TEXT("20 alive"), Setup.Spawner->GetAliveCount(), 20);
	TestNull(TEXT("no spawn at MaxAlive"), Setup.Spawner->TrySpawnEnemy());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerMaxAliveTest, "RogueRigs.Spawner.MaxAliveCap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerMaxAliveTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerMaxAlive"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 5, 0.1f, true, Setup))
	{
		return false;
	}
	// Older timers (director off): one robot every 0.1 s.
	FSpawnerAccess::DirectorOf(Setup.Spawner).bEnabled = false;

	// rr.Spawner.Enabled 0 stops the timed spawning.
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("rr.Spawner.Enabled"));
	if (!TestNotNull(TEXT("rr.Spawner.Enabled exists"), CVar))
	{
		return false;
	}
	CVar->Set(0, ECVF_SetByCode);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("no spawn while rr.Spawner.Enabled is 0"), Setup.Spawner->GetAliveCount(), 0);
	CVar->Set(1, ECVF_SetByCode);

	int32 MaxSeen = 0;
	int32 FirstSpawnFrame = INDEX_NONE;
	int32 CapFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 120; ++Frame)
	{
		TestWorld.Tick();
		const int32 Alive = Setup.Spawner->GetAliveCount();
		if (Alive > 0 && FirstSpawnFrame == INDEX_NONE)
		{
			FirstSpawnFrame = Frame;
		}
		if (Alive == 5 && CapFrame == INDEX_NONE)
		{
			CapFrame = Frame;
		}
		MaxSeen = FMath::Max(MaxSeen, Alive);
	}
	AddInfo(FString::Printf(TEXT("first spawn at frame %d, cap of 5 reached at frame %d, max alive %d"), FirstSpawnFrame, CapFrame, MaxSeen));
	TestTrue(TEXT("first spawn after one 0.1 s interval (frame 6 or 7)"), FirstSpawnFrame == 6 || FirstSpawnFrame == 7);
	TestEqual(TEXT("alive count reaches MaxAlive"), MaxSeen, 5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerFreedSlotTest, "RogueRigs.Spawner.KilledRobotFreesSlot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerFreedSlotTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerFreedSlot"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 2, 2.f, false, Setup))
	{
		return false;
	}
	ARREnemy* First = Setup.Spawner->TrySpawnEnemy();
	ARREnemy* Second = Setup.Spawner->TrySpawnEnemy();
	if (!TestNotNull(TEXT("first spawn"), First) || !TestNotNull(TEXT("second spawn"), Second))
	{
		return false;
	}
	TestNull(TEXT("full at 2"), Setup.Spawner->TrySpawnEnemy());

	ApplyFlatDamage(Setup.Miner->GetAbilitySystemComponent(), First->GetAbilitySystemComponent(), 1000.f);
	TestTrue(TEXT("first robot dead"), First->IsDead());
	TestEqual(TEXT("a killed robot frees its slot at once"), Setup.Spawner->GetAliveCount(), 1);
	ARREnemy* Third = Setup.Spawner->TrySpawnEnemy();
	TestNotNull(TEXT("a new robot fills the freed slot"), Third);
	TestEqual(TEXT("2 alive again"), Setup.Spawner->GetAliveCount(), 2);

	// The dead robot is destroyed after its shrink; it must not free a second slot.
	TWeakObjectPtr<ARREnemy> WeakFirst(First);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestTrue(TEXT("dead robot destroyed"), !WeakFirst.IsValid() || WeakFirst->IsActorBeingDestroyed());
	TestEqual(TEXT("still 2 alive after the destroy"), Setup.Spawner->GetAliveCount(), 2);

	// A destroyed (not killed) robot frees a slot too.
	Second->Destroy();
	TestEqual(TEXT("a destroyed robot frees its slot"), Setup.Spawner->GetAliveCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerTrickleThenPacksTest, "RogueRigs.Spawner.TrickleThenPacks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerTrickleThenPacksTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerTrickle"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 100, 1.f, false, Setup))
	{
		return false;
	}
	// Older timers (director off). Short phases: 6 s of trickle (one robot per second), then a pack every 3 s with a sure lone robot at +1 and +2 s.
	FSpawnerAccess::DirectorOf(Setup.Spawner).bEnabled = false;
	FSpawnerAccess::TrickleDurationOf(Setup.Spawner) = 6.f;
	FSpawnerAccess::PackIntervalOf(Setup.Spawner) = 3.f;
	FSpawnerAccess::LoneSpawnChanceOf(Setup.Spawner) = 1.f;
	Setup.Spawner->SeedRandomStream(20261003);
	Setup.Spawner->SetSpawningEnabled(true);

	struct FEvent
	{
		int32 Frame;
		int32 Delta;
		bool bPackPhase;
	};
	TArray<FEvent> Events;
	int32 PackPhaseFrame = INDEX_NONE;
	int32 Previous = 0;
	for (int32 Frame = 1; Frame <= 750; ++Frame)
	{
		TestWorld.Tick();
		const int32 Alive = Setup.Spawner->GetAliveCount();
		if (Setup.Spawner->IsInPackPhase() && PackPhaseFrame == INDEX_NONE)
		{
			PackPhaseFrame = Frame;
		}
		if (Alive != Previous)
		{
			Events.Add({ Frame, Alive - Previous, Setup.Spawner->IsInPackPhase() });
			Previous = Alive;
		}
	}

	FString Log;
	for (const FEvent& Event : Events)
	{
		Log += FString::Printf(TEXT(" f%d:%+d%s"), Event.Frame, Event.Delta, Event.bPackPhase ? TEXT("p") : TEXT(""));
	}
	AddInfo(FString::Printf(TEXT("pack phase from frame %d; alive changes:%s"), PackPhaseFrame, *Log));
	TestTrue(TEXT("pack phase starts after 6 s (frame 360 or 361)"), PackPhaseFrame == 360 || PackPhaseFrame == 361);

	int32 TrickleSpawns = 0;
	TArray<FEvent> PackPhaseEvents;
	for (const FEvent& Event : Events)
	{
		if (Event.bPackPhase)
		{
			PackPhaseEvents.Add(Event);
			continue;
		}
		++TrickleSpawns;
		TestEqual(*FString::Printf(TEXT("trickle spawn at frame %d is a single robot"), Event.Frame), Event.Delta, 1);
	}
	TestTrue(TEXT("5 or 6 trickle spawns in 6 s at 1 s"), TrickleSpawns == 5 || TrickleSpawns == 6);

	// Expected after the trickle: pack, lone, lone, pack, lone, lone, pack.
	const bool Pattern[] = { true, false, false, true, false, false, true };
	if (!TestEqual(TEXT("7 spawn events in the pack phase"), PackPhaseEvents.Num(), static_cast<int32>(UE_ARRAY_COUNT(Pattern))))
	{
		return false;
	}
	TArray<int32> PackFrames;
	for (int32 Index = 0; Index < PackPhaseEvents.Num(); ++Index)
	{
		const FEvent& Event = PackPhaseEvents[Index];
		if (Pattern[Index])
		{
			PackFrames.Add(Event.Frame);
			TestTrue(*FString::Printf(TEXT("pack at frame %d has 8 to 15 robots (%d)"), Event.Frame, Event.Delta), Event.Delta >= 8 && Event.Delta <= 15);
		}
		else
		{
			TestEqual(*FString::Printf(TEXT("lone robot at frame %d"), Event.Frame), Event.Delta, 1);
		}
	}
	TestTrue(TEXT("first pack as the pack phase starts"), PackFrames[0] == PackPhaseFrame);
	for (int32 Index = 1; Index < PackFrames.Num(); ++Index)
	{
		const int32 Gap = PackFrames[Index] - PackFrames[Index - 1];
		TestTrue(*FString::Printf(TEXT("packs 3 s apart (%d frames)"), Gap), FMath::Abs(Gap - 180) <= 1);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerPackOneSideTest, "RogueRigs.Spawner.PackFromOneSide",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerPackOneSideTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerPackSide"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 100, 2.f, false, Setup))
	{
		return false;
	}
	Setup.Spawner->SeedRandomStream(20261003);
	TSet<ERRSpawnSide> SidesSeen;
	for (int32 PackIndex = 0; PackIndex < 8; ++PackIndex)
	{
		const TArray<ARREnemy*> Pack = Setup.Spawner->TrySpawnPack();
		const FString Label = FString::Printf(TEXT("pack %d"), PackIndex);
		TestTrue(*FString::Printf(TEXT("%s has 8 to 15 robots (%d)"), *Label, Pack.Num()), Pack.Num() >= 8 && Pack.Num() <= 15);
		const ERRSpawnSide Side = CheckPackOnOneSide(*this, Setup, Pack, *Label);
		AddInfo(FString::Printf(TEXT("%s: %d robots, side %d"), *Label, Pack.Num(), static_cast<int32>(Side)));
		SidesSeen.Add(Side);
		TestEqual(*FString::Printf(TEXT("%s alive"), *Label), Setup.Spawner->GetAliveCount(), Pack.Num());
		for (ARREnemy* Enemy : Pack)
		{
			Enemy->Destroy();
		}
	}
	TestTrue(TEXT("packs come from more than one side"), SidesSeen.Num() >= 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerPackCapTest, "RogueRigs.Spawner.PackClampedToCap",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerPackCapTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerPackCap"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 10, 2.f, false, Setup))
	{
		return false;
	}
	Setup.Spawner->SeedRandomStream(7);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		TestNotNull(*FString::Printf(TEXT("single spawn %d"), Index), Setup.Spawner->TrySpawnEnemy());
	}
	const TArray<ARREnemy*> Clamped = Setup.Spawner->TrySpawnPack();
	TestEqual(TEXT("pack clamped to the 7 free slots"), Clamped.Num(), 7);
	CheckPackOnOneSide(*this, Setup, Clamped, TEXT("clamped pack"));
	TestEqual(TEXT("cap of 10 reached"), Setup.Spawner->GetAliveCount(), 10);
	TestEqual(TEXT("no pack at the cap"), Setup.Spawner->TrySpawnPack().Num(), 0);
	TestNull(TEXT("no single robot at the cap"), Setup.Spawner->TrySpawnEnemy());

	// Fixed pack size 12 with room: exactly 12.
	FSpawnerAccess::MaxAliveOf(Setup.Spawner) = 100;
	FSpawnerAccess::PackSizeMinOf(Setup.Spawner) = 12;
	FSpawnerAccess::PackSizeMaxOf(Setup.Spawner) = 12;
	const TArray<ARREnemy*> Full = Setup.Spawner->TrySpawnPack();
	TestEqual(TEXT("pack of exactly 12 with free slots"), Full.Num(), 12);
	CheckPackOnOneSide(*this, Setup, Full, TEXT("fixed pack"));
	TestEqual(TEXT("22 alive"), Setup.Spawner->GetAliveCount(), 22);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerKamikazeShareTest, "RogueRigs.Spawner.KamikazeShare",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerKamikazeShareTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerKamikazeShare"));
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("spawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);

	// M1 default of the placed spawner: chaser 85, kamikaze 15.
	FRRWeightedEnemyClass ChaserEntry;
	ChaserEntry.EnemyClass = ARRChaser::StaticClass();
	ChaserEntry.Weight = 85.f;
	FRRWeightedEnemyClass KamikazeEntry;
	KamikazeEntry.EnemyClass = ARRKamikaze::StaticClass();
	KamikazeEntry.Weight = 15.f;
	FSpawnerAccess::WeightsOf(Spawner) = { ChaserEntry, KamikazeEntry };

	FRandomStream Stream(20261003);
	constexpr int32 Picks = 2000;
	int32 Kamikazes = 0;
	int32 Chasers = 0;
	for (int32 Index = 0; Index < Picks; ++Index)
	{
		const TSubclassOf<ARREnemy> Picked = Spawner->PickEnemyClass(Stream);
		Kamikazes += Picked == ARRKamikaze::StaticClass() ? 1 : 0;
		Chasers += Picked == ARRChaser::StaticClass() ? 1 : 0;
	}
	const float Share = static_cast<float>(Kamikazes) / Picks;
	AddInfo(FString::Printf(TEXT("%d kamikazes, %d chasers over %d seeded picks: %.1f %%"), Kamikazes, Chasers, Picks, 100.f * Share));
	TestEqual(TEXT("every pick is one of the two classes"), Kamikazes + Chasers, Picks);
	TestTrue(TEXT("12 to 18 % kamikazes"), Share >= 0.12f && Share <= 0.18f);

	// The spawner's own stream, seeded, gives the same share band.
	Spawner->SeedRandomStream(20261003);
	int32 OwnKamikazes = 0;
	for (int32 Index = 0; Index < Picks; ++Index)
	{
		OwnKamikazes += Spawner->PickEnemyClass() == ARRKamikaze::StaticClass() ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("spawner stream: %d kamikazes over %d picks"), OwnKamikazes, Picks));
	TestEqual(TEXT("seeded spawner stream matches the seeded draw"), OwnKamikazes, Kamikazes);
	return true;
}

namespace RRSpawnerTests
{
	/** Run heading of the sector tests: +X, the top of the screen ("north"). */
	const FVector2D NorthHeading(1.f, 0.f);

	/** Spawn counts per sector (index = ERRSpawnSector - 1) and per side (index = ERRSpawnSide - 1). */
	struct FSpawnerSectorCounts
	{
		int32 Sectors[3] = { 0, 0, 0 };
		int32 Sides[4] = { 0, 0, 0, 0 };
		int32 Total = 0;

		/** Share (0..1) of sector Index. */
		float SectorShare(int32 Index) const { return Total > 0 ? static_cast<float>(Sectors[Index]) / Total : 0.f; }

		/** Share (0..1) of side Index. */
		float SideShare(int32 Index) const { return Total > 0 ? static_cast<float>(Sides[Index]) / Total : 0.f; }
	};

	/** Counts Point in Counts: its sector seen from the miner for NorthHeading (whatever the spawner heading) and its side of the view. */
	void SpawnerCountPoint(const FSetup& Setup, const FVector2D& Point, FSpawnerSectorCounts& Counts)
	{
		const ERRSpawnSector Sector = ARREnemySpawner::GetSectorOfPoint(FVector2D(Setup.Miner->GetActorLocation()), NorthHeading, Point, 45.f, 45.f);
		const ERRSpawnSide Side = ARREnemySpawner::GetSideOfRectangle(Setup.View, Point);
		if (Sector != ERRSpawnSector::None)
		{
			++Counts.Sectors[static_cast<int32>(Sector) - 1];
		}
		if (Side != ERRSpawnSide::None)
		{
			++Counts.Sides[static_cast<int32>(Side) - 1];
		}
		++Counts.Total;
	}

	/** Spawns Count lone robots one by one (each destroyed after it is counted) and counts their spawn cells. */
	FSpawnerSectorCounts SpawnerCountLoneSpawns(FAutomationTestBase& Test, const FSetup& Setup, int32 Count)
	{
		FSpawnerSectorCounts Counts;
		for (int32 Index = 0; Index < Count; ++Index)
		{
			ARREnemy* Enemy = Setup.Spawner->TrySpawnEnemy();
			if (!Enemy)
			{
				Test.AddError(FString::Printf(TEXT("lone spawn %d failed"), Index));
				break;
			}
			const FIntPoint Cell = Setup.Grid->GetCellAtWorld(Enemy->GetActorLocation());
			SpawnerCountPoint(Setup, FVector2D(Setup.Grid->GetCellCenter(Cell.X, Cell.Y)), Counts);
			Enemy->Destroy();
		}
		return Counts;
	}

	/** Spawns Count one-pack waves of PackType (each destroyed after it is counted) and counts each pack by the mean of its cells. */
	FSpawnerSectorCounts SpawnerCountPacks(FAutomationTestBase& Test, const FSetup& Setup, int32 PackType, int32 Count)
	{
		FSpawnerSectorCounts Counts;
		for (int32 Index = 0; Index < Count; ++Index)
		{
			const TArray<FRRSpawnedPack> Packs = Setup.Spawner->TrySpawnDirectorWave({ PackType });
			if (Packs.Num() != 1 || Packs[0].Cells.IsEmpty())
			{
				Test.AddError(FString::Printf(TEXT("pack %d failed"), Index));
				break;
			}
			FVector2D Mean = FVector2D::ZeroVector;
			for (const FIntPoint& Cell : Packs[0].Cells)
			{
				Mean += FVector2D(Setup.Grid->GetCellCenter(Cell.X, Cell.Y));
			}
			SpawnerCountPoint(Setup, Mean / Packs[0].Cells.Num(), Counts);
			for (ARREnemy* Robot : Packs[0].Robots)
			{
				Robot->Destroy();
			}
		}
		return Counts;
	}

	/** Plans Count fronts of horde 0 (nothing spawned) and counts their sides. */
	FSpawnerSectorCounts SpawnerCountHordeSides(const FSetup& Setup, int32 Count)
	{
		FSpawnerSectorCounts Counts;
		for (int32 Index = 0; Index < Count; ++Index)
		{
			const ERRSpawnSide Side = FSpawnerAccess::PrepareHordeOf(Setup.Spawner, 0);
			if (Side != ERRSpawnSide::None)
			{
				++Counts.Sides[static_cast<int32>(Side) - 1];
			}
			++Counts.Total;
		}
		return Counts;
	}

	/** Logs Counts with Label. */
	void SpawnerLogCounts(FAutomationTestBase& Test, const TCHAR* Label, const FSpawnerSectorCounts& Counts)
	{
		Test.AddInfo(FString::Printf(TEXT("%s (%d): front %.1f %%, sides %.1f %%, back %.1f %%; sides -X %.1f %%, +X %.1f %%, -Y %.1f %%, +Y %.1f %%"),
			Label, Counts.Total, 100.f * Counts.SectorShare(0), 100.f * Counts.SectorShare(1), 100.f * Counts.SectorShare(2),
			100.f * Counts.SideShare(0), 100.f * Counts.SideShare(1), 100.f * Counts.SideShare(2), 100.f * Counts.SideShare(3)));
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerHeadingSplitTest, "RogueRigs.Spawner.HeadingSectorSplit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerHeadingSplitTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerHeadingSplit"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 150, 2.f, false, Setup))
	{
		return false;
	}
	TestEqual(TEXT("default front weight 55"), FSpawnerAccess::FrontWeightOf(Setup.Spawner), 55.f);
	TestEqual(TEXT("default side weight 35"), FSpawnerAccess::SideWeightOf(Setup.Spawner), 35.f);
	TestEqual(TEXT("default back weight 10"), FSpawnerAccess::BackWeightOf(Setup.Spawner), 10.f);
	TestEqual(TEXT("default front half angle 45"), FSpawnerAccess::FrontHalfAngleOf(Setup.Spawner), 45.f);
	TestEqual(TEXT("default back half angle 45"), FSpawnerAccess::BackHalfAngleOf(Setup.Spawner), 45.f);
	Setup.Spawner->SeedRandomStream(31021);

	// Uniform reference: the share of each sector among the candidate cells.
	FSpawnerSectorCounts CellShare;
	TArray<FIntPoint> Candidates;
	Setup.Spawner->GetSpawnCandidates(Candidates);
	for (const FIntPoint& Cell : Candidates)
	{
		SpawnerCountPoint(Setup, FVector2D(Setup.Grid->GetCellCenter(Cell.X, Cell.Y)), CellShare);
	}
	SpawnerLogCounts(*this, TEXT("candidate cells"), CellShare);

	// Running north: lone robots.
	constexpr int32 LoneCount = 600;
	Setup.Spawner->SetHeadingOverride(NorthHeading);
	const FSpawnerSectorCounts Moving = SpawnerCountLoneSpawns(*this, Setup, LoneCount);
	SpawnerLogCounts(*this, TEXT("lone robots, running north"), Moving);
	TestEqual(TEXT("every lone spawn counted"), Moving.Total, LoneCount);
	TestTrue(TEXT("running: front 55 % (50-60)"), FMath::Abs(Moving.SectorShare(0) - 0.55f) <= 0.05f);
	TestTrue(TEXT("running: sides 35 % (30-40)"), FMath::Abs(Moving.SectorShare(1) - 0.35f) <= 0.05f);
	TestTrue(TEXT("running: back 10 % (7-13)"), FMath::Abs(Moving.SectorShare(2) - 0.10f) <= 0.03f);

	// Standing still: uniform over the cells.
	Setup.Spawner->SetHeadingOverride(FVector2D::ZeroVector);
	const FSpawnerSectorCounts Still = SpawnerCountLoneSpawns(*this, Setup, LoneCount);
	SpawnerLogCounts(*this, TEXT("lone robots, standing still"), Still);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		TestTrue(*FString::Printf(TEXT("still: sector %d share %.3f near its cell share %.3f"), Index + 1, Still.SectorShare(Index), CellShare.SectorShare(Index)),
			FMath::Abs(Still.SectorShare(Index) - CellShare.SectorShare(Index)) <= 0.05f);
	}

	// Packs (FirstChasers, 6-8 chasers) follow the same split.
	const int32 PackType = FSpawnerAccess::DirectorOf(Setup.Spawner).PackTypes.IndexOfByPredicate([](const FRRDirectorPackType& Type) { return Type.Name == TEXT("FirstChasers"); });
	if (TestTrue(TEXT("FirstChasers pack type"), PackType != INDEX_NONE))
	{
		constexpr int32 PackCount = 150;
		Setup.Spawner->SetHeadingOverride(NorthHeading);
		const FSpawnerSectorCounts MovingPacks = SpawnerCountPacks(*this, Setup, PackType, PackCount);
		SpawnerLogCounts(*this, TEXT("packs, running north"), MovingPacks);
		TestTrue(TEXT("packs running: front 55 % (45-65)"), FMath::Abs(MovingPacks.SectorShare(0) - 0.55f) <= 0.10f);
		TestTrue(TEXT("packs running: sides 35 % (25-45)"), FMath::Abs(MovingPacks.SectorShare(1) - 0.35f) <= 0.10f);
		TestTrue(TEXT("packs running: back 10 % (3-17)"), FMath::Abs(MovingPacks.SectorShare(2) - 0.10f) <= 0.07f);
		Setup.Spawner->SetHeadingOverride(FVector2D::ZeroVector);
		const FSpawnerSectorCounts StillPacks = SpawnerCountPacks(*this, Setup, PackType, PackCount);
		SpawnerLogCounts(*this, TEXT("packs, standing still"), StillPacks);
		TestTrue(TEXT("packs still: back no longer rare (> 17 %)"), StillPacks.SectorShare(2) > 0.17f);
		TestTrue(TEXT("packs still: front not dominant (< 45 %)"), StillPacks.SectorShare(0) < 0.45f);
	}

	// Horde side: the side of a random cell of the drawn sector.
	constexpr int32 HordeCount = 200;
	Setup.Spawner->SetHeadingOverride(NorthHeading);
	const FSpawnerSectorCounts MovingHordes = SpawnerCountHordeSides(Setup, HordeCount);
	SpawnerLogCounts(*this, TEXT("horde sides, running north"), MovingHordes);
	// Expected side shares: sum over the sectors of weight x share of that sector's candidate cells lying beyond the side.
	int32 SectorSideCells[3][4] = {};
	for (const FIntPoint& Cell : Candidates)
	{
		const FVector2D Point(Setup.Grid->GetCellCenter(Cell.X, Cell.Y));
		const ERRSpawnSector Sector = ARREnemySpawner::GetSectorOfPoint(FVector2D(Setup.Miner->GetActorLocation()), NorthHeading, Point, 45.f, 45.f);
		const ERRSpawnSide Side = ARREnemySpawner::GetSideOfRectangle(Setup.View, Point);
		if (Sector != ERRSpawnSector::None && Side != ERRSpawnSide::None)
		{
			++SectorSideCells[static_cast<int32>(Sector) - 1][static_cast<int32>(Side) - 1];
		}
	}
	const float SectorWeights[3] = { 0.55f, 0.35f, 0.10f };
	for (int32 Side = 0; Side < 4; ++Side)
	{
		float Expected = 0.f;
		for (int32 Sector = 0; Sector < 3; ++Sector)
		{
			const int32 SectorTotal = SectorSideCells[Sector][0] + SectorSideCells[Sector][1] + SectorSideCells[Sector][2] + SectorSideCells[Sector][3];
			Expected += SectorTotal > 0 ? SectorWeights[Sector] * SectorSideCells[Sector][Side] / SectorTotal : 0.f;
		}
		TestTrue(*FString::Printf(TEXT("hordes running: side %d share %.3f near the sector-weighted %.3f"), Side + 1, MovingHordes.SideShare(Side), Expected),
			FMath::Abs(MovingHordes.SideShare(Side) - Expected) <= 0.08f);
	}
	TestTrue(TEXT("hordes running: -X side (behind) at most 12 %"), MovingHordes.SideShare(0) <= 0.12f);
	Setup.Spawner->SetHeadingOverride(FVector2D::ZeroVector);
	const FSpawnerSectorCounts StillHordes = SpawnerCountHordeSides(Setup, HordeCount);
	SpawnerLogCounts(*this, TEXT("horde sides, standing still"), StillHordes);
	for (int32 Index = 0; Index < 4; ++Index)
	{
		TestTrue(*FString::Printf(TEXT("hordes still: side %d share %.3f in 15-35 %%"), Index + 1, StillHordes.SideShare(Index)),
			StillHordes.SideShare(Index) >= 0.15f && StillHordes.SideShare(Index) <= 0.35f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerHeadingFromMovementTest, "RogueRigs.Spawner.HeadingFromMovement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerHeadingFromMovementTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	FTestWorld TestWorld(TEXT("RRSpawnerHeadingFromMovement"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 150, 2.f, false, Setup))
	{
		return false;
	}
	TestEqual(TEXT("default heading window 1 s"), FSpawnerAccess::HeadingWindowOf(Setup.Spawner), 1.f);
	TestEqual(TEXT("default heading min speed 150 uu/s"), FSpawnerAccess::HeadingMinSpeedOf(Setup.Spawner), 150.f);

	// Moves the miner by Velocity (uu/s, world XY) for Seconds, ticking the world (the spawner samples every tick).
	auto Walk = [&](const FVector2D& Velocity, float Seconds)
	{
		const int32 Frames = FMath::RoundToInt(Seconds / FrameSeconds);
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			Setup.Miner->SetActorLocation(Setup.Miner->GetActorLocation() + FVector(Velocity * FrameSeconds, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
			TestWorld.Tick();
		}
	};
	FVector2D Heading;
	TestFalse(TEXT("no heading before moving"), Setup.Spawner->GetRunHeading(Heading));

	// Diagonal run at 400 uu/s for 1.5 s.
	const FVector2D Diagonal = FVector2D(1.f, 1.f).GetSafeNormal();
	Walk(Diagonal * 400.f, 1.5f);
	const bool bRunning = Setup.Spawner->GetRunHeading(Heading);
	AddInfo(FString::Printf(TEXT("running 400 uu/s diagonal: heading (%.3f, %.3f), speed %.1f"), Heading.X, Heading.Y, Setup.Spawner->GetRunSpeed()));
	TestTrue(TEXT("running: heading measured"), bRunning);
	TestTrue(TEXT("running: heading along the run"), FVector2D::DotProduct(Heading, Diagonal) > 0.999f);
	TestNearlyEqual(TEXT("running: speed 400 over the window"), Setup.Spawner->GetRunSpeed(), 400.f, 10.f);
	const FVector2D Ahead = FVector2D(Setup.Miner->GetActorLocation()) + Diagonal * 2000.f;
	TestEqual(TEXT("running: a point ahead is in the front sector"), Setup.Spawner->GetSpawnSector(Ahead), ERRSpawnSector::Front);

	// Slow walk below the threshold: still.
	Walk(FVector2D(100.f, 0.f), 1.5f);
	AddInfo(FString::Printf(TEXT("walking 100 uu/s: speed %.1f"), Setup.Spawner->GetRunSpeed()));
	TestFalse(TEXT("100 uu/s counts as standing still"), Setup.Spawner->GetRunHeading(Heading));
	TestEqual(TEXT("still: no sector"), Setup.Spawner->GetSpawnSector(Ahead), ERRSpawnSector::None);

	// Turn: one second of running west replaces the heading.
	Walk(FVector2D(-600.f, 0.f), 1.1f);
	TestTrue(TEXT("turned: heading measured"), Setup.Spawner->GetRunHeading(Heading));
	TestTrue(TEXT("turned: heading west"), Heading.X < -0.99f);

	// Stop: one second later, still again.
	Walk(FVector2D::ZeroVector, 1.1f);
	TestFalse(TEXT("stopped: no heading"), Setup.Spawner->GetRunHeading(Heading));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSpawnerFirstContactTest, "RogueRigs.Spawner.FirstContactStill",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSpawnerFirstContactTest::RunTest(const FString& Parameters)
{
	using namespace RRSpawnerTests;

	// Still miner, the miner camera's view (no override), default director, native chasers (MoveSpeed from EnemyStats.csv).
	FTestWorld TestWorld(TEXT("RRSpawnerFirstContact"));
	FSetup Setup;
	if (!MakeSetup(*this, TestWorld, 150, 2.f, true, Setup))
	{
		return false;
	}
	Setup.Spawner->ClearViewRectangleOverride();
	Setup.Spawner->SeedRandomStream(31022);
	FBox2D View;
	if (!TestTrue(TEXT("camera view rectangle"), Setup.Spawner->GetViewRectangle(View)))
	{
		return false;
	}
	double FirstSpawn = -1.0;
	double FirstContact = -1.0;
	float SpawnDistance = 0.f;
	const double Start = TestWorld.Get()->GetTimeSeconds();
	for (int32 Frame = 0; Frame < 600 && FirstContact < 0.0; ++Frame)
	{
		TestWorld.Tick();
		for (ARREnemy* Robot : Setup.Spawner->GetAliveEnemies())
		{
			const float Distance = ARREnemySpawner::GetDistanceToRectangle(View, FVector2D(Robot->GetActorLocation()));
			if (FirstSpawn < 0.0)
			{
				FirstSpawn = TestWorld.Get()->GetTimeSeconds() - Start;
				SpawnDistance = Distance;
			}
			if (Distance <= 0.f)
			{
				FirstContact = TestWorld.Get()->GetTimeSeconds() - Start;
				break;
			}
		}
	}
	AddInfo(FString::Printf(TEXT("still miner: first robot spawned at %.2f s, %.0f uu beyond the view edge; first robot on screen at %.2f s"),
		FirstSpawn, SpawnDistance, FirstContact));
	TestTrue(TEXT("first robot spawned at about 1 s"), FirstSpawn > 0.9 && FirstSpawn < 1.2);
	TestTrue(TEXT("first robot within the 100-500 uu band"), SpawnDistance >= BandMin && SpawnDistance <= BandMax);
	TestTrue(TEXT("first robot on screen within 4 s (target about 2 s)"), FirstContact > 0.0 && FirstContact <= 4.0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
