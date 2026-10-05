// Rogue Rigs - heavy integration tests with the final Blueprints (RogueRigs.Integration.Heavy*): BP_Heavy telegraphs then slams a vulnerable BP_Miner for 25 with its glow, tilt and shock ring, misses beyond 220 uu, takes the BP_Miner shotgun pellet knockback reduced by 0.8 and crosses a one-cell corridor of arena_m1.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RRHeavy.h"
#include "EngineUtils.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Projectiles/RRShotgunPellet.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRShotgun.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRHeavyIntegrationTests
{
	using namespace RRTestHarness;

	/** Final Blueprints of the heavy loop. */
	const TCHAR* const HeavyClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Heavy.BP_Heavy_C");
	const TCHAR* const ChaserClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");

	/** Open row 43 of arena_m1 (x 28 to 51 floor): miner on the west, heavy 600 uu east. */
	const FIntPoint OpenMinerCell(30, 43);
	constexpr float OpenHeavyOffset = 600.f;

	/** Open floor rows 41-43 from x 32 to 45: shotgun miner on (32, 42) aiming +X. */
	const FIntPoint ShotgunMinerCell(32, 42);

	/** One-cell corridor of arena_m1: column x 36, rows 66 to 68. */
	constexpr int32 CorridorX = 36;
	constexpr int32 CorridorMinY = 66;
	constexpr int32 CorridorMaxY = 68;

	/** Heavy CSV row: slam damage, radius, telegraph, cadence, knockback resistance; trigger 220 x SlamTriggerRatio 0.7. */
	constexpr float SlamDamage = 25.f;
	constexpr float SlamRadius = 220.f;
	constexpr float TriggerDistance = 154.f;
	constexpr double TelegraphSeconds = 0.8;
	constexpr double SlamPeriod = 2.5;
	constexpr float HeavyResistance = 0.8f;

	/** Spawns a robot of Blueprint class Class with its feet on the floor at FloorLocation. */
	template <typename T>
	T* SpawnRobot(FAutomationTestBase& Test, const FTestWorld& TestWorld, UClass* Class, const FVector& FloorLocation)
	{
		const float HalfHeight = Class->GetDefaultObject<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		T* Robot = TestWorld.Get()->SpawnActorDeferred<T>(Class, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(FString::Printf(TEXT("%s spawned"), *Class->GetName()), Robot))
		{
			Robot->FinishSpawning(SpawnTransform);
		}
		return Robot;
	}

	/** Health of Actor (health set on its ASC). */
	float Health(const IAbilitySystemInterface* Actor)
	{
		return Actor->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Grid, BP_Heavy class and BP_Miner (vulnerable, full health) on Cell with every weapon stopped; false when something failed. */
	bool SetUp(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner, UClass*& OutHeavyClass)
	{
		OutHeavyClass = LoadTestClass<ARRHeavy>(Test, HeavyClassPath);
		OutGrid = OutHeavyClass ? SpawnGrid(Test, TestWorld) : nullptr;
		OutMiner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, Cell) : nullptr;
		if (!OutMiner)
		{
			return false;
		}
		for (ARRWeapon* Weapon : OutMiner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		return true;
	}

	/** Blueprint component of Actor named Name (SCS variable name); null when absent. */
	UStaticMeshComponent* FindMesh(const AActor* Actor, FName Name)
	{
		TArray<UStaticMeshComponent*> Meshes;
		Actor->GetComponents(Meshes);
		for (UStaticMeshComponent* Mesh : Meshes)
		{
			if (Mesh->GetFName() == Name)
			{
				return Mesh;
			}
		}
		return nullptr;
	}

	/** True when Mesh renders in game (visible and not hidden in game). */
	bool IsShown(const UStaticMeshComponent* Mesh)
	{
		return Mesh->IsVisible() && !Mesh->bHiddenInGame;
	}

	/** Places Miner at Distance uu west (-X) of Heavy on the heavy's row, keeping the miner's height. */
	void TeleportWestOf(ARRMiner* Miner, const ARRHeavy* Heavy, float Distance)
	{
		const FVector HeavyLocation = Heavy->GetActorLocation();
		Miner->SetActorLocation(FVector(HeavyLocation.X - Distance, HeavyLocation.Y, Miner->GetActorLocation().Z));
	}

	/** Ticks until Heavy starts a telegraph (true) or MaxFrames pass (false). */
	bool TickUntilTelegraph(FTestWorld& TestWorld, const ARRHeavy* Heavy, int32 MaxFrames)
	{
		for (int32 Frame = 0; Frame < MaxFrames && !Heavy->IsTelegraphing(); ++Frame)
		{
			TestWorld.Tick();
		}
		return Heavy->IsTelegraphing();
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationHeavySlamTest, "RogueRigs.Integration.HeavyTelegraphSlamsMiner",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationHeavySlamTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationHeavySlam"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	UClass* HeavyClass = nullptr;
	if (!SetUp(*this, TestWorld, OpenMinerCell, Grid, Miner, HeavyClass))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, HeavyClass, MinerFloor + FVector(OpenHeavyOffset, 0.f, 0.f));
	if (!Heavy)
	{
		return false;
	}
	UStaticMeshComponent* Body = FindMesh(Heavy, TEXT("BodyMesh"));
	UStaticMeshComponent* Ring = FindMesh(Heavy, TEXT("ShockRing"));
	if (!TestNotNull(TEXT("BP_Heavy BodyMesh"), Body) || !TestNotNull(TEXT("BP_Heavy ShockRing"), Ring))
	{
		return false;
	}
	TestFalse(TEXT("shock ring hidden at rest"), IsShown(Ring));
	const float HealthBefore = Health(Miner);
	TestEqual(TEXT("miner at full health"), HealthBefore, Miner->GetHealthSet()->GetMaxHealth());

	TArray<double> TelegraphStarts;
	TArray<double> SlamTimes;
	TArray<float> HealthAfterSlams;
	TArray<double> RecoverEnds;
	float FirstTelegraphDistance = -1.f;
	float MaxTelegraphPitch = 0.f;
	float MaxRingScale = 0.f;
	bool bStoppedWhileTelegraphing = true;
	bool bRingFlagOnSlam = true;
	bool bRingShownOnSlam = true;
	bool bRingHiddenAfterRecover = true;
	bool bPoseResetAfterRecover = true;
	for (int32 Frame = 0; Frame < 60 * 10 && RecoverEnds.Num() < 2; ++Frame)
	{
		const ERRHeavyState Previous = Heavy->GetState();
		const int32 SlamsBefore = Heavy->GetSlamCount();
		TestWorld.Tick();
		const double Now = TestWorld.Get()->GetTimeSeconds();
		if (Previous != ERRHeavyState::Telegraph && Heavy->IsTelegraphing())
		{
			TelegraphStarts.Add(Now);
			if (FirstTelegraphDistance < 0.f)
			{
				FirstTelegraphDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
			}
		}
		if (Heavy->IsTelegraphing())
		{
			bStoppedWhileTelegraphing &= Heavy->GetVelocity().Size2D() < 1.f;
			MaxTelegraphPitch = FMath::Max(MaxTelegraphPitch, FMath::Abs(Body->GetRelativeRotation().Pitch));
		}
		if (Heavy->GetSlamCount() > SlamsBefore)
		{
			SlamTimes.Add(Heavy->GetLastSlamTime());
			HealthAfterSlams.Add(Health(Miner));
			bRingFlagOnSlam &= Ring->GetVisibleFlag();
			bRingShownOnSlam &= IsShown(Ring);
		}
		if (Ring->GetVisibleFlag())
		{
			MaxRingScale = FMath::Max(MaxRingScale, static_cast<float>(Ring->GetRelativeScale3D().X));
		}
		if (Previous == ERRHeavyState::Recover && Heavy->GetState() == ERRHeavyState::Chase)
		{
			RecoverEnds.Add(Now);
			bRingHiddenAfterRecover &= !Ring->GetVisibleFlag() && !IsShown(Ring);
			bPoseResetAfterRecover &= FMath::Abs(Body->GetRelativeRotation().Pitch) < 1.f;
		}
	}
	auto Join = [](const TArray<double>& Times)
	{
		return FString::JoinBy(Times, TEXT(", "), [](double Time) { return FString::Printf(TEXT("%.3f"), Time); });
	};
	AddInfo(FString::Printf(TEXT("BP_Heavy from 600 uu: telegraphs at %s (first at %.0f uu); slams at %s; recover ends at %s; miner %.0f -> %.0f; body pitch %.1f; ring scale %.2f (rendered on slam frames: %d)"),
		*Join(TelegraphStarts), FirstTelegraphDistance, *Join(SlamTimes), *Join(RecoverEnds), HealthBefore, Health(Miner), MaxTelegraphPitch, MaxRingScale, bRingShownOnSlam ? 1 : 0));

	if (!TestEqual(TEXT("two slams within 10 s"), SlamTimes.Num(), 2) || !TestTrue(TEXT("one telegraph per slam"), TelegraphStarts.Num() >= 2)
		|| !TestEqual(TEXT("two recoveries"), RecoverEnds.Num(), 2))
	{
		return false;
	}
	TestTrue(TEXT("telegraph starts within 154 uu (220 x 0.7)"), FirstTelegraphDistance > 0.f && FirstTelegraphDistance <= TriggerDistance + 1.f);
	for (int32 Index = 0; Index < 2; ++Index)
	{
		TestTrue(FString::Printf(TEXT("slam %d lands 0.8 s after its telegraph (within one frame)"), Index + 1),
			FMath::Abs(SlamTimes[Index] - TelegraphStarts[Index] - TelegraphSeconds) <= FrameSeconds + 1e-3);
		TestEqual(FString::Printf(TEXT("slam %d deals 25 to the vulnerable miner"), Index + 1), HealthAfterSlams[Index], HealthBefore - SlamDamage * (Index + 1));
		TestTrue(FString::Printf(TEXT("recovery %d ends 0.5 s after its slam (within one frame)"), Index + 1),
			FMath::Abs(RecoverEnds[Index] - SlamTimes[Index] - 0.5) <= FrameSeconds + 1e-3);
	}
	TestTrue(TEXT("slams 2.5 s apart (within one frame)"), FMath::Abs(SlamTimes[1] - SlamTimes[0] - SlamPeriod) <= FrameSeconds + 1e-3);
	TestTrue(TEXT("stopped while telegraphing"), bStoppedWhileTelegraphing);
	TestEqual(TEXT("two hits counted"), Heavy->GetSlamHitCount(), 2);
	TestTrue(TEXT("BP_Heavy tilts its body during the telegraph (pitch 14)"), FMath::IsNearlyEqual(MaxTelegraphPitch, 14.f, 1.f));
	TestTrue(TEXT("BP_Heavy OnSlam sets its shock ring visible on the slam frame"), bRingFlagOnSlam);
	// Rendering needs bHiddenInGame off too (USceneComponent::IsVisible): a ring left Hidden In Game never shows in a game world.
	TestTrue(TEXT("BP_Heavy shock ring renders in game on the slam frame (not Hidden In Game)"), bRingShownOnSlam);
	TestTrue(TEXT("shock ring grows to the slam radius (scale 220 / 50)"), FMath::IsNearlyEqual(MaxRingScale, SlamRadius / 50.f, 0.05f));
	TestTrue(TEXT("shock ring hidden once the recovery ends"), bRingHiddenAfterRecover);
	TestTrue(TEXT("body pose reset once the recovery ends"), bPoseResetAfterRecover);
	TestTrue(TEXT("miner alive"), !Miner->IsDead());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationHeavyRadiusTest, "RogueRigs.Integration.HeavySlamRadius",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationHeavyRadiusTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationHeavyRadius"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	UClass* HeavyClass = nullptr;
	if (!SetUp(*this, TestWorld, OpenMinerCell, Grid, Miner, HeavyClass))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, HeavyClass, MinerFloor + FVector(OpenHeavyOffset, 0.f, 0.f));
	if (!Heavy)
	{
		return false;
	}
	const float HealthBefore = Health(Miner);

	// 1. During the first telegraph the miner steps to 210 uu (inside the 220 uu radius, outside the 154 uu trigger): hit for 25.
	if (!TestTrue(TEXT("first telegraph within 5 s"), TickUntilTelegraph(TestWorld, Heavy, 60 * 5)))
	{
		return false;
	}
	TeleportWestOf(Miner, Heavy, 210.f);
	for (int32 Frame = 0; Frame < 60 && Heavy->GetSlamCount() < 1; ++Frame)
	{
		TestWorld.Tick();
	}
	const float InsideDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
	const float HealthAfterInside = Health(Miner);

	// 2. During the second telegraph the miner steps to 230 uu (just outside the radius): the slam lands and misses.
	if (!TestTrue(TEXT("second telegraph within 5 s"), TickUntilTelegraph(TestWorld, Heavy, 60 * 5)))
	{
		return false;
	}
	TeleportWestOf(Miner, Heavy, 230.f);
	for (int32 Frame = 0; Frame < 60 && Heavy->GetSlamCount() < 2; ++Frame)
	{
		TestWorld.Tick();
	}
	const float OutsideDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("slam 1 at %.1f uu: %.0f -> %.0f; slam 2 at %.1f uu: %.0f"), InsideDistance, HealthBefore, HealthAfterInside,
		OutsideDistance, Health(Miner)));

	TestEqual(TEXT("two slams landed"), Heavy->GetSlamCount(), 2);
	TestTrue(TEXT("slam 1 with the miner inside 220 uu"), InsideDistance < SlamRadius && InsideDistance > TriggerDistance);
	TestEqual(TEXT("slam 1 deals 25"), HealthAfterInside, HealthBefore - SlamDamage);
	TestTrue(TEXT("slam 2 with the miner beyond 220 uu"), OutsideDistance > SlamRadius);
	TestEqual(TEXT("slam 2 deals nothing"), Health(Miner), HealthAfterInside);
	TestEqual(TEXT("one hit in total"), Heavy->GetSlamHitCount(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationHeavyKnockbackTest, "RogueRigs.Integration.HeavyResistsShotgunKnockback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationHeavyKnockbackTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationHeavyKnockback"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	UClass* HeavyClass = nullptr;
	UClass* ChaserClass = LoadTestClass<ARRChaser>(*this, ChaserClassPath);
	if (!ChaserClass || !SetUp(*this, TestWorld, ShotgunMinerCell, Grid, Miner, HeavyClass))
	{
		return false;
	}
	ARRShotgun* Shotgun = FindShotgun(Miner);
	if (!TestNotNull(TEXT("BP_Miner holds a shotgun"), Shotgun))
	{
		return false;
	}
	Miner->ApplyStickAimInput(FVector2D(0.f, 1.f)); // Screen up = world +X.
	Shotgun->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);

	// BP_Heavy and BP_Chaser 500 uu from the shot origin on the edge pellet rays (+15 / -15 deg). Frozen (actor and movement ticks off)
	// until the pellets are gone so the pending launch can be read; the wider heavy (radius 48) may also take the +10 deg pellet.
	const FVector Origin = Shotgun->GetShotOrigin();
	const FVector Aim = Miner->GetAimDirection().GetSafeNormal2D();
	const float FloorZ = Grid->GetCellCenter(0, 0).Z;
	auto OnRay = [&Origin, &Aim, FloorZ](float AngleDeg)
	{
		const FVector Location = Origin + Aim.RotateAngleAxis(AngleDeg, FVector::UpVector) * 500.f;
		return FVector(Location.X, Location.Y, FloorZ);
	};
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, HeavyClass, OnRay(15.f));
	ARRChaser* Chaser = SpawnRobot<ARRChaser>(*this, TestWorld, ChaserClass, OnRay(-15.f));
	if (!Heavy || !Chaser)
	{
		return false;
	}
	for (ARREnemy* Robot : { static_cast<ARREnemy*>(Heavy), static_cast<ARREnemy*>(Chaser) })
	{
		Robot->SetActorTickEnabled(false);
		Robot->GetCharacterMovement()->SetComponentTickEnabled(false);
	}
	TestEqual(TEXT("BP_Heavy knockback resistance 0.8"), Heavy->GetKnockbackResistance(), HeavyResistance);
	TestEqual(TEXT("BP_Chaser knockback resistance 0"), Chaser->GetKnockbackResistance(), 0.f);
	const float HeavyHealth = Health(Heavy);
	const float ChaserHealth = Health(Chaser);
	const float Force = Shotgun->GetKnockbackForce();
	const float PelletDamage = Shotgun->GetAbilitySystemComponent()->GetNumericAttribute(URRWeaponAttributeSet::GetDamageAttribute());
	int32 HeavyHits = 0;
	int32 ChaserHits = 0;
	Heavy->GetHealthSet()->OnDamageReceived.AddLambda([&HeavyHits](float, bool, float) { ++HeavyHits; });
	Chaser->GetHealthSet()->OnDamageReceived.AddLambda([&ChaserHits](float, bool, float) { ++ChaserHits; });

	const double HeavyStart = FVector::Dist2D(Heavy->GetActorLocation(), Origin);
	const double ChaserStart = FVector::Dist2D(Chaser->GetActorLocation(), Origin);
	const int32 ShotsBefore = Shotgun->GetShotCount();
	URRAutoFireComponent* AutoFire = Shotgun->GetAutoFireComponent();
	AutoFire->StartFiring();
	AutoFire->AdvanceTime(0.001f);
	AutoFire->StopFiring();
	TestEqual(TEXT("exactly one shot fired"), Shotgun->GetShotCount(), ShotsBefore + 1);
	auto PelletsLeft = [&TestWorld]()
	{
		int32 Count = 0;
		for (TActorIterator<ARRShotgunPellet> It(TestWorld.Get()); It; ++It)
		{
			Count += IsValid(*It) ? 1 : 0;
		}
		return Count;
	};
	for (int32 Frame = 0; Frame < 60 && PelletsLeft() > 0; ++Frame)
	{
		TestWorld.Tick();
	}

	const FVector HeavyLaunch = Heavy->GetCharacterMovement()->PendingLaunchVelocity;
	const FVector ChaserLaunch = Chaser->GetCharacterMovement()->PendingLaunchVelocity;
	for (ARREnemy* Robot : { static_cast<ARREnemy*>(Heavy), static_cast<ARREnemy*>(Chaser) })
	{
		Robot->SetActorTickEnabled(true);
		Robot->GetCharacterMovement()->SetComponentTickEnabled(true);
	}
	for (int32 Frame = 0; Frame < 3; ++Frame)
	{
		TestWorld.Tick();
	}
	const double HeavyPush = FVector::Dist2D(Heavy->GetActorLocation(), Origin) - HeavyStart;
	const double ChaserPush = FVector::Dist2D(Chaser->GetActorLocation(), Origin) - ChaserStart;
	AddInfo(FString::Printf(TEXT("force %.0f per pellet: heavy %d hits, launch %s (%.0f uu/s); chaser %d hits, launch %s (%.0f uu/s); pushed %.1f / %.1f uu in 3 frames; health heavy %.0f -> %.0f, chaser %.0f -> %.0f"),
		Force, HeavyHits, *HeavyLaunch.ToCompactString(), HeavyLaunch.Size2D(), ChaserHits, *ChaserLaunch.ToCompactString(), ChaserLaunch.Size2D(), HeavyPush, ChaserPush,
		HeavyHealth, Health(Heavy), ChaserHealth, Health(Chaser)));

	TestEqual(TEXT("shotgun knockback 250 per pellet hit"), Force, 250.f);
	TestEqual(TEXT("BP_Chaser hit by its edge pellet"), ChaserHits, 1);
	TestTrue(TEXT("BP_Heavy hit by 1 or 2 pellets"), HeavyHits >= 1 && HeavyHits <= 2);
	TestTrue(TEXT("BP_Chaser launched at the full force along its pellet"), ChaserLaunch.Equals(Aim.RotateAngleAxis(-15.f, FVector::UpVector) * Force, 0.05f));
	const double HeavyPerHit = HeavyLaunch.Size2D() / FMath::Max(HeavyHits, 1);
	const double ChaserPerHit = ChaserLaunch.Size2D() / FMath::Max(ChaserHits, 1);
	TestTrue(TEXT("BP_Heavy launch per pellet hit is force x (1 - 0.8)"), FMath::IsNearlyEqual(HeavyPerHit, Force * (1.0 - HeavyResistance), 0.01 * Force));
	TestTrue(TEXT("heavy launch per hit is 0.2 of the chaser's"), FMath::IsNearlyEqual(HeavyPerHit / FMath::Max(ChaserPerHit, 1.0), 0.2, 5e-3));
	TestTrue(TEXT("BP_Heavy pushed outward"), HeavyLaunch.Dot(Aim) > 0.0);
	TestEqual(TEXT("BP_Heavy took one pellet damage per hit"), Health(Heavy), HeavyHealth - HeavyHits * PelletDamage);
	TestTrue(TEXT("BP_Heavy alive"), !Heavy->IsDead());
	TestTrue(TEXT("BP_Heavy pushed less than BP_Chaser"), HeavyPush < ChaserPush);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationHeavyCorridorTest, "RogueRigs.Integration.HeavyCrossesCorridor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationHeavyCorridorTest::RunTest(const FString& Parameters)
{
	using namespace RRHeavyIntegrationTests;

	// Miner north of the corridor on (36, 61); BP_Heavy south of it on (36, 69): the shortest path is the corridor x 36, rows 66-68.
	FTestWorld TestWorld(TEXT("RRIntegrationHeavyCorridor"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	UClass* HeavyClass = nullptr;
	if (!SetUp(*this, TestWorld, FIntPoint(CorridorX, 61), Grid, Miner, HeavyClass))
	{
		return false;
	}
	const URRFlowFieldSubsystem* FlowField = TestWorld.Get()->GetSubsystem<URRFlowFieldSubsystem>();
	TestWorld.Tick(); // The flow field binds the grid on its first tick.
	for (int32 Y = CorridorMinY; Y <= CorridorMaxY; ++Y)
	{
		TestTrue(FString::Printf(TEXT("corridor cell (%d, %d) walkable, sides blocked"), CorridorX, Y), FlowField->IsWalkableCell(FIntPoint(CorridorX, Y))
			&& !FlowField->IsWalkableCell(FIntPoint(CorridorX - 1, Y)) && !FlowField->IsWalkableCell(FIntPoint(CorridorX + 1, Y)));
	}
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, HeavyClass, Grid->GetCellCenter(CorridorX, 69));
	if (!Heavy)
	{
		return false;
	}
	TestEqual(TEXT("BP_Heavy keeps the C++ capsule radius 48"), Heavy->GetCapsuleComponent()->GetScaledCapsuleRadius(), 48.f);

	TSet<int32> CorridorRowsVisited;
	bool bAlwaysWalkable = true;
	double ExitTime = -1.0;
	for (int32 Frame = 0; Frame < 60 * 15 && ExitTime < 0.0; ++Frame)
	{
		TestWorld.Tick();
		const FIntPoint Cell = Grid->GetCellAtWorld(Heavy->GetActorLocation());
		bAlwaysWalkable &= FlowField->IsWalkableCell(Cell);
		if (Cell.X == CorridorX && Cell.Y >= CorridorMinY && Cell.Y <= CorridorMaxY)
		{
			CorridorRowsVisited.Add(Cell.Y);
		}
		if (Cell.Y < CorridorMinY - 1)
		{
			ExitTime = TestWorld.Get()->GetTimeSeconds();
		}
	}
	AddInfo(FString::Printf(TEXT("BP_Heavy left the corridor north at t=%.3f s, %d corridor rows crossed"), ExitTime, CorridorRowsVisited.Num()));
	TestTrue(TEXT("came out north of the corridor within 15 s"), ExitTime > 0.0);
	TestEqual(TEXT("crossed the three corridor cells"), CorridorRowsVisited.Num(), 3);
	TestTrue(TEXT("center always on a walkable cell"), bAlwaysWalkable);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
