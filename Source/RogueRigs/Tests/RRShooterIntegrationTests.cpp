// Rogue Rigs - shooter integration tests with the final Blueprints (RogueRigs.Integration.Shooter*, Integration.PlacedSpawnerMix): BP_Shooter keeps the distance ring and fires laser-then-shot at the real camera view, repositions around rock to fire, BP_ShooterProjectile hits BP_Miner for 12 and spares robots, and the BP_EnemySpawner placed in L_ArenaM1 mixes chaser 65 / kamikaze 12 / shooter 15 / heavy 8 (heavy MaxAlive 5) / shield chaser 0 (director only).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemySpawner.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShieldChaser.h"
#include "Enemies/RRShooter.h"
#include "Engine/Level.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Projectiles/RREnemyProjectile.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRShooterIntegrationTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Final Blueprints and map of the shooter loop. */
	const TCHAR* const ShooterClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Shooter.BP_Shooter_C");
	const TCHAR* const ShooterProjectileClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_ShooterProjectile.BP_ShooterProjectile_C");
	const TCHAR* const ChaserClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");
	const TCHAR* const SpawnerClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner.BP_EnemySpawner_C");
	const TCHAR* const ArenaMapPackage = TEXT("/Game/RogueRigs/Maps/L_ArenaM1");

	/** Open row 43 of arena_m1 (x 28 to 51 floor): miner on the west, shooter 8 cells east in plain sight. */
	const FIntPoint OpenMinerCell(30, 43);
	constexpr float OpenShooterOffset = 1600.f;

	/** Miner north of the rock mass x 35-41, rows 45-47; shooter 5 cells south of it (1000 uu, in the ring) without sight. */
	const FIntPoint CoverMinerCell(39, 43);
	const FIntPoint CoverShooterCell(39, 48);

	/** Ring of the Shooter CSV row: PreferredDistance 800 +/- PreferredDistanceTolerance 200. */
	constexpr float RingMin = 600.f;
	constexpr float RingMax = 1000.f;

	/** Shooter CSV damage per hit. */
	constexpr float ShotDamage = 12.f;

	/** One hit received by the miner. */
	struct FHit
	{
		double Time = 0.0;
		float Damage = 0.f;
	};

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

	/** Records every hit of the miner (time, damage). Remove the returned handle when done. */
	FDelegateHandle RecordHits(ARRMiner* Miner, TArray<FHit>& Hits)
	{
		UWorld* World = Miner->GetWorld();
		return Miner->GetHealthSet()->OnDamageReceived.AddLambda([&Hits, World](float Damage, bool, float)
		{
			Hits.Add({ World->GetTimeSeconds(), Damage });
		});
	}

	/** Grid, BP_Shooter class and BP_Miner on Cell with every weapon stopped (robots must not die); false when something failed. */
	bool SetUp(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, ARRArenaGrid*& OutGrid, ARRMiner*& OutMiner, UClass*& OutShooterClass)
	{
		OutShooterClass = LoadTestClass<ARRShooter>(Test, ShooterClassPath);
		OutGrid = OutShooterClass ? SpawnGrid(Test, TestWorld) : nullptr;
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

	/** View rectangle of +/- HalfSize around Center (XY). */
	FBox2D ViewAround(const FVector& Center, float HalfSize)
	{
		return FBox2D(FVector2D(Center.X - HalfSize, Center.Y - HalfSize), FVector2D(Center.X + HalfSize, Center.Y + HalfSize));
	}

	/** Live robot shots in the world; OutAllOfClass false when one is not of ExpectedClass. */
	int32 CountEnemyShots(UWorld* World, const UClass* ExpectedClass, bool& OutAllOfClass)
	{
		int32 Count = 0;
		for (TActorIterator<ARREnemyProjectile> It(World); It; ++It)
		{
			if (IsValid(*It) && !It->IsActorBeingDestroyed())
			{
				++Count;
				OutAllOfClass &= It->GetClass() == ExpectedClass;
			}
		}
		return Count;
	}

	/** Every hit in Hits deals exactly ShotDamage. */
	bool AllHitsAre12(const TArray<FHit>& Hits)
	{
		for (const FHit& Hit : Hits)
		{
			if (!FMath::IsNearlyEqual(Hit.Damage, ShotDamage))
			{
				return false;
			}
		}
		return true;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationShooterRingTest, "RogueRigs.Integration.ShooterHoldsRingAndFires",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationShooterRingTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationShooterRing"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	UClass* ShooterClass = nullptr;
	UClass* ShotClass = LoadTestClass<ARREnemyProjectile>(*this, ShooterProjectileClassPath);
	if (!ShotClass || !SetUp(*this, TestWorld, OpenMinerCell, Grid, Miner, ShooterClass))
	{
		return false;
	}
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, ShooterClass, MinerFloor + FVector(OpenShooterOffset, 0.f, 0.f));
	if (!Shooter)
	{
		return false;
	}
	TestTrue(TEXT("BP_Shooter fires BP_ShooterProjectile"), FShooterAccess::ProjectileClassOf(Shooter) == ShotClass);
	UStaticMeshComponent* Laser = FShooterAccess::LaserOf(Shooter);
	if (!TestTrue(TEXT("BP_Shooter laser has a mesh"), Laser && Laser->GetStaticMesh() != nullptr))
	{
		return false;
	}
	TestFalse(TEXT("laser hidden at rest"), Laser->IsVisible());

	// No view override: the shooter reads the real BP_Miner camera rectangle.
	TArray<FHit> Hits;
	const FDelegateHandle HitHandle = RecordHits(Miner, Hits);
	const float HealthBefore = Health(Miner);
	TArray<double> AimStarts;
	TArray<double> ShotTimes;
	bool bLaserShownWhileAiming = true;
	bool bLaserHiddenOtherwise = true;
	bool bStoppedWhileAiming = true;
	bool bAllShotsOfClass = true;
	int32 MaxShots = 0;
	float MinRing = TNumericLimits<float>::Max();
	float MaxRing = 0.f;
	double RingEntryTime = -1.0;
	for (int32 Frame = 0; Frame < 60 * 9 && Shooter->GetShotCount() < 3; ++Frame)
	{
		const ERRShooterState Previous = Shooter->GetState();
		const int32 ShotsBefore = Shooter->GetShotCount();
		TestWorld.Tick();
		if (Previous != ERRShooterState::Aim && Shooter->IsAiming())
		{
			AimStarts.Add(TestWorld.Get()->GetTimeSeconds());
		}
		if (Shooter->GetShotCount() > ShotsBefore)
		{
			ShotTimes.Add(Shooter->GetLastShotTime());
		}
		if (Shooter->IsAiming())
		{
			bLaserShownWhileAiming &= Laser->IsVisible();
			bStoppedWhileAiming &= Shooter->GetVelocity().Size2D() < 1.f;
		}
		else
		{
			bLaserHiddenOtherwise &= !Laser->IsVisible();
		}
		// It aims as soon as the miner is in range (960 uu), then closes to the ring: from its first frame in the ring it must stay there.
		const float Distance = FVector::Dist2D(Shooter->GetActorLocation(), Miner->GetActorLocation());
		if (RingEntryTime < 0.0 && Distance <= RingMax)
		{
			RingEntryTime = TestWorld.Get()->GetTimeSeconds();
		}
		if (RingEntryTime >= 0.0)
		{
			MinRing = FMath::Min(MinRing, Distance);
			MaxRing = FMath::Max(MaxRing, Distance);
		}
		MaxShots = FMath::Max(MaxShots, CountEnemyShots(TestWorld.Get(), ShotClass, bAllShotsOfClass));
	}
	// Let the third shot land.
	for (int32 Frame = 0; Frame < 60 && Hits.Num() < ShotTimes.Num(); ++Frame)
	{
		TestWorld.Tick();
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(HitHandle);

	FString Times;
	for (int32 Index = 0; Index < ShotTimes.Num(); ++Index)
	{
		Times += FString::Printf(TEXT(" aim %.3f / shot %.3f / hit %.3f;"), AimStarts.IsValidIndex(Index) ? AimStarts[Index] : -1.0,
			ShotTimes[Index], Hits.IsValidIndex(Index) ? Hits[Index].Time : -1.0);
	}
	AddInfo(FString::Printf(TEXT("BP_Shooter from 1600 uu:%s ring entered at %.3f s, then %.0f to %.0f uu; final %.0f uu; miner %.0f -> %.0f, %d hits"),
		*Times, RingEntryTime, MinRing, MaxRing, FVector::Dist2D(Shooter->GetActorLocation(), Miner->GetActorLocation()), HealthBefore, Health(Miner), Hits.Num()));

	if (!TestEqual(TEXT("three shots within 9 s"), ShotTimes.Num(), 3) || !TestTrue(TEXT("one aim per shot"), AimStarts.Num() >= 3))
	{
		return false;
	}
	for (int32 Index = 0; Index < 3; ++Index)
	{
		TestTrue(FString::Printf(TEXT("shot %d fires 0.6 s after its laser starts (within one frame)"), Index + 1),
			FMath::Abs(ShotTimes[Index] - AimStarts[Index] - 0.6) <= FrameSeconds + 1e-3);
	}
	TestTrue(TEXT("shots 2.0 s apart (within one frame)"), FMath::Abs(ShotTimes[1] - ShotTimes[0] - 2.0) <= FrameSeconds + 1e-3
		&& FMath::Abs(ShotTimes[2] - ShotTimes[1] - 2.0) <= FrameSeconds + 1e-3);
	TestTrue(TEXT("laser visible on every aim frame"), bLaserShownWhileAiming);
	TestTrue(TEXT("laser hidden outside the aim"), bLaserHiddenOtherwise);
	TestTrue(TEXT("stopped while aiming"), bStoppedWhileAiming);
	TestTrue(TEXT("live shots were BP_ShooterProjectile"), MaxShots >= 1 && bAllShotsOfClass);
	TestTrue(TEXT("ring reached before the second shot"), RingEntryTime > 0.0 && RingEntryTime < ShotTimes[1]);
	TestTrue(TEXT("distance ring held once reached (600 to 1000 uu)"), MinRing >= RingMin && MaxRing <= RingMax);
	TestEqual(TEXT("every shot hit the miner"), Hits.Num(), 3);
	TestTrue(TEXT("each hit deals 12"), AllHitsAre12(Hits));
	TestEqual(TEXT("miner lost 36"), Health(Miner), HealthBefore - 3.f * ShotDamage);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationShooterRepositionTest, "RogueRigs.Integration.ShooterRepositionsAroundRock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationShooterRepositionTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationShooterReposition"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	UClass* ShooterClass = nullptr;
	if (!SetUp(*this, TestWorld, CoverMinerCell, Grid, Miner, ShooterClass))
	{
		return false;
	}
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, ShooterClass, Grid->GetCellCenter(CoverShooterCell.X, CoverShooterCell.Y));
	if (!Shooter)
	{
		return false;
	}
	Shooter->SetViewRectangleOverride(ViewAround(Miner->GetActorLocation(), 4000.f));
	const float StartDistance = FVector::Dist2D(Shooter->GetActorLocation(), Miner->GetActorLocation());
	TestTrue(TEXT("starts inside the ring"), StartDistance >= RingMin && StartDistance <= RingMax);
	TestFalse(TEXT("rock blocks the sight at spawn"), Grid->HasLineOfSight(Shooter->GetActorLocation(), Miner->GetActorLocation()));

	TArray<FHit> Hits;
	const FDelegateHandle HitHandle = RecordHits(Miner, Hits);
	const float HealthBefore = Health(Miner);
	bool bRepositioned = false;
	bool bAimedWithoutSight = false;
	bool bOnBlockedCell = false;
	double FirstAim = -1.0;
	FVector AimLocation = FVector::ZeroVector;
	for (int32 Frame = 0; Frame < 60 * 10 && Hits.IsEmpty(); ++Frame)
	{
		const ERRShooterState Previous = Shooter->GetState();
		TestWorld.Tick();
		bRepositioned |= Shooter->GetState() == ERRShooterState::Reposition;
		const FIntPoint Cell = Grid->GetCellAtWorld(Shooter->GetActorLocation());
		const ERRCellType Type = Grid->GetCellType(Cell);
		bOnBlockedCell |= Type == ERRCellType::Rock || Type == ERRCellType::Barrel;
		if (Previous != ERRShooterState::Aim && Shooter->IsAiming())
		{
			bAimedWithoutSight |= !Grid->HasLineOfSight(Shooter->GetActorLocation(), Miner->GetActorLocation());
			if (FirstAim < 0.0)
			{
				FirstAim = TestWorld.Get()->GetTimeSeconds();
				AimLocation = Shooter->GetActorLocation();
			}
		}
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(HitHandle);
	const FIntPoint AimCell = Grid->GetCellAtWorld(AimLocation);
	const float AimDistance = FVector::Dist2D(AimLocation, Miner->GetActorLocation());
	AddInfo(FString::Printf(TEXT("BP_Shooter behind rock: first aim at t=%.3f s from (%d, %d), %.0f uu; first hit at %.3f s (%.0f)"),
		FirstAim, AimCell.X, AimCell.Y, AimDistance, Hits.IsEmpty() ? -1.0 : Hits[0].Time, Hits.IsEmpty() ? 0.f : Hits[0].Damage));

	TestTrue(TEXT("entered Reposition"), bRepositioned);
	TestFalse(TEXT("never stood on a rock or barrel cell"), bOnBlockedCell);
	TestFalse(TEXT("never aimed without sight"), bAimedWithoutSight);
	TestTrue(TEXT("aimed from a ring position with sight"), FirstAim > 0.0 && AimDistance >= RingMin && AimDistance <= RingMax
		&& Grid->HasLineOfSight(AimLocation, Miner->GetActorLocation()));
	if (TestEqual(TEXT("its shot reached the miner"), Hits.Num(), 1))
	{
		TestEqual(TEXT("hit deals 12"), Hits[0].Damage, ShotDamage);
		TestEqual(TEXT("miner health -12"), Health(Miner), HealthBefore - ShotDamage);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationShooterSparesRobotsTest, "RogueRigs.Integration.ShooterShotsSpareRobots",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationShooterSparesRobotsTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterIntegrationTests;

	FTestWorld TestWorld(TEXT("RRIntegrationShooterSpares"));
	ARRArenaGrid* Grid = nullptr;
	ARRMiner* Miner = nullptr;
	UClass* ShooterClass = nullptr;
	UClass* ChaserClass = LoadTestClass<ARRChaser>(*this, ChaserClassPath);
	if (!ChaserClass || !SetUp(*this, TestWorld, OpenMinerCell, Grid, Miner, ShooterClass))
	{
		return false;
	}
	// Shooter in the ring (900 uu east, sight along row 43, muzzle 41 uu to its right = world -Y), a frozen BP_Chaser on the muzzle-to-miner line.
	const FVector MinerFloor = Grid->GetCellCenter(OpenMinerCell.X, OpenMinerCell.Y);
	ARRShooter* Shooter = SpawnRobot<ARRShooter>(*this, TestWorld, ShooterClass, MinerFloor + FVector(900.f, 0.f, 0.f));
	ARRChaser* Chaser = SpawnRobot<ARRChaser>(*this, TestWorld, ChaserClass, MinerFloor + FVector(450.f, -22.f, 0.f));
	if (!Shooter || !Chaser)
	{
		return false;
	}
	Chaser->SetActorTickEnabled(false);
	Chaser->GetCharacterMovement()->SetComponentTickEnabled(false);
	Shooter->SetViewRectangleOverride(ViewAround(MinerFloor, 4000.f));

	TArray<FHit> Hits;
	const FDelegateHandle HitHandle = RecordHits(Miner, Hits);
	const float MinerHealth = Health(Miner);
	const float ChaserHealth = Health(Chaser);
	const float ShooterHealth = Health(Shooter);
	float WorstLineOffset = 0.f;
	const float ChaserRadius = Chaser->GetCapsuleComponent()->GetScaledCapsuleRadius();
	for (int32 Frame = 0; Frame < 60 * 6 && Hits.Num() < 2; ++Frame)
	{
		const bool bWasAiming = Shooter->IsAiming();
		const FVector Origin = Shooter->GetAimOrigin();
		const FVector Target = Shooter->GetAimTargetPoint();
		const int32 ShotsBefore = Shooter->GetShotCount();
		TestWorld.Tick();
		if (bWasAiming && Shooter->GetShotCount() > ShotsBefore)
		{
			// The shot leaves the muzzle toward the aim point: it must cross the chaser's capsule (2D) on its way.
			const float Offset = FMath::PointDistToSegment(FVector(Chaser->GetActorLocation().X, Chaser->GetActorLocation().Y, 0.f),
				FVector(Origin.X, Origin.Y, 0.f), FVector(Target.X, Target.Y, 0.f));
			WorstLineOffset = FMath::Max(WorstLineOffset, Offset);
		}
	}
	for (int32 Frame = 0; Frame < 60 && Hits.Num() < Shooter->GetShotCount(); ++Frame)
	{
		TestWorld.Tick();
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(HitHandle);
	AddInfo(FString::Printf(TEXT("%d shots, %d hits on the miner (%.0f -> %.0f); chaser %.0f -> %.0f, %.1f uu off the line of fire (radius %.0f)"),
		Shooter->GetShotCount(), Hits.Num(), MinerHealth, Health(Miner), ChaserHealth, Health(Chaser), WorstLineOffset, ChaserRadius));

	TestTrue(TEXT("at least two shots fired"), Shooter->GetShotCount() >= 2);
	TestTrue(TEXT("the chaser stood on the line of fire"), WorstLineOffset < ChaserRadius);
	TestEqual(TEXT("chaser unharmed"), Health(Chaser), ChaserHealth);
	TestEqual(TEXT("shooter unharmed"), Health(Shooter), ShooterHealth);
	TestEqual(TEXT("every shot passed through to the miner"), Hits.Num(), Shooter->GetShotCount());
	TestTrue(TEXT("each hit deals 12"), AllHitsAre12(Hits));
	TestEqual(TEXT("miner lost 12 per shot"), Health(Miner), MinerHealth - ShotDamage * Shooter->GetShotCount());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRIntegrationPlacedSpawnerMixTest, "RogueRigs.Integration.PlacedSpawnerMix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRIntegrationPlacedSpawnerMixTest::RunTest(const FString& Parameters)
{
	using namespace RRShooterIntegrationTests;

	// Expected entries (task-022, task-028-02): chaser (not kamikaze, not shield) 65, kamikaze 12, shooter 15, heavy 8 with MaxAlive 5,
	// one shield chaser entry of weight 0 (director only); no cap on the others.
	auto CheckMix = [this](const TCHAR* What, const TArray<FRRWeightedEnemyClass>& Entries)
	{
		FString Listing;
		float Chaser = 0.f;
		float Kamikaze = 0.f;
		float Shooter = 0.f;
		float Heavy = 0.f;
		int32 HeavyEntries = 0;
		int32 HeavyCap = -1;
		float Shield = -1.f;
		int32 ShieldEntries = 0;
		int32 OtherCaps = 0;
		int32 Unknown = 0;
		for (const FRRWeightedEnemyClass& Entry : Entries)
		{
			const UClass* Class = Entry.EnemyClass.Get();
			Listing += FString::Printf(TEXT(" %s=%.1f(cap %d)"), Class ? *Class->GetName() : TEXT("None"), Entry.Weight, Entry.MaxAlive);
			if (Class && Class->IsChildOf(ARRHeavy::StaticClass()))
			{
				Heavy += Entry.Weight;
				++HeavyEntries;
				HeavyCap = Entry.MaxAlive;
				continue;
			}
			OtherCaps += Entry.MaxAlive != 0 ? 1 : 0;
			if (Class && Class->IsChildOf(ARRShieldChaser::StaticClass()))
			{
				Shield = FMath::Max(Shield, 0.f) + Entry.Weight;
				++ShieldEntries;
			}
			else if (Class && Class->IsChildOf(ARRKamikaze::StaticClass()))
			{
				Kamikaze += Entry.Weight;
			}
			else if (Class && Class->IsChildOf(ARRShooter::StaticClass()))
			{
				Shooter += Entry.Weight;
			}
			else if (Class && Class->IsChildOf(ARRChaser::StaticClass()))
			{
				Chaser += Entry.Weight;
			}
			else
			{
				++Unknown;
			}
		}
		AddInfo(FString::Printf(TEXT("%s WeightedEnemyClasses:%s"), What, *Listing));
		TestEqual(FString::Printf(TEXT("%s: chaser weight 65"), What), Chaser, 65.f);
		TestEqual(FString::Printf(TEXT("%s: kamikaze weight 12"), What), Kamikaze, 12.f);
		TestEqual(FString::Printf(TEXT("%s: shooter weight 15"), What), Shooter, 15.f);
		TestEqual(FString::Printf(TEXT("%s: heavy weight 8"), What), Heavy, 8.f);
		TestEqual(FString::Printf(TEXT("%s: one heavy entry"), What), HeavyEntries, 1);
		TestEqual(FString::Printf(TEXT("%s: heavy MaxAlive 3"), What), HeavyCap, 3);
		TestEqual(FString::Printf(TEXT("%s: one shield chaser entry"), What), ShieldEntries, 1);
		TestEqual(FString::Printf(TEXT("%s: shield chaser weight 0"), What), Shield, 0.f);
		TestEqual(FString::Printf(TEXT("%s: no cap on the other entries"), What), OtherCaps, 0);
		TestEqual(FString::Printf(TEXT("%s: no other entry"), What), Unknown, 0);
	};

	UClass* SpawnerClass = LoadTestClass<ARREnemySpawner>(*this, SpawnerClassPath);
	if (!SpawnerClass)
	{
		return false;
	}
	CheckMix(TEXT("BP_EnemySpawner defaults"), FSpawnerAccess::WeightsOf(SpawnerClass->GetDefaultObject<ARREnemySpawner>()));

	// The map as saved on disk: the level and its external actor packages (one file per actor).
	UPackage* MapPackage = LoadPackage(nullptr, ArenaMapPackage, LOAD_None);
	UWorld* MapWorld = MapPackage ? UWorld::FindWorldInPackage(MapPackage) : nullptr;
	if (!TestNotNull(TEXT("L_ArenaM1 loads"), MapWorld) || !TestNotNull(TEXT("L_ArenaM1 level"), MapWorld->PersistentLevel.Get()))
	{
		return false;
	}
	ULevel* Level = MapWorld->PersistentLevel;
	TArray<ARREnemySpawner*> Spawners;
	for (AActor* Actor : Level->Actors)
	{
		if (ARREnemySpawner* Spawner = Cast<ARREnemySpawner>(Actor))
		{
			Spawners.AddUnique(Spawner);
		}
	}
	if (Spawners.IsEmpty() && Level->IsUsingExternalActors())
	{
		for (const FString& ActorPackageName : Level->GetOnDiskExternalActorPackages(/*bTryUsingPackageLoadedPath*/ true))
		{
			if (UPackage* ActorPackage = LoadPackage(nullptr, *ActorPackageName, LOAD_None))
			{
				ForEachObjectWithPackage(ActorPackage, [&Spawners](UObject* Object)
				{
					if (ARREnemySpawner* Spawner = Cast<ARREnemySpawner>(Object); Spawner && !Spawner->IsTemplate())
					{
						Spawners.AddUnique(Spawner);
					}
					return true;
				});
			}
		}
	}
	if (!TestEqual(TEXT("exactly one spawner placed in L_ArenaM1"), Spawners.Num(), 1))
	{
		return false;
	}
	ARREnemySpawner* Placed = Spawners[0];
	AddInfo(FString::Printf(TEXT("placed spawner %s (class %s, package %s)"), *Placed->GetName(), *Placed->GetClass()->GetName(), *Placed->GetPackage()->GetName()));
	TestTrue(TEXT("placed spawner is a BP_EnemySpawner"), Placed->GetClass()->IsChildOf(SpawnerClass));
	CheckMix(TEXT("placed spawner in L_ArenaM1"), FSpawnerAccess::WeightsOf(Placed));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
