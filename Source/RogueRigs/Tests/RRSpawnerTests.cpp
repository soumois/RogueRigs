// Rogue Rigs - automation tests of the enemy spawner (RogueRigs.Spawner.*): spawn cells outside the view and reachable, alive cap, freed slot.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GAS/RRGameplayTags.h"
#include "HAL/IConsoleManager.h"
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

	/** Reads and writes protected members (never instantiated). */
	struct FSpawnerAccess : public ARREnemySpawner
	{
		static TSubclassOf<ARREnemy>& EnemyClassOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::EnemyClass); }
		static float& SpawnIntervalOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::SpawnInterval); }
		static int32& MaxAliveOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::MaxAlive); }
		static int32& ViewMarginOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::ViewMargin); }
		static int32& MaxSpawnDistanceOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::MaxSpawnDistanceCells); }
	};

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
		Test.TestEqual(TEXT("default MaxAlive 20"), FSpawnerAccess::MaxAliveOf(Out.Spawner), 20);
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

#endif // WITH_DEV_AUTOMATION_TESTS
