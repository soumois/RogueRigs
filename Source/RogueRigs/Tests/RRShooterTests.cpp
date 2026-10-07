// Rogue Rigs - automation tests of the shooter (RogueRigs.Shooter.*): CSV stats, approach then aim and fire cadence, on-screen gating, repositioning for line of sight, holding when approached, robot shots on rock and through robots, spawner share.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShooter.h"
#include "EngineUtils.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Projectiles/RREnemyProjectile.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRShooterTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Open row 43 of arena_m1 (x 28 to 51 floor): miner on the west, shooter 8 cells east in plain sight. */
	const FIntPoint OpenMinerCell(30, 43);
	constexpr float OpenShooterOffset = 1600.f;

	/** Miner north of the rock mass x 35-41, rows 45-47; shooter 5 cells south of it (1000 uu, in the ring) without sight. */
	const FIntPoint CoverMinerCell(39, 43);
	const FIntPoint CoverShooterCell(39, 48);

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

	/** Health of Actor (health set on its ASC). */
	float Health(const IAbilitySystemInterface* Actor)
	{
		return Actor->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Stops every weapon of Miner (robots must not die). */
	void StopWeapons(const ARRMiner* Miner)
	{
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
	}

	/** Grid, unarmed miner on StartCell; false when something failed. */
	bool MakeArena(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint StartCell, ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner)
	{
		OutGrid = SpawnGrid(Test, TestWorld);
		OutMiner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, StartCell) : nullptr;
		if (!OutMiner)
		{
			return false;
		}
		StopWeapons(OutMiner);
		return true;
	}

	/** View rectangle of +/- HalfSize around Center (XY). */
	FBox2D ViewAround(const FVector& Center, float HalfSize)
	{
		return FBox2D(FVector2D(Center.X - HalfSize, Center.Y - HalfSize), FVector2D(Center.X + HalfSize, Center.Y + HalfSize));
	}

	/** Live robot shots in the world. */
	int32 CountEnemyShots(UWorld* World)
	{
		int32 Count = 0;
		for (TActorIterator<ARREnemyProjectile> It(World); It; ++It)
		{
			Count += IsValid(*It) && !It->IsActorBeingDestroyed() ? 1 : 0;
		}
		return Count;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShooterStatsTest, "RogueRigs.Shooter.StatsFromCSV",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShooterStatsTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterTests;

	FRREnemyStatsRow Row;
	if (TestTrue(TEXT("EnemyStats.csv row Shooter read"), ARREnemy::LoadStatsRow(TEXT("Shooter"), Row)))
	{
		TestEqual(TEXT("MaxHealth 40"), Row.MaxHealth, 40.f);
		TestEqual(TEXT("MoveSpeed 180"), Row.MoveSpeed, 180.f);
		TestEqual(TEXT("Damage 12"), Row.Damage, 12.f);
		TestEqual(TEXT("AttackRate 0.5 (one shot every 2 s)"), Row.AttackRate, 0.5f);
		TestEqual(TEXT("AttackRange 960 (owner 2026-10-06, was 1200)"), Row.AttackRange, 960.f);
		TestEqual(TEXT("WindUp 0.6 (laser)"), Row.WindUp, 0.6f);
		TestEqual(TEXT("XPValue 2"), Row.XPValue, 2);
		TestEqual(TEXT("KnockbackResistance 0.2"), Row.KnockbackResistance, 0.2f);
		TestEqual(TEXT("PreferredDistance 800"), Row.PreferredDistance, 800.f);
		TestEqual(TEXT("PreferredDistanceTolerance 200"), Row.PreferredDistanceTolerance, 200.f);
		TestEqual(TEXT("ProjectileSpeed 1200"), Row.ProjectileSpeed, 1200.f);
	}
	FRREnemyStatsRow KamikazeRow;
	if (TestTrue(TEXT("EnemyStats.csv row Kamikaze read"), ARREnemy::LoadStatsRow(TEXT("Kamikaze"), KamikazeRow)))
	{
		TestEqual(TEXT("kamikaze arm range = AttackRange 250"), KamikazeRow.AttackRange, 250.f);
		TestEqual(TEXT("kamikaze fuse = WindUp 0.65"), KamikazeRow.WindUp, 0.65f);
	}

	FTestWorld TestWorld(TEXT("RRShooterStats"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y));
	ARRKamikaze* Kamikaze = SpawnRobot<ARRKamikaze>(*this, TestWorld, Grid->GetCellCenter(OpenMinerCell.X + 4, OpenMinerCell.Y));
	if (!Shooter || !Kamikaze)
	{
		return false;
	}
	TestEqual(TEXT("Health 40"), Health(Shooter), 40.f);
	TestEqual(TEXT("MaxWalkSpeed 180"), Shooter->GetCharacterMovement()->MaxWalkSpeed, 180.f);
	TestEqual(TEXT("knockback resistance 0.2"), Shooter->GetKnockbackResistance(), 0.2f);
	TestEqual(TEXT("arrival at the preferred distance"), Shooter->GetEnemyMovement()->GetArrivalDistance(), 800.f);
	TestEqual(TEXT("body radius 34 (miner size)"), Shooter->GetBodyRadius(), 34.f);
	TestFalse(TEXT("laser hidden at rest"), FShooterAccess::LaserOf(Shooter)->IsVisible());
	TestEqual(TEXT("kamikaze arm range from the CSV"), Kamikaze->GetArmRange(), 250.f);
	TestEqual(TEXT("kamikaze fuse from the CSV"), Kamikaze->GetFuseTime(), 0.65f);
	TestEqual(TEXT("kamikaze arrival 125 (0.5 x the 250 uu arm range)"), Kamikaze->GetEnemyMovement()->GetArrivalDistance(), 125.f);

	const TSubclassOf<ARRProjectile> ShotClass = FShooterAccess::ProjectileClassOf(Shooter);
	TestTrue(TEXT("default shot class is ARREnemyProjectile"), ShotClass == ARREnemyProjectile::StaticClass());
	const ARRProjectile* ShotDefaults = ShotClass ? ShotClass->GetDefaultObject<ARRProjectile>() : nullptr;
	if (TestNotNull(TEXT("shot defaults"), ShotDefaults))
	{
		TestFalse(TEXT("robot shots do not damage the terrain"), ShotDefaults->DamagesTerrain());
		TestFalse(TEXT("robot shots never hit robots"), ShotDefaults->CanHit(Kamikaze));
		TestTrue(TEXT("robot shots hit the miner class"), ShotDefaults->CanHit(GetDefault<ARRMiner>()));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShooterFireTest, "RogueRigs.Shooter.ApproachAimFire",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShooterFireTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterTests;

	FTestWorld TestWorld(TEXT("RRShooterFire"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, OpenMinerCell, Grid, Miner))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, MinerFloor + FVector(OpenShooterOffset, 0.f, 0.f));
	if (!Shooter)
	{
		return false;
	}
	Shooter->SetViewRectangleOverride(ViewAround(MinerFloor, 4000.f));
	const float HealthBefore = Health(Miner);
	UStaticMeshComponent* Laser = FShooterAccess::LaserOf(Shooter);

	float AimStartDistance = -1.f;
	double FirstAimStart = -1.0;
	double FirstShotTime = -1.0;
	double SecondShotTime = -1.0;
	bool bLaserShownWhileAiming = true;
	float LaserLengthError = 0.f;
	bool bStoppedWhileAiming = true;
	float HealthAfterFirstHit = -1.f;
	int32 MaxShots = 0;
	for (int32 Frame = 0; Frame < 60 * 7 && SecondShotTime < 0.0; ++Frame)
	{
		const ERRShooterState Previous = Shooter->GetState();
		TestWorld.Tick();
		const double Now = TestWorld.Get()->GetTimeSeconds();
		const float Distance = FVector::Dist2D(Shooter->GetActorLocation(), Miner->GetActorLocation());
		if (Previous != ERRShooterState::Aim && Shooter->IsAiming() && FirstAimStart < 0.0)
		{
			FirstAimStart = Now;
			AimStartDistance = Distance;
		}
		if (Shooter->IsAiming())
		{
			bLaserShownWhileAiming &= Laser->IsVisible();
			LaserLengthError = FMath::Max(LaserLengthError, FMath::Abs(Shooter->GetLaserLength()
				- FVector::Dist2D(Shooter->GetAimOrigin(), Shooter->GetAimTargetPoint())));
			bStoppedWhileAiming &= Shooter->GetVelocity().Size2D() < 1.f;
		}
		if (Shooter->GetShotCount() == 1 && FirstShotTime < 0.0)
		{
			FirstShotTime = Shooter->GetLastShotTime();
		}
		if (Shooter->GetShotCount() == 2)
		{
			SecondShotTime = Shooter->GetLastShotTime();
		}
		if (HealthAfterFirstHit < 0.f && Health(Miner) < HealthBefore)
		{
			HealthAfterFirstHit = Health(Miner);
		}
		MaxShots = FMath::Max(MaxShots, CountEnemyShots(TestWorld.Get()));
	}
	const float FinalDistance = FVector::Dist2D(Shooter->GetActorLocation(), Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("first aim at t=%.3f s, %.0f uu; shots at %.3f and %.3f s; miner health %.0f -> %.0f; distance after the second shot %.0f uu, state %d"),
		FirstAimStart, AimStartDistance, FirstShotTime, SecondShotTime, HealthBefore, HealthAfterFirstHit, FinalDistance, static_cast<int32>(Shooter->GetState())));

	TestTrue(TEXT("aimed once within range (<= 960 uu)"), AimStartDistance > 0.f && AimStartDistance <= 960.f);
	TestTrue(TEXT("first aim only after approaching (beyond range at spawn)"), FirstAimStart > 0.0);
	TestTrue(TEXT("first shot 0.6 s after the aim start (within one frame)"),
		FirstShotTime > 0.0 && FMath::Abs(FirstShotTime - FirstAimStart - 0.6) <= FrameSeconds + 1e-3);
	TestTrue(TEXT("second shot 2.0 s after the first (within one frame)"),
		SecondShotTime > 0.0 && FMath::Abs(SecondShotTime - FirstShotTime - 2.0) <= FrameSeconds + 1e-3);
	TestTrue(TEXT("laser visible during every aim frame"), bLaserShownWhileAiming);
	TestTrue(TEXT("laser length = muzzle to aim point in open sight"), LaserLengthError < 1.f);
	TestTrue(TEXT("stopped while aiming"), bStoppedWhileAiming);
	TestEqual(TEXT("first hit deals 12"), HealthAfterFirstHit, HealthBefore - 12.f);
	TestTrue(TEXT("shots were live projectiles"), MaxShots >= 1);
	TestTrue(TEXT("keeps to the preferred ring (600 to 1000 uu)"), FinalDistance >= 600.f && FinalDistance <= 1000.f);
	TestFalse(TEXT("laser hidden after the shot"), Shooter->IsAiming() && !Laser->IsVisible());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShooterOnScreenTest, "RogueRigs.Shooter.FiresOnlyOnScreen",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShooterOnScreenTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterTests;

	FTestWorld TestWorld(TEXT("RRShooterOnScreen"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, OpenMinerCell, Grid, Miner))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, MinerFloor + FVector(800.f, 0.f, 0.f));
	if (!Shooter)
	{
		return false;
	}

	// The camera fallback (miner camera, headless; floor rectangle about miner X -883 to +2123 uu) sees a robot 800 uu east, not one 2600 uu east.
	TestWorld.Tick();
	TestTrue(TEXT("camera rectangle: shooter 800 uu east is on screen"), Shooter->IsOnScreen());
	ARRShooter* FarShooter = SpawnRobot<ARRShooter>(*this, TestWorld, Grid->GetCellCenter(OpenMinerCell.X + 13, OpenMinerCell.Y));
	TestTrue(TEXT("camera rectangle: shooter 13 cells east is off screen"), FarShooter && !FarShooter->IsOnScreen());
	if (FarShooter)
	{
		FarShooter->Destroy();
	}

	// View that shows the miner only: in range with sight, it never aims.
	Shooter->SetViewRectangleOverride(ViewAround(MinerFloor, 300.f));
	bool bAimedOffScreen = false;
	bool bSightHeld = true;
	for (int32 Frame = 0; Frame < 60 * 3; ++Frame)
	{
		TestWorld.Tick();
		bAimedOffScreen |= Shooter->IsAiming() || Shooter->GetShotCount() > 0;
		bSightHeld &= Shooter->HasSightOfTarget();
	}
	TestTrue(TEXT("sight held while off screen"), bSightHeld);
	TestFalse(TEXT("no aim, no shot while off screen"), bAimedOffScreen);
	TestEqual(TEXT("holds in the ring"), Shooter->GetState(), ERRShooterState::Hold);

	// Back on screen: aims on the next frame; leaving the screen mid-aim cancels it without a shot.
	Shooter->SetViewRectangleOverride(ViewAround(MinerFloor, 4000.f));
	TestWorld.Tick();
	TestTrue(TEXT("aims as soon as it is on screen"), Shooter->IsAiming());
	TestNotNull(TEXT("aim target is the miner"), Shooter->GetAimTarget());
	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
	}
	Shooter->SetViewRectangleOverride(ViewAround(MinerFloor, 300.f));
	TestWorld.Tick();
	TestFalse(TEXT("aim cancelled off screen"), Shooter->IsAiming());
	TestFalse(TEXT("laser hidden after the cancel"), FShooterAccess::LaserOf(Shooter)->IsVisible());
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("no shot from a cancelled aim"), Shooter->GetShotCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShooterRepositionTest, "RogueRigs.Shooter.RepositionsForSight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShooterRepositionTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterTests;

	FTestWorld TestWorld(TEXT("RRShooterReposition"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, CoverMinerCell, Grid, Miner))
	{
		return false;
	}
	URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, Grid->GetCellCenter(CoverShooterCell.X, CoverShooterCell.Y));
	if (!Shooter || !TestNotNull(TEXT("flow field"), FlowField))
	{
		return false;
	}
	Shooter->SetViewRectangleOverride(ViewAround(Miner->GetActorLocation(), 4000.f));
	const FVector MinerLocation = Miner->GetActorLocation();
	TestFalse(TEXT("no sight through the rock at spawn"), Grid->HasLineOfSight(Shooter->GetActorLocation(), MinerLocation));

	bool bRepositioned = false;
	bool bAimedWithoutSight = false;
	double FirstAim = -1.0;
	float MaxRetreat = 0.f;
	float RetreatAt = 0.f;
	for (int32 Frame = 0; Frame < 60 * 10 && Shooter->GetShotCount() == 0; ++Frame)
	{
		const ERRShooterState Previous = Shooter->GetState();
		const float Before = FVector::Dist2D(Shooter->GetActorLocation(), MinerLocation);
		TestWorld.Tick();
		const float Step = static_cast<float>(FVector::Dist2D(Shooter->GetActorLocation(), MinerLocation)) - Before;
		// Only the detour itself (Reposition before and after the step): beyond the hold distance Approach is a plain flow-field walk.
		if (Previous == ERRShooterState::Reposition && Shooter->GetState() == ERRShooterState::Reposition && Step > MaxRetreat)
		{
			MaxRetreat = Step;
			RetreatAt = Before;
		}
		bRepositioned |= Shooter->GetState() == ERRShooterState::Reposition;
		if (Previous != ERRShooterState::Aim && Shooter->IsAiming())
		{
			bAimedWithoutSight |= !Grid->HasLineOfSight(Shooter->GetActorLocation(), Miner->GetActorLocation());
			if (FirstAim < 0.0)
			{
				FirstAim = TestWorld.Get()->GetTimeSeconds();
			}
		}
	}
	const float Distance = FVector::Dist2D(Shooter->GetActorLocation(), Miner->GetActorLocation());
	const FIntPoint EndCell = Grid->GetCellAtWorld(Shooter->GetActorLocation());
	AddInfo(FString::Printf(TEXT("first aim at t=%.3f s from cell (%d, %d), %.0f uu; shots %d; largest detour step away %.2f uu at %.0f uu"), FirstAim, EndCell.X, EndCell.Y, Distance,
		Shooter->GetShotCount(), MaxRetreat, RetreatAt));
	TestTrue(TEXT("entered Reposition (flow-field detour)"), bRepositioned);
	TestTrue(TEXT("never increases its distance to the miner while detouring (0.5 uu tolerance)"), MaxRetreat <= 0.5f);
	TestFalse(TEXT("never aimed without sight"), bAimedWithoutSight);
	TestTrue(TEXT("fired after moving to a firing position"), Shooter->GetShotCount() >= 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShooterHoldsWhenApproachedTest, "RogueRigs.Shooter.HoldsWhenApproached",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShooterHoldsWhenApproachedTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterTests;

	// Open floor, shooter 800 uu east of the miner with sight; the miner walks into it at 300 uu/s and stays in contact.
	FTestWorld TestWorld(TEXT("RRShooterHoldsWhenApproached"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, OpenMinerCell, Grid, Miner))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, MinerFloor + FVector(800.f, 0.f, 0.f));
	if (!Shooter)
	{
		return false;
	}
	Shooter->SetViewRectangleOverride(ViewAround(MinerFloor, 4000.f));
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	float MaxRetreat = 0.f;
	float MinDistance = TNumericLimits<float>::Max();
	bool bRepositioned = false;
	for (int32 Frame = 0; Frame < 60 * 5; ++Frame)
	{
		// The miner walks east until 150 uu from the shooter (capsules almost touching), then stays.
		const FVector ToShooter = Shooter->GetActorLocation() - Miner->GetActorLocation();
		if (ToShooter.Size2D() > 150.f)
		{
			Miner->SetActorLocation(Miner->GetActorLocation() + FVector(ToShooter.GetSafeNormal2D() * 300.f * FrameSeconds), false, nullptr, ETeleportType::TeleportPhysics);
		}
		// The shooter's own move this frame, measured against the miner's new position.
		const FVector MinerLocation = Miner->GetActorLocation();
		const float Before = FVector::Dist2D(Shooter->GetActorLocation(), MinerLocation);
		TestWorld.Tick();
		const float After = FVector::Dist2D(Shooter->GetActorLocation(), MinerLocation);
		MaxRetreat = FMath::Max(MaxRetreat, After - Before);
		MinDistance = FMath::Min(MinDistance, After);
		bRepositioned |= Shooter->GetState() == ERRShooterState::Reposition;
	}
	AddInfo(FString::Printf(TEXT("approached shooter: largest step away %.2f uu, closest %.0f uu, state %d, shots %d"), MaxRetreat, MinDistance,
		static_cast<int32>(Shooter->GetState()), Shooter->GetShotCount()));
	TestTrue(TEXT("never increases its distance to the miner (0.5 uu tolerance)"), MaxRetreat <= 0.5f);
	TestTrue(TEXT("the miner came into contact range"), MinDistance <= 160.f);
	TestFalse(TEXT("never repositions with sight"), bRepositioned);
	TestTrue(TEXT("keeps firing at point blank"), Shooter->GetShotCount() >= 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShooterShotRulesTest, "RogueRigs.Shooter.ShotRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShooterShotRulesTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterTests;

	FTestWorld TestWorld(TEXT("RRShooterShotRules"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	if (!MakeArena(*this, TestWorld, OpenMinerCell, Grid, Miner))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	// The shooter stays off screen (no own shot); it is only the owner and the damage source of the hand-fired shots.
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, Grid->GetCellCenter(52, 48));
	ARRChaser* Chaser = SpawnRobot<ARRChaser>(*this, TestWorld, MinerFloor + FVector(600.f, 0.f, 0.f));
	if (!Shooter || !Chaser)
	{
		return false;
	}
	Shooter->SetViewRectangleOverride(FBox2D(FVector2D(-2.f, -2.f), FVector2D(-1.f, -1.f)));
	const float MinerHealth = Health(Miner);
	const float ChaserHealth = Health(Chaser);
	const float ShotHeight = 50.f;

	// 1. Through a robot to the miner: fired west along row 43 from 1000 uu, the chaser in between.
	const FVector Origin = MinerFloor + FVector(1000.f, 0.f, ShotHeight);
	TWeakObjectPtr<ARRProjectile> Shot = ARRProjectile::SpawnProjectile(TestWorld.Get(), ARREnemyProjectile::StaticClass(),
		FTransform(FRotator(0.f, 180.f, 0.f), Origin), Shooter, Shooter, FShooterAccess::DamageSpecOf(Shooter), 1200.f, 1.f);
	TestTrue(TEXT("shot spawned"), Shot.IsValid());
	for (int32 Frame = 0; Frame < 60 && Shot.IsValid() && !Shot->IsActorBeingDestroyed(); ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("chaser untouched"), Health(Chaser), ChaserHealth);
	TestEqual(TEXT("miner hit for 12"), Health(Miner), MinerHealth - 12.f);

	// 2. Into rock (43, 38) from (42, 38): stops, the rock keeps its health.
	const float RockHealth = Grid->GetCellHealth(RockCell);
	const FVector RockOrigin = Grid->GetCellCenter(RockWestFloorCell.X, RockWestFloorCell.Y) + FVector(0.f, 0.f, ShotHeight);
	Shot = ARRProjectile::SpawnProjectile(TestWorld.Get(), ARREnemyProjectile::StaticClass(), FTransform(FRotator::ZeroRotator, RockOrigin),
		Shooter, Shooter, FShooterAccess::DamageSpecOf(Shooter), 1200.f, 1.f);
	int32 StopFrame = -1;
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
		if (!Shot.IsValid() || Shot->IsActorBeingDestroyed())
		{
			StopFrame = Frame;
			break;
		}
	}
	TestTrue(TEXT("shot stops on the rock within 10 frames"), StopFrame >= 0 && StopFrame < 10);
	TestEqual(TEXT("rock still standing"), Grid->GetCellType(RockCell), ERRCellType::Rock);
	TestEqual(TEXT("rock health unchanged"), Grid->GetCellHealth(RockCell), RockHealth);

	// 3. Into the barrel (48, 41) from (48, 43): stops, the barrel keeps its shot health.
	const FIntPoint BarrelCell(48, 41);
	if (TestEqual(TEXT("barrel cell"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel))
	{
		const float BarrelShotHealth = Grid->GetCellShotHealth(BarrelCell);
		const FVector BarrelOrigin = Grid->GetCellCenter(48, 43) + FVector(0.f, 0.f, ShotHeight);
		Shot = ARRProjectile::SpawnProjectile(TestWorld.Get(), ARREnemyProjectile::StaticClass(), FTransform(FRotator(0.f, -90.f, 0.f), BarrelOrigin),
			Shooter, Shooter, FShooterAccess::DamageSpecOf(Shooter), 1200.f, 1.f);
		for (int32 Frame = 0; Frame < 30 && Shot.IsValid() && !Shot->IsActorBeingDestroyed(); ++Frame)
		{
			TestWorld.Tick();
		}
		TestTrue(TEXT("shot stopped at the barrel"), !Shot.IsValid() || Shot->IsActorBeingDestroyed());
		TestEqual(TEXT("barrel still standing"), Grid->GetCellType(BarrelCell), ERRCellType::Barrel);
		TestEqual(TEXT("barrel shot health unchanged"), Grid->GetCellShotHealth(BarrelCell), BarrelShotHealth);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShooterSpawnerShareTest, "RogueRigs.Shooter.SpawnerShare",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShooterSpawnerShareTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterTests;

	FTestWorld TestWorld(TEXT("RRShooterSpawnerShare"));
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("spawner spawned"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);

	// Default of the placed spawner (set on BP_EnemySpawner): chaser 70, kamikaze 15, shooter 15.
	FRRWeightedEnemyClass ChaserEntry;
	ChaserEntry.EnemyClass = ARRChaser::StaticClass();
	ChaserEntry.Weight = 70.f;
	FRRWeightedEnemyClass KamikazeEntry;
	KamikazeEntry.EnemyClass = ARRKamikaze::StaticClass();
	KamikazeEntry.Weight = 15.f;
	FRRWeightedEnemyClass ShooterEntry;
	ShooterEntry.EnemyClass = ARRShooter::StaticClass();
	ShooterEntry.Weight = 15.f;
	FSpawnerAccess::WeightsOf(Spawner) = { ChaserEntry, KamikazeEntry, ShooterEntry };

	FRandomStream Stream(20261004);
	constexpr int32 Picks = 2000;
	int32 Counts[3] = { 0, 0, 0 };
	for (int32 Index = 0; Index < Picks; ++Index)
	{
		const TSubclassOf<ARREnemy> Picked = Spawner->PickEnemyClass(Stream);
		Counts[0] += Picked == ARRChaser::StaticClass() ? 1 : 0;
		Counts[1] += Picked == ARRKamikaze::StaticClass() ? 1 : 0;
		Counts[2] += Picked == ARRShooter::StaticClass() ? 1 : 0;
	}
	AddInfo(FString::Printf(TEXT("%d chasers, %d kamikazes, %d shooters over %d seeded picks"), Counts[0], Counts[1], Counts[2], Picks));
	TestEqual(TEXT("every pick is one of the three classes"), Counts[0] + Counts[1] + Counts[2], Picks);
	TestTrue(TEXT("66 to 74 % chasers"), Counts[0] >= 1320 && Counts[0] <= 1480);
	TestTrue(TEXT("12 to 18 % kamikazes"), Counts[1] >= 240 && Counts[1] <= 360);
	TestTrue(TEXT("12 to 18 % shooters"), Counts[2] >= 240 && Counts[2] <= 360);

	TArray<TSubclassOf<ARREnemy>> Classes;
	Spawner->GetSpawnableClasses(Classes);
	TestTrue(TEXT("shooter listed as spawnable"), Classes.Contains(ARRShooter::StaticClass()));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
