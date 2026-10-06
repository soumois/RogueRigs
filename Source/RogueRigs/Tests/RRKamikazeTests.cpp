// Rogue Rigs - automation tests of the kamikaze (RogueRigs.Kamikaze.*): arming and fuse, death by shots with explosion and XP, chain death in another explosion, weighted spawner picks.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRKamikaze.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Projectiles/RRProjectile.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRKamikazeTests
{
	using namespace RRTestHarness;

	/** Burst rifle Blueprint (10 damage per shot). */
	const TCHAR* const BurstRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");

	/** Open floor row of arena_m1: the kamikaze stands on KamikazeCell, the muzzle two cells west (as the enemy tests). */
	const FIntPoint KamikazeCell(44, 41);
	const FIntPoint KamikazeShooterCell(42, 41);

	/** Miner cell of the fuse test (open floor; row 40 is open up to x 45). */
	const FIntPoint FuseMinerCell(40, 40);

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** Spawns a native robot of class T with its feet on the floor at FloorLocation (XP drop: ARRTestPickupXP). */
	template <typename T>
	T* SpawnRobot(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation)
	{
		const float HalfHeight = GetDefault<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		T* Robot = TestWorld.Get()->SpawnActorDeferred<T>(T::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("robot spawned"), Robot))
		{
			FEnemyAccess::XPPickupClassOf(Robot) = ARRTestPickupXP::StaticClass();
			Robot->FinishSpawning(SpawnTransform);
		}
		return Robot;
	}

	/** Explosion subsystem of the test world, with Recorder bound to OnExplosion. */
	URRExplosionSubsystem* GetExplosions(FAutomationTestBase& Test, const FTestWorld& TestWorld, URRTestEventRecorder* Recorder)
	{
		URRExplosionSubsystem* Subsystem = TestWorld.Get()->GetSubsystem<URRExplosionSubsystem>();
		if (Test.TestNotNull(TEXT("explosion subsystem exists"), Subsystem))
		{
			Subsystem->OnExplosion.AddDynamic(Recorder, &URRTestEventRecorder::HandleExplosion);
		}
		return Subsystem;
	}

	/** Health of Actor (health set on its ASC). */
	float Health(const IAbilitySystemInterface* Actor)
	{
		return Actor->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Live XP pickups in the world. */
	TArray<ARRPickup*> GetXPPickups(UWorld* World)
	{
		TArray<ARRPickup*> Result;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			if (IsValid(*It) && It->GetPickupType() == ERRPickupType::XP)
			{
				Result.Add(*It);
			}
		}
		return Result;
	}

	/** Fires exactly one shot from Weapon now and ticks until the projectile is gone (max 30 frames). True when it stopped. */
	bool FireOneShot(FAutomationTestBase& Test, const FTestWorld& TestWorld, ARRWeapon* Weapon)
	{
		TSet<ARRProjectile*> Before;
		for (TActorIterator<ARRProjectile> It(TestWorld.Get()); It; ++It)
		{
			Before.Add(*It);
		}
		URRAutoFireComponent* AutoFire = Weapon->GetAutoFireComponent();
		AutoFire->Configure(Weapon->GetAbilitySystemComponent(), Weapon->GetBurstCount(), Weapon->GetBurstDuty(), Weapon->GetReloadTime());
		AutoFire->StartFiring();
		AutoFire->AdvanceTime(0.001f);
		AutoFire->StopFiring();
		TWeakObjectPtr<ARRProjectile> Shot;
		for (TActorIterator<ARRProjectile> It(TestWorld.Get()); It; ++It)
		{
			if (IsValid(*It) && !Before.Contains(*It))
			{
				Shot = *It;
			}
		}
		if (!Shot.IsValid())
		{
			Test.AddError(TEXT("the shot spawned no projectile"));
			return false;
		}
		for (int32 Frame = 0; Frame < 30; ++Frame)
		{
			TestWorld.Tick();
			if (!Shot.IsValid() || Shot->IsActorBeingDestroyed())
			{
				return true;
			}
		}
		return false;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRKamikazeFuseTest, "RogueRigs.Kamikaze.ArmsAndExplodesOnFuse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRKamikazeFuseTest::RunTest(const FString& Parameters)
{
	using namespace RRKamikazeTests;

	FRREnemyStatsRow Row;
	if (TestTrue(TEXT("EnemyStats.csv row Kamikaze read"), ARREnemy::LoadStatsRow(TEXT("Kamikaze"), Row)))
	{
		TestEqual(TEXT("MaxHealth 100"), Row.MaxHealth, 100.f);
		TestEqual(TEXT("MoveSpeed 240"), Row.MoveSpeed, 240.f);
		TestEqual(TEXT("XPValue 1"), Row.XPValue, 1);
	}

	FTestWorld TestWorld(TEXT("RRKamikazeFuse"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Explosions = Grid ? GetExplosions(*this, TestWorld, Recorder.Get()) : nullptr;
	ARRMiner* Miner = Explosions ? SpawnMiner(*this, TestWorld, Grid, FuseMinerCell) : nullptr;
	if (!Miner)
	{
		return false;
	}
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}
	const float MinerHealthBefore = Health(Miner);

	const FVector MinerFloor = Grid->GetCellCenter(FuseMinerCell.X, FuseMinerCell.Y);
	ARRKamikaze* Kamikaze = SpawnRobot<ARRKamikaze>(*this, TestWorld, MinerFloor + FVector(600.f, 0.f, 0.f));
	if (!Kamikaze)
	{
		return false;
	}
	TestEqual(TEXT("Health 100"), Health(Kamikaze), 100.f);
	TestEqual(TEXT("starts in Chase"), Kamikaze->GetState(), ERRKamikazeState::Chase);
	TWeakObjectPtr<ARRKamikaze> Weak(Kamikaze);

	float ArmDistance = -1.f;
	float MaxCoreScaleArmed = 0.f;
	for (int32 Frame = 0; Frame < 300 && Weak.IsValid() && !Weak->HasDetonated(); ++Frame)
	{
		const ERRKamikazeState Previous = Weak->GetState();
		TestWorld.Tick();
		if (!Weak.IsValid())
		{
			break;
		}
		if (Previous == ERRKamikazeState::Chase && Weak->GetState() == ERRKamikazeState::Armed)
		{
			ArmDistance = FVector::Dist2D(Weak->GetActorLocation(), Miner->GetActorLocation());
		}
		if (Weak->GetState() == ERRKamikazeState::Armed)
		{
			MaxCoreScaleArmed = FMath::Max(MaxCoreScaleArmed, Weak->GetCoreScaleFactor());
		}
	}
	if (!TestTrue(TEXT("kamikaze detonated"), Weak.IsValid() && Weak->HasDetonated()))
	{
		return false;
	}
	const double Fuse = Kamikaze->GetExplosionQueuedTime() - Kamikaze->GetArmTime();
	AddInfo(FString::Printf(TEXT("armed at %.1f uu from the miner at t=%.3f s, explosion queued %.3f s later, max armed core scale %.2f"),
		ArmDistance, Kamikaze->GetArmTime(), Fuse, MaxCoreScaleArmed));
	TestTrue(TEXT("armed within 150 uu"), ArmDistance > 0.f && ArmDistance <= 150.f);
	TestTrue(TEXT("armed near the 150 uu edge (one frame of travel)"), ArmDistance > 150.f - 2.f * Row.MoveSpeed * FrameSeconds);
	const float ContactDistance = Kamikaze->GetCapsuleComponent()->GetScaledCapsuleRadius() + Miner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	TestTrue(TEXT("armed before its capsule touches the miner"), ArmDistance > ContactDistance);
	TestTrue(TEXT("fuse 0.5 s (within one frame)"), Fuse >= 0.5 - 1e-3 && Fuse <= 0.5 + FrameSeconds + 1e-3);
	TestTrue(TEXT("core grew while armed"), MaxCoreScaleArmed > 1.5f);
	TestTrue(TEXT("dead after the fuse"), Kamikaze->IsDead());
	TestEqual(TEXT("state Dead"), Kamikaze->GetState(), ERRKamikazeState::Dead);
	TestTrue(TEXT("its source is spent"), Explosions->IsSourceSpent(Kamikaze));

	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("one explosion"), Recorder->GetExplosionRows().Num(), 1);
	if (Recorder->GetExplosionRows().Num() == 1)
	{
		TestEqual(TEXT("row Kamikaze"), Recorder->GetExplosionRows()[0], FName(TEXT("Kamikaze")));
	}
	const float MinerHealthAfter = Health(Miner);
	AddInfo(FString::Printf(TEXT("miner health %.1f -> %.1f"), MinerHealthBefore, MinerHealthAfter));
	TestEqual(TEXT("miner took 30 damage"), MinerHealthBefore - MinerHealthAfter, 30.f);
	TestFalse(TEXT("kamikaze destroyed"), Weak.IsValid() && !Weak->IsActorBeingDestroyed());
	TestEqual(TEXT("no XP after a fuse explosion"), GetXPPickups(TestWorld.Get()).Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRKamikazeKilledTest, "RogueRigs.Kamikaze.KilledExplodesAndDropsXP",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRKamikazeKilledTest::RunTest(const FString& Parameters)
{
	using namespace RRKamikazeTests;

	FTestWorld TestWorld(TEXT("RRKamikazeKilled"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Explosions = Grid ? GetExplosions(*this, TestWorld, Recorder.Get()) : nullptr;
	UClass* RifleClass = Explosions ? LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath) : nullptr;
	ARRWeapon* Rifle = RifleClass ? TestWorld.Spawn<ARRWeapon>(RifleClass, FVector::ZeroVector) : nullptr;
	if (!Rifle || !TestTrue(TEXT("rifle acquired"), Rifle->Acquire(nullptr)))
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);

	// No miner: both robots stay in Chase without a target. The chaser stands behind the kamikaze (shots stop on the kamikaze).
	const FVector TargetFloor = Grid->GetCellCenter(KamikazeCell.X, KamikazeCell.Y);
	ARRKamikaze* Kamikaze = SpawnRobot<ARRKamikaze>(*this, TestWorld, TargetFloor);
	ARRChaser* Chaser = SpawnRobot<ARRChaser>(*this, TestWorld, TargetFloor + FVector(150.f, 0.f, 0.f));
	if (!Kamikaze || !Chaser)
	{
		return false;
	}
	TWeakObjectPtr<ARRKamikaze> WeakKamikaze(Kamikaze);
	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
	}

	const FVector MuzzleTarget = Grid->GetCellCenter(KamikazeShooterCell.X, KamikazeShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight());
	const FVector MuzzleOffset = FWeaponAccess::MuzzleOf(Rifle)->GetComponentLocation() - Rifle->GetActorLocation();
	Rifle->SetActorLocation(MuzzleTarget - MuzzleOffset, false, nullptr, ETeleportType::TeleportPhysics);
	const FBox Bounds = Kamikaze->GetCapsuleComponent()->Bounds.GetBox();
	TestTrue(TEXT("kamikaze capsule covers the shot height"), Bounds.Min.Z < MuzzleTarget.Z && Bounds.Max.Z > MuzzleTarget.Z);

	// Each rifle shot deals 10 (crit 0): health steps down by 10 per shot, the last of MaxHealth / 10 shots kills it.
	const float StartHealth = Health(Kamikaze);
	TestEqual(TEXT("starts at MaxHealth 100 (EnemyStats.csv)"), StartHealth, 100.f);
	const int32 ShotsToKill = FMath::CeilToInt(StartHealth / 10.f);
	for (int32 Shot = 1; Shot < ShotsToKill; ++Shot)
	{
		TestTrue(*FString::Printf(TEXT("shot %d stopped"), Shot), FireOneShot(*this, TestWorld, Rifle));
		TestEqual(*FString::Printf(TEXT("Health after shot %d"), Shot), Health(Kamikaze), StartHealth - 10.f * Shot);
		TestFalse(*FString::Printf(TEXT("alive after shot %d"), Shot), Kamikaze->IsDead());
		TestEqual(*FString::Printf(TEXT("no explosion while alive (shot %d)"), Shot), Explosions->IsSourceSpent(Kamikaze), false);
	}

	TestTrue(TEXT("last shot stopped"), FireOneShot(*this, TestWorld, Rifle));
	TestTrue(*FString::Printf(TEXT("dead after %d shots"), ShotsToKill), Kamikaze->IsDead());
	TestFalse(TEXT("killed, not detonated"), Kamikaze->HasDetonated());
	TestTrue(TEXT("explosion queued at death"), Explosions->IsSourceSpent(Kamikaze));
	const FVector DeathLocation = Kamikaze->GetActorLocation();

	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestFalse(TEXT("kamikaze destroyed"), WeakKamikaze.IsValid() && !WeakKamikaze->IsActorBeingDestroyed());
	TestEqual(TEXT("one explosion"), Recorder->GetExplosionRows().Num(), 1);
	AddInfo(FString::Printf(TEXT("chaser at 150 uu: health %.1f, dead %d"), Health(Chaser), Chaser->IsDead() ? 1 : 0));
	TestTrue(TEXT("the explosion killed the chaser (30 damage on 30 health)"), Chaser->IsDead());

	int32 NearKamikaze = 0;
	const TArray<ARRPickup*> Pickups = GetXPPickups(TestWorld.Get());
	for (const ARRPickup* Pickup : Pickups)
	{
		// Dropped where the kamikaze died (the pop then ejects it 80-220 uu away).
		NearKamikaze += FVector::Dist2D(FPickupAccess::PopStartOf(Pickup), DeathLocation) < 1.f ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("%d XP pickups, %d dropped at the kamikaze"), Pickups.Num(), NearKamikaze));
	TestEqual(TEXT("1 XP from the kamikaze, in 4 cubes"), NearKamikaze, 4);
	TestEqual(TEXT("2 XP in total (kamikaze 4 cubes + chaser 3)"), Pickups.Num(), 7);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRKamikazeChainTest, "RogueRigs.Kamikaze.CaughtInExplosion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRKamikazeChainTest::RunTest(const FString& Parameters)
{
	using namespace RRKamikazeTests;

	FTestWorld TestWorld(TEXT("RRKamikazeChain"));
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	URRExplosionSubsystem* Explosions = Grid ? GetExplosions(*this, TestWorld, Recorder.Get()) : nullptr;
	const FVector Floor = Grid ? Grid->GetCellCenter(KamikazeCell.X, KamikazeCell.Y + 1) : FVector::ZeroVector;
	ARRKamikaze* Kamikaze = Explosions ? SpawnRobot<ARRKamikaze>(*this, TestWorld, Floor) : nullptr;
	if (!Kamikaze)
	{
		return false;
	}
	TestWorld.Tick();
	// A full-health kamikaze (100) survives one Barrel explosion (40): bring it down to that damage so the explosion kills it.
	FRRExplosionStatsRow BarrelStats;
	if (TestTrue(TEXT("Barrel row loads"), Explosions->GetExplosionStats(TEXT("Barrel"), BarrelStats)))
	{
		TestTrue(TEXT("full health survives one Barrel explosion"), Health(Kamikaze) > BarrelStats.Damage);
		Kamikaze->GetAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), BarrelStats.Damage);
	}
	TestTrue(TEXT("barrel explosion queued"), Explosions->QueueExplosion(Floor + FVector(100.f, 0.f, 0.f), TEXT("Barrel"), nullptr, nullptr));
	for (int32 Frame = 0; Frame < 40; ++Frame)
	{
		TestWorld.Tick();
	}
	const TArray<FName>& Rows = Recorder->GetExplosionRows();
	TestEqual(TEXT("two explosions (barrel, then kamikaze)"), Rows.Num(), 2);
	if (Rows.Num() == 2)
	{
		TestEqual(TEXT("first Barrel"), Rows[0], FName(TEXT("Barrel")));
		TestEqual(TEXT("second Kamikaze"), Rows[1], FName(TEXT("Kamikaze")));
	}
	TestEqual(TEXT("killed by the explosion: 1 XP in 4 cubes"), GetXPPickups(TestWorld.Get()).Num(), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRKamikazeWeightedSpawnTest, "RogueRigs.Kamikaze.WeightedSpawnerShare",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRKamikazeWeightedSpawnTest::RunTest(const FString& Parameters)
{
	using namespace RRKamikazeTests;

	FTestWorld TestWorld(TEXT("RRKamikazeWeights"));
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("spawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);
	FRandomStream Stream(20261002);

	// Empty array: the single EnemyClass.
	FSpawnerAccess::EnemyClassOf(Spawner) = ARRChaser::StaticClass();
	TestTrue(TEXT("empty array picks EnemyClass"), Spawner->PickEnemyClass(Stream) == ARRChaser::StaticClass());

	// Chaser 3, kamikaze 1: 25 % kamikazes expected.
	FRRWeightedEnemyClass ChaserEntry;
	ChaserEntry.EnemyClass = ARRChaser::StaticClass();
	ChaserEntry.Weight = 3.f;
	FRRWeightedEnemyClass KamikazeEntry;
	KamikazeEntry.EnemyClass = ARRKamikaze::StaticClass();
	KamikazeEntry.Weight = 1.f;
	FSpawnerAccess::WeightsOf(Spawner) = { ChaserEntry, KamikazeEntry };

	constexpr int32 Picks = 400;
	int32 Kamikazes = 0;
	int32 Chasers = 0;
	for (int32 Index = 0; Index < Picks; ++Index)
	{
		const TSubclassOf<ARREnemy> Picked = Spawner->PickEnemyClass(Stream);
		Kamikazes += Picked == ARRKamikaze::StaticClass() ? 1 : 0;
		Chasers += Picked == ARRChaser::StaticClass() ? 1 : 0;
	}
	const float Share = static_cast<float>(Kamikazes) / Picks;
	AddInfo(FString::Printf(TEXT("%d kamikazes, %d chasers over %d picks: %.1f %%"), Kamikazes, Chasers, Picks, 100.f * Share));
	TestEqual(TEXT("every pick is one of the two classes"), Kamikazes + Chasers, Picks);
	TestTrue(TEXT("20 to 30 % kamikazes"), Share >= 0.2f && Share <= 0.3f);

	// Global-random overload also draws from the array.
	int32 GlobalKamikazes = 0;
	for (int32 Index = 0; Index < Picks; ++Index)
	{
		GlobalKamikazes += Spawner->PickEnemyClass() == ARRKamikaze::StaticClass() ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("global random: %d kamikazes over %d picks"), GlobalKamikazes, Picks));
	TestTrue(TEXT("global random picks both classes"), GlobalKamikazes > 0 && GlobalKamikazes < Picks);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
