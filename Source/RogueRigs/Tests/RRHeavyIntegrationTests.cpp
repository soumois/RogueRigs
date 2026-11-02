// Rogue Rigs - heavy integration tests with the final Blueprints (RogueRigs.Integration.Heavy*): BP_Heavy telegraphs then slams a vulnerable BP_Miner for its CSV Damage with its glow and telegraph clip (id 1), misses beyond its CSV AttackRange, takes the BP_Miner shotgun pellet knockback reduced by its CSV KnockbackResistance and crosses a one-cell corridor of arena_m1.

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
#include "Tests/RRTestStatsRows.h"
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

	/** ARRHeavy SlamTriggerRatio default (EditDefaultsOnly, not a CSV value): the telegraph starts within AttackRange x this ratio. */
	constexpr float SlamTriggerRatio = 0.7f;

	/** Slam values derived from the Heavy row of EnemyStats.csv (task-420: never copied in the test). */
	struct FSlamValues
	{
		/** Damage of one slam (Damage). */
		float Damage = 0.f;
		/** Slam radius (AttackRange). */
		float Radius = 0.f;
		/** Telegraph trigger distance (AttackRange x SlamTriggerRatio). */
		float TriggerDistance = 0.f;
		/** Telegraph duration (WindUp). */
		double TelegraphSeconds = 0.0;
		/** Seconds between two slams (1 / AttackRate). */
		double Period = 0.0;
		/** Knockback resistance (KnockbackResistance). */
		float Resistance = 0.f;
	};

	/** Reads the Heavy row and derives the slam values (test error when the row is missing). */
	FSlamValues ReadSlamValues(FAutomationTestBase& Test)
	{
		const FRREnemyStatsRow Row = RRTestStats::EnemyRow(Test, RRTestStats::HeavyRow);
		FSlamValues Values;
		Values.Damage = Row.Damage;
		Values.Radius = Row.AttackRange;
		Values.TriggerDistance = Row.AttackRange * SlamTriggerRatio;
		Values.TelegraphSeconds = Row.WindUp;
		Values.Period = Row.AttackRate > 0.f ? 1.0 / Row.AttackRate : 0.0;
		Values.Resistance = Row.KnockbackResistance;
		return Values;
	}

	/** ARRHeavy TelegraphClipId: clip written on entering Telegraph (task-285-02; replaced the BP body tilt removed in task-285-03). */
	constexpr float TelegraphClipId = 1.f;

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
		OutMiner = OutGrid ? SpawnMiner(Test, TestWorld, OutGrid, Cell, M1TestLoadout) : nullptr;
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
	if (!TestNotNull(TEXT("BP_Heavy BodyMesh"), Body))
	{
		return false;
	}
	const float HealthBefore = Health(Miner);
	TestEqual(TEXT("miner at full health"), HealthBefore, Miner->GetHealthSet()->GetMaxHealth());

	TArray<double> TelegraphStarts;
	TArray<double> SlamTimes;
	TArray<float> HealthAfterSlams;
	TArray<double> RecoverEnds;
	float FirstTelegraphDistance = -1.f;
	bool bTelegraphClipAtStart = true;
	TArray<float> TelegraphStartClips;
	bool bStoppedWhileTelegraphing = true;
	bool bPoseResetAfterRecover = true;
	TArray<FString> RaiseFrames;
	double SlamClipStart = -1.0;
	for (int32 Frame = 0; Frame < 60 * 10 && RecoverEnds.Num() < 2; ++Frame)
	{
		const ERRHeavyState Previous = Heavy->GetState();
		const int32 SlamsBefore = Heavy->GetSlamCount();
		TestWorld.Tick();
		const double Now = TestWorld.Get()->GetTimeSeconds();
		if (Previous != ERRHeavyState::Telegraph && Heavy->IsTelegraphing())
		{
			TelegraphStarts.Add(Now);
			TelegraphStartClips.Add(Heavy->GetAnimClipId());
			bTelegraphClipAtStart &= FMath::IsNearlyEqual(Heavy->GetAnimClipId(), TelegraphClipId);
			if (FirstTelegraphDistance < 0.f)
			{
				FirstTelegraphDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
			}
		}
		if (Heavy->IsTelegraphing())
		{
			bStoppedWhileTelegraphing &= Heavy->GetVelocity().Size2D() < 1.f;
			// First telegraph only (task-293-01, info): clip-1 shown frame per step, then the slam-down clip start.
			if (TelegraphStarts.Num() == 1 && FMath::IsNearlyEqual(Heavy->GetAnimClipId(), TelegraphClipId))
			{
				RaiseFrames.Add(FString::Printf(TEXT("%.3f:%.1f"), Now - TelegraphStarts[0], (Now - Heavy->GetAnimClipStart()) * 30.0));
			}
			else if (TelegraphStarts.Num() == 1 && SlamClipStart < 0.0)
			{
				SlamClipStart = Heavy->GetAnimClipStart();
			}
		}
		if (Heavy->GetSlamCount() > SlamsBefore)
		{
			SlamTimes.Add(Heavy->GetLastSlamTime());
			HealthAfterSlams.Add(Health(Miner));
		}
		if (Previous == ERRHeavyState::Recover && Heavy->GetState() == ERRHeavyState::Chase)
		{
			RecoverEnds.Add(Now);
			bPoseResetAfterRecover &= FMath::Abs(Body->GetRelativeRotation().Pitch) < 1.f;
		}
	}
	auto Join = [](const TArray<double>& Times)
	{
		return FString::JoinBy(Times, TEXT(", "), [](double Time) { return FString::Printf(TEXT("%.3f"), Time); });
	};
	AddInfo(FString::Printf(TEXT("BP_Heavy from 600 uu: telegraphs at %s (first at %.0f uu); slams at %s; recover ends at %s; miner %.0f -> %.0f; telegraph start clips %s"),
		*Join(TelegraphStarts), FirstTelegraphDistance, *Join(SlamTimes), *Join(RecoverEnds), HealthBefore, Health(Miner),
		*FString::JoinBy(TelegraphStartClips, TEXT(", "), [](float Clip) { return FString::Printf(TEXT("%.0f"), Clip); })));

	AddInfo(FString::Printf(TEXT("telegraph 1 clip-1 frames (s:frame): %s; slam-down clip at +%.3f s, slam at +%.3f s (%.3f s after the slam-down start)"),
		*FString::Join(RaiseFrames, TEXT(" ")), TelegraphStarts.Num() > 0 ? SlamClipStart - TelegraphStarts[0] : -1.0,
		TelegraphStarts.Num() > 0 && SlamTimes.Num() > 0 ? SlamTimes[0] - TelegraphStarts[0] : -1.0,
		SlamTimes.Num() > 0 ? SlamTimes[0] - SlamClipStart : -1.0));

	if (!TestEqual(TEXT("two slams within 10 s"), SlamTimes.Num(), 2) || !TestTrue(TEXT("one telegraph per slam"), TelegraphStarts.Num() >= 2)
		|| !TestEqual(TEXT("two recoveries"), RecoverEnds.Num(), 2))
	{
		return false;
	}
	const FSlamValues Slam = ReadSlamValues(*this);
	TestTrue(TEXT("telegraph starts within AttackRange x SlamTriggerRatio"), FirstTelegraphDistance > 0.f && FirstTelegraphDistance <= Slam.TriggerDistance + 1.f);
	for (int32 Index = 0; Index < 2; ++Index)
	{
		TestTrue(FString::Printf(TEXT("slam %d lands WindUp s after its telegraph (within one frame)"), Index + 1),
			FMath::Abs(SlamTimes[Index] - TelegraphStarts[Index] - Slam.TelegraphSeconds) <= FrameSeconds + 1e-3);
		TestEqual(FString::Printf(TEXT("slam %d deals the CSV Damage to the vulnerable miner"), Index + 1), HealthAfterSlams[Index], HealthBefore - Slam.Damage * (Index + 1));
		TestTrue(FString::Printf(TEXT("recovery %d ends 0.5 s after its slam (within one frame)"), Index + 1),
			FMath::Abs(RecoverEnds[Index] - SlamTimes[Index] - 0.5) <= FrameSeconds + 1e-3);
	}
	TestTrue(TEXT("slams 1 / AttackRate s apart (within one frame)"), FMath::Abs(SlamTimes[1] - SlamTimes[0] - Slam.Period) <= FrameSeconds + 1e-3);
	TestTrue(TEXT("stopped while telegraphing"), bStoppedWhileTelegraphing);
	TestEqual(TEXT("two hits counted"), Heavy->GetSlamHitCount(), 2);
	TestTrue(TEXT("BP_Heavy plays the telegraph clip (id 1) when the telegraph starts"), bTelegraphClipAtStart);
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
	const FSlamValues Slam = ReadSlamValues(*this);

	// 1. During the first telegraph the miner steps to 10 uu inside the slam radius (outside the trigger): hit for the CSV Damage.
	if (!TestTrue(TEXT("first telegraph within 5 s"), TickUntilTelegraph(TestWorld, Heavy, 60 * 5)))
	{
		return false;
	}
	TeleportWestOf(Miner, Heavy, Slam.Radius - 10.f);
	for (int32 Frame = 0; Frame < 90 && Heavy->GetSlamCount() < 1; ++Frame)
	{
		TestWorld.Tick();
	}
	const float InsideDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
	const float HealthAfterInside = Health(Miner);

	// 2. During the second telegraph the miner steps to 10 uu outside the radius: the slam lands and misses.
	if (!TestTrue(TEXT("second telegraph within 5 s"), TickUntilTelegraph(TestWorld, Heavy, 60 * 5)))
	{
		return false;
	}
	TeleportWestOf(Miner, Heavy, Slam.Radius + 10.f);
	for (int32 Frame = 0; Frame < 90 && Heavy->GetSlamCount() < 2; ++Frame)
	{
		TestWorld.Tick();
	}
	const float OutsideDistance = FVector::Dist2D(Heavy->GetActorLocation(), Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("slam 1 at %.1f uu: %.0f -> %.0f; slam 2 at %.1f uu: %.0f"), InsideDistance, HealthBefore, HealthAfterInside,
		OutsideDistance, Health(Miner)));

	TestEqual(TEXT("two slams landed"), Heavy->GetSlamCount(), 2);
	TestTrue(TEXT("slam 1 with the miner inside the radius"), InsideDistance < Slam.Radius && InsideDistance > Slam.TriggerDistance);
	TestEqual(TEXT("slam 1 deals the CSV Damage"), HealthAfterInside, HealthBefore - Slam.Damage);
	TestTrue(TEXT("slam 2 with the miner beyond the radius"), OutsideDistance > Slam.Radius);
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
	const float EdgeAngle = RRTestStats::WeaponRow(*this, RRTestStats::ShotgunRow).SpreadDeg;
	ARRHeavy* Heavy = SpawnRobot<ARRHeavy>(*this, TestWorld, HeavyClass, OnRay(EdgeAngle));
	ARRChaser* Chaser = SpawnRobot<ARRChaser>(*this, TestWorld, ChaserClass, OnRay(-EdgeAngle));
	if (!Heavy || !Chaser)
	{
		return false;
	}
	for (ARREnemy* Robot : { static_cast<ARREnemy*>(Heavy), static_cast<ARREnemy*>(Chaser) })
	{
		Robot->SetActorTickEnabled(false);
		Robot->GetCharacterMovement()->SetComponentTickEnabled(false);
	}
	const FSlamValues Slam = ReadSlamValues(*this);
	const float ChaserResistance = RRTestStats::EnemyRow(*this, RRTestStats::ChaserRow).KnockbackResistance;
	TestEqual(TEXT("BP_Heavy knockback resistance from the CSV"), Heavy->GetKnockbackResistance(), Slam.Resistance);
	TestEqual(TEXT("BP_Chaser knockback resistance from the CSV"), Chaser->GetKnockbackResistance(), ChaserResistance);
	if (!TestTrue(TEXT("CSV: the heavy resists knockback more than the chaser"), Slam.Resistance > ChaserResistance))
	{
		return false;
	}
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

	TestEqual(TEXT("shotgun knockback per pellet hit from the CSV"), Force, RRTestStats::WeaponRow(*this, RRTestStats::ShotgunRow).KnockbackForce);
	TestEqual(TEXT("BP_Chaser hit by its edge pellet"), ChaserHits, 1);
	TestTrue(TEXT("BP_Heavy hit by 1 or 2 pellets"), HeavyHits >= 1 && HeavyHits <= 2);
	TestTrue(TEXT("BP_Chaser launched at force x (1 - resistance) along its pellet"),
		ChaserLaunch.Equals(Aim.RotateAngleAxis(-EdgeAngle, FVector::UpVector) * Force * (1.f - ChaserResistance), 0.05f));
	const double HeavyPerHit = HeavyLaunch.Size2D() / FMath::Max(HeavyHits, 1);
	const double ChaserPerHit = ChaserLaunch.Size2D() / FMath::Max(ChaserHits, 1);
	TestTrue(TEXT("BP_Heavy launch per pellet hit is force x (1 - resistance)"), FMath::IsNearlyEqual(HeavyPerHit, Force * (1.0 - Slam.Resistance), 0.01 * Force));
	TestTrue(TEXT("heavy launch per hit relative to the chaser's = (1 - heavy resistance) / (1 - chaser resistance)"),
		FMath::IsNearlyEqual(HeavyPerHit / FMath::Max(ChaserPerHit, 1.0), (1.0 - Slam.Resistance) / FMath::Max(1.0 - ChaserResistance, 1e-3), 5e-3));
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
	TestEqual(TEXT("BP_Heavy keeps the C++ body radius 48"), Heavy->GetBodyRadius(), 48.f);

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
