// Rogue Rigs - automation tests of the heavy slam ground signal (RogueRigs.Heavy.GroundTelegraph.*, task-115-01): shown in Danger mode at the slam radius only during the telegraph with a rising progress, hidden at the slam, at once on death mid-telegraph, and after pool release and reuse.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRHeavy.h"
#include "GAS/RRGameplayTags.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRHeavyTelegraphTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Open row 43 of arena_m1 (x 28 to 51 floor): miner on the west, heavy east of it (as RogueRigs.Heavy.SlamCadence). */
	const FIntPoint OpenMinerCell(30, 43);

	/** Grid and an invulnerable unarmed miner on OpenMinerCell; false when something failed. */
	bool MakeArena(FAutomationTestBase& Test, const FTestWorld& TestWorld, ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner)
	{
		OutGrid = SpawnGrid(Test, TestWorld);
		OutMiner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, OpenMinerCell) : nullptr;
		if (!OutMiner)
		{
			return false;
		}
		OutMiner->GetAbilitySystemComponent()->AddLooseGameplayTag(RRGameplayTags::State_Invulnerable);
		for (ARRWeapon* Weapon : OutMiner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		return true;
	}

	/** Native heavy with its feet on FloorLocation (XP drop: ARRTestPickupXP). */
	ARRHeavy* SpawnHeavy(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation)
	{
		const float HalfHeight = GetDefault<ARRHeavy>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		ARRHeavy* Heavy = TestWorld.Get()->SpawnActorDeferred<ARRHeavy>(ARRHeavy::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("heavy spawned"), Heavy))
		{
			FEnemyAccess::XPPickupClassOf(Heavy) = ARRTestPickupXP::StaticClass();
			Heavy->FinishSpawning(SpawnTransform);
		}
		return Heavy;
	}

	/** Kills Robot with a flat damage from the miner. */
	void Kill(ARRMiner* Miner, ARREnemy* Robot)
	{
		UAbilitySystemComponent* Source = Miner->GetAbilitySystemComponent();
		FGameplayEffectSpecHandle Spec = Source->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, Source->MakeEffectContext());
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, 100000.f);
		Source->ApplyGameplayEffectSpecToTarget(*Spec.Data, Robot->GetAbilitySystemComponent());
	}

	/** Ticks until Heavy telegraphs (at most MaxTicks); true when it does. */
	bool TickUntilTelegraph(const FTestWorld& TestWorld, const ARRHeavy* Heavy, int32 MaxTicks)
	{
		for (int32 Tick = 0; Tick < MaxTicks && !Heavy->IsTelegraphing(); ++Tick)
		{
			TestWorld.Tick();
		}
		return Heavy->IsTelegraphing();
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavyTelegraphShownTest, "RogueRigs.Heavy.GroundTelegraph.ShownDuringTelegraph",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavyTelegraphShownTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTelegraphTests;

	FTestWorld TestWorld(TEXT("RRHeavyTelegraphShown"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, Grid, Miner))
	{
		return false;
	}
	ARRHeavy* Heavy = SpawnHeavy(*this, TestWorld, Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y) + FVector(600.f, 0.f, 0.f));
	URRGroundTelegraphComponent* Telegraph = Heavy ? Heavy->GetGroundTelegraph() : nullptr;
	if (!TestNotNull(TEXT("heavy owns a ground telegraph"), Telegraph))
	{
		return false;
	}
	TestTrue(TEXT("attached under the root"), Telegraph->GetAttachParent() == Heavy->GetRootComponent());
	TestEqual(TEXT("centered on the feet"), static_cast<float>(Telegraph->GetComponentLocation().Z),
		static_cast<float>(Heavy->GetActorLocation().Z - Heavy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), 0.5f);
	TestFalse(TEXT("hidden at spawn"), Telegraph->IsShown());

	// Two full telegraph -> slam cycles: shown exactly while telegraphing, progress rising, hidden from the slam on.
	int32 Slams = 0;
	int32 TelegraphFrames = 0;
	bool bShownIffTelegraph = true;
	bool bRightRadius = true;
	bool bDanger = true;
	bool bMatchesHeavy = true;
	bool bRising = true;
	bool bHiddenAfterSlam = true;
	float PreviousProgress = -1.f;
	float MaxProgress = 0.f;
	float ProgressAtSlam = -1.f;
	for (int32 Frame = 0; Frame < 60 * 8 && Slams < 2; ++Frame)
	{
		const bool bWasTelegraphing = Heavy->IsTelegraphing();
		const int32 SlamsBefore = Heavy->GetSlamCount();
		TestWorld.Tick();
		bShownIffTelegraph &= Telegraph->IsShown() == Heavy->IsTelegraphing();
		if (Heavy->IsTelegraphing())
		{
			++TelegraphFrames;
			bRightRadius &= FMath::IsNearlyEqual(Telegraph->GetRadius(), Heavy->GetSlamRadius());
			bDanger &= Telegraph->GetMode() == ERRGroundTelegraphMode::Danger;
			bMatchesHeavy &= FMath::IsNearlyEqual(Telegraph->GetProgress(), Heavy->GetTelegraphProgress(), 1e-4f);
			if (bWasTelegraphing)
			{
				bRising &= Telegraph->GetProgress() > PreviousProgress;
			}
			PreviousProgress = Telegraph->GetProgress();
			MaxProgress = FMath::Max(MaxProgress, PreviousProgress);
		}
		if (Heavy->GetSlamCount() > SlamsBefore)
		{
			++Slams;
			ProgressAtSlam = Telegraph->GetProgress();
			bHiddenAfterSlam &= !Telegraph->IsShown() && Heavy->GetState() == ERRHeavyState::Slam;
		}
		if (Heavy->GetState() == ERRHeavyState::Recover || (Slams > 0 && Heavy->GetState() == ERRHeavyState::Chase))
		{
			bHiddenAfterSlam &= !Telegraph->IsShown();
		}
	}
	AddInfo(FString::Printf(TEXT("%d slams, %d telegraph frames, max progress %.3f, progress on the slam frame %.3f, radius %.1f"), Slams,
		TelegraphFrames, MaxProgress, ProgressAtSlam, Telegraph->GetRadius()));
	if (!TestEqual(TEXT("two slams within 8 s"), Slams, 2))
	{
		return false;
	}
	TestTrue(TEXT("telegraph frames seen"), TelegraphFrames > 20);
	TestTrue(TEXT("shown if and only if telegraphing"), bShownIffTelegraph);
	TestTrue(TEXT("radius = slam radius while shown"), bRightRadius);
	TestEqual(TEXT("slam radius follows the stats (289)"), Telegraph->GetRadius(), 289.f);
	TestTrue(TEXT("Danger mode while shown"), bDanger);
	TestTrue(TEXT("progress = GetTelegraphProgress"), bMatchesHeavy);
	TestTrue(TEXT("progress rises every telegraph frame"), bRising);
	TestTrue(TEXT("progress reaches 0.95 during the telegraph"), MaxProgress >= 0.95f);
	TestEqual(TEXT("progress circle at the edge when the slam lands"), ProgressAtSlam, 1.f, 0.01f);
	TestTrue(TEXT("hidden at the slam and during the recovery"), bHiddenAfterSlam);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavyTelegraphDeathTest, "RogueRigs.Heavy.GroundTelegraph.HiddenOnDeath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavyTelegraphDeathTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTelegraphTests;

	FTestWorld TestWorld(TEXT("RRHeavyTelegraphDeath"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, Grid, Miner))
	{
		return false;
	}
	ARRHeavy* Heavy = SpawnHeavy(*this, TestWorld, Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y) + FVector(150.f, 0.f, 0.f));
	URRGroundTelegraphComponent* Telegraph = Heavy ? Heavy->GetGroundTelegraph() : nullptr;
	if (!Telegraph || !TestTrue(TEXT("telegraph within 2 s"), TickUntilTelegraph(TestWorld, Heavy, 120)))
	{
		return false;
	}
	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
	}
	if (!TestTrue(TEXT("still telegraphing mid wind-up"), Heavy->IsTelegraphing()) || !TestTrue(TEXT("shown mid wind-up"), Telegraph->IsShown()))
	{
		return false;
	}
	TestTrue(TEXT("progress under way"), Telegraph->GetProgress() > 0.1f && Telegraph->GetProgress() < 1.f);

	// Killed mid-telegraph: the signal goes with the robot at once (same frame, no tick in between).
	Kill(Miner, Heavy);
	TestTrue(TEXT("heavy dead"), Heavy->IsDead());
	TestEqual(TEXT("state Dead"), Heavy->GetState(), ERRHeavyState::Dead);
	TestFalse(TEXT("hidden at once on death"), Telegraph->IsShown());
	bool bStaysHidden = true;
	for (int32 Frame = 0; Frame < 60 && IsValid(Heavy) && !URRActorPoolSubsystem::IsPooledIdle(Heavy); ++Frame)
	{
		TestWorld.Tick();
		bStaysHidden &= !IsValid(Heavy) || !Telegraph->IsShown();
	}
	TestTrue(TEXT("stays hidden through the death shrink"), bStaysHidden);
	TestEqual(TEXT("no slam after death"), IsValid(Heavy) ? Heavy->GetSlamCount() : 0, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRHeavyTelegraphPoolTest, "RogueRigs.Heavy.GroundTelegraph.HiddenAfterPoolReuse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRHeavyTelegraphPoolTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyTelegraphTests;

	FTestWorld TestWorld(TEXT("RRHeavyTelegraphPool"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, Grid, Miner))
	{
		return false;
	}
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("actor pool"), Pool) || !TestNotNull(TEXT("spawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawnMaxFrames(0);
	Spawner->SetSpawningEnabled(false);
	FSpawnerAccess::EnemyClassOf(Spawner) = ARRHeavy::StaticClass();
	FSpawnerAccess::MaxAliveOf(Spawner) = 1;
	FSpawnerAccess::RecycleEnabledOf(Spawner) = false;
	const FVector MinerLocation = Miner->GetActorLocation();
	Spawner->SetViewRectangleOverride(FBox2D(FVector2D(MinerLocation.X - 1600.f, MinerLocation.Y - 1000.f),
		FVector2D(MinerLocation.X + 1600.f, MinerLocation.Y + 1000.f)));

	const FVector HeavyFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y) + FVector(150.f, 0.f, 0.f);
	const float HalfHeight = GetDefault<ARRHeavy>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	auto PlaceNearMiner = [&](ARRHeavy* Heavy)
	{
		Heavy->SetActorLocation(HeavyFloor + FVector(0.f, 0.f, HalfHeight + 2.f), false, nullptr, ETeleportType::ResetPhysics);
	};

	ARRHeavy* Heavy = Cast<ARRHeavy>(Spawner->TrySpawnEnemy());
	URRGroundTelegraphComponent* Telegraph = Heavy ? Heavy->GetGroundTelegraph() : nullptr;
	if (!TestNotNull(TEXT("pooled heavy"), Telegraph) || !TestTrue(TEXT("pool member"), Pool->IsPoolMember(Heavy)))
	{
		return false;
	}
	PlaceNearMiner(Heavy);
	if (!TestTrue(TEXT("first life: telegraph within 2 s"), TickUntilTelegraph(TestWorld, Heavy, 120)))
	{
		return false;
	}
	for (int32 Frame = 0; Frame < 15; ++Frame)
	{
		TestWorld.Tick();
	}
	const float ProgressBeforeDeath = Telegraph->GetProgress();
	TestTrue(TEXT("first life: shown with progress under way"), Telegraph->IsShown() && ProgressBeforeDeath > 0.1f);

	Kill(Miner, Heavy);
	TestFalse(TEXT("hidden on death"), Telegraph->IsShown());
	for (int32 Tick = 0; Tick < 120 && !URRActorPoolSubsystem::IsPooledIdle(Heavy); ++Tick)
	{
		TestWorld.Tick();
	}
	if (!TestTrue(TEXT("released to the pool"), URRActorPoolSubsystem::IsPooledIdle(Heavy)))
	{
		return false;
	}
	TestFalse(TEXT("hidden while idle in the pool"), Telegraph->IsShown());
	TestFalse(TEXT("a hidden component does not tick"), Telegraph->IsComponentTickEnabled());
	TestFalse(TEXT("Show refused while idle in the pool"), Telegraph->Show());

	ARRHeavy* Reused = Cast<ARRHeavy>(Spawner->TrySpawnEnemy());
	if (!TestTrue(TEXT("the same heavy is reused"), Reused == Heavy))
	{
		return false;
	}
	TestEqual(TEXT("second life"), Heavy->GetPoolLifeCount(), 2);
	TestEqual(TEXT("reused in Chase"), Heavy->GetState(), ERRHeavyState::Chase);
	TestFalse(TEXT("reused heavy: no stale signal"), Telegraph->IsShown());
	TestEqual(TEXT("reused heavy: progress back to 0"), Telegraph->GetProgress(), 0.f);
	TestEqual(TEXT("reused heavy: radius = slam radius"), Telegraph->GetRadius(), Heavy->GetSlamRadius());
	TestWorld.Tick();
	TestFalse(TEXT("still hidden after a tick in Chase"), Heavy->IsTelegraphing() || Telegraph->IsShown());

	// The second life telegraphs again with a fresh signal.
	PlaceNearMiner(Heavy);
	if (!TestTrue(TEXT("second life: telegraph within 2 s"), TickUntilTelegraph(TestWorld, Heavy, 120)))
	{
		return false;
	}
	TestTrue(TEXT("second life: shown"), Telegraph->IsShown());
	TestTrue(TEXT("second life: progress restarts near 0"), Telegraph->GetProgress() < ProgressBeforeDeath);
	TestEqual(TEXT("second life: Danger mode"), Telegraph->GetMode(), ERRGroundTelegraphMode::Danger);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
