// Rogue Rigs - automation tests of the weapon rules (RogueRigs.Weapons.*): burst timing, magazine and reload, projectile stop on rock, GAS damage and crits on a still chaser, miner starting weapon and shot origin, miner moving while firing, miner shots hitting a still chaser, projectile hit filter, pierce (robots per bullet, rock stop, no double hit, piercing upgrade).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemies/RRChaser.h"
#include "EngineUtils.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Misc/App.h"
#include "NiagaraComponent.h"
#include "NiagaraDataSetAccessor.h"
#include "NiagaraEmitterInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraSystemInstanceController.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "Projectiles/RRCollisionChannels.h"
#include "Projectiles/RREnemyProjectile.h"
#include "Projectiles/RRProjectile.h"
#include "Projectiles/RRTurretBullet.h"
#include "SceneInterface.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Tests/RRTestStatsRows.h"
#include "Upgrades/RRUpgradeCard.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRWeaponTests
{
	using namespace RRTestHarness;

	/** Blueprint class made in task-007-04. */
	const TCHAR* const BurstRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");

	/** Open floor of arena_m1: the still chaser stands on ChaserCell, the rifle fires from two cells west of it. */
	const FIntPoint ChaserCell(44, 41);
	const FIntPoint ChaserShooterCell(42, 41);

	/** Tolerance on times measured from the accumulator (the schedule is exact up to float rounding). */
	constexpr double ExactTolerance = 1.0e-4;

	/** Step used to fire a single shot: the shot due now fires, the next one (>= 0.033 s away) does not. */
	constexpr float SingleShotStep = 0.001f;

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** One shot seen by RecordShots: frame index of the step it fired in (step k covers [k, k + 1] * FrameSeconds) and its exact time. */
	struct FShotSample
	{
		int32 Frame = INDEX_NONE;
		double ExactSeconds = 0.0;
	};

	/** One hit received by the still chaser (URRHealthAttributeSet::OnDamageReceived). */
	struct FTargetHit
	{
		float Damage = 0.f;
		bool bIsCritical = false;
		float HealthLeft = 0.f;
	};

	/** Spawns BP_BurstRifle at Location facing +X and acquires it without miner (aim = its forward vector). */
	ARRWeapon* SpawnRifle(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& Location)
	{
		UClass* RifleClass = LoadTestClass<ARRWeapon>(Test, BurstRifleClassPath);
		if (!RifleClass)
		{
			return nullptr;
		}
		ARRWeapon* Rifle = TestWorld.Spawn<ARRWeapon>(RifleClass, Location);
		if (!Test.TestNotNull(TEXT("rifle spawned"), Rifle))
		{
			return nullptr;
		}
		Test.TestTrue(TEXT("rifle acquired (CSV row loaded, auto-fire configured)"), Rifle->Acquire(nullptr));
		return Rifle;
	}

	/** Spawns BP_Miner on Cell with WeaponAttachSocketName forced to None before BeginPlay: starting weapons (M1TestLoadout: rifle + shotgun) go on the capsule at WeaponAttachOffset. */
	ARRMiner* SpawnMinerWithCapsuleWeapons(FAutomationTestBase& Test, const FTestWorld& TestWorld, const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		return SpawnMiner(Test, TestWorld, Grid, Cell, [](ARRMiner* Miner) { FMinerAccess::WeaponAttachSocketNameOf(Miner) = NAME_None; M1TestLoadout(Miner); });
	}

	/**
	 * Lets an unpossessed miner walk as in play: CharacterMovement simulates without a controller (bRunPhysicsWithNoController)
	 * and leaves MOVE_None, the mode of an unpossessed character, for the default mode (walking, falling until it lands).
	 */
	void EnableMinerMovement(ARRMiner* Miner)
	{
		UCharacterMovementComponent* Movement = Miner->GetCharacterMovement();
		Movement->bRunPhysicsWithNoController = true;
		Movement->SetDefaultMovementMode();
	}

	/** Current value of a weapon attribute. */
	float WeaponAttribute(const ARRWeapon* Weapon, const FGameplayAttribute& Attribute)
	{
		return Weapon->GetAbilitySystemComponent()->GetNumericAttribute(Attribute);
	}

	/** Current Health of the still chaser. */
	float TargetHealth(const ARRChaser* StillChaser)
	{
		return StillChaser->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Moves the weapon (facing +X) so its muzzle sits on Target. */
	void PlaceMuzzleAt(ARRWeapon* Weapon, const FVector& Target)
	{
		const FVector MuzzleOffset = FWeaponAccess::MuzzleOf(Weapon)->GetComponentLocation() - Weapon->GetActorLocation();
		Weapon->SetActorLocation(Target - MuzzleOffset, /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
	}

	/** Live projectiles in the world. */
	TArray<ARRProjectile*> GetProjectiles(UWorld* World)
	{
		TArray<ARRProjectile*> Result;
		for (TActorIterator<ARRProjectile> It(World); It; ++It)
		{
			if (IsValid(*It))
			{
				Result.Add(*It);
			}
		}
		return Result;
	}

	/**
	 * Fires exactly one shot now: re-configures the auto-fire (next shot due now, ammo kept), advances it by SingleShotStep and
	 * stops it, so world ticks fire nothing else. Returns the projectile spawned by that shot (null if none).
	 */
	ARRProjectile* FireSingleShot(FAutomationTestBase& Test, ARRWeapon* Weapon)
	{
		URRAutoFireComponent* AutoFire = Weapon->GetAutoFireComponent();
		const TArray<ARRProjectile*> Before = GetProjectiles(Weapon->GetWorld());
		AutoFire->Configure(Weapon->GetAbilitySystemComponent(), Weapon->GetBurstCount(), Weapon->GetBurstDuty(), Weapon->GetReloadTime());
		AutoFire->StartFiring();
		AutoFire->AdvanceTime(SingleShotStep);
		AutoFire->StopFiring();
		Test.TestEqual(TEXT("one shot fired"), AutoFire->GetTotalShotsFired(), 1);

		ARRProjectile* Spawned = nullptr;
		for (ARRProjectile* Projectile : GetProjectiles(Weapon->GetWorld()))
		{
			if (!Before.Contains(Projectile))
			{
				Test.TestNull(TEXT("only one new projectile"), Spawned);
				Spawned = Projectile;
			}
		}
		Test.TestNotNull(TEXT("the shot spawned a projectile"), Spawned);
		return Spawned;
	}

	/** Ticks the world until Projectile is destroyed or MaxFrames pass. Returns the frame of destruction (INDEX_NONE if alive), OutLastLocation its last location. */
	int32 TickUntilDestroyed(const FTestWorld& TestWorld, ARRProjectile* Projectile, int32 MaxFrames, FVector& OutLastLocation)
	{
		TWeakObjectPtr<ARRProjectile> Weak(Projectile);
		OutLastLocation = Projectile ? Projectile->GetActorLocation() : FVector::ZeroVector;
		for (int32 Frame = 1; Frame <= MaxFrames; ++Frame)
		{
			TestWorld.Tick();
			if (!Weak.IsValid() || Weak->IsActorBeingDestroyed())
			{
				return Frame;
			}
			OutLastLocation = Weak->GetActorLocation();
		}
		return INDEX_NONE;
	}

	/**
	 * Advances the auto-fire alone (no world tick) by FrameSeconds steps from t = 0 until NumShots shots are seen or MaxFrames pass.
	 * Exact shot times come from the accumulator: after each step ending at T, the next shot is due at T + GetTimeToNextShot()
	 * (or at T + GetReloadTimeRemaining() while reloading). Fails the test if a step fires more than one shot.
	 * PerStep, if set, runs after each step with the step index.
	 */
	TArray<FShotSample> RecordShots(FAutomationTestBase& Test, ARRWeapon* Weapon, const URRTestEventRecorder* Recorder, int32 NumShots, int32 MaxFrames,
		TFunctionRef<void(int32 Frame)> PerStep = [](int32) {})
	{
		TArray<FShotSample> Shots;
		URRAutoFireComponent* AutoFire = Weapon->GetAutoFireComponent();
		double NextDue = AutoFire->GetTimeToNextShot();
		for (int32 Frame = 0; Frame < MaxFrames && Shots.Num() < NumShots; ++Frame)
		{
			const int32 Before = Recorder->GetShotIndices().Num();
			AutoFire->AdvanceTime(FrameSeconds);
			const int32 NewShots = Recorder->GetShotIndices().Num() - Before;
			if (NewShots > 1)
			{
				Test.AddError(FString::Printf(TEXT("%d shots in step %d"), NewShots, Frame));
			}
			if (NewShots > 0)
			{
				Shots.Add({ Frame, NextDue });
			}
			const double StepEnd = (Frame + 1) * static_cast<double>(FrameSeconds);
			NextDue = StepEnd + (AutoFire->IsReloading() ? AutoFire->GetReloadTimeRemaining() : AutoFire->GetTimeToNextShot());
			PerStep(Frame);
		}
		return Shots;
	}

	/** Checks one shot against its expected time: exact time within ExactTolerance, step within 1 frame. */
	void CheckShotTime(FAutomationTestBase& Test, const TCHAR* Label, int32 ShotNumber, const FShotSample& Shot, double Expected)
	{
		const double FrameStart = Shot.Frame * static_cast<double>(FrameSeconds);
		Test.TestTrue(FString::Printf(TEXT("%s shot %d at %.4f s (exact %.5f s)"), Label, ShotNumber, Expected, Shot.ExactSeconds),
			FMath::Abs(Shot.ExactSeconds - Expected) <= ExactTolerance);
		Test.TestTrue(FString::Printf(TEXT("%s shot %d fired in frame %d, within 1 frame of %.4f s"), Label, ShotNumber, Shot.Frame, Expected),
			FMath::Abs(FrameStart - Expected) <= FrameSeconds + ExactTolerance);
	}

	/** BurstRifle row of WeaponStats.csv (test error when missing). */
	FRRWeaponStatsRow ReadRifleRow(FAutomationTestBase& Test)
	{
		return RRTestStats::WeaponRow(Test, RRTestStats::BurstRifleRow);
	}

	/** Bullet gap inside a burst at FireRate: BurstDuty of the period shared by the BurstCount - 1 gaps (0 for single shots). */
	double BurstGapOf(const FRRWeaponStatsRow& Row, float FireRate)
	{
		return Row.BurstCount > 1 ? Row.BurstDuty / FireRate / (Row.BurstCount - 1) : 0.0;
	}

	/** Pause between the last bullet of a burst and the next burst at FireRate: the rest of the period. */
	double BurstPauseOf(const FRRWeaponStatsRow& Row, float FireRate)
	{
		return 1.0 / FireRate - (Row.BurstCount - 1) * BurstGapOf(Row, FireRate);
	}

	/** Time of shot ShotIndex (0-based) from the first one, uninterrupted bursts at FireRate. */
	double BurstShotTimeOf(const FRRWeaponStatsRow& Row, float FireRate, int32 ShotIndex)
	{
		const int32 Burst = FMath::Max(Row.BurstCount, 1);
		return (ShotIndex / Burst) / static_cast<double>(FireRate) + (ShotIndex % Burst) * BurstGapOf(Row, FireRate);
	}

	/**
	 * Fires a fresh BP_BurstRifle for two bursts and a shot at FireRate and checks the schedule: bullets Gap apart, the next burst
	 * Period after the first bullet (pause = Period - (BurstCount - 1) * Gap). Returns false if the rifle could not be set up.
	 */
	bool CheckBurstSchedule(FAutomationTestBase& Test, const FTestWorld& TestWorld, float FireRate, double ExpectedGap, double ExpectedPause)
	{
		ARRWeapon* Rifle = SpawnRifle(Test, TestWorld, FVector(0.f, FireRate * 1000.f, 100.f));
		if (!Rifle)
		{
			return false;
		}
		URRAutoFireComponent* AutoFire = Rifle->GetAutoFireComponent();
		AutoFire->StopFiring();
		Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetFireRateAttribute(), FireRate);
		// Restart the schedule from t = 0 with the FireRate under test.
		AutoFire->Configure(Rifle->GetAbilitySystemComponent(), Rifle->GetBurstCount(), Rifle->GetBurstDuty(), Rifle->GetReloadTime());

		TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
		AutoFire->OnShotFired.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleShotFired);
		AutoFire->StartFiring();

		const int32 Burst = FMath::Max(Rifle->GetBurstCount(), 1);
		const int32 NumShots = 2 * Burst + 1;
		const int32 MaxFrames = FMath::CeilToInt32((2.0 / FireRate + 0.5) / FrameSeconds);
		const TArray<FShotSample> Shots = RecordShots(Test, Rifle, Recorder.Get(), NumShots, MaxFrames);
		if (!Test.TestEqual(FString::Printf(TEXT("FireRate %g: two bursts and a shot recorded"), FireRate), Shots.Num(), NumShots))
		{
			return false;
		}

		const FString LabelText = FString::Printf(TEXT("FireRate %g"), FireRate);
		const TCHAR* Label = *LabelText;
		const double Period = ExpectedPause + (Burst - 1) * ExpectedGap;
		FString Times;
		for (int32 Shot = 0; Shot < Shots.Num(); ++Shot)
		{
			const double Expected = (Shot / Burst) * Period + (Shot % Burst) * ExpectedGap;
			CheckShotTime(Test, Label, Shot + 1, Shots[Shot], Expected);
			Test.TestEqual(FString::Printf(TEXT("%s shot %d index in burst"), Label, Shot + 1), Recorder->GetShotIndices()[Shot], Shot % Burst);
			Times += FString::Printf(TEXT("%.4f s (frame %d) "), Shots[Shot].ExactSeconds, Shots[Shot].Frame);
		}

		bool bGapsOk = true;
		for (int32 Shot = 1; Shot < Burst; ++Shot)
		{
			bGapsOk &= FMath::Abs((Shots[Shot].ExactSeconds - Shots[Shot - 1].ExactSeconds) - ExpectedGap) <= ExactTolerance;
		}
		const double Pause = Shots[Burst].ExactSeconds - Shots[Burst - 1].ExactSeconds;
		Test.AddInfo(FString::Printf(TEXT("%s: shots at %s; pause %.4f s"), Label, *Times, Pause));
		Test.TestTrue(FString::Printf(TEXT("%s gap %.3f s"), Label, ExpectedGap), bGapsOk);
		Test.TestTrue(FString::Printf(TEXT("%s pause %.3f s"), Label, ExpectedPause), FMath::Abs(Pause - ExpectedPause) <= ExactTolerance);
		return true;
	}
}

using namespace RRWeaponTests;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponBurstTimingTest, "RogueRigs.Weapons.BurstTiming",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponBurstTimingTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRWeapon* Rifle = SpawnRifle(*this, TestWorld, FVector(0.f, 0.f, 100.f));
	if (!Rifle)
	{
		return false;
	}

	// CSV row BurstRifle: FireRate, BurstCount, BurstDuty.
	const FRRWeaponStatsRow Row = ReadRifleRow(*this);
	TestNearlyEqual(TEXT("FireRate from the CSV"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetFireRateAttribute()), Row.FireRate);
	TestEqual(TEXT("BurstCount from the CSV"), Rifle->GetBurstCount(), Row.BurstCount);
	TestNearlyEqual(TEXT("BurstDuty from the CSV"), Rifle->GetBurstDuty(), Row.BurstDuty);
	TestTrue(TEXT("auto-fire started at acquisition"), Rifle->GetAutoFireComponent()->IsFiring());
	Rifle->Destroy();
	if (!TestTrue(TEXT("CSV: bursts of at least 2 bullets"), Row.BurstCount >= 2))
	{
		return false;
	}

	// CSV FireRate: period 1 / FireRate, bullets BurstDuty x period / (BurstCount - 1) apart, the next burst one period after the first bullet.
	CheckBurstSchedule(*this, TestWorld, Row.FireRate, BurstGapOf(Row, Row.FireRate), BurstPauseOf(Row, Row.FireRate));
	// An upgraded FireRate (scenario value, not a CSV value): the same rule at a shorter period (gap of two frames at 3 shots/s).
	constexpr float UpgradedFireRate = 3.f;
	CheckBurstSchedule(*this, TestWorld, UpgradedFireRate, BurstGapOf(Row, UpgradedFireRate), BurstPauseOf(Row, UpgradedFireRate));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponMagazineReloadTest, "RogueRigs.Weapons.MagazineReload",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponMagazineReloadTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRWeapon* Rifle = SpawnRifle(*this, TestWorld, FVector(0.f, 0.f, 100.f));
	if (!Rifle)
	{
		return false;
	}
	URRAutoFireComponent* AutoFire = Rifle->GetAutoFireComponent();
	AutoFire->StopFiring();
	AutoFire->Configure(Rifle->GetAbilitySystemComponent(), Rifle->GetBurstCount(), Rifle->GetBurstDuty(), Rifle->GetReloadTime());

	// CSV row BurstRifle: MagazineSize bullets, a ReloadTime / ReloadSpeed reload.
	const FRRWeaponStatsRow Row = ReadRifleRow(*this);
	const int32 Magazine = FMath::RoundToInt32(Row.MagazineSize);
	const double Reload = Row.ReloadTime / Row.ReloadSpeed;
	TestNearlyEqual(TEXT("MagazineSize from the CSV"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetMagazineSizeAttribute()), Row.MagazineSize);
	TestNearlyEqual(TEXT("magazine full at acquisition"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute()), Row.MagazineSize);
	TestNearlyEqual(TEXT("ReloadTime from the CSV"), Rifle->GetReloadTime(), Row.ReloadTime);
	TestNearlyEqual(TEXT("reload duration = ReloadTime / ReloadSpeed"), AutoFire->GetReloadDuration(), static_cast<float>(Reload));
	if (!TestTrue(TEXT("CSV: a magazine of at least 2 bullets"), Magazine >= 2))
	{
		return false;
	}
	// Last bullet of the magazine at its burst time; the reload replaces the pause, the next burst starts at its end.
	const double LastShotTime = BurstShotTimeOf(Row, Row.FireRate, Magazine - 1);
	const int32 MaxFrames = FMath::CeilToInt32((LastShotTime + Reload + 1.0) / FrameSeconds);

	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Recorder->SetAmmoSource(Rifle->GetAbilitySystemComponent());
	AutoFire->OnShotFired.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleShotFired);
	AutoFire->OnReloadStarted.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleReloadStarted);
	AutoFire->OnReloadFinished.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleReloadFinished);
	AutoFire->StartFiring();

	int32 ReloadFrames = 0;
	int32 BadAmmoFrames = 0;
	float AmmoBeforeLastShot = -1.f;
	const TArray<FShotSample> Shots = RecordShots(*this, Rifle, Recorder.Get(), Magazine + 1, MaxFrames, [&](int32 Frame)
	{
		if (AutoFire->IsReloading())
		{
			++ReloadFrames;
			BadAmmoFrames += WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute()) != 0.f ? 1 : 0;
		}
		if (Recorder->GetShotIndices().Num() == Magazine - 1 && AmmoBeforeLastShot < 0.f)
		{
			AmmoBeforeLastShot = WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute());
		}
	});
	if (!TestEqual(TEXT("a magazine and one shot recorded"), Shots.Num(), Magazine + 1))
	{
		return false;
	}

	const FShotSample& LastShot = Shots[Magazine - 1];
	const FShotSample& NextShot = Shots[Magazine];
	const double ReloadStart = LastShot.ExactSeconds;
	const double ReloadEnd = NextShot.ExactSeconds;
	AddInfo(FString::Printf(TEXT("shot %d at %.4f s (frame %d), shot %d at %.4f s (frame %d): reload %.4f s over %d frames; ammo before the last shot %.0f; ammo at reload end %s"),
		Magazine, ReloadStart, LastShot.Frame, Magazine + 1, ReloadEnd, NextShot.Frame, ReloadEnd - ReloadStart, ReloadFrames, AmmoBeforeLastShot,
		Recorder->GetAmmoAtReloadFinished().Num() > 0 ? *FString::SanitizeFloat(Recorder->GetAmmoAtReloadFinished()[0]) : TEXT("none")));
	CheckShotTime(*this, TEXT("magazine"), Magazine, LastShot, LastShotTime);
	CheckShotTime(*this, TEXT("magazine"), Magazine + 1, NextShot, LastShotTime + Reload);
	TestNearlyEqual(TEXT("1 bullet left before the last shot"), AmmoBeforeLastShot, 1.f);
	TestTrue(TEXT("reload of ReloadTime / ReloadSpeed"), FMath::Abs((ReloadEnd - ReloadStart) - Reload) <= ExactTolerance);
	TestTrue(TEXT("reloading for the reload duration +- 1 frame"), FMath::Abs(ReloadFrames - FMath::RoundToInt32(Reload / FrameSeconds)) <= 1);
	TestEqual(TEXT("Ammo 0 during the whole reload"), BadAmmoFrames, 0);
	TestEqual(TEXT("one reload started"), Recorder->GetReloadDurations().Num(), 1);
	if (Recorder->GetReloadDurations().Num() > 0)
	{
		TestNearlyEqual(TEXT("OnReloadStarted duration = ReloadTime / ReloadSpeed"), Recorder->GetReloadDurations()[0], static_cast<float>(Reload));
	}
	TestEqual(TEXT("one reload finished"), Recorder->GetAmmoAtReloadFinished().Num(), 1);
	if (Recorder->GetAmmoAtReloadFinished().Num() > 0)
	{
		TestNearlyEqual(TEXT("Ammo back to MagazineSize at the reload end"), Recorder->GetAmmoAtReloadFinished()[0], Row.MagazineSize);
	}
	TestEqual(TEXT("the shot after the reload opens a new burst"), Recorder->GetShotIndices()[Magazine], 0);
	TestNearlyEqual(TEXT("MagazineSize - 1 bullets left after it"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute()), Row.MagazineSize - 1.f);
	TestFalse(TEXT("not reloading after the refill"), AutoFire->IsReloading());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponProjectileRockTest, "RogueRigs.Weapons.ProjectileStopsOnRock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponProjectileRockTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRWeapon* Rifle = Grid ? SpawnRifle(*this, TestWorld, FVector::ZeroVector) : nullptr;
	if (!Rifle)
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	TestEqual(TEXT("(43, 38) is rock"), Grid->GetCellType(RockCell), ERRCellType::Rock);
	TestEqual(TEXT("(42, 38) is floor"), Grid->GetCellType(RockWestFloorCell), ERRCellType::Floor);

	// Muzzle on the floor cell west of the rock, at half the block height, aiming east (+X) at the rock.
	const FVector MuzzleTarget = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight());
	PlaceMuzzleAt(Rifle, MuzzleTarget);

	// A still chaser on the first open cell behind the rock, within range: it must not be hit through the rock.
	ARRChaser* StillChaser = nullptr;
	const int32 RangeCells = FMath::FloorToInt(Rifle->GetRange() / Grid->GetCellSize());
	for (int32 X = RockCell.X + 1; X <= RockWestFloorCell.X + RangeCells - 1 && !StillChaser; ++X)
	{
		const FIntPoint Cell(X, RockCell.Y);
		const ERRCellType Type = Grid->GetCellType(Cell);
		if (Type == ERRCellType::Floor || Type == ERRCellType::Spawn)
		{
			StillChaser = SpawnStillChaser(*this, TestWorld, Grid->GetCellCenter(Cell.X, Cell.Y));
			AddInfo(FString::Printf(TEXT("still chaser placed behind the rock on (%d, %d)"), Cell.X, Cell.Y));
		}
	}
	if (!StillChaser)
	{
		AddInfo(TEXT("no open cell behind the rock within range: no still chaser behind it"));
	}
	const float StillChaserHealthBefore = StillChaser ? TargetHealth(StillChaser) : 0.f;
	const float RockHealthBefore = Grid->GetCellHealth(RockCell);

	ARRProjectile* Projectile = FireSingleShot(*this, Rifle);
	if (!Projectile)
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Projectile->GetProjectileMovement()->OnProjectileStop.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleProjectileStop);

	const FVector Direction = Projectile->GetActorForwardVector();
	const float YawOffAxis = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
	TestTrue(TEXT("projectile spawned at the muzzle"), Projectile->GetActorLocation().Equals(MuzzleTarget, 0.5f));
	TestTrue(TEXT("projectile flies toward the aim, within SpreadDeg"), FMath::Abs(YawOffAxis) <= Rifle->GetSpreadDeg() + 0.01f && FMath::Abs(Direction.Z) < 1e-3f);

	FVector LastLocation;
	const int32 LifetimeFrames = FMath::CeilToInt(Rifle->GetRange() / Rifle->GetProjectileSpeed() / FrameSeconds);
	const int32 DestroyFrame = TickUntilDestroyed(TestWorld, Projectile, LifetimeFrames + 10, LastLocation);

	AddInfo(FString::Printf(TEXT("projectile yaw %.2f deg, destroyed at frame %d (lifetime %d frames), last location %s"),
		YawOffAxis, DestroyFrame, LifetimeFrames, *LastLocation.ToCompactString()));
	// 90 uu from the muzzle to the rock face, minus the sphere radius, at 3000 uu/s: 2 frames.
	TestTrue(TEXT("projectile destroyed within 3 frames, long before its lifetime"), DestroyFrame != INDEX_NONE && DestroyFrame <= 3);
	TestEqual(TEXT("one blocking stop"), Recorder->GetProjectileStops().Num(), 1);
	if (Recorder->GetProjectileStops().Num() > 0)
	{
		const FHitResult& Hit = Recorder->GetProjectileStops()[0];
		const FIntPoint HitCell = Grid->GetCellAtWorld(Hit.ImpactPoint + 1.f * Direction);
		AddInfo(FString::Printf(TEXT("stopped by %s / %s at %s (cell %d, %d)"), *GetNameSafe(Hit.GetActor()), *GetNameSafe(Hit.GetComponent()),
			*Hit.ImpactPoint.ToCompactString(), HitCell.X, HitCell.Y));
		TestEqual(TEXT("stopped by the grid"), Hit.GetActor(), static_cast<AActor*>(Grid));
		TestEqual(TEXT("stopped on the rock cell"), HitCell, RockCell);
	}
	TestTrue(TEXT("rock still stands"), Grid->IsBlockCell(RockCell));
	TestNearlyEqual(TEXT("rock health unchanged"), Grid->GetCellHealth(RockCell), RockHealthBefore);

	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("no projectile left"), GetProjectiles(TestWorld.Get()).Num(), 0);
	if (StillChaser)
	{
		TestNearlyEqual(TEXT("still chaser behind the rock not hit"), TargetHealth(StillChaser), StillChaserHealthBefore);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponProjectileDamageTest, "RogueRigs.Weapons.ProjectileDamagesStillChaser",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponProjectileDamageTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRWeapon* Rifle = Grid ? SpawnRifle(*this, TestWorld, FVector::ZeroVector) : nullptr;
	ARRChaser* StillChaser = Rifle ? SpawnStillChaser(*this, TestWorld, Grid->GetCellCenter(ChaserCell.X, ChaserCell.Y)) : nullptr;
	if (!StillChaser)
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	for (int32 X = ChaserShooterCell.X; X <= ChaserCell.X; ++X)
	{
		TestFalse(FString::Printf(TEXT("(%d, %d) is open floor"), X, ChaserCell.Y), Grid->IsBlockCell(FIntPoint(X, ChaserCell.Y)));
	}

	// Muzzle two cells west of the still chaser, at half the block height (as in ProjectileStopsOnRock), aiming east (+X).
	const FBox StillChaserBounds = StillChaser->GetComponentsBoundingBox(/*bNonColliding*/ false);
	const FVector MuzzleTarget = Grid->GetCellCenter(ChaserShooterCell.X, ChaserShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight());
	AddInfo(FString::Printf(TEXT("still chaser collision bounds %s; muzzle at %s"), *StillChaserBounds.ToString(), *MuzzleTarget.ToCompactString()));
	TestTrue(TEXT("still chaser stands on the floor and covers the shot height"),
		StillChaserBounds.IsValid && FMath::IsNearlyEqual(StillChaserBounds.Min.Z, 0.f, 0.5f) && StillChaserBounds.Max.Z > MuzzleTarget.Z + 10.f);
	PlaceMuzzleAt(Rifle, MuzzleTarget);

	UAbilitySystemComponent* StillChaserASC = StillChaser->GetAbilitySystemComponent();
	const float MaxHealth = StillChaserASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	TestNearlyEqual(TEXT("still chaser MaxHealth 1000"), MaxHealth, 1000.f);
	TestNearlyEqual(TEXT("still chaser starts at MaxHealth"), TargetHealth(StillChaser), MaxHealth);

	TArray<FTargetHit> Hits;
	URRHealthAttributeSet* HealthSet = StillChaser->GetHealthSet();
	const FDelegateHandle HitHandle = HealthSet->OnDamageReceived.AddLambda([&Hits](float Damage, bool bIsCritical, float HealthLeft)
	{
		Hits.Add({ Damage, bIsCritical, HealthLeft });
	});

	// CSV row BurstRifle: a normal hit deals Damage, a critical one Damage x CritDamage.
	const FRRWeaponStatsRow Row = ReadRifleRow(*this);
	const float NormalDamage = Row.Damage;
	const float CritHitDamage = Row.Damage * Row.CritDamage;
	UAbilitySystemComponent* RifleASC = Rifle->GetAbilitySystemComponent();
	TestNearlyEqual(TEXT("rifle Damage from the CSV"), RifleASC->GetNumericAttribute(URRWeaponAttributeSet::GetDamageAttribute()), Row.Damage);
	TestNearlyEqual(TEXT("rifle CritDamage from the CSV"), RifleASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritDamageAttribute()), Row.CritDamage);

	// Pierce 1: these checks are about one bullet ending on the still chaser (pierce: PierceFourRobots).
	RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), 1.f);

	// Normal hit: CritChance forced to 0 so the CSV's 5 % cannot roll a crit.
	RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	FVector LastLocation;
	const int32 NormalFrame = TickUntilDestroyed(TestWorld, FireSingleShot(*this, Rifle), 30, LastLocation);
	TestTrue(TEXT("normal shot destroyed on the still chaser"), NormalFrame != INDEX_NONE);
	TestEqual(TEXT("one hit after the normal shot"), Hits.Num(), 1);
	const float HealthAfterNormal = TargetHealth(StillChaser);
	AddInfo(FString::Printf(TEXT("normal shot: destroyed at frame %d near %s; health %.1f"), NormalFrame, *LastLocation.ToCompactString(), HealthAfterNormal));
	TestNearlyEqual(TEXT("Health lowered by Damage"), HealthAfterNormal, MaxHealth - NormalDamage);
	if (Hits.Num() >= 1)
	{
		TestNearlyEqual(TEXT("hit 1 damage = Damage"), Hits[0].Damage, NormalDamage);
		TestFalse(TEXT("hit 1 not critical"), Hits[0].bIsCritical);
		TestNearlyEqual(TEXT("hit 1 health left"), Hits[0].HealthLeft, MaxHealth - NormalDamage);
	}

	// Critical hit: CritChance forced to 1 on the weapon ASC (captured when the shot makes its spec).
	RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 1.f);
	const int32 CritFrame = TickUntilDestroyed(TestWorld, FireSingleShot(*this, Rifle), 30, LastLocation);
	TestTrue(TEXT("critical shot destroyed on the still chaser"), CritFrame != INDEX_NONE);
	TestEqual(TEXT("two hits after the critical shot"), Hits.Num(), 2);
	const float HealthAfterCrit = TargetHealth(StillChaser);
	AddInfo(FString::Printf(TEXT("critical shot: destroyed at frame %d near %s; health %.1f"), CritFrame, *LastLocation.ToCompactString(), HealthAfterCrit));
	TestNearlyEqual(TEXT("Health lowered by Damage x CritDamage"), HealthAfterNormal - HealthAfterCrit, CritHitDamage);
	if (Hits.Num() >= 2)
	{
		TestNearlyEqual(TEXT("hit 2 damage = Damage x CritDamage"), Hits[1].Damage, CritHitDamage);
		TestTrue(TEXT("hit 2 critical"), Hits[1].bIsCritical);
		TestNearlyEqual(TEXT("hit 2 health left"), Hits[1].HealthLeft, MaxHealth - NormalDamage - CritHitDamage);
	}
	TestFalse(TEXT("still chaser not dead"), StillChaserASC->HasMatchingGameplayTag(RRGameplayTags::State_Dead));

	HealthSet->OnDamageReceived.Remove(HitHandle);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponMinerStartingWeaponTest, "RogueRigs.Weapons.MinerStartingWeapon",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponMinerStartingWeaponTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMinerWithCapsuleWeapons(*this, TestWorld, Grid, RockWestFloorCell) : nullptr;
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath);
	if (!Miner || !RifleClass)
	{
		return false;
	}
	EnableMinerMovement(Miner);
	// Let the miner land on the floor and the weapon fire a few bursts.
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}

	// M1 loadout: burst rifle + shotgun (shotgun pellets: see RogueRigs.Shotgun.*).
	if (!TestEqual(TEXT("two starting weapons"), Miner->GetWeapons().Num(), 2) || !TestNotNull(TEXT("starting shotgun"), FindShotgun(Miner)))
	{
		return false;
	}
	ARRWeapon* Weapon = FindRifle(Miner);
	if (!TestTrue(TEXT("starting weapon valid"), IsValid(Weapon)))
	{
		return false;
	}
	TestTrue(TEXT("starting weapon is a BP_BurstRifle"), Weapon->IsA(RifleClass));
	TestEqual(TEXT("acquired by the miner"), Weapon->GetOwnerMiner(), Miner);
	TestEqual(TEXT("owned by the miner"), Weapon->GetOwner(), static_cast<AActor*>(Miner));
	TestTrue(TEXT("auto-fire running"), Weapon->GetAutoFireComponent()->IsFiring());
	TestTrue(TEXT("the weapon fired during the first second"), Weapon->GetAutoFireComponent()->GetTotalShotsFired() > 0);
	TestTrue(TEXT("miner walking on the floor"), Miner->GetCharacterMovement()->IsMovingOnGround());

	// Attachment with WeaponAttachSocketName None: the rifle hand socket when set and present on the miner mesh (task-400),
	// else on the capsule at WeaponAttachOffset, turning with the aim.
	UCapsuleComponent* Capsule = Miner->GetCapsuleComponent();
	TestEqual(TEXT("attached to the miner"), Weapon->GetAttachParentActor(), static_cast<AActor*>(Miner));
	const FName HandSocketName = Weapon->GetHandSocketName();
	if (!HandSocketName.IsNone() && Miner->GetMesh() && Miner->GetMesh()->DoesSocketExist(HandSocketName))
	{
		TestEqual(TEXT("attached to the miner mesh"), Weapon->GetRootComponent()->GetAttachParent(), static_cast<USceneComponent*>(Miner->GetMesh()));
		TestEqual(TEXT("attached to the weapon hand socket"), Weapon->GetAttachParentSocketName(), HandSocketName);
	}
	else
	{
		const FVector AttachOffset = FMinerAccess::WeaponAttachOffsetOf(Miner);
		TestEqual(TEXT("attached to the capsule"), Weapon->GetRootComponent()->GetAttachParent(), static_cast<USceneComponent*>(Capsule));
		TestEqual(TEXT("no attach socket"), Weapon->GetAttachParentSocketName(), FName(NAME_None));
		TestTrue(FString::Printf(TEXT("relative location = WeaponAttachOffset %s"), *AttachOffset.ToCompactString()),
			Weapon->GetRootComponent()->GetRelativeLocation().Equals(AttachOffset, 0.01f));
		TestTrue(TEXT("weapon points along the miner aim"), Weapon->GetActorForwardVector().Equals(Miner->GetAimDirection(), 1e-3f));
	}
	TestFalse(TEXT("weapon actor has no collision"), Weapon->GetActorEnableCollision());
	TestEqual(TEXT("capsule overlaps the Projectile channel"), static_cast<int32>(Capsule->GetCollisionResponseToChannel(RRCollisionChannels::Projectile)), static_cast<int32>(ECR_Overlap));

	// One shot from the miner standing on (42, 38), aiming east (+X) at the rock (43, 38).
	Weapon->GetAutoFireComponent()->StopFiring();
	const float FloorZ = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y).Z;
	const FVector MinerLocation = Miner->GetActorLocation();
	TestTrue(TEXT("miner aims east"), Miner->GetAimDirection().Equals(FVector::ForwardVector, 1e-3f));
	TestTrue(TEXT("ShotHeight is below the block height"), Weapon->GetShotHeight() > 0.f && Weapon->GetShotHeight() < Grid->GetBlockHeight());

	// Shot placement read at fire time (task-412): from the hand weapon Muzzle with a vertical capsule when the rifle is held on its
	// hand socket, else the pre-task-412 point ShotHeight above the floor and a sphere.
	const FVector MuzzleLocation = FWeaponAccess::MuzzleOf(Weapon)->GetComponentLocation();
	const FRRShotPlacement Placement = Weapon->GetShotPlacement();
	const float Radius = ARRProjectile::GetDefaultCollisionRadius(Weapon->GetProjectileClass());
	const float Clearance = Weapon->GetShotFloorClearance();
	AddInfo(FString::Printf(TEXT("miner at %s (capsule bottom %.2f uu above the floor), weapon at %s (held on hand socket: %s), muzzle at %s %.1f uu above the floor; placement origin %s %.2f uu above the floor, capsule half height %.2f (radius %.1f, clearance %.1f), sweep %s from %s"),
		*MinerLocation.ToCompactString(), MinerLocation.Z - Capsule->GetScaledCapsuleHalfHeight() - FloorZ, *Weapon->GetActorLocation().ToCompactString(),
		Weapon->IsHeldOnHandSocket() ? TEXT("yes") : TEXT("no"), *MuzzleLocation.ToCompactString(), MuzzleLocation.Z - FloorZ, *Placement.Origin.ToCompactString(),
		Placement.Origin.Z - FloorZ, Placement.CapsuleHalfHeight, Radius, Clearance, Placement.bSweepFromMiner ? TEXT("on") : TEXT("off"), *Placement.SweepStart.ToCompactString()));
	TestEqual(TEXT("miner shot radius 14 uu"), Radius, 14.f, 0.01f);
	if (Weapon->IsHeldOnHandSocket())
	{
		TestEqual(TEXT("the hand weapon is the rifle"), Weapon->FindHandWeapon(), Weapon);
		TestTrue(TEXT("swept from the miner axis at the shot height"), Placement.bSweepFromMiner
			&& FVector::Dist2D(Placement.SweepStart, MinerLocation) < 0.5f && FMath::IsNearlyEqual(Placement.SweepStart.Z, Placement.Origin.Z, 0.01));
		if (MuzzleLocation.Z - FloorZ >= Clearance + Radius)
		{
			const double Bottom = Placement.Origin.Z - Placement.CapsuleHalfHeight;
			const double Top = Placement.Origin.Z + Placement.CapsuleHalfHeight;
			TestTrue(TEXT("shot origin at the Muzzle (1 uu)"), Placement.Origin.Equals(MuzzleLocation, 1.f));
			TestTrue(TEXT("capsule bottom = floor + ShotFloorClearance"), FMath::IsNearlyEqual(Bottom - FloorZ, static_cast<double>(Clearance), 0.5));
			TestTrue(TEXT("capsule bottom above the floor and below the block top"), Bottom > FloorZ && Bottom - FloorZ < Grid->GetBlockHeight());
			TestTrue(TEXT("capsule reaches the Muzzle height"), Top >= MuzzleLocation.Z);
		}
		else
		{
			TestTrue(TEXT("low Muzzle: sphere ShotHeight above the floor under the Muzzle"), Placement.CapsuleHalfHeight == 0.f
				&& FVector::Dist2D(Placement.Origin, MuzzleLocation) < 0.5f && FMath::IsNearlyEqual(Placement.Origin.Z - FloorZ, static_cast<double>(Weapon->GetShotHeight()), 0.5));
		}
	}
	else
	{
		const FVector ToShot = Placement.Origin - MinerLocation;
		const float ExpectedForward = Capsule->GetScaledCapsuleRadius() + Weapon->GetShotForwardOffset();
		TestTrue(TEXT("shot spawned ShotHeight above the floor"), FMath::IsNearlyEqual(Placement.Origin.Z - FloorZ, static_cast<double>(Weapon->GetShotHeight()), 0.5));
		TestTrue(TEXT("shot spawned capsule radius + ShotForwardOffset ahead along the aim"),
			FMath::IsNearlyEqual(ToShot.X, ExpectedForward, 0.5f) && FMath::Abs(ToShot.Y) < 0.5f);
		TestTrue(TEXT("no hand weapon: sphere, no sweep"), Placement.CapsuleHalfHeight == 0.f && !Placement.bSweepFromMiner);
	}

	// One shot: against the rock the miner-to-Muzzle sweep may already stop it, so the projectile is caught at spawn.
	ARRProjectile* Projectile = nullptr;
	const FDelegateHandle SpawnHandle = TestWorld.Get()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateLambda([&Projectile](AActor* Actor)
	{
		if (ARRProjectile* Shot = Cast<ARRProjectile>(Actor))
		{
			Projectile = Shot;
		}
	}));
	URRAutoFireComponent* AutoFire = Weapon->GetAutoFireComponent();
	AutoFire->Configure(Weapon->GetAbilitySystemComponent(), Weapon->GetBurstCount(), Weapon->GetBurstDuty(), Weapon->GetReloadTime());
	AutoFire->StartFiring();
	AutoFire->AdvanceTime(SingleShotStep);
	AutoFire->StopFiring();
	TestWorld.Get()->RemoveOnActorSpawnedHandler(SpawnHandle);
	if (!TestNotNull(TEXT("the shot spawned a projectile"), Projectile))
	{
		return false;
	}
	const float ExpectedHalfHeight = Placement.CapsuleHalfHeight > 0.f ? Placement.CapsuleHalfHeight : Radius;
	TestEqual(TEXT("projectile capsule half height as placed"), Projectile->GetCollisionCapsule()->GetUnscaledCapsuleHalfHeight(), ExpectedHalfHeight, 0.01f);
	TestEqual(TEXT("projectile capsule radius"), Projectile->GetCollisionCapsule()->GetUnscaledCapsuleRadius(), Radius, 0.01f);

	// The shot stops on the rock in front of the miner: in the sweep, or in flight within 3 frames; never beyond the miner's cell.
	FVector LastLocation = Projectile->GetActorLocation();
	int32 DestroyFrame = 0;
	if (IsValid(Projectile) && !Projectile->IsActorBeingDestroyed())
	{
		DestroyFrame = TickUntilDestroyed(TestWorld, Projectile, 30, LastLocation);
	}
	AddInfo(FString::Printf(TEXT("miner shot destroyed at frame %d (0 = in the miner-to-Muzzle sweep), last location %s"), DestroyFrame, *LastLocation.ToCompactString()));
	TestTrue(TEXT("miner shot destroyed within 3 frames"), DestroyFrame != INDEX_NONE && DestroyFrame <= 3);
	TestEqual(TEXT("miner shot stopped before the rock cell"), Grid->GetCellAtWorld(LastLocation), RockWestFloorCell);
	TestTrue(TEXT("rock still stands"), Grid->IsBlockCell(RockCell));
	return true;
}

namespace RRWeaponTests
{
	/** Distance covered and capsule hits of one MinerMovesWhileFiring run. */
	struct FMoveRun
	{
		bool bValid = false;
		double Distance = 0.0;
		double LateralDrift = 0.0;
		int32 ShotsFired = 0;
		int32 ProjectileHits = 0;
		int32 OtherHits = 0;
		float MaxWalkSpeed = 0.f;
		float MaxAcceleration = 0.f;
		FString HitActors;
	};

	/**
	 * Spawns BP_Miner (weapons on the capsule) on MoveStartCell of a fresh world, lets it land, then pushes it toward +X (its aim)
	 * with AddMovementInput for MoveFrames frames (1 s), with its starting weapon firing or stopped. No controller:
	 * EnableMinerMovement lets CharacterMovement simulate the input. Records the
	 * blocking hits of the capsule.
	 */
	FMoveRun RunMinerMove(FAutomationTestBase& Test, bool bFiring)
	{
		const FIntPoint MoveStartCell(30, 43);
		constexpr int32 SettleFrames = 30;
		constexpr int32 MoveFrames = 60;

		FMoveRun Run;
		FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
		ARRArenaGrid* Grid = SpawnGrid(Test, TestWorld);
		ARRMiner* Miner = Grid ? SpawnMinerWithCapsuleWeapons(Test, TestWorld, Grid, MoveStartCell) : nullptr;
		if (!Miner || !Test.TestEqual(TEXT("two starting weapons (rifle + shotgun)"), Miner->GetWeapons().Num(), 2) || !FindRifle(Miner))
		{
			return Run;
		}
		for (int32 X = MoveStartCell.X; X <= MoveStartCell.X + 6; ++X)
		{
			for (int32 Y = MoveStartCell.Y - 1; Y <= MoveStartCell.Y + 1; ++Y)
			{
				Test.TestFalse(FString::Printf(TEXT("(%d, %d) is open floor"), X, Y), Grid->IsBlockCell(FIntPoint(X, Y)));
			}
		}
		EnableMinerMovement(Miner);
		UCharacterMovementComponent* Movement = Miner->GetCharacterMovement();

		ARRWeapon* Weapon = FindRifle(Miner);
		URRAutoFireComponent* AutoFire = Weapon->GetAutoFireComponent();
		if (!bFiring)
		{
			AutoFire->StopFiring();
			StopShotgun(Miner);
		}

		TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
		Miner->GetCapsuleComponent()->OnComponentHit.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleComponentHit);

		for (int32 Frame = 0; Frame < SettleFrames; ++Frame)
		{
			TestWorld.Tick();
		}
		Test.TestTrue(TEXT("miner walking on the floor before the move"), Movement->IsMovingOnGround());
		const int32 HitsBeforeMove = Recorder->GetComponentHits().Num();
		const int32 ShotsBeforeMove = AutoFire->GetTotalShotsFired();
		const FVector Start = Miner->GetActorLocation();
		for (int32 Frame = 0; Frame < MoveFrames; ++Frame)
		{
			Miner->AddMovementInput(FVector::ForwardVector, 1.f);
			TestWorld.Tick();
		}
		const FVector End = Miner->GetActorLocation();

		Run.bValid = true;
		Run.Distance = FVector::Dist2D(Start, End);
		Run.LateralDrift = FMath::Abs(End.Y - Start.Y);
		Run.ShotsFired = AutoFire->GetTotalShotsFired() - ShotsBeforeMove;
		Run.MaxWalkSpeed = Movement->MaxWalkSpeed;
		Run.MaxAcceleration = Movement->MaxAcceleration;
		for (int32 Index = HitsBeforeMove; Index < Recorder->GetComponentHits().Num(); ++Index)
		{
			const FHitResult& Hit = Recorder->GetComponentHits()[Index];
			const AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor->IsA<ARRProjectile>())
			{
				++Run.ProjectileHits;
			}
			else if (HitActor != Grid || Hit.ImpactNormal.Z < 0.7)
			{
				// Floor contacts (grid, normal up) are ordinary walking; anything else is reported.
				++Run.OtherHits;
			}
			const FString HitName = GetNameSafe(HitActor) + TEXT("/") + GetNameSafe(Hit.GetComponent());
			if (!Run.HitActors.Contains(HitName))
			{
				Run.HitActors += HitName + TEXT(" ");
			}
		}
		Test.AddInfo(FString::Printf(TEXT("%s: miner moved %.1f uu in %d frames (from %s to %s, lateral drift %.1f uu), %d shots, MaxWalkSpeed %.0f, MaxAcceleration %.0f; capsule hits: %d projectile, %d non-floor [%s]"),
			bFiring ? TEXT("firing") : TEXT("not firing"), Run.Distance, MoveFrames, *Start.ToCompactString(), *End.ToCompactString(), Run.LateralDrift,
			Run.ShotsFired, Run.MaxWalkSpeed, Run.MaxAcceleration, Run.ProjectileHits, Run.OtherHits, *Run.HitActors));
		return Run;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponMinerMovesWhileFiringTest, "RogueRigs.Weapons.MinerMovesWhileFiring",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponMinerMovesWhileFiringTest::RunTest(const FString& Parameters)
{
	const FMoveRun Free = RunMinerMove(*this, /*bFiring*/ false);
	const FMoveRun Firing = RunMinerMove(*this, /*bFiring*/ true);
	if (!Free.bValid || !Firing.bValid)
	{
		return false;
	}
	TestEqual(TEXT("no shot in the reference run"), Free.ShotsFired, 0);
	TestTrue(TEXT("the weapon fired during the firing run"), Firing.ShotsFired > 0);

	// Reference: from rest, accelerate to MaxWalkSpeed then cruise for the rest of 1 s.
	const double AccelTime = Free.MaxAcceleration > 0.f ? FMath::Min(1.0, static_cast<double>(Free.MaxWalkSpeed) / Free.MaxAcceleration) : 1.0;
	const double Kinematic = 0.5 * Free.MaxAcceleration * AccelTime * AccelTime + Free.MaxWalkSpeed * (1.0 - AccelTime);
	AddInfo(FString::Printf(TEXT("kinematic distance in 1 s: %.1f uu"), Kinematic));
	TestTrue(TEXT("reference run covers at least 90 % of the kinematic distance"), Free.Distance >= 0.9 * Kinematic);

	const double Ratio = Free.Distance > 0.0 ? Firing.Distance / Free.Distance : 0.0;
	AddInfo(FString::Printf(TEXT("distance while firing / without firing = %.1f / %.1f = %.3f"), Firing.Distance, Free.Distance, Ratio));
	TestEqual(TEXT("the capsule is never blocked by a projectile"), Firing.ProjectileHits, 0);
	TestEqual(TEXT("no non-floor hit while firing in the open corridor"), Firing.OtherHits, 0);
	TestTrue(TEXT("no sideways push while firing (drift < 5 % of the distance)"), Firing.LateralDrift < 0.05 * FMath::Max(Firing.Distance, 1.0));
	TestTrue(TEXT("firing keeps at least 90 % of the distance"), Ratio >= 0.9);
	return true;
}

namespace RRWeaponTests
{
	/** Open column of arena_m1 (x = 38, y = 39..44 floor, neighbours open): the miner stands on MinerAimCell, the still chaser 3 cells north (-Y). */
	const FIntPoint MinerAimCell(38, 43);
	const FIntPoint MinerAimChaserCell(38, 40);

	/** Stick input (screen axes: X right, Y up) aiming from MinerAimCell at MinerAimChaserCell: screen left = world -Y with CameraYaw 0. */
	const FVector2D MinerAimStickInput(-1.f, 0.f);

	/** Time the still chaser has to take its first hit from the miner. */
	constexpr float MinerHitDeadlineSeconds = 2.f;

}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponMinerHitsStillChaserTest, "RogueRigs.Weapons.MinerHitsStillChaser",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponMinerHitsStillChaserTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	for (int32 Y = MinerAimChaserCell.Y; Y <= MinerAimCell.Y; ++Y)
	{
		TestFalse(FString::Printf(TEXT("(%d, %d) is open floor"), MinerAimCell.X, Y), Grid->IsBlockCell(FIntPoint(MinerAimCell.X, Y)));
	}

	// Still chaser resting on the floor of its cell (capsule 124 uu tall, so actor Z = floor + 62).
	const FVector StillChaserFloor = Grid->GetCellCenter(MinerAimChaserCell.X, MinerAimChaserCell.Y);
	ARRChaser* StillChaser = SpawnStillChaser(*this, TestWorld, StillChaserFloor);
	// Final BP_Miner as authored (no socket override): starting BP_BurstRifle, firing from BeginPlay.
	ARRMiner* Miner = StillChaser ? SpawnMiner(*this, TestWorld, Grid, MinerAimCell, M1TestLoadout) : nullptr;
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath);
	if (!Miner || !RifleClass)
	{
		return false;
	}
	EnableMinerMovement(Miner);

	const FBox StillChaserBounds = StillChaser->GetComponentsBoundingBox(/*bNonColliding*/ false);
	AddInfo(FString::Printf(TEXT("still chaser at %s (%.1f uu above the floor), collision bounds %s"),
		*StillChaser->GetActorLocation().ToCompactString(), StillChaser->GetActorLocation().Z - StillChaserFloor.Z, *StillChaserBounds.ToString()));
	TestTrue(TEXT("still chaser rests on the floor"), StillChaserBounds.IsValid && FMath::IsNearlyEqual(StillChaserBounds.Min.Z, StillChaserFloor.Z, 0.5f));

	// M1 loadout: burst rifle + shotgun (shotgun pellets: see RogueRigs.Shotgun.*).
	if (!TestEqual(TEXT("two starting weapons"), Miner->GetWeapons().Num(), 2) || !TestNotNull(TEXT("starting shotgun"), FindShotgun(Miner)))
	{
		return false;
	}
	ARRWeapon* Weapon = FindRifle(Miner);
	if (!TestTrue(TEXT("starting weapon valid"), IsValid(Weapon)))
	{
		return false;
	}
	TestTrue(TEXT("starting weapon is a BP_BurstRifle"), Weapon->IsA(RifleClass));
	TestTrue(TEXT("auto-fire running"), Weapon->GetAutoFireComponent()->IsFiring());

	// Aim toward the still chaser through the stick-aim path, before the first tick.
	Miner->ApplyStickAimInput(MinerAimStickInput);
	const FVector ToChaser = (StillChaserFloor - Miner->GetActorLocation()).GetSafeNormal2D();
	TestTrue(FString::Printf(TEXT("miner aims at the still chaser (aim %s, to still chaser %s)"), *Miner->GetAimDirection().ToCompactString(), *ToChaser.ToCompactString()),
		Miner->GetAimDirection().Equals(ToChaser, 1e-3f));

	UAbilitySystemComponent* StillChaserASC = StillChaser->GetAbilitySystemComponent();
	const float MaxHealth = StillChaserASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	TestNearlyEqual(TEXT("still chaser starts at MaxHealth"), TargetHealth(StillChaser), MaxHealth);

	TArray<FTargetHit> Hits;
	URRHealthAttributeSet* HealthSet = StillChaser->GetHealthSet();
	const FDelegateHandle HitHandle = HealthSet->OnDamageReceived.AddLambda([&Hits](float Damage, bool bIsCritical, float HealthLeft)
	{
		Hits.Add({ Damage, bIsCritical, HealthLeft });
	});

	// Tick until the still chaser loses health or the deadline passes (it never refills: one hit leaves Health below MaxHealth).
	const int32 MaxFrames = FMath::CeilToInt32(MinerHitDeadlineSeconds / FrameSeconds);
	int32 FirstHitFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= MaxFrames && FirstHitFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (TargetHealth(StillChaser) < MaxHealth)
		{
			FirstHitFrame = Frame;
		}
	}
	const float HealthAfter = TargetHealth(StillChaser);
	AddInfo(FString::Printf(TEXT("first hit at frame %d (%.3f s); shots fired %d; hits %d; health %.1f / %.1f"),
		FirstHitFrame, FirstHitFrame * FrameSeconds, Weapon->GetAutoFireComponent()->GetTotalShotsFired(), Hits.Num(), HealthAfter, MaxHealth));
	TestTrue(TEXT("the miner's shots hit the still chaser within 2 s"), FirstHitFrame != INDEX_NONE);
	TestTrue(TEXT("still chaser Health below MaxHealth"), HealthAfter < MaxHealth);
	TestTrue(TEXT("damage received through the health set"), Hits.Num() > 0);

	HealthSet->OnDamageReceived.Remove(HitHandle);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponProjectileHitFilterTest, "RogueRigs.Weapons.ProjectileHitFilter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponProjectileHitFilterTest::RunTest(const FString& Parameters)
{
	// Robot shots (task-021) rely on the filter: HitActorClass = the miner class passes through every other actor.
	const ARRProjectile* Defaults = GetDefault<ARRProjectile>();
	TestTrue(TEXT("projectiles damage terrain by default"), Defaults->DamagesTerrain());

	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRWeapon* Rifle = Grid ? SpawnRifle(*this, TestWorld, FVector::ZeroVector) : nullptr;
	ARRChaser* StillChaser = Rifle ? SpawnStillChaser(*this, TestWorld, Grid->GetCellCenter(ChaserCell.X, ChaserCell.Y)) : nullptr;
	if (!StillChaser)
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	// Pierce 1: the matching shot must end on the still chaser (pierce: PierceFourRobots).
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), 1.f);
	PlaceMuzzleAt(Rifle, Grid->GetCellCenter(ChaserShooterCell.X, ChaserShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight()));

	int32 HitCount = 0;
	URRHealthAttributeSet* HealthSet = StillChaser->GetHealthSet();
	const FDelegateHandle HitHandle = HealthSet->OnDamageReceived.AddLambda([&HitCount](float, bool, float) { ++HitCount; });
	const float MaxHealth = TargetHealth(StillChaser);

	// Miner-only shot: the still chaser is not hittable, the projectile passes through it.
	ARRProjectile* Filtered = FireSingleShot(*this, Rifle);
	if (!Filtered)
	{
		return false;
	}
	TestTrue(TEXT("unfiltered projectile can hit the still chaser"), Filtered->CanHit(StillChaser));
	FProjectileAccess::HitActorClassOf(Filtered) = ARRMiner::StaticClass();
	TestFalse(TEXT("miner-only projectile cannot hit the still chaser"), Filtered->CanHit(StillChaser));
	TWeakObjectPtr<ARRProjectile> WeakFiltered(Filtered);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("no hit from the miner-only shot"), HitCount, 0);
	TestNearlyEqual(TEXT("still chaser health unchanged"), TargetHealth(StillChaser), MaxHealth);
	if (WeakFiltered.IsValid())
	{
		WeakFiltered->Destroy();
	}

	// Filter matching the chaser class: hit as usual.
	ARRProjectile* Matching = FireSingleShot(*this, Rifle);
	if (!Matching)
	{
		return false;
	}
	FProjectileAccess::HitActorClassOf(Matching) = ARRChaser::StaticClass();
	FVector LastLocation;
	TestTrue(TEXT("matching shot destroyed on the still chaser"), TickUntilDestroyed(TestWorld, Matching, 30, LastLocation) != INDEX_NONE);
	TestEqual(TEXT("one hit from the matching shot"), HitCount, 1);
	TestNearlyEqual(TEXT("Health lowered by the CSV Damage"), TargetHealth(StillChaser), MaxHealth - ReadRifleRow(*this).Damage);

	HealthSet->OnDamageReceived.Remove(HitHandle);
	return true;
}

namespace RRWeaponTests
{
	/** Pierce tests: open floor row 42 of arena_m1 from x 32 to 39; the rifle fires +X from (32, 42), robots stand on (34..38, 42). */
	const FIntPoint PierceShooterCell(32, 42);
	constexpr int32 PierceFirstRobotX = 34;

	/** Rock pierce test: the rifle fires -X from (36, 40) past robots on (36, 40) and (35, 40) into rock (34, 40); floor (33, 40) behind it. */
	const FIntPoint PierceRockShooterCell(36, 40);
	const FIntPoint PierceRockCell(34, 40);
	const FIntPoint PierceBehindRockCell(33, 40);

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

	/** Current Health of a robot. */
	float RobotHealth(const ARREnemy* Enemy)
	{
		return Enemy->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Sets MaxHealth and Health of a robot to Health (a scenario value derived from a CSV row). */
	void SetRobotHealth(ARREnemy* Enemy, float Health)
	{
		UAbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent();
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), Health);
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), Health);
	}

	/**
	 * Spawns the grid and BP_BurstRifle (no miner, firing stopped, CritChance 0), turns it to Yaw and puts its muzzle at half
	 * block height above Floor (a floor-top point). Returns null if the setup failed.
	 */
	ARRWeapon* SpawnPierceRifle(FAutomationTestBase& Test, const FTestWorld& TestWorld, ARRArenaGrid*& OutGrid, const FVector& Floor, float Yaw)
	{
		OutGrid = SpawnGrid(Test, TestWorld);
		ARRWeapon* Rifle = OutGrid ? SpawnRifle(Test, TestWorld, FVector::ZeroVector) : nullptr;
		if (!Rifle)
		{
			return nullptr;
		}
		Rifle->GetAutoFireComponent()->StopFiring();
		Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
		// No random spread: a 2 deg draw can make a bullet miss the far robots of the line (flaky with other tests' RNG draws).
		RRTestAccess::FWeaponAccess::SpreadDegOf(Rifle) = 0.f;
		Rifle->SetActorRotation(FRotator(0.f, Yaw, 0.f), ETeleportType::TeleportPhysics);
		PlaceMuzzleAt(Rifle, Floor + FVector(0.f, 0.f, 0.5f * OutGrid->GetBlockHeight()));
		return Rifle;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponPierceFourRobotsTest, "RogueRigs.Weapons.PierceFourRobots",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponPierceFourRobotsTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponPierceWorld"));
	ARRArenaGrid* Grid = nullptr;
	ARRWeapon* Rifle = SpawnPierceRifle(*this, TestWorld, Grid, FVector::ZeroVector, 0.f);
	if (!Rifle)
	{
		return false;
	}
	PlaceMuzzleAt(Rifle, Grid->GetCellCenter(PierceShooterCell.X, PierceShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight()));
	for (int32 X = PierceShooterCell.X; X <= PierceFirstRobotX + 5; ++X)
	{
		TestFalse(FString::Printf(TEXT("(%d, %d) is open floor"), X, PierceShooterCell.Y), Grid->IsBlockCell(FIntPoint(X, PierceShooterCell.Y)));
	}

	// CSV base: the rifle pierces Pierce robots.
	const FRRWeaponStatsRow Row = ReadRifleRow(*this);
	const int32 Pierce = FMath::RoundToInt32(Row.Pierce);
	TestNearlyEqual(TEXT("rifle Pierce from WeaponStats.csv"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetPierceAttribute()), Row.Pierce);
	TestEqual(TEXT("rifle pierce count = CSV Pierce"), Rifle->GetPierceCount(), Pierce);
	if (!TestTrue(TEXT("CSV Pierce fits the open row (1 to 5 robots)"), Pierce >= 1 && Pierce <= 5))
	{
		return false;
	}

	// Pierce + 1 robots in a line, 200 uu apart, all within range; each survives two bullets (health from the row).
	const int32 RobotCount = Pierce + 1;
	TArray<ARRChaser*> Robots;
	for (int32 Index = 0; Index < RobotCount; ++Index)
	{
		ARRChaser* Robot = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(PierceFirstRobotX + Index, PierceShooterCell.Y));
		if (!Robot)
		{
			return false;
		}
		Robots.Add(Robot);
	}
	TestNearlyEqual(TEXT("chaser starts at the CSV MaxHealth"), RobotHealth(Robots[0]), RRTestStats::EnemyRow(*this, RRTestStats::ChaserRow).MaxHealth);
	const float StartHealth = 3.f * Row.Damage;
	for (ARRChaser* Robot : Robots)
	{
		SetRobotHealth(Robot, StartHealth);
	}
	ARRChaser* LastRobot = Robots.Last();

	// One bullet: the first Pierce robots take Damage each, the last one is untouched and the bullet is gone.
	ARRProjectile* Shot = FireSingleShot(*this, Rifle);
	if (!Shot)
	{
		return false;
	}
	TestEqual(TEXT("bullet carries CSV Pierce hits"), Shot->GetMaxHits(), Pierce);
	FVector LastLocation;
	const int32 Frame = TickUntilDestroyed(TestWorld, Shot, 40, LastLocation);
	FString Healths;
	for (const ARRChaser* Robot : Robots)
	{
		Healths += FString::Printf(TEXT("%.0f "), RobotHealth(Robot));
	}
	AddInfo(FString::Printf(TEXT("pierce %d: bullet destroyed at frame %d near %s; health %s"), Pierce, Frame, *LastLocation.ToCompactString(), *Healths));
	TestTrue(TEXT("bullet destroyed on the last pierced robot"), Frame != INDEX_NONE);
	TestTrue(TEXT("bullet destroyed before the last robot"), LastLocation.X < LastRobot->GetActorLocation().X);
	for (int32 Index = 0; Index < Pierce; ++Index)
	{
		TestNearlyEqual(FString::Printf(TEXT("robot %d hit once"), Index + 1), RobotHealth(Robots[Index]), StartHealth - Row.Damage);
	}
	TestNearlyEqual(TEXT("last robot untouched"), RobotHealth(LastRobot), StartHealth);

	// Pierce is read at fire time: one more point and the next bullet reaches the last robot.
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), static_cast<float>(RobotCount));
	ARRProjectile* Second = FireSingleShot(*this, Rifle);
	if (!Second)
	{
		return false;
	}
	TestEqual(TEXT("second bullet carries CSV Pierce + 1 hits"), Second->GetMaxHits(), RobotCount);
	TestTrue(TEXT("second bullet destroyed on the last robot"), TickUntilDestroyed(TestWorld, Second, 40, LastLocation) != INDEX_NONE);
	for (int32 Index = 0; Index < Pierce; ++Index)
	{
		TestNearlyEqual(FString::Printf(TEXT("robot %d hit twice in total"), Index + 1), RobotHealth(Robots[Index]), StartHealth - 2.f * Row.Damage);
	}
	TestNearlyEqual(TEXT("last robot hit once"), RobotHealth(LastRobot), StartHealth - Row.Damage);

	// A weapon without pierce still hits one robot.
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), 0.f);
	TestEqual(TEXT("Pierce 0 still hits one robot"), Rifle->GetPierceCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponPierceStopsOnRockTest, "RogueRigs.Weapons.PierceStopsOnRock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponPierceStopsOnRockTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponPierceRockWorld"));
	ARRArenaGrid* Grid = nullptr;
	ARRWeapon* Rifle = SpawnPierceRifle(*this, TestWorld, Grid, FVector::ZeroVector, 180.f);
	if (!Rifle)
	{
		return false;
	}
	const FVector ShooterFloor = Grid->GetCellCenter(PierceRockShooterCell.X, PierceRockShooterCell.Y);
	PlaceMuzzleAt(Rifle, ShooterFloor + FVector(60.f, 0.f, 0.5f * Grid->GetBlockHeight()));

	TestFalse(TEXT("(36, 40) is open floor"), Grid->IsBlockCell(PierceRockShooterCell));
	TestFalse(TEXT("(35, 40) is open floor"), Grid->IsBlockCell(FIntPoint(35, 40)));
	TestTrue(TEXT("(34, 40) is a block"), Grid->IsBlockCell(PierceRockCell));
	TestFalse(TEXT("(33, 40) is open floor"), Grid->IsBlockCell(PierceBehindRockCell));

	ARRChaser* Near = SpawnFrozenChaser(*this, TestWorld, ShooterFloor - FVector(60.f, 0.f, 0.f));
	ARRChaser* Far = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(35, 40));
	ARRChaser* Behind = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(PierceBehindRockCell.X, PierceBehindRockCell.Y));
	if (!Near || !Far || !Behind)
	{
		return false;
	}
	// Health from the BurstRifle row: each robot survives one bullet.
	const FRRWeaponStatsRow Row = ReadRifleRow(*this);
	const float StartHealth = 2.f * Row.Damage;
	SetRobotHealth(Near, StartHealth);
	SetRobotHealth(Far, StartHealth);
	SetRobotHealth(Behind, StartHealth);

	ARRProjectile* Shot = FireSingleShot(*this, Rifle);
	if (!Shot)
	{
		return false;
	}
	TestEqual(TEXT("bullet carries CSV Pierce hits"), Shot->GetMaxHits(), FMath::RoundToInt32(Row.Pierce));
	FVector LastLocation;
	const int32 Frame = TickUntilDestroyed(TestWorld, Shot, 40, LastLocation);
	AddInfo(FString::Printf(TEXT("rock: bullet destroyed at frame %d near %s"), Frame, *LastLocation.ToCompactString()));
	TestTrue(TEXT("bullet stopped"), Frame != INDEX_NONE);
	const float RockEastFace = Grid->GetCellCenter(PierceRockCell.X, PierceRockCell.Y).X + 100.f;
	TestTrue(TEXT("bullet stopped at the rock, not beyond"), LastLocation.X >= RockEastFace - 60.f);
	TestNearlyEqual(TEXT("near robot hit"), RobotHealth(Near), StartHealth - Row.Damage);
	TestNearlyEqual(TEXT("far robot hit"), RobotHealth(Far), StartHealth - Row.Damage);
	TestNearlyEqual(TEXT("robot behind the rock untouched"), RobotHealth(Behind), StartHealth);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponPierceNoDoubleHitTest, "RogueRigs.Weapons.PierceNoDoubleHit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponPierceNoDoubleHitTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponPierceTwiceWorld"));
	ARRArenaGrid* Grid = nullptr;
	ARRWeapon* Rifle = SpawnPierceRifle(*this, TestWorld, Grid, FVector::ZeroVector, 0.f);
	if (!Rifle)
	{
		return false;
	}
	PlaceMuzzleAt(Rifle, Grid->GetCellCenter(PierceShooterCell.X, PierceShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight()));
	ARRChaser* First = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(PierceFirstRobotX, PierceShooterCell.Y));
	ARRChaser* Second = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(PierceFirstRobotX + 4, PierceShooterCell.Y));
	if (!First || !Second)
	{
		return false;
	}
	// Health from the BurstRifle row: each robot survives two bullets.
	const FRRWeaponStatsRow Row = ReadRifleRow(*this);
	const float StartHealth = 3.f * Row.Damage;
	SetRobotHealth(First, StartHealth);
	SetRobotHealth(Second, StartHealth);

	ARRProjectile* Shot = FireSingleShot(*this, Rifle);
	if (!Shot)
	{
		return false;
	}
	TWeakObjectPtr<ARRProjectile> WeakShot(Shot);

	// Tick until the first robot is hit.
	int32 HitFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 20 && HitFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (RobotHealth(First) < StartHealth)
		{
			HitFrame = Frame;
		}
	}
	if (!TestTrue(TEXT("first robot hit"), HitFrame != INDEX_NONE) || !TestTrue(TEXT("bullet still flying after the first robot"), WeakShot.IsValid()))
	{
		return false;
	}
	TestEqual(TEXT("one hit counted"), WeakShot->GetHitCount(), 1);

	// The pierced robot jumps ahead of the bullet: the same bullet passes through it again without hurting it.
	const FVector FirstLocation = First->GetActorLocation();
	First->SetActorLocation(FVector(WeakShot->GetActorLocation().X + 250.f, FirstLocation.Y, FirstLocation.Z), /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
	bool bPassedFirstAgain = false;
	for (int32 Frame = 0; Frame < 40 && WeakShot.IsValid(); ++Frame)
	{
		TestWorld.Tick();
		if (WeakShot.IsValid() && WeakShot->GetActorLocation().X > First->GetActorLocation().X + 50.f)
		{
			bPassedFirstAgain = true;
		}
		if (RobotHealth(Second) < StartHealth)
		{
			break;
		}
	}
	AddInfo(FString::Printf(TEXT("first hit at frame %d; first robot moved to x %.0f"), HitFrame, First->GetActorLocation().X));
	TestTrue(TEXT("bullet flew through the moved robot"), bPassedFirstAgain);
	TestNearlyEqual(TEXT("pierced robot hit only once"), RobotHealth(First), StartHealth - Row.Damage);
	TestNearlyEqual(TEXT("next robot hit"), RobotHealth(Second), StartHealth - Row.Damage);
	if (WeakShot.IsValid())
	{
		TestEqual(TEXT("two distinct hits counted"), WeakShot->GetHitCount(), 2);
		WeakShot->Destroy();
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponPierceUpgradeTest, "RogueRigs.Weapons.PierceUpgrade",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponPierceUpgradeTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponPierceUpgradeWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, ChaserShooterCell, KeepRifleOnly) : nullptr;
	ARRWeapon* Rifle = Miner ? FindRifle(Miner) : nullptr;
	URRUpgradeComponent* Upgrades = Miner ? Miner->GetUpgrades() : nullptr;
	if (!TestNotNull(TEXT("miner rifle"), Rifle) || !TestNotNull(TEXT("upgrade component"), Upgrades))
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	Upgrades->SetRandomSeed(20261005);

	// Value rows (UpgradeValues.csv Pierce_Blue, Pierce_Purple, Pierce_Yellow): whole points, targeted = GlobalValue x TargetedMultiplier;
	// none for grey or green.
	float Expected[URRUpgradeComponent::TierCount] = {};
	float ExpectedTargeted[URRUpgradeComponent::TierCount] = {};
	for (int32 Rank = 2; Rank < URRUpgradeComponent::TierCount; ++Rank)
	{
		Expected[Rank] = RRTestStats::UpgradeCardValue(*this, ERRUpgradeStat::Pierce, static_cast<ERRUpgradeRarity>(Rank), false);
		ExpectedTargeted[Rank] = RRTestStats::UpgradeCardValue(*this, ERRUpgradeStat::Pierce, static_cast<ERRUpgradeRarity>(Rank), true);
	}
	for (int32 Rank = 0; Rank < URRUpgradeComponent::TierCount; ++Rank)
	{
		const ERRUpgradeRarity Tier = static_cast<ERRUpgradeRarity>(Rank);
		const FString Label = UEnum::GetValueAsString(Tier);
		float Global = -1.f;
		float Targeted = -1.f;
		const bool bHasRow = Upgrades->GetUpgradeValue(ERRUpgradeStat::Pierce, Tier, false, Global);
		Upgrades->GetUpgradeValue(ERRUpgradeStat::Pierce, Tier, true, Targeted);
		FRRUpgradeCard Card;
		const bool bCard = Upgrades->MakeCard(ERRUpgradeStat::Pierce, Tier, ERRUpgradeTarget::AllWeapons, nullptr, Card);
		if (Rank < 2)
		{
			TestFalse(Label + TEXT(": no piercing row"), bHasRow);
			TestFalse(Label + TEXT(": no piercing card"), bCard);
			continue;
		}
		TestTrue(Label + TEXT(": piercing row"), bHasRow);
		TestNearlyEqual(Label + TEXT(": global value"), Global, Expected[Rank]);
		TestNearlyEqual(Label + TEXT(": targeted value"), Targeted, ExpectedTargeted[Rank]);
		TestTrue(Label + TEXT(": card"), bCard);
		TestEqual(Label + TEXT(": card name"), Card.DisplayName.ToString(), FString(TEXT("Piercing")));
		TestEqual(Label + TEXT(": card value text"), Card.ValueText.ToString(), FString::Printf(TEXT("+%d"), FMath::RoundToInt32(Expected[Rank])));
		TestTrue(Label + TEXT(": weapon stat"), URRUpgradeComponent::IsWeaponStat(Card.Stat));
	}
	FRRUpgradeCard PlayerPierce;
	TestFalse(TEXT("piercing cannot target the player"), Upgrades->MakeCard(ERRUpgradeStat::Pierce, ERRUpgradeRarity::Blue, ERRUpgradeTarget::Player, nullptr, PlayerPierce));

	// Offers: piercing cards appear, never grey or green.
	int32 PierceCards = 0;
	int32 LowTierPierce = 0;
	int32 ShortOffers = 0;
	for (int32 Offer = 0; Offer < 400; ++Offer)
	{
		const TArray<FRRUpgradeCard> Cards = Upgrades->BuildOffer(3, static_cast<float>(Offer % 11));
		ShortOffers += Cards.Num() != 3 ? 1 : 0;
		for (const FRRUpgradeCard& Card : Cards)
		{
			if (Card.Stat == ERRUpgradeStat::Pierce)
			{
				++PierceCards;
				LowTierPierce += Card.Tier < ERRUpgradeRarity::Blue ? 1 : 0;
			}
		}
	}
	AddInfo(FString::Printf(TEXT("400 offers: %d piercing cards"), PierceCards));
	TestEqual(TEXT("every offer has 3 cards"), ShortOffers, 0);
	TestTrue(TEXT("piercing cards are offered"), PierceCards > 0);
	TestEqual(TEXT("no grey or green piercing card"), LowTierPierce, 0);

	// Application: all-weapons blue then targeted yellow on the CSV Pierce; the next bullet carries the sum.
	const float BasePierce = ReadRifleRow(*this).Pierce;
	const float AfterBlue = BasePierce + Expected[2];
	const float AfterYellow = AfterBlue + ExpectedTargeted[4];
	TestNearlyEqual(TEXT("rifle Pierce = CSV Pierce"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetPierceAttribute()), BasePierce);
	FRRUpgradeCard Blue;
	FRRUpgradeCard Yellow;
	TestTrue(TEXT("blue all-weapons card"), Upgrades->MakeCard(ERRUpgradeStat::Pierce, ERRUpgradeRarity::Blue, ERRUpgradeTarget::AllWeapons, nullptr, Blue));
	TestTrue(TEXT("yellow rifle card"), Upgrades->MakeCard(ERRUpgradeStat::Pierce, ERRUpgradeRarity::Yellow, ERRUpgradeTarget::Weapon, Rifle, Yellow));
	TestTrue(TEXT("blue applied"), Upgrades->ApplyUpgrade(Blue));
	TestNearlyEqual(TEXT("Pierce + blue value after blue"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetPierceAttribute()), AfterBlue);
	TestTrue(TEXT("yellow applied"), Upgrades->ApplyUpgrade(Yellow));
	TestNearlyEqual(TEXT("Pierce + blue + yellow after yellow"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetPierceAttribute()), AfterYellow);
	TestEqual(TEXT("pierce count after both cards"), Rifle->GetPierceCount(), FMath::RoundToInt32(AfterYellow));
	TestNearlyEqual(TEXT("base Pierce unchanged"),
		Rifle->GetAbilitySystemComponent()->GetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute()), BasePierce);

	ARRProjectile* Shot = FireSingleShot(*this, Rifle);
	if (Shot)
	{
		TestEqual(TEXT("upgraded bullet carries the upgraded pierce"), Shot->GetMaxHits(), FMath::RoundToInt32(AfterYellow));
		Shot->Destroy();
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponImpactSparksTest, "RogueRigs.Weapons.ImpactSparks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponImpactSparksTest::RunTest(const FString& Parameters)
{
	// The private empty system below cannot run under a real RHI (engine crash on tick); RogueRigs.Weapons.ImpactSparkAsset covers that case.
	if (FApp::CanEverRender())
	{
		AddInfo(TEXT("SKIPPED under a real RHI: run with -nullrhi."));
		return true;
	}

	FTestWorld TestWorld(TEXT("RRWeaponImpactSparksWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRWeapon* Rifle = Grid ? SpawnRifle(*this, TestWorld, FVector::ZeroVector) : nullptr;
	ARRChaser* StillChaser = Rifle ? SpawnStillChaser(*this, TestWorld, Grid->GetCellCenter(ChaserCell.X, ChaserCell.Y)) : nullptr;
	URRNiagaraPoolSubsystem* FXPool = TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>();
	if (!StillChaser || !TestNotNull(TEXT("Niagara pool"), FXPool))
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), 1.f);

	// A private system, so the counts below hold whatever the shipped asset does (Niagara does not run under -nullrhi).
	TStrongObjectPtr<UNiagaraSystem> Sparks(NewObject<UNiagaraSystem>());
	const float ShotZ = 0.5f * Grid->GetBlockHeight();
	const auto Requests = [&]() { return FXPool->GetRequestedCount(Sparks.Get()); };
	const auto FlyOut = [&](ARRProjectile* Shot)
	{
		FVector LastLocation;
		TestTrue(TEXT("the shot ends on its target"), TickUntilDestroyed(TestWorld, Shot, 30, LastLocation) != INDEX_NONE);
	};
	const auto FireRifleFrom = [&](FIntPoint Cell) -> ARRProjectile*
	{
		PlaceMuzzleAt(Rifle, Grid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.f, 0.f, ShotZ));
		ARRProjectile* Shot = FireSingleShot(*this, Rifle);
		if (Shot)
		{
			FProjectileAccess::ImpactSparkSystemOf(Shot) = Sparks.Get();
		}
		return Shot;
	};

	// Rifle shot on a robot: one spark, near the robot and at the shot's height.
	const float HealthBefore = TargetHealth(StillChaser);
	FlyOut(FireRifleFrom(ChaserShooterCell));
	TestEqual(TEXT("rifle on a robot: one spark"), Requests(), 1);
	TestTrue(TEXT("rifle on a robot: damage dealt"), TargetHealth(StillChaser) < HealthBefore);
	const FRRNiagaraSystemPool* SparkPool = FNiagaraPoolAccess::PoolOf(FXPool, Sparks.Get());
	if (TestNotNull(TEXT("spark pool"), SparkPool))
	{
		TestEqual(TEXT("one component created"), SparkPool->CreatedCount, 1);
		const UNiagaraComponent* Component = SparkPool->Free.Num() > 0 ? SparkPool->Free[0].Get() : (SparkPool->InUse.Num() > 0 ? SparkPool->InUse[0].Get() : nullptr);
		if (TestNotNull(TEXT("spark component"), Component))
		{
			const FVector ChaserCenter = StillChaser->GetActorLocation();
			const FVector SparkLocation = Component->GetComponentLocation();
			AddInfo(FString::Printf(TEXT("spark at %s, robot at %s"), *SparkLocation.ToCompactString(), *ChaserCenter.ToCompactString()));
			TestTrue(TEXT("spark on the near side of the robot, within its reach"), SparkLocation.X < ChaserCenter.X && ChaserCenter.X - SparkLocation.X < 100.f);
			TestNearlyEqual(TEXT("spark at the shot's height"), SparkLocation.Z, Grid->GetCellCenter(ChaserShooterCell.X, ChaserShooterCell.Y).Z + ShotZ, 1.0);
		}
	}

	// Turret bullet on a robot: one spark.
	const FVector TurretOrigin = Grid->GetCellCenter(ChaserShooterCell.X, ChaserShooterCell.Y) + FVector(0.f, 0.f, ShotZ);
	ARRProjectile* TurretShot = ARRProjectile::SpawnProjectile(TestWorld.Get(), ARRTurretBullet::StaticClass(), FTransform(TurretOrigin), Rifle, nullptr, FGameplayEffectSpecHandle(), 3000.f, 1.f);
	if (TestNotNull(TEXT("turret bullet"), TurretShot))
	{
		FProjectileAccess::ImpactSparkSystemOf(TurretShot) = Sparks.Get();
		FlyOut(TurretShot);
		TestEqual(TEXT("turret bullet on a robot: one more spark"), Requests(), 2);
	}

	// Rifle shot stopped by rock: one spark on the rock's face, at the shot's height; the rock is untouched.
	TestEqual(TEXT("(43, 38) is rock"), Grid->GetCellType(RockCell), ERRCellType::Rock);
	const float RockHealthBefore = Grid->GetCellHealth(RockCell);
	FlyOut(FireRifleFrom(RockWestFloorCell));
	TestEqual(TEXT("rifle on rock: one spark"), Requests(), 3);
	TestNearlyEqual(TEXT("rifle on rock: rock health unchanged"), Grid->GetCellHealth(RockCell), RockHealthBefore);
	if (TestNotNull(TEXT("spark pool"), SparkPool))
	{
		const FVector RockCenter = Grid->GetCellCenter(RockCell.X, RockCell.Y);
		const FVector ExpectedSpark(RockCenter.X - 0.5f * Grid->GetCellSize(), RockCenter.Y, RockCenter.Z + ShotZ);
		bool bSparkOnFace = false;
		const auto CheckComponents = [&](const auto& Components)
		{
			for (const auto& Component : Components)
			{
				bSparkOnFace |= Component && FVector::Dist(Component->GetComponentLocation(), ExpectedSpark) < 30.0;
			}
		};
		CheckComponents(SparkPool->Free);
		CheckComponents(SparkPool->InUse);
		TestTrue(TEXT("rifle on rock: spark on the rock's face at the shot's height"), bSparkOnFace);
	}

	// Turret bullet stopped by rock: one spark.
	ARRProjectile* TurretOnRock = ARRProjectile::SpawnProjectile(TestWorld.Get(), ARRTurretBullet::StaticClass(), FTransform(Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y) + FVector(0.f, 0.f, ShotZ)), Rifle, nullptr, FGameplayEffectSpecHandle(), 3000.f, 1.f);
	if (TestNotNull(TEXT("turret bullet on rock"), TurretOnRock))
	{
		FProjectileAccess::ImpactSparkSystemOf(TurretOnRock) = Sparks.Get();
		FlyOut(TurretOnRock);
		TestEqual(TEXT("turret bullet on rock: one spark"), Requests(), 4);
	}

	// Rifle shot on a barrel: one spark.
	const FIntPoint BarrelCell(43, 6);
	TestEqual(TEXT("(43, 6) is a barrel"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel);
	FlyOut(FireRifleFrom(BarrelCell - FIntPoint(1, 0)));
	TestEqual(TEXT("rifle on a barrel: one spark"), Requests(), 5);

	// Every ore type of the layout, reached from the open cell on its west: one spark each.
	for (const ERRCellType OreType : { ERRCellType::Ore, ERRCellType::HealOre, ERRCellType::SpecialOre })
	{
		FIntPoint OreCell(INDEX_NONE, INDEX_NONE);
		for (int32 Y = 0; Y < 128 && OreCell.X == INDEX_NONE; ++Y)
		{
			for (int32 X = 1; X < 128 && OreCell.X == INDEX_NONE; ++X)
			{
				if (Grid->GetCellType(FIntPoint(X, Y)) == OreType && Grid->GetCellType(FIntPoint(X - 1, Y)) == ERRCellType::Floor)
				{
					OreCell = FIntPoint(X, Y);
				}
			}
		}
		const FString OreName = UEnum::GetValueAsString(OreType);
		if (OreCell.X == INDEX_NONE)
		{
			AddInfo(FString::Printf(TEXT("%s: no cell with open floor on its west in this layout, not fired at"), *OreName));
			continue;
		}
		AddInfo(FString::Printf(TEXT("%s: fired at (%d, %d)"), *OreName, OreCell.X, OreCell.Y));
		const int32 RequestsBefore = Requests();
		FlyOut(FireRifleFrom(OreCell - FIntPoint(1, 0)));
		TestEqual(FString::Printf(TEXT("rifle on %s: one spark"), *OreName), Requests(), RequestsBefore + 1);
	}

	// Robot shots have no system at all.
	TestNull(TEXT("robot shots have no spark system"), FProjectileAccess::ImpactSparkSystemOf(GetMutableDefault<ARREnemyProjectile>()).Get());

	// Pool full: its only slot waits to start (tick on, inactive: never reclaimed). The spark is counted as skipped, nothing else happens.
	ARRProjectile* LastShot = ARRProjectile::SpawnProjectile(TestWorld.Get(), ARRTurretBullet::StaticClass(), FTransform(TurretOrigin), Rifle, nullptr, FGameplayEffectSpecHandle(), 0.f, 1.f);
	if (TestNotNull(TEXT("idle bullet"), LastShot) && TestNotNull(TEXT("spark pool"), SparkPool))
	{
		FRRNiagaraSystemPool* MutablePool = FNiagaraPoolAccess::PoolOf(FXPool, Sparks.Get());
		MutablePool->InUse.Append(MutablePool->Free);
		MutablePool->Free.Reset();
		for (UNiagaraComponent* Busy : MutablePool->InUse)
		{
			Busy->SetComponentTickEnabled(true);
		}
		FProjectileAccess::ImpactSparkSystemOf(LastShot) = Sparks.Get();
		FProjectileAccess::ImpactSparkPoolSizeOf(LastShot) = MutablePool->InUse.Num();
		const int32 SkippedBefore = FXPool->GetSkippedCount(Sparks.Get());
		const int32 CreatedBefore = MutablePool->CreatedCount;
		FProjectileAccess::PlayImpactSparkOf(LastShot, nullptr);
		TestEqual(TEXT("full pool: the spark is skipped"), FXPool->GetSkippedCount(Sparks.Get()), SkippedBefore + 1);
		TestEqual(TEXT("full pool: no component created"), MutablePool->CreatedCount, CreatedBefore);
		TestFalse(TEXT("full pool: the shot goes on"), LastShot->IsActorBeingDestroyed());
		LastShot->Destroy();
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponImpactSparkAssetTest, "RogueRigs.Weapons.ImpactSparkAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponImpactSparkAssetTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponImpactSparkAssetWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRWeapon* Rifle = Grid ? SpawnRifle(*this, TestWorld, FVector::ZeroVector) : nullptr;
	ARRChaser* StillChaser = Rifle ? SpawnStillChaser(*this, TestWorld, Grid->GetCellCenter(ChaserCell.X, ChaserCell.Y)) : nullptr;
	URRNiagaraPoolSubsystem* FXPool = TestWorld.Get()->GetSubsystem<URRNiagaraPoolSubsystem>();
	UNiagaraSystem* Shipped = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/NS_ShotImpactSpark.NS_ShotImpactSpark"));
	if (!StillChaser || !TestNotNull(TEXT("Niagara pool"), FXPool) || !TestNotNull(TEXT("NS_ShotImpactSpark exists"), Shipped))
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), 1.f);
	TestEqual(TEXT("turret bullet plays it"), FProjectileAccess::ImpactSparkSystemOf(GetMutableDefault<ARRTurretBullet>()).Get(), Shipped);
	TestEqual(TEXT("rifle shot plays it"), FProjectileAccess::ImpactSparkSystemOf(Rifle->GetProjectileClass()->GetDefaultObject<ARRProjectile>()).Get(), Shipped);
	TestTrue(TEXT("Acquire prewarmed the pool"), FXPool->GetComponentCount(Shipped) > 0);
	const int32 CreatedAfterPrewarm = FXPool->GetCreatedCount(Shipped);

	PlaceMuzzleAt(Rifle, Grid->GetCellCenter(ChaserShooterCell.X, ChaserShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight()));
	FVector LastLocation;
	TestTrue(TEXT("the shot ends on the robot"), TickUntilDestroyed(TestWorld, FireSingleShot(*this, Rifle), 30, LastLocation) != INDEX_NONE);
	TestEqual(TEXT("one spark requested"), FXPool->GetRequestedCount(Shipped), 1);
	TestEqual(TEXT("no component created at the hit"), FXPool->GetCreatedCount(Shipped), CreatedAfterPrewarm);

	// Niagara only runs under a real RHI (-RenderOffScreen): the spark plays and comes back to the pool (one-shot).
	if (!FApp::CanEverRender())
	{
		AddInfo(TEXT("play check SKIPPED: FApp::CanEverRender() is false under -nullrhi; run with -RenderOffScreen."));
		return true;
	}
	TestEqual(TEXT("the spark played"), FXPool->GetPlayedCount(Shipped), 1);
	int32 Frames = 0;
	while (FXPool->GetInUseCount(Shipped) > 0 && Frames < 600)
	{
		TestWorld.Tick();
		++Frames;
	}
	AddInfo(FString::Printf(TEXT("spark back in the pool after %d frame(s) of %.4f s"), Frames, FrameSeconds));
	TestEqual(TEXT("one-shot: back in the pool"), FXPool->GetInUseCount(Shipped), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponMuzzleFlashAssetTest, "RogueRigs.Weapons.MuzzleFlashAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * task-440: both muzzle flash systems, read from the simulation under -RenderOffScreen: at most 10 sprites, gone within 0.1 s, the Core
 * emitter holds one streak, one shape and one core, the shape changes from shot to shot, the shotgun flash is larger; BP_Shotgun plays
 * its own system, the rifle the default one; the material compiles.
 */
bool FRRWeaponMuzzleFlashAssetTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;

	UNiagaraSystem* Rifle = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/NS_MuzzleFlash.NS_MuzzleFlash"));
	UNiagaraSystem* Shotgun = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/RogueRigs/FX/NS_MuzzleFlashShotgun.NS_MuzzleFlashShotgun"));
	UMaterial* Material = LoadObject<UMaterial>(nullptr, TEXT("/Game/RogueRigs/FX/M_MuzzleFlash.M_MuzzleFlash"));
	UClass* RifleClass = LoadClass<ARRWeapon>(nullptr, TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C"));
	UClass* ShotgunClass = LoadClass<ARRWeapon>(nullptr, TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_Shotgun.BP_Shotgun_C"));
	if (!TestNotNull(TEXT("NS_MuzzleFlash"), Rifle) || !TestNotNull(TEXT("NS_MuzzleFlashShotgun"), Shotgun) || !TestNotNull(TEXT("M_MuzzleFlash"), Material)
		|| !TestNotNull(TEXT("BP_BurstRifle"), RifleClass) || !TestNotNull(TEXT("BP_Shotgun"), ShotgunClass))
	{
		return false;
	}
	const auto SystemOf = [](UClass* Class) -> UObject*
	{
		const FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Class, TEXT("MuzzleFlashSystem"));
		return Property ? Property->GetObjectPropertyValue_InContainer(Class->GetDefaultObject()) : nullptr;
	};
	TestTrue(TEXT("the rifle plays NS_MuzzleFlash"), SystemOf(RifleClass) == Rifle);
	TestTrue(TEXT("the shotgun plays NS_MuzzleFlashShotgun"), SystemOf(ShotgunClass) == Shotgun);

	// Niagara only runs under a real RHI (-RenderOffScreen).
	if (!FApp::CanEverRender())
	{
		AddInfo(TEXT("SKIPPED: FApp::CanEverRender() is false under -nullrhi; run with -RenderOffScreen."));
		return true;
	}

	FTestWorld TestWorld(TEXT("RRMuzzleFlashAsset"));
	if (FMaterialResource* Resource = Material->GetMaterialResource(TestWorld.Get()->Scene->GetShaderPlatform()))
	{
		Resource->FinishCompilation();
		TestEqual(TEXT("M_MuzzleFlash compile errors"), Resource->GetCompileErrors().Num(), 0);
		for (const FString& Error : Resource->GetCompileErrors())
		{
			AddError(Error);
		}
	}

	double ShapeSize[2] = {0.0, 0.0};
	for (int32 SystemIndex = 0; SystemIndex < 2; ++SystemIndex)
	{
		UNiagaraSystem* System = SystemIndex == 0 ? Rifle : Shotgun;
		constexpr int32 Shots = 16;
		TArray<UNiagaraComponent*> Components;
		for (int32 Shot = 0; Shot < Shots; ++Shot)
		{
			Components.Add(UNiagaraFunctionLibrary::SpawnSystemAtLocation(TestWorld.Get(), System, FVector(0.f, 200.f * Shot, 100.f), FRotator::ZeroRotator,
				FVector::OneVector, /*bAutoDestroy*/ false, /*bAutoActivate*/ true, ENCPoolMethod::None, /*bPreCullCheck*/ false));
		}
		const auto ParticlesOf = [](UNiagaraComponent* Component, FNiagaraEmitterInstance*& OutCore)
		{
			int32 Count = 0;
			OutCore = nullptr;
			FNiagaraSystemInstanceControllerPtr Controller = Component ? Component->GetSystemInstanceController() : nullptr;
			FNiagaraSystemInstance* Instance = Controller.IsValid() ? Controller->GetSystemInstance_Unsafe() : nullptr;
			if (Instance)
			{
				Controller->WaitForConcurrentTickAndFinalize();
				for (const FNiagaraEmitterInstanceRef& Emitter : Instance->GetEmitters())
				{
					Count += Emitter->GetNumParticles();
					if (Emitter->GetEmitterHandle().GetName() == FName(TEXT("Core")))
					{
						OutCore = &Emitter.Get();
					}
				}
			}
			return Count;
		};
		FNiagaraEmitterInstance* Core = nullptr;
		int32 Frame = 0;
		while (ParticlesOf(Components[0], Core) == 0 && Frame < 60)
		{
			TestWorld.Tick();
			++Frame;
		}
		TSet<int32> Variants;
		int32 Largest = 0;
		for (UNiagaraComponent* Component : Components)
		{
			const int32 Count = ParticlesOf(Component, Core);
			Largest = FMath::Max(Largest, Count);
			if (!TestNotNull(TEXT("Core emitter"), Core) || !TestEqual(TEXT("Core holds 3 sprites"), Core->GetNumParticles(), 3))
			{
				return false;
			}
			const FNiagaraDataSet& Data = Core->GetParticleData();
			const auto Colors = FNiagaraDataSetAccessor<FLinearColor>::CreateReader(Data, TEXT("Color"));
			const auto Sizes = FNiagaraDataSetAccessor<FVector2f>::CreateReader(Data, TEXT("SpriteSize"));
			const auto Lifetimes = FNiagaraDataSetAccessor<float>::CreateReader(Data, TEXT("Lifetime"));
			const auto Velocities = FNiagaraDataSetAccessor<FVector3f>::CreateReader(Data, TEXT("Velocity"));
			if (!TestTrue(TEXT("particle attributes readable"), Colors.IsValid() && Sizes.IsValid() && Lifetimes.IsValid() && Velocities.IsValid()))
			{
				return false;
			}
			TSet<int32> Kinds;
			for (int32 Index = 0; Index < 3; ++Index)
			{
				const FLinearColor Color = Colors[Index];
				Kinds.Add(FMath::RoundToInt(Color.B * 2.f));
				TestTrue(TEXT("at most 0.1 s"), Lifetimes[Index] <= 0.1f + UE_KINDA_SMALL_NUMBER);
				TestTrue(TEXT("aligned forward"), Velocities[Index].X > 0.f && FMath::IsNearlyZero(Velocities[Index].Y) && FMath::IsNearlyZero(Velocities[Index].Z));
				if (FMath::IsNearlyZero(Color.B))
				{
					Variants.Add(FMath::FloorToInt(FMath::Clamp(Color.R, 0.f, 1.f) * 3.999f));
					ShapeSize[SystemIndex] = FMath::Max<double>(ShapeSize[SystemIndex], Sizes[Index].X);
				}
			}
			TestEqual(TEXT("one streak, one shape, one core"), Kinds.Num(), 3);
		}
		AddInfo(FString::Printf(TEXT("%s: largest %d sprites, %d of 4 shapes over %d shots, shape size up to %.0f uu"), *System->GetName(), Largest, Variants.Num(), Shots, ShapeSize[SystemIndex]));
		TestTrue(TEXT("at most 10 sprites"), Largest > 0 && Largest <= 10);
		TestTrue(TEXT("the shape changes from shot to shot"), Variants.Num() >= 2);

		// Gone within 0.1 s, two frames of margin.
		for (int32 Tick = 0; Tick < 8; ++Tick)
		{
			TestWorld.Tick();
		}
		for (UNiagaraComponent* Component : Components)
		{
			TestEqual(TEXT("gone within 0.1 s"), ParticlesOf(Component, Core), 0);
			Component->DestroyComponent();
		}
	}
	TestTrue(TEXT("the shotgun flash is larger"), ShapeSize[1] >= ShapeSize[0] * 1.3);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
