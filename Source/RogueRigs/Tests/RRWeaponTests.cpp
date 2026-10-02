// Rogue Rigs - automation tests of the weapon rules (RogueRigs.Weapons.*): burst timing, magazine and reload, projectile stop on rock, GAS damage and crits on the target dummy, dummy refill, miner starting weapon and shot origin, miner moving while firing, miner shots hitting a floor dummy.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Damage/RRGE_Damage.h"
#include "Debug/RRTargetDummy.h"
#include "EngineUtils.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Projectiles/RRCollisionChannels.h"
#include "Projectiles/RRProjectile.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRWeaponTests
{
	using namespace RRTestHarness;

	/** Blueprint classes made in task-007-04. */
	const TCHAR* const BurstRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");
	const TCHAR* const TargetDummyClassPath = TEXT("/Game/RogueRigs/Blueprints/Debug/BP_TargetDummy.BP_TargetDummy_C");

	/** Open floor of arena_m1: the dummy stands on DummyCell, the rifle fires from two cells west of it. */
	const FIntPoint DummyCell(44, 41);
	const FIntPoint DummyShooterCell(42, 41);

	/** Tolerance on times measured from the accumulator (the schedule is exact up to float rounding). */
	constexpr double ExactTolerance = 1.0e-4;

	/** Step used to fire a single shot: the shot due now fires, the next one (>= 0.033 s away) does not. */
	constexpr float SingleShotStep = 0.001f;

	/** Read access to the weapon's protected muzzle. Never instantiated. */
	struct FWeaponAccess : public ARRWeapon
	{
		static USceneComponent* MuzzleOf(const ARRWeapon* Weapon) { return Weapon->*(&FWeaponAccess::Muzzle); }
	};

	/** Read and write access to the miner's protected weapon attachment settings. Never instantiated. */
	struct FMinerAccess : public ARRMiner
	{
		static FName& WeaponAttachSocketNameOf(ARRMiner* Miner) { return Miner->*(&FMinerAccess::WeaponAttachSocketName); }
		static FVector WeaponAttachOffsetOf(const ARRMiner* Miner) { return Miner->*(&FMinerAccess::WeaponAttachOffset); }
	};

	/** Read access to the dummy's protected health set. Never instantiated. */
	struct FDummyAccess : public ARRTargetDummy
	{
		static URRHealthAttributeSet* HealthSetOf(const ARRTargetDummy* Dummy) { return Dummy->*(&FDummyAccess::HealthSet); }
	};

	/** One shot seen by RecordShots: frame index of the step it fired in (step k covers [k, k + 1] * FrameSeconds) and its exact time. */
	struct FShotSample
	{
		int32 Frame = INDEX_NONE;
		double ExactSeconds = 0.0;
	};

	/** One hit received by the dummy (URRHealthAttributeSet::OnDamageReceived). */
	struct FDummyHit
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

	/** Spawns BP_TargetDummy at Location, then lifts it so the bottom of its collision bounds rests on Location (a floor top). */
	ARRTargetDummy* SpawnDummy(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& Location)
	{
		UClass* DummyClass = LoadTestClass<ARRTargetDummy>(Test, TargetDummyClassPath);
		if (!DummyClass)
		{
			return nullptr;
		}
		ARRTargetDummy* Dummy = TestWorld.Spawn<ARRTargetDummy>(DummyClass, Location);
		if (Test.TestNotNull(TEXT("dummy spawned"), Dummy))
		{
			const FBox Bounds = Dummy->GetComponentsBoundingBox(/*bNonColliding*/ false);
			if (Bounds.IsValid)
			{
				Dummy->SetActorLocation(Location + FVector(0.f, 0.f, Location.Z - Bounds.Min.Z), /*bSweep*/ false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
		return Dummy;
	}

	/** Spawns BP_Miner on Cell with WeaponAttachSocketName forced to None before BeginPlay: starting weapons go on the capsule at WeaponAttachOffset. */
	ARRMiner* SpawnMinerWithCapsuleWeapons(FAutomationTestBase& Test, const FTestWorld& TestWorld, const ARRArenaGrid* Grid, FIntPoint Cell)
	{
		return SpawnMiner(Test, TestWorld, Grid, Cell, [](ARRMiner* Miner) { FMinerAccess::WeaponAttachSocketNameOf(Miner) = NAME_None; });
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

	/** Current Health of the dummy. */
	float DummyHealth(const ARRTargetDummy* Dummy)
	{
		return Dummy->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
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

	/**
	 * Fires a fresh BP_BurstRifle for two bursts and a shot at FireRate and checks the schedule: bullets Gap apart, the next burst
	 * Period after the first bullet (pause = Period - 2 * Gap). Returns false if the rifle could not be set up.
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

		const TArray<FShotSample> Shots = RecordShots(Test, Rifle, Recorder.Get(), 7, 120);
		if (!Test.TestEqual(FString::Printf(TEXT("FireRate %g: 7 shots recorded"), FireRate), Shots.Num(), 7))
		{
			return false;
		}

		const FString LabelText = FString::Printf(TEXT("FireRate %g"), FireRate);
		const TCHAR* Label = *LabelText;
		const double Period = ExpectedPause + 2.0 * ExpectedGap;
		for (int32 Shot = 0; Shot < Shots.Num(); ++Shot)
		{
			const double Expected = (Shot / 3) * Period + (Shot % 3) * ExpectedGap;
			CheckShotTime(Test, Label, Shot + 1, Shots[Shot], Expected);
			Test.TestEqual(FString::Printf(TEXT("%s shot %d index in burst"), Label, Shot + 1), Recorder->GetShotIndices()[Shot], Shot % 3);
		}

		const double Gap1 = Shots[1].ExactSeconds - Shots[0].ExactSeconds;
		const double Gap2 = Shots[2].ExactSeconds - Shots[1].ExactSeconds;
		const double Pause = Shots[3].ExactSeconds - Shots[2].ExactSeconds;
		Test.AddInfo(FString::Printf(TEXT("%s: shots at %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f s (frames %d, %d, %d, %d, %d, %d, %d); gaps %.4f, %.4f s; pause %.4f s"),
			Label, Shots[0].ExactSeconds, Shots[1].ExactSeconds, Shots[2].ExactSeconds, Shots[3].ExactSeconds, Shots[4].ExactSeconds,
			Shots[5].ExactSeconds, Shots[6].ExactSeconds, Shots[0].Frame, Shots[1].Frame, Shots[2].Frame, Shots[3].Frame, Shots[4].Frame,
			Shots[5].Frame, Shots[6].Frame, Gap1, Gap2, Pause));
		Test.TestTrue(FString::Printf(TEXT("%s gap %.3f s"), Label, ExpectedGap), FMath::Abs(Gap1 - ExpectedGap) <= ExactTolerance && FMath::Abs(Gap2 - ExpectedGap) <= ExactTolerance);
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

	// CSV row BurstRifle (burst-rifle, owner values 2026-10-02): FireRate 1.25, BurstCount 3, BurstDuty 0.2.
	TestNearlyEqual(TEXT("FireRate 1.25 from the CSV"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetFireRateAttribute()), 1.25f);
	TestEqual(TEXT("BurstCount 3 from the CSV"), Rifle->GetBurstCount(), 3);
	TestNearlyEqual(TEXT("BurstDuty 0.2 from the CSV"), Rifle->GetBurstDuty(), 0.2f);
	TestTrue(TEXT("auto-fire started at acquisition"), Rifle->GetAutoFireComponent()->IsFiring());
	Rifle->Destroy();

	// FireRate 1.25 (CSV): period 0.8 s, bullets at 0, 0.08, 0.16 s, next burst at 0.8 s (pause 0.64 s).
	CheckBurstSchedule(*this, TestWorld, 1.25f, 0.08, 0.64);
	// FireRate 3 (upgraded): period 1/3 s, gap 0.2 / 3 / 2 = 0.0333 s, pause 0.8 / 3 = 0.2667 s.
	CheckBurstSchedule(*this, TestWorld, 3.f, 0.2 / 3.0 / 2.0, (1.0 - 0.2) / 3.0);
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

	TestNearlyEqual(TEXT("MagazineSize 30"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetMagazineSizeAttribute()), 30.f);
	TestNearlyEqual(TEXT("magazine full at acquisition"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute()), 30.f);
	TestNearlyEqual(TEXT("ReloadTime 1.5 s"), Rifle->GetReloadTime(), 1.5f);
	TestNearlyEqual(TEXT("reload duration 1.5 s at ReloadSpeed 1"), AutoFire->GetReloadDuration(), 1.5f);

	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Recorder->SetAmmoSource(Rifle->GetAbilitySystemComponent());
	AutoFire->OnShotFired.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleShotFired);
	AutoFire->OnReloadStarted.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleReloadStarted);
	AutoFire->OnReloadFinished.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleReloadFinished);
	AutoFire->StartFiring();

	int32 ReloadFrames = 0;
	int32 BadAmmoFrames = 0;
	float AmmoAfterShot29 = -1.f;
	const TArray<FShotSample> Shots = RecordShots(*this, Rifle, Recorder.Get(), 31, 600, [&](int32 Frame)
	{
		if (AutoFire->IsReloading())
		{
			++ReloadFrames;
			BadAmmoFrames += WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute()) != 0.f ? 1 : 0;
		}
		if (Recorder->GetShotIndices().Num() == 29 && AmmoAfterShot29 < 0.f)
		{
			AmmoAfterShot29 = WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute());
		}
	});
	if (!TestEqual(TEXT("31 shots recorded"), Shots.Num(), 31))
	{
		return false;
	}

	// Shot 30 is the last bullet of burst 10: 9 * 0.8 + 0.16 = 7.36 s; the reload replaces the pause, the next burst starts at its end.
	const double ReloadStart = Shots[29].ExactSeconds;
	const double ReloadEnd = Shots[30].ExactSeconds;
	AddInfo(FString::Printf(TEXT("shot 30 at %.4f s (frame %d), shot 31 at %.4f s (frame %d): reload %.4f s over %d frames; ammo after shot 29 %.0f; ammo at reload end %s"),
		ReloadStart, Shots[29].Frame, ReloadEnd, Shots[30].Frame, ReloadEnd - ReloadStart, ReloadFrames, AmmoAfterShot29,
		Recorder->GetAmmoAtReloadFinished().Num() > 0 ? *FString::SanitizeFloat(Recorder->GetAmmoAtReloadFinished()[0]) : TEXT("none")));
	CheckShotTime(*this, TEXT("magazine"), 30, Shots[29], 7.36);
	CheckShotTime(*this, TEXT("magazine"), 31, Shots[30], 7.36 + 1.5);
	TestNearlyEqual(TEXT("1 bullet left after shot 29"), AmmoAfterShot29, 1.f);
	TestTrue(TEXT("reload of 1.5 s"), FMath::Abs((ReloadEnd - ReloadStart) - 1.5) <= ExactTolerance);
	TestTrue(TEXT("reloading for 1.5 s +- 1 frame"), FMath::Abs(ReloadFrames - 90) <= 1);
	TestEqual(TEXT("Ammo 0 during the whole reload"), BadAmmoFrames, 0);
	TestEqual(TEXT("one reload started"), Recorder->GetReloadDurations().Num(), 1);
	if (Recorder->GetReloadDurations().Num() > 0)
	{
		TestNearlyEqual(TEXT("OnReloadStarted duration 1.5 s"), Recorder->GetReloadDurations()[0], 1.5f);
	}
	TestEqual(TEXT("one reload finished"), Recorder->GetAmmoAtReloadFinished().Num(), 1);
	if (Recorder->GetAmmoAtReloadFinished().Num() > 0)
	{
		TestNearlyEqual(TEXT("Ammo back to 30 at the reload end"), Recorder->GetAmmoAtReloadFinished()[0], 30.f);
	}
	TestEqual(TEXT("shot 31 opens a new burst"), Recorder->GetShotIndices()[30], 0);
	TestNearlyEqual(TEXT("29 bullets left after shot 31"), WeaponAttribute(Rifle, URRWeaponAttributeSet::GetAmmoAttribute()), 29.f);
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

	// A dummy on the first open cell behind the rock, within range: it must not be hit through the rock.
	ARRTargetDummy* Dummy = nullptr;
	const int32 RangeCells = FMath::FloorToInt(Rifle->GetRange() / Grid->GetCellSize());
	for (int32 X = RockCell.X + 1; X <= RockWestFloorCell.X + RangeCells - 1 && !Dummy; ++X)
	{
		const FIntPoint Cell(X, RockCell.Y);
		const ERRCellType Type = Grid->GetCellType(Cell);
		if (Type == ERRCellType::Floor || Type == ERRCellType::Spawn)
		{
			Dummy = SpawnDummy(*this, TestWorld, Grid->GetCellCenter(Cell.X, Cell.Y));
			AddInfo(FString::Printf(TEXT("dummy placed behind the rock on (%d, %d)"), Cell.X, Cell.Y));
		}
	}
	if (!Dummy)
	{
		AddInfo(TEXT("no open cell behind the rock within range: no dummy behind it"));
	}
	const float DummyHealthBefore = Dummy ? DummyHealth(Dummy) : 0.f;
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
	if (Dummy)
	{
		TestNearlyEqual(TEXT("dummy behind the rock not hit"), DummyHealth(Dummy), DummyHealthBefore);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponProjectileDamageTest, "RogueRigs.Weapons.ProjectileDamagesDummy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponProjectileDamageTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRWeapon* Rifle = Grid ? SpawnRifle(*this, TestWorld, FVector::ZeroVector) : nullptr;
	ARRTargetDummy* Dummy = Rifle ? SpawnDummy(*this, TestWorld, Grid->GetCellCenter(DummyCell.X, DummyCell.Y)) : nullptr;
	if (!Dummy)
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	for (int32 X = DummyShooterCell.X; X <= DummyCell.X; ++X)
	{
		TestFalse(FString::Printf(TEXT("(%d, %d) is open floor"), X, DummyCell.Y), Grid->IsBlockCell(FIntPoint(X, DummyCell.Y)));
	}

	// Muzzle two cells west of the dummy, at half the block height (as in ProjectileStopsOnRock), aiming east (+X).
	const FBox DummyBounds = Dummy->GetComponentsBoundingBox(/*bNonColliding*/ false);
	const FVector MuzzleTarget = Grid->GetCellCenter(DummyShooterCell.X, DummyShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight());
	AddInfo(FString::Printf(TEXT("dummy collision bounds %s; muzzle at %s"), *DummyBounds.ToString(), *MuzzleTarget.ToCompactString()));
	TestTrue(TEXT("dummy stands on the floor and covers the shot height"),
		DummyBounds.IsValid && FMath::IsNearlyEqual(DummyBounds.Min.Z, 0.f, 0.5f) && DummyBounds.Max.Z > MuzzleTarget.Z + 10.f);
	PlaceMuzzleAt(Rifle, MuzzleTarget);

	UAbilitySystemComponent* DummyASC = Dummy->GetAbilitySystemComponent();
	const float MaxHealth = DummyASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	TestNearlyEqual(TEXT("dummy MaxHealth 1000"), MaxHealth, 1000.f);
	TestNearlyEqual(TEXT("dummy starts at MaxHealth"), DummyHealth(Dummy), MaxHealth);

	TArray<FDummyHit> Hits;
	URRHealthAttributeSet* HealthSet = FDummyAccess::HealthSetOf(Dummy);
	const FDelegateHandle HitHandle = HealthSet->OnDamageReceived.AddLambda([&Hits](float Damage, bool bIsCritical, float HealthLeft)
	{
		Hits.Add({ Damage, bIsCritical, HealthLeft });
	});

	UAbilitySystemComponent* RifleASC = Rifle->GetAbilitySystemComponent();
	TestNearlyEqual(TEXT("rifle Damage 10"), RifleASC->GetNumericAttribute(URRWeaponAttributeSet::GetDamageAttribute()), 10.f);
	TestNearlyEqual(TEXT("rifle CritDamage 1.5"), RifleASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritDamageAttribute()), 1.5f);

	// Normal hit: CritChance forced to 0 so the CSV's 5 % cannot roll a crit.
	RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	FVector LastLocation;
	const int32 NormalFrame = TickUntilDestroyed(TestWorld, FireSingleShot(*this, Rifle), 30, LastLocation);
	TestTrue(TEXT("normal shot destroyed on the dummy"), NormalFrame != INDEX_NONE);
	TestEqual(TEXT("one hit after the normal shot"), Hits.Num(), 1);
	const float HealthAfterNormal = DummyHealth(Dummy);
	AddInfo(FString::Printf(TEXT("normal shot: destroyed at frame %d near %s; health %.1f"), NormalFrame, *LastLocation.ToCompactString(), HealthAfterNormal));
	TestNearlyEqual(TEXT("Health lowered by 10"), HealthAfterNormal, MaxHealth - 10.f);
	if (Hits.Num() >= 1)
	{
		TestNearlyEqual(TEXT("hit 1 damage 10"), Hits[0].Damage, 10.f);
		TestFalse(TEXT("hit 1 not critical"), Hits[0].bIsCritical);
		TestNearlyEqual(TEXT("hit 1 health left"), Hits[0].HealthLeft, MaxHealth - 10.f);
	}

	// Critical hit: CritChance forced to 1 on the weapon ASC (captured when the shot makes its spec).
	RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 1.f);
	const int32 CritFrame = TickUntilDestroyed(TestWorld, FireSingleShot(*this, Rifle), 30, LastLocation);
	TestTrue(TEXT("critical shot destroyed on the dummy"), CritFrame != INDEX_NONE);
	TestEqual(TEXT("two hits after the critical shot"), Hits.Num(), 2);
	const float HealthAfterCrit = DummyHealth(Dummy);
	AddInfo(FString::Printf(TEXT("critical shot: destroyed at frame %d near %s; health %.1f"), CritFrame, *LastLocation.ToCompactString(), HealthAfterCrit));
	TestNearlyEqual(TEXT("Health lowered by 15"), HealthAfterNormal - HealthAfterCrit, 15.f);
	if (Hits.Num() >= 2)
	{
		TestNearlyEqual(TEXT("hit 2 damage 15"), Hits[1].Damage, 15.f);
		TestTrue(TEXT("hit 2 critical"), Hits[1].bIsCritical);
		TestNearlyEqual(TEXT("hit 2 health left"), Hits[1].HealthLeft, MaxHealth - 25.f);
	}
	TestFalse(TEXT("dummy not dead"), DummyASC->HasMatchingGameplayTag(RRGameplayTags::State_Dead));

	HealthSet->OnDamageReceived.Remove(HitHandle);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponDummyRefillTest, "RogueRigs.Weapons.DummyRefill",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponDummyRefillTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRWeapon* Rifle = SpawnRifle(*this, TestWorld, FVector(0.f, -500.f, 100.f));
	ARRTargetDummy* Dummy = Rifle ? SpawnDummy(*this, TestWorld, FVector::ZeroVector) : nullptr;
	if (!Dummy)
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	UAbilitySystemComponent* RifleASC = Rifle->GetAbilitySystemComponent();
	UAbilitySystemComponent* DummyASC = Dummy->GetAbilitySystemComponent();
	RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	const float MaxHealth = DummyASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());

	TArray<FDummyHit> Hits;
	URRHealthAttributeSet* HealthSet = FDummyAccess::HealthSetOf(Dummy);
	const FDelegateHandle HitHandle = HealthSet->OnDamageReceived.AddLambda([&Hits](float Damage, bool bIsCritical, float HealthLeft)
	{
		Hits.Add({ Damage, bIsCritical, HealthLeft });
	});

	// Same spec as a projectile carries: URRGE_Damage made by the rifle ASC, applied to the dummy.
	auto ApplyHit = [RifleASC, DummyASC]()
	{
		const FGameplayEffectSpecHandle Spec = RifleASC->MakeOutgoingSpec(URRGE_Damage::StaticClass(), 1.f, RifleASC->MakeEffectContext());
		DummyASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	};

	// Hit at t = 0 and t = 1 s: the refill counts from the last hit, so it lands at 4 s (frame 180 after the second hit).
	ApplyHit();
	TestNearlyEqual(TEXT("first hit: 10 damage"), DummyHealth(Dummy), MaxHealth - 10.f);
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestNearlyEqual(TEXT("no refill 1 s after the first hit"), DummyHealth(Dummy), MaxHealth - 10.f);
	ApplyHit();
	TestNearlyEqual(TEXT("second hit: 20 damage in total"), DummyHealth(Dummy), MaxHealth - 20.f);

	int32 RefillFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= 240 && RefillFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (DummyHealth(Dummy) >= MaxHealth)
		{
			RefillFrame = Frame;
		}
	}
	AddInfo(FString::Printf(TEXT("refilled %d frames (%.4f s) after the last hit"), RefillFrame, RefillFrame * FrameSeconds));
	TestTrue(TEXT("refill 3 s after the last hit +- 1 frame"), RefillFrame >= 179 && RefillFrame <= 181);
	TestNearlyEqual(TEXT("refilled to MaxHealth"), DummyHealth(Dummy), MaxHealth);

	// Lethal hit: Health reaches 0, the dummy refills at once and never gets State.Dead.
	RifleASC->SetNumericAttributeBase(URRWeaponAttributeSet::GetDamageAttribute(), 5000.f);
	const int32 HitsBefore = Hits.Num();
	ApplyHit();
	TestEqual(TEXT("lethal hit received"), Hits.Num(), HitsBefore + 1);
	if (Hits.Num() > HitsBefore)
	{
		AddInfo(FString::Printf(TEXT("lethal hit: damage %.1f, health left %.1f"), Hits.Last().Damage, Hits.Last().HealthLeft));
		TestNearlyEqual(TEXT("lethal hit leaves 0 health"), Hits.Last().HealthLeft, 0.f);
	}
	TestNearlyEqual(TEXT("instant refill after a lethal hit"), DummyHealth(Dummy), MaxHealth);
	TestFalse(TEXT("no State.Dead after a lethal hit"), DummyASC->HasMatchingGameplayTag(RRGameplayTags::State_Dead));

	TWeakObjectPtr<ARRTargetDummy> DummyWeak(Dummy);
	for (int32 Frame = 0; Frame < 240; ++Frame)
	{
		TestWorld.Tick();
	}
	TestTrue(TEXT("dummy still alive 4 s later"), DummyWeak.IsValid() && !Dummy->IsActorBeingDestroyed());
	TestNearlyEqual(TEXT("health still MaxHealth"), DummyHealth(Dummy), MaxHealth);
	TestFalse(TEXT("still no State.Dead"), DummyASC->HasMatchingGameplayTag(RRGameplayTags::State_Dead));

	// A second lethal hit still lands (no dead state blocks damage).
	ApplyHit();
	TestEqual(TEXT("damage still accepted after the lethal hit"), Hits.Num(), HitsBefore + 2);
	TestNearlyEqual(TEXT("instant refill again"), DummyHealth(Dummy), MaxHealth);

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

	if (!TestEqual(TEXT("one starting weapon"), Miner->GetWeapons().Num(), 1))
	{
		return false;
	}
	ARRWeapon* Weapon = Miner->GetWeapons()[0];
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

	// Attachment with WeaponAttachSocketName None: on the capsule at WeaponAttachOffset, turning with the aim.
	UCapsuleComponent* Capsule = Miner->GetCapsuleComponent();
	const FVector AttachOffset = FMinerAccess::WeaponAttachOffsetOf(Miner);
	TestEqual(TEXT("attached to the miner"), Weapon->GetAttachParentActor(), static_cast<AActor*>(Miner));
	TestEqual(TEXT("attached to the capsule"), Weapon->GetRootComponent()->GetAttachParent(), static_cast<USceneComponent*>(Capsule));
	TestEqual(TEXT("no attach socket"), Weapon->GetAttachParentSocketName(), FName(NAME_None));
	TestTrue(FString::Printf(TEXT("relative location = WeaponAttachOffset %s"), *AttachOffset.ToCompactString()),
		Weapon->GetRootComponent()->GetRelativeLocation().Equals(AttachOffset, 0.01f));
	TestTrue(TEXT("weapon points along the miner aim"), Weapon->GetActorForwardVector().Equals(Miner->GetAimDirection(), 1e-3f));
	TestFalse(TEXT("weapon actor has no collision"), Weapon->GetActorEnableCollision());
	TestEqual(TEXT("capsule overlaps the Projectile channel"), static_cast<int32>(Capsule->GetCollisionResponseToChannel(RRCollisionChannels::Projectile)), static_cast<int32>(ECR_Overlap));

	// One shot from the miner standing on (42, 38), aiming east (+X) at the rock (43, 38).
	Weapon->GetAutoFireComponent()->StopFiring();
	const float FloorZ = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y).Z;
	const FVector MinerLocation = Miner->GetActorLocation();
	TestTrue(TEXT("miner aims east"), Miner->GetAimDirection().Equals(FVector::ForwardVector, 1e-3f));
	ARRProjectile* Projectile = FireSingleShot(*this, Weapon);
	if (!Projectile)
	{
		return false;
	}
	const FVector ShotLocation = Projectile->GetActorLocation();
	const float ShotHeight = ShotLocation.Z - FloorZ;
	const FVector ToShot = ShotLocation - MinerLocation;
	const float ExpectedForward = Capsule->GetScaledCapsuleRadius() + Weapon->GetShotForwardOffset();
	const FVector MuzzleLocation = FWeaponAccess::MuzzleOf(Weapon)->GetComponentLocation();
	AddInfo(FString::Printf(TEXT("miner at %s (capsule bottom %.2f uu above the floor), weapon at %s, muzzle (visual) at %s %.1f uu above the floor; shot spawned at %s: %.2f uu above the floor (ShotHeight %.1f, block height %.1f), %.2f uu ahead of the miner axis (expected %.1f)"),
		*MinerLocation.ToCompactString(), MinerLocation.Z - Capsule->GetScaledCapsuleHalfHeight() - FloorZ, *Weapon->GetActorLocation().ToCompactString(),
		*MuzzleLocation.ToCompactString(), MuzzleLocation.Z - FloorZ, *ShotLocation.ToCompactString(), ShotHeight, Weapon->GetShotHeight(),
		Grid->GetBlockHeight(), ToShot.X, ExpectedForward));
	TestTrue(TEXT("ShotHeight is below the block height"), Weapon->GetShotHeight() > 0.f && Weapon->GetShotHeight() < Grid->GetBlockHeight());
	TestTrue(TEXT("shot spawned ShotHeight above the floor"), FMath::IsNearlyEqual(ShotHeight, Weapon->GetShotHeight(), 0.5f));
	TestTrue(TEXT("shot spawned below the block top (shots hit rocks)"), ShotHeight > 0.f && ShotHeight < Grid->GetBlockHeight());
	TestTrue(TEXT("shot spawned capsule radius + ShotForwardOffset ahead along the aim"),
		FMath::IsNearlyEqual(ToShot.X, ExpectedForward, 0.5f) && FMath::Abs(ToShot.Y) < 0.5f);

	// The shot stops on the rock in front of the miner.
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Projectile->GetProjectileMovement()->OnProjectileStop.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleProjectileStop);
	FVector LastLocation;
	const int32 DestroyFrame = TickUntilDestroyed(TestWorld, Projectile, 30, LastLocation);
	AddInfo(FString::Printf(TEXT("miner shot destroyed at frame %d, last location %s"), DestroyFrame, *LastLocation.ToCompactString()));
	TestTrue(TEXT("miner shot destroyed within 3 frames"), DestroyFrame != INDEX_NONE && DestroyFrame <= 3);
	if (TestEqual(TEXT("miner shot: one blocking stop"), Recorder->GetProjectileStops().Num(), 1))
	{
		const FHitResult& Hit = Recorder->GetProjectileStops()[0];
		TestEqual(TEXT("miner shot stopped by the grid"), Hit.GetActor(), static_cast<AActor*>(Grid));
		TestEqual(TEXT("miner shot stopped on the rock cell"), Grid->GetCellAtWorld(Hit.ImpactPoint + FVector(1.f, 0.f, 0.f)), RockCell);
	}
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
		if (!Miner || !Test.TestEqual(TEXT("one starting weapon"), Miner->GetWeapons().Num(), 1))
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

		ARRWeapon* Weapon = Miner->GetWeapons()[0];
		URRAutoFireComponent* AutoFire = Weapon->GetAutoFireComponent();
		if (!bFiring)
		{
			AutoFire->StopFiring();
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
	/** Open column of arena_m1 (x = 38, y = 39..44 floor, neighbours open): the miner stands on MinerAimCell, the dummy 3 cells north (-Y). */
	const FIntPoint MinerAimCell(38, 43);
	const FIntPoint MinerAimDummyCell(38, 40);

	/** Stick input (screen axes: X right, Y up) aiming from MinerAimCell at MinerAimDummyCell: screen left = world -Y with CameraYaw 0. */
	const FVector2D MinerAimStickInput(-1.f, 0.f);

	/** Time the dummy has to take its first hit from the miner. */
	constexpr float MinerHitDeadlineSeconds = 2.f;

	/** Expected height of BP_TargetDummy (scale 1.6) and tolerance. */
	constexpr float DummyHeight = 160.f;
	constexpr float DummyHeightTolerance = 1.f;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRWeaponMinerHitsFloorDummyTest, "RogueRigs.Weapons.MinerHitsFloorDummy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRWeaponMinerHitsFloorDummyTest::RunTest(const FString& Parameters)
{
	FTestWorld TestWorld(TEXT("RRWeaponTestWorld"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	for (int32 Y = MinerAimDummyCell.Y; Y <= MinerAimCell.Y; ++Y)
	{
		TestFalse(FString::Printf(TEXT("(%d, %d) is open floor"), MinerAimCell.X, Y), Grid->IsBlockCell(FIntPoint(MinerAimCell.X, Y)));
	}

	// Dummy resting on the floor of its cell (final BP_TargetDummy: 160 uu, centered mesh, so actor Z = floor + 80).
	const FVector DummyFloor = Grid->GetCellCenter(MinerAimDummyCell.X, MinerAimDummyCell.Y);
	ARRTargetDummy* Dummy = SpawnDummy(*this, TestWorld, DummyFloor);
	// Final BP_Miner as authored (no socket override): starting BP_BurstRifle, firing from BeginPlay.
	ARRMiner* Miner = Dummy ? SpawnMiner(*this, TestWorld, Grid, MinerAimCell) : nullptr;
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath);
	if (!Miner || !RifleClass)
	{
		return false;
	}
	EnableMinerMovement(Miner);

	const FBox DummyBounds = Dummy->GetComponentsBoundingBox(/*bNonColliding*/ false);
	AddInfo(FString::Printf(TEXT("dummy at %s (%.1f uu above the floor), collision bounds %s"),
		*Dummy->GetActorLocation().ToCompactString(), Dummy->GetActorLocation().Z - DummyFloor.Z, *DummyBounds.ToString()));
	TestTrue(TEXT("dummy rests on the floor"), DummyBounds.IsValid && FMath::IsNearlyEqual(DummyBounds.Min.Z, DummyFloor.Z, 0.5f));
	TestTrue(TEXT("dummy is 160 uu tall"), FMath::IsNearlyEqual(DummyBounds.Max.Z - DummyBounds.Min.Z, DummyHeight, DummyHeightTolerance));

	if (!TestEqual(TEXT("one starting weapon"), Miner->GetWeapons().Num(), 1))
	{
		return false;
	}
	ARRWeapon* Weapon = Miner->GetWeapons()[0];
	if (!TestTrue(TEXT("starting weapon valid"), IsValid(Weapon)))
	{
		return false;
	}
	TestTrue(TEXT("starting weapon is a BP_BurstRifle"), Weapon->IsA(RifleClass));
	TestTrue(TEXT("auto-fire running"), Weapon->GetAutoFireComponent()->IsFiring());

	// Aim toward the dummy through the stick-aim path, before the first tick.
	Miner->ApplyStickAimInput(MinerAimStickInput);
	const FVector ToDummy = (DummyFloor - Miner->GetActorLocation()).GetSafeNormal2D();
	TestTrue(FString::Printf(TEXT("miner aims at the dummy (aim %s, to dummy %s)"), *Miner->GetAimDirection().ToCompactString(), *ToDummy.ToCompactString()),
		Miner->GetAimDirection().Equals(ToDummy, 1e-3f));

	UAbilitySystemComponent* DummyASC = Dummy->GetAbilitySystemComponent();
	const float MaxHealth = DummyASC->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	TestNearlyEqual(TEXT("dummy starts at MaxHealth"), DummyHealth(Dummy), MaxHealth);

	TArray<FDummyHit> Hits;
	URRHealthAttributeSet* HealthSet = FDummyAccess::HealthSetOf(Dummy);
	const FDelegateHandle HitHandle = HealthSet->OnDamageReceived.AddLambda([&Hits](float Damage, bool bIsCritical, float HealthLeft)
	{
		Hits.Add({ Damage, bIsCritical, HealthLeft });
	});

	// Tick until the dummy loses health or the deadline passes. The dummy refills 3 s after the last hit, so within 2 s
	// a hit leaves Health below MaxHealth.
	const int32 MaxFrames = FMath::CeilToInt32(MinerHitDeadlineSeconds / FrameSeconds);
	int32 FirstHitFrame = INDEX_NONE;
	for (int32 Frame = 1; Frame <= MaxFrames && FirstHitFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (DummyHealth(Dummy) < MaxHealth)
		{
			FirstHitFrame = Frame;
		}
	}
	const float HealthAfter = DummyHealth(Dummy);
	AddInfo(FString::Printf(TEXT("first hit at frame %d (%.3f s); shots fired %d; hits %d; health %.1f / %.1f"),
		FirstHitFrame, FirstHitFrame * FrameSeconds, Weapon->GetAutoFireComponent()->GetTotalShotsFired(), Hits.Num(), HealthAfter, MaxHealth));
	TestTrue(TEXT("the miner's shots hit the floor dummy within 2 s"), FirstHitFrame != INDEX_NONE);
	TestTrue(TEXT("dummy Health below MaxHealth"), HealthAfter < MaxHealth);
	TestTrue(TEXT("damage received through the health set"), Hits.Num() > 0);

	HealthSet->OnDamageReceived.Remove(HitHandle);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
