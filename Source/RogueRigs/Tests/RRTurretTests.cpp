// Rogue Rigs - automation tests of the turret launcher and its turrets (RogueRigs.Turret.*): CSV stats, button placement and charges, recharge one at a time, placement cell, stat copy at placement, nearest-target lock with grid line of sight, range, fire and lifetime.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Enemies/RRChaser.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Projectiles/RRTurretBullet.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRTurret.h"
#include "Weapons/RRTurretLauncher.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRTurretTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Open floor of arena_m1: rows 41-43 are floor from x 32 to 45. */
	const FIntPoint OpenMinerCell(32, 42);

	/** Rock row 40 (cells of 200 uu): rock (34, 40), floor (33, 40) behind it, floor from x 35 to 45 (spawn (40, 40)). */
	const FIntPoint RockMinerCell(36, 40);
	const FIntPoint NextToRockMinerCell(35, 40);
	const FIntPoint RockCellWest(34, 40);
	const FIntPoint BehindRockCell(33, 40);
	const FIntPoint FrontOfRockCell(35, 40);
	const FIntPoint VisibleCell(39, 40);

	/** TurretLauncher CSV row (turret gdd, owner 2026-10-05). */
	constexpr float TurretDamage = 6.f;
	constexpr float TurretFireRate = 3.3f;
	constexpr float TurretPierce = 4.f;
	constexpr float TurretCritChance = 0.05f;
	constexpr float TurretCritDamage = 1.5f;
	constexpr int32 TurretCharges = 2;
	constexpr float RechargeSeconds = 6.f;
	constexpr float TurretLifetime = 10.f;
	constexpr float TurretRange = 1500.f;

	/** Points the miner's stick aim along Direction (camera yaw 0: screen up = world +X, screen right = world +Y). */
	void AimAlong(ARRMiner* Miner, const FVector& Direction)
	{
		const FVector Dir = Direction.GetSafeNormal2D();
		Miner->ApplyStickAimInput(FVector2D(Dir.Y, Dir.X));
	}

	/** Spawns the grid and BP_Miner on Cell, stops its weapons, aims along Aim and gives it a native turret launcher (AddWeapon). */
	ARRTurretLauncher* SetUpMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, const FVector& Aim,
		ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner)
	{
		OutGrid = SpawnGrid(Test, TestWorld);
		OutMiner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, Cell) : nullptr;
		if (!OutMiner)
		{
			return nullptr;
		}
		for (ARRWeapon* Weapon : OutMiner->GetWeapons())
		{
			if (IsValid(Weapon))
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		AimAlong(OutMiner, Aim);
		Test.TestNull(TEXT("the default loadout has no turret launcher"), OutMiner->FindTurretLauncher());
		ARRTurretLauncher* Launcher = Cast<ARRTurretLauncher>(OutMiner->AddWeapon(ARRTurretLauncher::StaticClass()));
		Test.TestNotNull(TEXT("AddWeapon gives a turret launcher"), Launcher);
		return Launcher;
	}

	/** Spawns a native ARRChaser with its feet on the floor at FloorLocation, ticking off (it neither moves nor attacks); XP drop ARRTestPickupXP. */
	ARRChaser* SpawnFrozenChaser(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation)
	{
		const float HalfHeight = GetDefault<ARRChaser>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		ARRChaser* Chaser = TestWorld.Get()->SpawnActorDeferred<ARRChaser>(ARRChaser::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Test.TestNotNull(TEXT("chaser spawned"), Chaser))
		{
			return nullptr;
		}
		FEnemyAccess::XPPickupClassOf(Chaser) = ARRTestPickupXP::StaticClass();
		Chaser->FinishSpawning(SpawnTransform);
		Chaser->SetActorTickEnabled(false);
		Chaser->GetCharacterMovement()->SetComponentTickEnabled(false);
		return Chaser;
	}

	/** Sets a base value on a weapon's ASC. */
	void SetAttribute(ARRWeapon* Weapon, const FGameplayAttribute& Attribute, float Value)
	{
		Weapon->GetAbilitySystemComponent()->SetNumericAttributeBase(Attribute, Value);
	}

	/** Current value of a weapon attribute. */
	float GetAttribute(const ARRWeapon* Weapon, const FGameplayAttribute& Attribute)
	{
		return Weapon->GetAbilitySystemComponent()->GetNumericAttribute(Attribute);
	}

	/** Ticks Frames fixed frames. */
	void TickFrames(const FTestWorld& TestWorld, int32 Frames)
	{
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTurretLauncherStatsTest, "RogueRigs.Turret.LauncherStats",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTurretLauncherStatsTest::RunTest(const FString& Parameters)
{
	using namespace RRTurretTests;

	FTestWorld TestWorld(TEXT("RRTurretLauncherStats"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRTurretLauncher* Launcher = SetUpMiner(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, Grid, Miner);
	if (!Launcher)
	{
		return false;
	}

	TestTrue(TEXT("the launcher is in the miner's weapon list"), Miner->GetWeapons().Contains(Launcher));
	TestEqual(TEXT("FindTurretLauncher finds it"), Miner->FindTurretLauncher(), Launcher);
	TestEqual(TEXT("launcher owned by the miner"), Launcher->GetOwnerMiner(), Miner);
	TestEqual(TEXT("damage 6"), GetAttribute(Launcher, URRWeaponAttributeSet::GetDamageAttribute()), TurretDamage, 1e-4f);
	TestEqual(TEXT("fire rate 3.3 shots/s"), GetAttribute(Launcher, URRWeaponAttributeSet::GetFireRateAttribute()), TurretFireRate, 1e-4f);
	TestEqual(TEXT("pierce 4"), GetAttribute(Launcher, URRWeaponAttributeSet::GetPierceAttribute()), TurretPierce, 1e-4f);
	TestEqual(TEXT("crit chance 5 %"), GetAttribute(Launcher, URRWeaponAttributeSet::GetCritChanceAttribute()), TurretCritChance, 1e-4f);
	TestEqual(TEXT("crit damage x1.5"), GetAttribute(Launcher, URRWeaponAttributeSet::GetCritDamageAttribute()), TurretCritDamage, 1e-4f);
	TestEqual(TEXT("2 charges max"), Launcher->GetMaxCharges(), TurretCharges);
	TestEqual(TEXT("starts with 2 charges"), Launcher->GetCharges(), TurretCharges);
	TestEqual(TEXT("recharge 6 s"), Launcher->GetRechargeDuration(), RechargeSeconds, 1e-4f);
	TestEqual(TEXT("turret lifetime 10 s"), Launcher->GetTurretLifetime(), TurretLifetime, 1e-4f);
	TestEqual(TEXT("range 1500 uu"), Launcher->GetRange(), TurretRange, 1e-3f);
	TestFalse(TEXT("not recharging when full"), Launcher->IsRecharging());
	TestFalse(TEXT("the launcher never auto-fires"), Launcher->GetAutoFireComponent()->IsFiring());

	// Never automatic: standing still for 2 s places nothing.
	TickFrames(TestWorld, 120);
	int32 Turrets = 0;
	for (TActorIterator<ARRTurret> It(TestWorld.Get()); It; ++It)
	{
		++Turrets;
	}
	TestEqual(TEXT("no turret without the button"), Turrets, 0);
	TestEqual(TEXT("charges untouched"), Launcher->GetCharges(), TurretCharges);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTurretChargesTest, "RogueRigs.Turret.ChargesAndRecharge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTurretChargesTest::RunTest(const FString& Parameters)
{
	using namespace RRTurretTests;

	FTestWorld TestWorld(TEXT("RRTurretCharges"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRTurretLauncher* Launcher = SetUpMiner(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, Grid, Miner);
	if (!Launcher)
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Launcher->OnTurretChargesChanged.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleTurretChargesChanged);

	// The button path: ARRMiner::PlaceTurret.
	ARRTurret* First = Miner->PlaceTurret();
	TestNotNull(TEXT("first press places a turret"), First);
	TestEqual(TEXT("1 charge left"), Launcher->GetCharges(), 1);
	TestTrue(TEXT("the spent charge recharges"), Launcher->IsRecharging());
	TestEqual(TEXT("recharge 6 s left"), Launcher->GetRechargeRemaining(), RechargeSeconds, 1e-4f);
	if (TestEqual(TEXT("one charge broadcast"), Recorder->GetTurretCharges().Num(), 1))
	{
		TestEqual(TEXT("broadcast (1, 2, 6)"), Recorder->GetTurretCharges()[0], FVector(1.f, TurretCharges, RechargeSeconds));
	}

	ARRTurret* Second = Miner->PlaceTurret();
	TestNotNull(TEXT("second press places a turret"), Second);
	TestEqual(TEXT("0 charges left"), Launcher->GetCharges(), 0);
	TestEqual(TEXT("still one recharge, not restarted"), Launcher->GetRechargeRemaining(), RechargeSeconds, 1e-4f);
	TestNull(TEXT("third press without a charge places nothing"), Miner->PlaceTurret());
	TestEqual(TEXT("2 turrets placed"), Launcher->GetTurretsPlaced(), 2);

	// One charge at a time: 5.9 s nothing, 6 s one charge, 12 s the second.
	Launcher->AdvanceRecharge(5.9f);
	TestEqual(TEXT("no charge after 5.9 s"), Launcher->GetCharges(), 0);
	TestEqual(TEXT("progress 5.9 / 6"), Launcher->GetRechargeProgress(), 5.9f / 6.f, 1e-3f);
	Launcher->AdvanceRecharge(0.2f);
	TestEqual(TEXT("1 charge after 6.1 s"), Launcher->GetCharges(), 1);
	TestTrue(TEXT("the second charge recharges next"), Launcher->IsRecharging());
	TestEqual(TEXT("leftover 0.1 s carried"), Launcher->GetRechargeRemaining(), 5.9f, 1e-3f);
	Launcher->AdvanceRecharge(5.9f);
	TestEqual(TEXT("2 charges after 12 s"), Launcher->GetCharges(), TurretCharges);
	TestFalse(TEXT("no recharge when full"), Launcher->IsRecharging());
	TestEqual(TEXT("last broadcast (2, 2, 0)"), Recorder->GetTurretCharges().Last(), FVector(TurretCharges, TurretCharges, 0.f));

	// ReloadSpeed shortens the recharge (reload speed upgrades).
	SetAttribute(Launcher, URRWeaponAttributeSet::GetReloadSpeedAttribute(), 2.f);
	TestNotNull(TEXT("placement after the recharge"), Miner->PlaceTurret());
	TestEqual(TEXT("recharge 6 / 2 = 3 s"), Launcher->GetRechargeRemaining(), 3.f, 1e-4f);

	// A MagazineSize upgrade adds a charge to recharge.
	Launcher->AdvanceRecharge(3.f);
	SetAttribute(Launcher, URRWeaponAttributeSet::GetMagazineSizeAttribute(), 3.f);
	Launcher->AdvanceRecharge(0.01f);
	TestEqual(TEXT("3 charges max after the upgrade"), Launcher->GetMaxCharges(), 3);
	TestTrue(TEXT("the new charge recharges"), Launcher->IsRecharging());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTurretPlacementTest, "RogueRigs.Turret.Placement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTurretPlacementTest::RunTest(const FString& Parameters)
{
	using namespace RRTurretTests;

	// Open floor, stick aim +X: the floor cell 200 uu ahead.
	{
		FTestWorld TestWorld(TEXT("RRTurretPlacementOpen"));
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRTurretLauncher* Launcher = SetUpMiner(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, Grid, Miner);
		if (!Launcher)
		{
			return false;
		}
		const FIntPoint Aimed = Grid->GetCellAtWorld(Miner->GetActorLocation() + FVector(200.f, 0.f, 0.f));
		AddInfo(FString::Printf(TEXT("cell size %.0f uu, aimed cell (%d, %d)"), Grid->GetCellSize(), Aimed.X, Aimed.Y));
		TestTrue(TEXT("aimed cell is ahead of the miner"), Aimed.X > OpenMinerCell.X && Aimed.Y == OpenMinerCell.Y);
		TestEqual(TEXT("aimed cell is floor"), Grid->GetCellType(Aimed), ERRCellType::Floor);
		ARRTurret* Turret = Miner->PlaceTurret();
		if (!TestNotNull(TEXT("turret placed"), Turret))
		{
			return false;
		}
		TestTrue(TEXT("turret on the aimed floor cell center"), Turret->GetActorLocation().Equals(Grid->GetCellCenter(Aimed.X, Aimed.Y), 0.5f));
		TestEqual(TEXT("turret faces the aim"), Turret->GetActorRotation().Yaw, 0.0, 0.01);
		TestEqual(TEXT("turret owner: the launcher"), Turret->GetOwner(), static_cast<AActor*>(Launcher));
		TestEqual(TEXT("turret instigator: the miner"), Turret->GetInstigator(), static_cast<APawn*>(Miner));
	}

	// Aimed cell is rock: the floor cell along the aim the miner sees nearest to the aimed point (in front of the rock, not behind it).
	{
		FTestWorld TestWorld(TEXT("RRTurretPlacementRock"));
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRTurretLauncher* Launcher = SetUpMiner(*this, TestWorld, NextToRockMinerCell, -FVector::ForwardVector, Grid, Miner);
		if (!Launcher)
		{
			return false;
		}
		TestTrue(TEXT("layout: the aimed cell (200 uu ahead) is rock"), Grid->IsBlockCell(Grid->GetCellAtWorld(Miner->GetActorLocation() - FVector(200.f, 0.f, 0.f))));
		TestTrue(TEXT("layout: rock next to the miner"), Grid->IsBlockCell(RockCellWest));
		TestEqual(TEXT("layout: floor behind the rock"), Grid->GetCellType(BehindRockCell), ERRCellType::Floor);
		FVector Location;
		TestTrue(TEXT("a placement is found"), Launcher->FindPlacementLocation(Location));
		TestEqual(TEXT("placement cell: in front of the rock"), Grid->GetCellAtWorld(Location), FrontOfRockCell);
		ARRTurret* Turret = Miner->PlaceTurret();
		if (TestNotNull(TEXT("turret placed"), Turret))
		{
			TestTrue(TEXT("turret on the floor cell center"), Turret->GetActorLocation().Equals(Grid->GetCellCenter(FrontOfRockCell.X, FrontOfRockCell.Y), 0.5f));
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTurretStatCopyTest, "RogueRigs.Turret.StatCopy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTurretStatCopyTest::RunTest(const FString& Parameters)
{
	using namespace RRTurretTests;

	FTestWorld TestWorld(TEXT("RRTurretStatCopy"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRTurretLauncher* Launcher = SetUpMiner(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, Grid, Miner);
	if (!Launcher)
	{
		return false;
	}

	// Upgraded launcher at placement time.
	SetAttribute(Launcher, URRWeaponAttributeSet::GetDamageAttribute(), 12.f);
	SetAttribute(Launcher, URRWeaponAttributeSet::GetPierceAttribute(), 6.f);
	ARRTurret* Turret = Miner->PlaceTurret();
	if (!TestNotNull(TEXT("turret placed"), Turret))
	{
		return false;
	}
	TestEqual(TEXT("copied damage 12"), GetAttribute(Turret, URRWeaponAttributeSet::GetDamageAttribute()), 12.f, 1e-4f);
	TestEqual(TEXT("copied pierce 6"), Turret->GetPierceCount(), 6);
	TestEqual(TEXT("copied fire rate 3.3"), GetAttribute(Turret, URRWeaponAttributeSet::GetFireRateAttribute()), TurretFireRate, 1e-4f);
	TestEqual(TEXT("copied crit chance"), GetAttribute(Turret, URRWeaponAttributeSet::GetCritChanceAttribute()), TurretCritChance, 1e-4f);
	TestEqual(TEXT("copied crit damage"), GetAttribute(Turret, URRWeaponAttributeSet::GetCritDamageAttribute()), TurretCritDamage, 1e-4f);
	TestEqual(TEXT("copied range"), Turret->GetRange(), TurretRange, 1e-3f);
	TestEqual(TEXT("copied projectile speed"), Turret->GetProjectileSpeed(), Launcher->GetProjectileSpeed(), 1e-3f);
	TestTrue(TEXT("bullet class from the launcher"), Turret->GetProjectileClass() == ARRTurretBullet::StaticClass());
	TestEqual(TEXT("lifetime 10 s"), Turret->GetLifetime(), TurretLifetime, 1e-4f);
	TestEqual(TEXT("life span 10 s"), Turret->GetLifeSpan(), TurretLifetime, 0.05f);
	TestFalse(TEXT("turret fires without magazine"), Turret->GetAutoFireComponent()->UsesMagazine());
	TestFalse(TEXT("turret has no collision (invincible, never blocks)"), Turret->GetActorEnableCollision());
	TestFalse(TEXT("turret is not a miner weapon"), Miner->GetWeapons().Contains(static_cast<ARRWeapon*>(Turret)));
	TestNull(TEXT("turret not held by the miner"), Turret->GetOwnerMiner());

	// Later upgrades reach only turrets placed later.
	SetAttribute(Launcher, URRWeaponAttributeSet::GetDamageAttribute(), 20.f);
	TestEqual(TEXT("placed turret keeps damage 12"), GetAttribute(Turret, URRWeaponAttributeSet::GetDamageAttribute()), 12.f, 1e-4f);
	ARRTurret* Later = Miner->PlaceTurret();
	if (TestNotNull(TEXT("second turret placed"), Later))
	{
		TestEqual(TEXT("new turret has damage 20"), GetAttribute(Later, URRWeaponAttributeSet::GetDamageAttribute()), 20.f, 1e-4f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTurretLineOfSightTest, "RogueRigs.Turret.LineOfSight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTurretLineOfSightTest::RunTest(const FString& Parameters)
{
	using namespace RRTurretTests;

	FTestWorld TestWorld(TEXT("RRTurretLineOfSight"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	ARRTurretLauncher* Launcher = SetUpMiner(*this, TestWorld, RockMinerCell, -FVector::ForwardVector, Grid, Miner);
	if (!Launcher)
	{
		return false;
	}
	ARRTurret* Turret = Miner->PlaceTurret();
	if (!TestNotNull(TEXT("turret placed"), Turret))
	{
		return false;
	}
	TestEqual(TEXT("turret in front of the rock"), Grid->GetCellAtWorld(Turret->GetActorLocation()), FrontOfRockCell);

	// Hidden robot 400 uu away behind the rock, visible robot 800 uu away on the open side.
	ARRChaser* Hidden = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(BehindRockCell.X, BehindRockCell.Y));
	ARRChaser* Visible = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(VisibleCell.X, VisibleCell.Y));
	if (!Hidden || !Visible)
	{
		return false;
	}
	TestFalse(TEXT("the hidden robot is not a valid target"), Turret->IsValidTarget(Hidden));
	TestTrue(TEXT("the visible robot is a valid target"), Turret->IsValidTarget(Visible));
	TestWorld.Tick();
	TestEqual(TEXT("locks the visible robot, not the nearer hidden one"), Turret->GetTarget(), static_cast<ARREnemy*>(Visible));
	TestTrue(TEXT("fires while locked"), Turret->GetAutoFireComponent()->IsFiring());

	// Visible robot gone: nothing in sight, the turret holds fire.
	Visible->Destroy();
	TickFrames(TestWorld, 2);
	TestNull(TEXT("no target without sight"), Turret->GetTarget());
	TestFalse(TEXT("holds fire without a target"), Turret->GetAutoFireComponent()->IsFiring());

	// Rock dug: the robot behind it comes into sight and is locked at the next search.
	Grid->DestroyCell(RockCellWest, ERRTerrainDamageSource::Drone);
	TickFrames(TestWorld, 10);
	TestEqual(TEXT("retargets the robot now in sight"), Turret->GetTarget(), static_cast<ARREnemy*>(Hidden));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTurretFireAndLifetimeTest, "RogueRigs.Turret.FireRangeLifetime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTurretFireAndLifetimeTest::RunTest(const FString& Parameters)
{
	using namespace RRTurretTests;

	// No grid, no miner: a native launcher acquired alone, facing +X; the turret lands 200 uu ahead at the launcher height.
	FTestWorld TestWorld(TEXT("RRTurretFire"));
	ARRTurretLauncher* Launcher = TestWorld.Spawn<ARRTurretLauncher>(ARRTurretLauncher::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("launcher spawned"), Launcher) || !TestTrue(TEXT("launcher acquired without a miner"), Launcher->Acquire(nullptr)))
	{
		return false;
	}
	SetAttribute(Launcher, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	ARRTurret* Turret = Launcher->TryPlaceTurret();
	if (!TestNotNull(TEXT("turret placed"), Turret))
	{
		return false;
	}
	const FVector Base = Turret->GetActorLocation();
	TestTrue(TEXT("turret 200 uu ahead"), Base.Equals(FVector(200.f, 0.f, 0.f), 0.5f));

	ARRChaser* Near = SpawnFrozenChaser(*this, TestWorld, Base + FVector(700.f, 0.f, 0.f));
	ARRChaser* Far = SpawnFrozenChaser(*this, TestWorld, Base + FVector(0.f, 1000.f, 0.f));
	ARRChaser* OutOfRange = SpawnFrozenChaser(*this, TestWorld, Base + FVector(-1600.f, 0.f, 0.f));
	if (!Near || !Far || !OutOfRange)
	{
		return false;
	}
	TestFalse(TEXT("1600 uu is beyond the 1500 uu range"), Turret->IsValidTarget(OutOfRange));
	TestTrue(TEXT("1000 uu is within range"), Turret->IsValidTarget(Far));
	int32 NearHits = 0;
	float NearDamage = 0.f;
	Near->GetHealthSet()->OnDamageReceived.AddLambda([&NearHits, &NearDamage](float Damage, bool, float) { ++NearHits; NearDamage = Damage; });

	TestWorld.Tick();
	TestEqual(TEXT("locks the nearest robot"), Turret->GetTarget(), static_cast<ARREnemy*>(Near));

	// Lock: a nearer robot appearing does not steal the target.
	ARRChaser* Newcomer = SpawnFrozenChaser(*this, TestWorld, Base + FVector(0.f, -300.f, 0.f));
	TestWorld.Tick();
	TestEqual(TEXT("keeps the locked robot"), Turret->GetTarget(), static_cast<ARREnemy*>(Near));

	// 1 s of fire at 3.3 shots/s: shots at 0, 0.30, 0.61, 0.91 s.
	TickFrames(TestWorld, 58);
	TestEqual(TEXT("4 bullets in the first second"), Turret->GetShotCount(), 4);
	bool bCheckedBullet = false;
	for (TActorIterator<ARRTurretBullet> It(TestWorld.Get()); It; ++It)
	{
		TestEqual(TEXT("bullet pierces 4 robots"), It->GetMaxHits(), 4);
		TestEqual(TEXT("bullet owner: the turret"), It->GetOwner(), static_cast<AActor*>(Turret));
		bCheckedBullet = true;
		break;
	}
	TestTrue(TEXT("a bullet is in flight"), bCheckedBullet);
	TestTrue(TEXT("the near robot was hit"), NearHits >= 3);
	TestEqual(TEXT("6 damage per bullet"), NearDamage, TurretDamage, 1e-3f);

	// Target gone: retarget the nearest valid robot (the newcomer at 300 uu).
	Near->Destroy();
	TestWorld.Tick();
	TestEqual(TEXT("retargets the nearest robot"), Turret->GetTarget(), static_cast<ARREnemy*>(Newcomer));

	// Lifetime 10 s from placement (61 frames already ticked).
	TWeakObjectPtr<ARRTurret> WeakTurret(Turret);
	const int32 FramesTo99 = FMath::CeilToInt32(9.9f / FrameSeconds) - 61;
	TickFrames(TestWorld, FramesTo99);
	TestTrue(TEXT("turret still there at 9.9 s"), WeakTurret.IsValid() && !WeakTurret->IsActorBeingDestroyed());
	TickFrames(TestWorld, 9);
	TestTrue(TEXT("turret gone after 10 s"), !WeakTurret.IsValid() || WeakTurret->IsActorBeingDestroyed());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
