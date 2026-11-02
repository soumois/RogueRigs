// Rogue Rigs - automation tests of the turret launcher and its turrets (RogueRigs.Turret.*): CSV stats, button placement and charges, recharge one at a time, placement cell, stat copy at placement, nearest-target lock with grid line of sight, range, fire and lifetime, fixed base with a yawing head and shots from the canon tip.

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
#include "Tests/RRTestStatsRows.h"
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

	// Expected stats: WeaponStats.csv row TurretLauncher, read through Tests/RRTestStatsRows.h (no literal copy, task-420).

	/** TurretLauncher row of WeaponStats.csv (test error when missing). */
	FRRWeaponStatsRow ReadLauncherRow(FAutomationTestBase& Test)
	{
		return RRTestStats::WeaponRow(Test, RRTestStats::TurretLauncherRow);
	}

	/** Turret charges of the row: its MagazineSize. */
	int32 ChargesOf(const FRRWeaponStatsRow& Row)
	{
		return FMath::RoundToInt32(Row.MagazineSize);
	}

	/** Seconds to recharge one turret charge: ReloadTime / ReloadSpeed of the row. */
	float RechargeOf(const FRRWeaponStatsRow& Row)
	{
		return Row.ReloadTime / Row.ReloadSpeed;
	}

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
	const FRRWeaponStatsRow Row = ReadLauncherRow(*this);
	const int32 Charges = ChargesOf(Row);
	TestEqual(TEXT("damage = CSV Damage"), GetAttribute(Launcher, URRWeaponAttributeSet::GetDamageAttribute()), Row.Damage, 1e-4f);
	TestEqual(TEXT("fire rate = CSV FireRate"), GetAttribute(Launcher, URRWeaponAttributeSet::GetFireRateAttribute()), Row.FireRate, 1e-4f);
	TestEqual(TEXT("pierce = CSV Pierce"), GetAttribute(Launcher, URRWeaponAttributeSet::GetPierceAttribute()), Row.Pierce, 1e-4f);
	TestEqual(TEXT("crit chance = CSV CritChance"), GetAttribute(Launcher, URRWeaponAttributeSet::GetCritChanceAttribute()), Row.CritChance, 1e-4f);
	TestEqual(TEXT("crit damage = CSV CritDamage"), GetAttribute(Launcher, URRWeaponAttributeSet::GetCritDamageAttribute()), Row.CritDamage, 1e-4f);
	TestEqual(TEXT("max charges = CSV MagazineSize"), Launcher->GetMaxCharges(), Charges);
	TestEqual(TEXT("starts with every charge"), Launcher->GetCharges(), Charges);
	TestEqual(TEXT("recharge = CSV ReloadTime / ReloadSpeed"), Launcher->GetRechargeDuration(), RechargeOf(Row), 1e-4f);
	TestEqual(TEXT("turret lifetime = CSV TurretLifetime"), Launcher->GetTurretLifetime(), Row.TurretLifetime, 1e-4f);
	TestEqual(TEXT("range = CSV Range"), Launcher->GetRange(), Row.Range, 1e-3f);
	TestFalse(TEXT("not recharging when full"), Launcher->IsRecharging());
	TestFalse(TEXT("the launcher never auto-fires"), Launcher->GetAutoFireComponent()->IsFiring());

	// Never automatic: standing still for 2 s places nothing.
	RRTurretTests::TickFrames(TestWorld, 120);
	int32 Turrets = 0;
	for (TActorIterator<ARRTurret> It(TestWorld.Get()); It; ++It)
	{
		++Turrets;
	}
	TestEqual(TEXT("no turret without the button"), Turrets, 0);
	TestEqual(TEXT("charges untouched"), Launcher->GetCharges(), Charges);
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

	// Charges and recharge time from the row (MagazineSize charges, ReloadTime / ReloadSpeed each).
	const FRRWeaponStatsRow Row = ReadLauncherRow(*this);
	const int32 Charges = ChargesOf(Row);
	const float Recharge = RechargeOf(Row);
	if (!TestTrue(TEXT("CSV: at least one charge and a recharge longer than 0.2 s"), Charges >= 1 && Recharge > 0.2f))
	{
		return false;
	}

	// The button path: ARRMiner::PlaceTurret.
	ARRTurret* First = Miner->PlaceTurret();
	TestNotNull(TEXT("first press places a turret"), First);
	TestEqual(TEXT("one charge spent"), Launcher->GetCharges(), Charges - 1);
	TestTrue(TEXT("the spent charge recharges"), Launcher->IsRecharging());
	TestEqual(TEXT("full recharge left"), Launcher->GetRechargeRemaining(), Recharge, 1e-4f);
	if (TestEqual(TEXT("one charge broadcast"), Recorder->GetTurretCharges().Num(), 1))
	{
		TestEqual(TEXT("broadcast (charges - 1, charges, recharge)"), Recorder->GetTurretCharges()[0], FVector(Charges - 1, Charges, Recharge));
	}

	// Every other charge spent: still one recharge, not restarted.
	for (int32 Press = 1; Press < Charges; ++Press)
	{
		TestNotNull(FString::Printf(TEXT("press %d places a turret"), Press + 1), Miner->PlaceTurret());
	}
	TestEqual(TEXT("0 charges left"), Launcher->GetCharges(), 0);
	TestEqual(TEXT("still one recharge, not restarted"), Launcher->GetRechargeRemaining(), Recharge, 1e-4f);
	TestNull(TEXT("a press without a charge places nothing"), Miner->PlaceTurret());
	TestEqual(TEXT("one turret per charge placed"), Launcher->GetTurretsPlaced(), Charges);

	// One charge at a time: 0.1 s before the recharge nothing, 0.1 s after it one charge with the leftover carried, then one more
	// charge per recharge until full (float steps: the last one overshoots).
	Launcher->AdvanceRecharge(Recharge - 0.1f);
	TestEqual(TEXT("no charge 0.1 s before the recharge ends"), Launcher->GetCharges(), 0);
	TestEqual(TEXT("progress (recharge - 0.1) / recharge"), Launcher->GetRechargeProgress(), (Recharge - 0.1f) / Recharge, 1e-3f);
	Launcher->AdvanceRecharge(0.2f);
	TestEqual(TEXT("1 charge 0.1 s after the recharge"), Launcher->GetCharges(), 1);
	TestEqual(TEXT("the next charge recharges unless full"), Launcher->IsRecharging(), Charges > 1);
	if (Charges > 1)
	{
		TestEqual(TEXT("leftover 0.1 s carried"), Launcher->GetRechargeRemaining(), Recharge - 0.1f, 1e-3f);
	}
	for (int32 Charge = 1; Charge < Charges; ++Charge)
	{
		Launcher->AdvanceRecharge(Recharge);
	}
	TestEqual(TEXT("every charge back"), Launcher->GetCharges(), Charges);
	TestFalse(TEXT("no recharge when full"), Launcher->IsRecharging());
	TestEqual(TEXT("last broadcast (charges, charges, 0)"), Recorder->GetTurretCharges().Last(), FVector(Charges, Charges, 0.f));

	// ReloadSpeed shortens the recharge (reload speed upgrades): twice the CSV ReloadSpeed halves it.
	SetAttribute(Launcher, URRWeaponAttributeSet::GetReloadSpeedAttribute(), 2.f * Row.ReloadSpeed);
	TestNotNull(TEXT("placement after the recharge"), Miner->PlaceTurret());
	TestEqual(TEXT("recharge halved"), Launcher->GetRechargeRemaining(), 0.5f * Recharge, 1e-4f);

	// A MagazineSize upgrade adds a charge to recharge.
	Launcher->AdvanceRecharge(0.5f * Recharge);
	SetAttribute(Launcher, URRWeaponAttributeSet::GetMagazineSizeAttribute(), Row.MagazineSize + 1.f);
	Launcher->AdvanceRecharge(0.01f);
	TestEqual(TEXT("one more max charge after the upgrade"), Launcher->GetMaxCharges(), Charges + 1);
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

	// Upgraded launcher at placement time: damage x2 and pierce +2 over the CSV row.
	const FRRWeaponStatsRow Row = ReadLauncherRow(*this);
	const float UpgradedDamage = 2.f * Row.Damage;
	const int32 UpgradedPierce = FMath::RoundToInt32(Row.Pierce) + 2;
	SetAttribute(Launcher, URRWeaponAttributeSet::GetDamageAttribute(), UpgradedDamage);
	SetAttribute(Launcher, URRWeaponAttributeSet::GetPierceAttribute(), static_cast<float>(UpgradedPierce));
	ARRTurret* Turret = Miner->PlaceTurret();
	if (!TestNotNull(TEXT("turret placed"), Turret))
	{
		return false;
	}
	TestEqual(TEXT("copied upgraded damage"), GetAttribute(Turret, URRWeaponAttributeSet::GetDamageAttribute()), UpgradedDamage, 1e-4f);
	TestEqual(TEXT("copied upgraded pierce"), Turret->GetPierceCount(), UpgradedPierce);
	TestEqual(TEXT("copied fire rate = CSV FireRate"), GetAttribute(Turret, URRWeaponAttributeSet::GetFireRateAttribute()), Row.FireRate, 1e-4f);
	TestEqual(TEXT("copied crit chance = CSV CritChance"), GetAttribute(Turret, URRWeaponAttributeSet::GetCritChanceAttribute()), Row.CritChance, 1e-4f);
	TestEqual(TEXT("copied crit damage = CSV CritDamage"), GetAttribute(Turret, URRWeaponAttributeSet::GetCritDamageAttribute()), Row.CritDamage, 1e-4f);
	TestEqual(TEXT("copied range = CSV Range"), Turret->GetRange(), Row.Range, 1e-3f);
	TestEqual(TEXT("copied projectile speed"), Turret->GetProjectileSpeed(), Launcher->GetProjectileSpeed(), 1e-3f);
	TestTrue(TEXT("bullet class from the launcher"), Turret->GetProjectileClass() == ARRTurretBullet::StaticClass());
	TestEqual(TEXT("lifetime = CSV TurretLifetime"), Turret->GetLifetime(), Row.TurretLifetime, 1e-4f);
	TestEqual(TEXT("life span = CSV TurretLifetime"), Turret->GetLifeSpan(), Row.TurretLifetime, 0.05f);
	TestFalse(TEXT("turret fires without magazine"), Turret->GetAutoFireComponent()->UsesMagazine());
	TestFalse(TEXT("turret has no collision (invincible, never blocks)"), Turret->GetActorEnableCollision());
	TestFalse(TEXT("turret is not a miner weapon"), Miner->GetWeapons().Contains(static_cast<ARRWeapon*>(Turret)));
	TestNull(TEXT("turret not held by the miner"), Turret->GetOwnerMiner());

	// Later upgrades (damage x3) reach only turrets placed later.
	const float LaterDamage = 3.f * Row.Damage;
	SetAttribute(Launcher, URRWeaponAttributeSet::GetDamageAttribute(), LaterDamage);
	TestEqual(TEXT("placed turret keeps its damage"), GetAttribute(Turret, URRWeaponAttributeSet::GetDamageAttribute()), UpgradedDamage, 1e-4f);
	ARRTurret* Later = Miner->PlaceTurret();
	if (TestNotNull(TEXT("second turret placed"), Later))
	{
		TestEqual(TEXT("new turret has the later damage"), GetAttribute(Later, URRWeaponAttributeSet::GetDamageAttribute()), LaterDamage, 1e-4f);
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
	RRTurretTests::TickFrames(TestWorld, 2);
	TestNull(TEXT("no target without sight"), Turret->GetTarget());
	TestFalse(TEXT("holds fire without a target"), Turret->GetAutoFireComponent()->IsFiring());

	// Rock dug: the robot behind it comes into sight and is locked at the next search.
	Grid->DestroyCell(RockCellWest, ERRTerrainDamageSource::Drone);
	RRTurretTests::TickFrames(TestWorld, 10);
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

	// Expected fire, hits and lifetime derived from the TurretLauncher row.
	const FRRWeaponStatsRow Row = ReadLauncherRow(*this);
	constexpr float NearDistance = 700.f;
	ARRChaser* Near = SpawnFrozenChaser(*this, TestWorld, Base + FVector(NearDistance, 0.f, 0.f));
	ARRChaser* Far = SpawnFrozenChaser(*this, TestWorld, Base + FVector(0.f, 1000.f, 0.f));
	ARRChaser* OutOfRange = SpawnFrozenChaser(*this, TestWorld, Base + FVector(-(Row.Range + 100.f), 0.f, 0.f));
	if (!Near || !Far || !OutOfRange)
	{
		return false;
	}
	TestFalse(TEXT("100 uu beyond the CSV range is out of range"), Turret->IsValidTarget(OutOfRange));
	TestTrue(TEXT("1000 uu is within range"), Turret->IsValidTarget(Far));

	// Shots in the first second: the first one in the lock frame, then one every 1 / FireRate; 59 frames follow the lock frame.
	// One frame of tolerance on the first shot gives the bounds; bullets reach the near robot after NearDistance / speed.
	constexpr int32 FireFrames = 59;
	const double Elapsed = FireFrames * static_cast<double>(FrameSeconds);
	const int32 MinShots = FMath::FloorToInt32((Elapsed - FrameSeconds) * Row.FireRate) + 1;
	const int32 MaxShots = FMath::FloorToInt32((Elapsed + FrameSeconds) * Row.FireRate) + 1;
	const double Flight = NearDistance / FMath::Max(Launcher->GetProjectileSpeed(), 1.f);
	const int32 MinArrived = FMath::Max(0, FMath::FloorToInt32((Elapsed - 2.0 * FrameSeconds - Flight) * Row.FireRate) + 1);
	AddInfo(FString::Printf(TEXT("CSV FireRate %g: %d to %d shots and at least %d hits in the first second"), Row.FireRate, MinShots, MaxShots, MinArrived));

	// The near robot survives every hit of that second (health from the row).
	UAbilitySystemComponent* NearASC = Near->GetAbilitySystemComponent();
	const float NearHealth = (MaxShots + 1) * Row.Damage;
	NearASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), NearHealth);
	NearASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), NearHealth);
	int32 NearHits = 0;
	float NearDamage = 0.f;
	Near->GetHealthSet()->OnDamageReceived.AddLambda([&NearHits, &NearDamage](float Damage, bool, float) { ++NearHits; NearDamage = Damage; });

	TestWorld.Tick();
	TestEqual(TEXT("locks the nearest robot"), Turret->GetTarget(), static_cast<ARREnemy*>(Near));

	// Lock: a nearer robot appearing does not steal the target.
	ARRChaser* Newcomer = SpawnFrozenChaser(*this, TestWorld, Base + FVector(0.f, -300.f, 0.f));
	TestWorld.Tick();
	TestEqual(TEXT("keeps the locked robot"), Turret->GetTarget(), static_cast<ARREnemy*>(Near));

	// 1 s of fire at the CSV FireRate.
	RRTurretTests::TickFrames(TestWorld, FireFrames - 1);
	const int32 Shots = Turret->GetShotCount();
	AddInfo(FString::Printf(TEXT("%d bullets, %d hits on the near robot in the first second"), Shots, NearHits));
	TestTrue(FString::Printf(TEXT("%d to %d bullets in the first second (%d)"), MinShots, MaxShots, Shots), Shots >= MinShots && Shots <= MaxShots);
	bool bCheckedBullet = false;
	for (TActorIterator<ARRTurretBullet> It(TestWorld.Get()); It; ++It)
	{
		TestEqual(TEXT("bullet pierces CSV Pierce robots"), It->GetMaxHits(), FMath::RoundToInt32(Row.Pierce));
		TestEqual(TEXT("bullet owner: the turret"), It->GetOwner(), static_cast<AActor*>(Turret));
		bCheckedBullet = true;
		break;
	}
	TestTrue(TEXT("a bullet is in flight"), bCheckedBullet);
	TestTrue(FString::Printf(TEXT("the near robot was hit by the bullets that arrived (%d to %d)"), MinArrived, Shots), NearHits >= MinArrived && NearHits <= Shots);
	TestEqual(TEXT("CSV Damage per bullet"), NearDamage, Row.Damage, 1e-3f);

	// Target gone: retarget the nearest valid robot (the newcomer at 300 uu).
	Near->Destroy();
	TestWorld.Tick();
	TestEqual(TEXT("retargets the nearest robot"), Turret->GetTarget(), static_cast<ARREnemy*>(Newcomer));

	// CSV TurretLifetime from placement (FireFrames + 2 frames already ticked): there 0.1 s before it, gone 0.05 s after it.
	TWeakObjectPtr<ARRTurret> WeakTurret(Turret);
	const int32 FramesTicked = FireFrames + 2;
	const int32 FramesBeforeEnd = FMath::RoundToInt32((Row.TurretLifetime - 0.1f) / FrameSeconds) - FramesTicked;
	const int32 FramesAfterEnd = FMath::RoundToInt32((Row.TurretLifetime + 0.05f) / FrameSeconds) - FramesTicked - FramesBeforeEnd;
	RRTurretTests::TickFrames(TestWorld, FramesBeforeEnd);
	TestTrue(TEXT("turret still there 0.1 s before its lifetime"), WeakTurret.IsValid() && !WeakTurret->IsActorBeingDestroyed());
	RRTurretTests::TickFrames(TestWorld, FramesAfterEnd);
	TestTrue(TEXT("turret gone after its lifetime"), !WeakTurret.IsValid() || WeakTurret->IsActorBeingDestroyed());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTurretHeadYawTest, "RogueRigs.Turret.HeadYaw",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTurretHeadYawTest::RunTest(const FString& Parameters)
{
	using namespace RRTurretTests;

	// task-350-01 (owner Q3, Q4): the base keeps its placement yaw, HeadPivot turns toward the locked robot, shots leave the
	// Muzzle (under HeadPivot) at ShotHeight while range and sight stay measured from the turret centre.
	FTestWorld TestWorld(TEXT("RRTurretHeadYaw"));
	ARRTurretLauncher* Launcher = TestWorld.Spawn<ARRTurretLauncher>(ARRTurretLauncher::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("launcher spawned"), Launcher) || !TestTrue(TEXT("launcher acquired without a miner"), Launcher->Acquire(nullptr)))
	{
		return false;
	}
	SetAttribute(Launcher, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	ARRTurret* Turret = Launcher->TryPlaceTurret();
	if (!TestNotNull(TEXT("turret placed"), Turret) || !TestNotNull(TEXT("turret has a head pivot"), Turret->GetHeadPivot()))
	{
		return false;
	}
	USceneComponent* HeadPivot = Turret->GetHeadPivot();
	USceneComponent* Muzzle = FWeaponAccess::MuzzleOf(Turret);
	if (!TestNotNull(TEXT("turret has a muzzle"), Muzzle))
	{
		return false;
	}
	TestTrue(TEXT("the head pivot hangs under the root"), HeadPivot->GetAttachParent() == Turret->GetRootComponent());
	TestTrue(TEXT("the muzzle hangs under the head pivot"), Muzzle->GetAttachParent() == HeadPivot);

	// A canon tip 40 uu ahead of the yaw axis (BP_Turret sets the real one).
	constexpr float CanonLength = 40.f;
	Muzzle->SetRelativeLocation(FVector(CanonLength, 0.f, 30.f));
	const FVector Base = Turret->GetActorLocation();
	const double PlacementYaw = Turret->GetActorRotation().Yaw;
	TestEqual(TEXT("placed facing the aim (+X)"), PlacementYaw, 0.0, 0.01);
	const float ShotHeight = Turret->GetShotHeight();
	TestTrue(TEXT("targeting origin: ShotHeight above the turret centre"), Turret->GetTargetingOrigin().Equals(Base + FVector(0.f, 0.f, ShotHeight), 0.01f));

	// Locks a robot 700 uu to its left (+Y): actor yaw unchanged, head world yaw 90 within 1 degree.
	ARRChaser* Left = SpawnFrozenChaser(*this, TestWorld, Base + FVector(0.f, 700.f, 0.f));
	if (!Left)
	{
		return false;
	}
	int32 LeftHits = 0;
	Left->GetHealthSet()->OnDamageReceived.AddLambda([&LeftHits](float, bool, float) { ++LeftHits; });
	TestWorld.Tick();
	TestEqual(TEXT("locks the robot on its left"), Turret->GetTarget(), static_cast<ARREnemy*>(Left));
	auto YawToward = [HeadPivot](const AActor* Robot)
	{
		return (Robot->GetActorLocation() - HeadPivot->GetComponentLocation()).Rotation().Yaw;
	};
	TestEqual(TEXT("actor yaw unchanged after the lock"), Turret->GetActorRotation().Yaw, PlacementYaw, 0.01);
	TestTrue(FString::Printf(TEXT("head world yaw %.2f points at the left robot (%.2f) within 1 degree"), HeadPivot->GetComponentRotation().Yaw, YawToward(Left)),
		FMath::Abs(FMath::FindDeltaAngleDegrees(HeadPivot->GetComponentRotation().Yaw, YawToward(Left))) <= 1.0);
	const FVector ShotOrigin = Turret->GetShotOrigin();
	TestTrue(FString::Printf(TEXT("shot origin %s: the canon tip's horizontal position at ShotHeight"), *ShotOrigin.ToCompactString()),
		ShotOrigin.Equals(Base + FVector(0.f, CanonLength, ShotHeight), 0.5f));
	TestTrue(TEXT("aim from the canon tip toward the robot"), Turret->GetAimDirection().Equals(FVector(0.f, 1.f, 0.f), 1e-3f));

	// The bullets from the canon tip hit it.
	RRTurretTests::TickFrames(TestWorld, 30);
	TestTrue(TEXT("the turret fired"), Turret->GetShotCount() > 0);
	TestTrue(TEXT("the left robot was hit"), LeftHits > 0);
	TestEqual(TEXT("actor yaw unchanged while firing"), Turret->GetActorRotation().Yaw, PlacementYaw, 0.01);

	// Retarget behind-right (-135 degrees): the head follows at once, the base still does not turn.
	Left->Destroy();
	ARRChaser* Back = SpawnFrozenChaser(*this, TestWorld, Base + FVector(-500.f, -500.f, 0.f));
	if (!Back)
	{
		return false;
	}
	TestWorld.Tick();
	TestEqual(TEXT("retargets the robot behind"), Turret->GetTarget(), static_cast<ARREnemy*>(Back));
	TestEqual(TEXT("actor yaw unchanged after the retarget"), Turret->GetActorRotation().Yaw, PlacementYaw, 0.01);
	TestTrue(FString::Printf(TEXT("head world yaw %.2f points at the robot behind (%.2f) within 1 degree"), HeadPivot->GetComponentRotation().Yaw, YawToward(Back)),
		FMath::Abs(FMath::FindDeltaAngleDegrees(HeadPivot->GetComponentRotation().Yaw, YawToward(Back))) <= 1.0);
	const FVector Diagonal = FVector(-1.f, -1.f, 0.f).GetSafeNormal();
	TestTrue(TEXT("shot origin follows the head"), Turret->GetShotOrigin().Equals(Base + Diagonal * CanonLength + FVector(0.f, 0.f, ShotHeight), 0.5f));
	TestTrue(TEXT("range still measured from the centre"), FVector::Dist2D(Turret->GetTargetingOrigin(), Base) < 0.01);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
