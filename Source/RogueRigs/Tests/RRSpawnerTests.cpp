// Rogue Rigs - automation tests of the enemy spawner (RogueRigs.Spawner.*): spawn cells outside the view and reachable, alive cap, freed slot, trickle then packs, packs from one side, kamikaze share.

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

	/** Miner cell (open floor, flow field tests). */
	const FIntPoint MinerCell(40, 40);

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
		Out.Grid = SpawnGrid(Test, TestWorld);
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
		Test.TestEqual(TEXT("default MaxAlive 100"), FSpawnerAccess::MaxAliveOf(Out.Spawner), 100);
		Test.TestEqual(TEXT("default TrickleDuration 30 s"), FSpawnerAccess::TrickleDurationOf(Out.Spawner), 30.f);
		Test.TestEqual(TEXT("default PackInterval 6 s"), FSpawnerAccess::PackIntervalOf(Out.Spawner), 6.f);
		Test.TestEqual(TEXT("default PackSizeMin 8"), FSpawnerAccess::PackSizeMinOf(Out.Spawner), 8);
		Test.TestEqual(TEXT("default PackSizeMax 15"), FSpawnerAccess::PackSizeMaxOf(Out.Spawner), 15);
		Test.TestEqual(TEXT("default LoneSpawnChance 0.1"), FSpawnerAccess::LoneSpawnChanceOf(Out.Spawner), 0.1f);
		Test.TestEqual(TEXT("default ViewMargin 1"), FSpawnerAccess::ViewMarginOf(Out.Spawner), 1);
		Test.TestEqual(TEXT("default MaxSpawnDistanceCells 6"), FSpawnerAccess::MaxSpawnDistanceOf(Out.Spawner), 6);
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
		const float CellSize = Setup.Grid->GetCellSize();
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
			Test.TestTrue(*FString::Printf(TEXT("%s: (%d, %d) in the spawn band"), Label, Cell.X, Cell.Y), Distance >= 1.5f * CellSize && Distance <= 6.5f * CellSize);
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
	const float CellSize = Setup.Grid->GetCellSize();
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
		// The whole cell lies at least ViewMargin (1) cell beyond the rectangle, and within 6 cells of its edge.
		const float Distance = ARREnemySpawner::GetDistanceToRectangle(Setup.View, FVector2D(Center.X, Center.Y));
		TestTrue(*FString::Printf(TEXT("spawn %d (%d, %d) at least one cell outside the view (%.0f uu)"), Index, Cell.X, Cell.Y, Distance),
			Distance >= 1.5f * CellSize);
		TestTrue(*FString::Printf(TEXT("spawn %d within 6 cells of the view edge"), Index), Distance <= 6.5f * CellSize);
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
	// Short phases: 6 s of trickle (one robot per second), then a pack every 3 s with a sure lone robot at +1 and +2 s.
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

#endif // WITH_DEV_AUTOMATION_TESTS
