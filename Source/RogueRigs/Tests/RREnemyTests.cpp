// Rogue Rigs - automation tests of the robots (RogueRigs.Enemies.*): CSV stats and their parse cache, chaser death and XP drop, attack timing, i-frames against two chasers, damage number per hit, robots moving through robots but not through the miner; shots hit a walking BP_Chaser with the passive hit capsule and its parts shrink at death.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Damage/RRGE_DamageFlat.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RRHitCapsuleComponent.h"
#include "Enemies/RRRobotMovementComponent.h"
#include "EngineUtils.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RREnemyAttributeSet.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "Projectiles/RRCollisionChannels.h"
#include "Projectiles/RRProjectile.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Tests/RRTestPickupXP.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponAttributeSet.h"
#include "World/RRBlobShadowComponent.h"

namespace RREnemyTests
{
	using namespace RRTestHarness;

	/** Chaser Blueprint (the robot of the arena). */
	const TCHAR* const ChaserClassPath = TEXT("/Game/RogueRigs/Blueprints/Enemies/BP_Chaser.BP_Chaser_C");

	/** Burst rifle Blueprint (the miner's starting weapon). */
	const TCHAR* const BurstRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");

	/** Open floor row of arena_m1 (as the weapon still-chaser tests): the chaser stands on TargetCell, the muzzle two cells west. */
	const FIntPoint TargetCell(44, 41);
	const FIntPoint ShooterCell(42, 41);

	/** Spawn cell of the miner in the attack tests (open floor, flow field tests). */
	const FIntPoint MinerCell(40, 40);

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** One hit received by the miner. */
	struct FHit
	{
		double Time = 0.0;
		float Damage = 0.f;
	};

	/** Spawns a native ARRChaser with its feet on the floor at FloorLocation (XP drop: ARRTestPickupXP). */
	ARRChaser* SpawnChaser(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FVector& FloorLocation)
	{
		const float HalfHeight = GetDefault<ARRChaser>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const FTransform SpawnTransform(FloorLocation + FVector(0.f, 0.f, HalfHeight + 2.f));
		ARRChaser* Chaser = TestWorld.Get()->SpawnActorDeferred<ARRChaser>(ARRChaser::StaticClass(), SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Test.TestNotNull(TEXT("chaser spawned"), Chaser))
		{
			FEnemyAccess::XPPickupClassOf(Chaser) = ARRTestPickupXP::StaticClass();
			Chaser->FinishSpawning(SpawnTransform);
		}
		return Chaser;
	}

	/** Current value of an attribute on Actor's ASC. */
	float Attribute(const IAbilitySystemInterface* Actor, const FGameplayAttribute& Attr)
	{
		return Actor->GetAbilitySystemComponent()->GetNumericAttribute(Attr);
	}

	/** Live XP pickups in the world. */
	TArray<ARRPickup*> GetXPPickups(UWorld* World)
	{
		TArray<ARRPickup*> Result;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			// task-092-01: idle pool pickups are not live pickups.
			if (URRActorPoolSubsystem::IsPooledIdle(*It))
			{
				continue;
			}
			if (IsValid(*It) && It->GetPickupType() == ERRPickupType::XP)
			{
				Result.Add(*It);
			}
		}
		return Result;
	}

	/** Fires exactly one shot from Weapon now (auto-fire re-configured, advanced 1 ms, stopped). Returns the new projectile. */
	ARRProjectile* FireSingleShot(FAutomationTestBase& Test, ARRWeapon* Weapon)
	{
		TSet<ARRProjectile*> Before;
		for (TActorIterator<ARRProjectile> It(Weapon->GetWorld()); It; ++It)
		{
			Before.Add(*It);
		}
		URRAutoFireComponent* AutoFire = Weapon->GetAutoFireComponent();
		AutoFire->Configure(Weapon->GetAbilitySystemComponent(), Weapon->GetBurstCount(), Weapon->GetBurstDuty(), Weapon->GetReloadTime());
		AutoFire->StartFiring();
		AutoFire->AdvanceTime(0.001f);
		AutoFire->StopFiring();
		for (TActorIterator<ARRProjectile> It(Weapon->GetWorld()); It; ++It)
		{
			if (IsValid(*It) && !Before.Contains(*It))
			{
				return *It;
			}
		}
		Test.AddError(TEXT("the shot spawned no projectile"));
		return nullptr;
	}

	/** Ticks until Projectile is gone (max MaxFrames). Returns the frame, INDEX_NONE if still alive. */
	int32 TickUntilDestroyed(const FTestWorld& TestWorld, ARRProjectile* Projectile, int32 MaxFrames)
	{
		TWeakObjectPtr<ARRProjectile> Weak(Projectile);
		for (int32 Frame = 1; Frame <= MaxFrames; ++Frame)
		{
			TestWorld.Tick();
			if (!Weak.IsValid() || Weak->IsActorBeingDestroyed())
			{
				return Frame;
			}
		}
		return INDEX_NONE;
	}

	/** Spawns BP_Miner on MinerCell and stops its starting weapons (they would shoot the chasers in front of it). */
	ARRMiner* SpawnUnarmedMiner(FAutomationTestBase& Test, const FTestWorld& TestWorld, const ARRArenaGrid* Grid)
	{
		ARRMiner* Miner = SpawnMiner(Test, TestWorld, Grid, MinerCell);
		if (Miner)
		{
			for (ARRWeapon* Weapon : Miner->GetWeapons())
			{
				if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
				{
					Weapon->GetAutoFireComponent()->StopFiring();
				}
			}
		}
		return Miner;
	}

	/** Records every hit of the miner (time, damage) on its health set. Remove the returned handle when done. */
	FDelegateHandle RecordHits(ARRMiner* Miner, TArray<FHit>& Hits)
	{
		UWorld* World = Miner->GetWorld();
		return Miner->GetHealthSet()->OnDamageReceived.AddLambda([&Hits, World](float Damage, bool, float)
		{
			Hits.Add({ World->GetTimeSeconds(), Damage });
		});
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyStatsTest, "RogueRigs.Enemies.StatsFromCSV",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyStatsTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	FRREnemyStatsRow Row;
	if (!TestTrue(TEXT("EnemyStats.csv row Chaser read"), ARREnemy::LoadStatsRow(TEXT("Chaser"), Row)))
	{
		return false;
	}
	TestEqual(TEXT("MaxHealth 30"), Row.MaxHealth, 30.f);
	TestEqual(TEXT("MoveSpeed 180"), Row.MoveSpeed, 180.f);
	TestEqual(TEXT("Damage 10"), Row.Damage, 10.f);
	TestEqual(TEXT("AttackRate 1"), Row.AttackRate, 1.f);
	TestEqual(TEXT("AttackRange 120"), Row.AttackRange, 120.f);
	TestEqual(TEXT("WindUp 0.1"), Row.WindUp, 0.1f);
	TestEqual(TEXT("XPValue 1"), Row.XPValue, 1);
	TestEqual(TEXT("KnockbackResistance 0"), Row.KnockbackResistance, 0.f);

	FTestWorld TestWorld(TEXT("RREnemyStats"));
	ARRChaser* Chaser = SpawnChaser(*this, TestWorld, FVector::ZeroVector);
	if (!Chaser)
	{
		return false;
	}
	TestEqual(TEXT("chaser Stats.Damage from the CSV"), Chaser->GetStats().Damage, 10.f);
	TestEqual(TEXT("chaser Stats.XPValue from the CSV"), Chaser->GetStats().XPValue, 1);
	TestEqual(TEXT("MaxHealth attribute 30"), Attribute(Chaser, URRHealthAttributeSet::GetMaxHealthAttribute()), 30.f);
	TestEqual(TEXT("Health attribute 30"), Attribute(Chaser, URRHealthAttributeSet::GetHealthAttribute()), 30.f);
	TestEqual(TEXT("MoveSpeed attribute 180"), Attribute(Chaser, URRMovementAttributeSet::GetMoveSpeedAttribute()), 180.f);
	TestEqual(TEXT("AttackPower starts at 1"), Attribute(Chaser, URREnemyAttributeSet::GetAttackPowerAttribute()), 1.f);
	TestEqual(TEXT("MaxWalkSpeed mirrors MoveSpeed"), Chaser->GetCharacterMovement()->MaxWalkSpeed, 180.f);
	TestFalse(TEXT("RVO avoidance off (light separation, task-034-01)"), Chaser->GetCharacterMovement()->bUseRVOAvoidance);
	const float Arrival = Chaser->GetEnemyMovement()->GetArrivalDistance();
	AddInfo(FString::Printf(TEXT("arrival distance %.1f uu"), Arrival));
	TestTrue(TEXT("arrival distance just under the attack range"), Arrival > 0.f && Arrival < 120.f);
	TestEqual(TEXT("hit capsule overlaps Projectile"), Chaser->GetHitCapsule()->GetCollisionResponseToChannel(RRCollisionChannels::Projectile), ECR_Overlap);
	TestEqual(TEXT("root capsule ignores Projectile"), Chaser->GetCapsuleComponent()->GetCollisionResponseToChannel(RRCollisionChannels::Projectile), ECR_Ignore);
	TestEqual(TEXT("starts in Chase"), Chaser->GetState(), ERRChaserState::Chase);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyStatsCacheTest, "RogueRigs.Enemies.StatsTableCached",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyStatsCacheTest::RunTest(const FString& Parameters)
{
	// EnemyStats.csv is parsed once and shared: a second lookup returns the same table, rows stay readable,
	// and a missing row fails with one logged error.
	const UDataTable* First = FRRBaseStatsLoader::GetCachedTable(ARREnemy::EnemyStatsPath, FRREnemyStatsRow::StaticStruct());
	const UDataTable* Second = FRRBaseStatsLoader::GetCachedTable(ARREnemy::EnemyStatsPath, FRREnemyStatsRow::StaticStruct());
	if (!TestNotNull(TEXT("EnemyStats.csv cached"), First))
	{
		return false;
	}
	TestTrue(TEXT("second lookup returns the cached table"), First == Second);

	FRREnemyStatsRow Row;
	TestTrue(TEXT("Kamikaze row read through the cache"), ARREnemy::LoadStatsRow(TEXT("Kamikaze"), Row));
	AddExpectedError(TEXT("row 'NoSuchRobot' not found"), EAutomationExpectedErrorFlags::Contains, 1);
	TestFalse(TEXT("missing row fails"), ARREnemy::LoadStatsRow(TEXT("NoSuchRobot"), Row));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyDeathTest, "RogueRigs.Enemies.ChaserDiesAndDropsXP",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyDeathTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	FTestWorld TestWorld(TEXT("RREnemyDeath"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	UClass* RifleClass = Grid ? LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath) : nullptr;
	ARRWeapon* Rifle = RifleClass ? TestWorld.Spawn<ARRWeapon>(RifleClass, FVector::ZeroVector) : nullptr;
	if (!Rifle || !TestTrue(TEXT("rifle acquired"), Rifle->Acquire(nullptr)))
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	// Pierce 1: each shot must end on the chaser so the death-to-destroy frames are counted from the killing hit.
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), 1.f);

	ARRChaser* Chaser = SpawnChaser(*this, TestWorld, Grid->GetCellCenter(TargetCell.X, TargetCell.Y));
	if (!Chaser)
	{
		return false;
	}
	TWeakObjectPtr<ARRChaser> WeakChaser(Chaser);
	for (int32 Frame = 0; Frame < 10; ++Frame)
	{
		TestWorld.Tick();
	}

	// Muzzle two cells west at half the block height, aiming +X (as the weapon still-chaser tests).
	const FVector MuzzleTarget = Grid->GetCellCenter(ShooterCell.X, ShooterCell.Y) + FVector(0.f, 0.f, 0.5f * Grid->GetBlockHeight());
	const FVector MuzzleOffset = FWeaponAccess::MuzzleOf(Rifle)->GetComponentLocation() - Rifle->GetActorLocation();
	Rifle->SetActorLocation(MuzzleTarget - MuzzleOffset, false, nullptr, ETeleportType::TeleportPhysics);
	const FBox ChaserBounds = Chaser->GetCapsuleComponent()->Bounds.GetBox();
	TestTrue(TEXT("chaser capsule covers the shot height"), ChaserBounds.Min.Z < MuzzleTarget.Z && ChaserBounds.Max.Z > MuzzleTarget.Z);

	const float ExpectedHealth[] = { 20.f, 10.f, 0.f };
	FVector DeathLocation = Chaser->GetActorLocation();
	for (int32 Shot = 0; Shot < 3; ++Shot)
	{
		const int32 Frame = TickUntilDestroyed(TestWorld, FireSingleShot(*this, Rifle), 30);
		TestTrue(FString::Printf(TEXT("shot %d stopped"), Shot + 1), Frame != INDEX_NONE);
		TestEqual(FString::Printf(TEXT("Health after shot %d"), Shot + 1), Attribute(Chaser, URRHealthAttributeSet::GetHealthAttribute()), ExpectedHealth[Shot]);
		AddInfo(FString::Printf(TEXT("shot %d hit at frame %d, health %.1f"), Shot + 1, Frame, Attribute(Chaser, URRHealthAttributeSet::GetHealthAttribute())));
		if (Shot < 2)
		{
			TestFalse(FString::Printf(TEXT("alive after shot %d"), Shot + 1), Chaser->IsDead());
		}
	}
	TestTrue(TEXT("dead after 3 hits"), Chaser->IsDead());
	TestEqual(TEXT("state Dead"), Chaser->GetState(), ERRChaserState::Dead);
	TestFalse(TEXT("collision disabled"), Chaser->GetActorEnableCollision());
	TestEqual(TEXT("no XP before the shrink ends"), GetXPPickups(TestWorld.Get()).Num(), 0);

	if (URRPickupSubsystem* PickupSystem = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>())
	{
		PickupSystem->SetSpawnMaxFrames(0); // task-092-01: drops delivered at once (these checks read them in the same frame); spread covered by RogueRigs.PickupPool.
	}
	// Shrink 0.15 s (9 frames), then drop and destroy.
	int32 DestroyFrame = INDEX_NONE;
	float MinScale = 1.f;
	for (int32 Frame = 1; Frame <= 20 && DestroyFrame == INDEX_NONE; ++Frame)
	{
		TestWorld.Tick();
		if (!WeakChaser.IsValid() || WeakChaser->IsActorBeingDestroyed())
		{
			DestroyFrame = Frame;
		}
		else
		{
			MinScale = FMath::Min(MinScale, static_cast<float>(WeakChaser->GetActorScale3D().X));
		}
	}
	AddInfo(FString::Printf(TEXT("destroyed %d frames after death, smallest scale seen %.3f"), DestroyFrame, MinScale));
	TestTrue(TEXT("destroyed about 0.15 s after death"), DestroyFrame >= 8 && DestroyFrame <= 11);
	TestTrue(TEXT("shrank before destruction"), MinScale < 0.2f);
	const TArray<ARRPickup*> Pickups = GetXPPickups(TestWorld.Get());
	TestEqual(TEXT("1 XP in 3 cubes dropped"), Pickups.Num(), 3);
	float CubeXP = 0.f;
	for (const ARRPickup* Pickup : Pickups)
	{
		// Ejected 80-220 uu from the death point (task-027-01).
		TestTrue(TEXT("XP cube ejected from the death point"), FVector::Dist2D(Pickup->GetPopStart(), DeathLocation) < 1.f && FVector::Dist2D(Pickup->GetActorLocation(), DeathLocation) <= 221.f);
		CubeXP += Pickup->GetXPAmount();
	}
	TestEqual(TEXT("cubes add up to 1 XP"), CubeXP, 1.f, 1e-4f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyAttackTest, "RogueRigs.Enemies.ChaserAttackTiming",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyAttackTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	FTestWorld TestWorld(TEXT("RREnemyAttack"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnUnarmedMiner(*this, TestWorld, Grid) : nullptr;
	if (!Miner)
	{
		return false;
	}
	TestEqual(TEXT("miner Health 100"), Attribute(Miner, URRHealthAttributeSet::GetHealthAttribute()), 100.f);
	TArray<FHit> Hits;
	const FDelegateHandle Handle = RecordHits(Miner, Hits);

	const FVector MinerFloor = Grid->GetCellCenter(MinerCell.X, MinerCell.Y);
	ARRChaser* Chaser = SpawnChaser(*this, TestWorld, MinerFloor + FVector(90.f, 0.f, 0.f));
	if (!Chaser)
	{
		return false;
	}

	double WindUpStart = -1.0;
	ERRChaserState Previous = Chaser->GetState();
	for (int32 Frame = 0; Frame < 210; ++Frame)
	{
		TestWorld.Tick();
		if (Chaser->GetState() == ERRChaserState::WindUp && Previous != ERRChaserState::WindUp && WindUpStart < 0.0)
		{
			WindUpStart = TestWorld.Get()->GetTimeSeconds();
		}
		Previous = Chaser->GetState();
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(Handle);

	AddInfo(FString::Printf(TEXT("miner capsule radius %.1f; chaser at %.1f uu, state %s, %d attacks, %d hits"),
		Miner->GetCapsuleComponent()->GetScaledCapsuleRadius(), FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation()),
		*UEnum::GetValueAsString(Chaser->GetState()), Chaser->GetAttackCount(), Hits.Num()));
	TestTrue(TEXT("wind-up started"), WindUpStart >= 0.0);
	if (!TestTrue(TEXT("at least 2 hits in 3.5 s"), Hits.Num() >= 2))
	{
		return false;
	}
	FString Times;
	for (const FHit& Hit : Hits)
	{
		Times += FString::Printf(TEXT(" %.3f"), Hit.Time - WindUpStart);
		TestEqual(TEXT("each hit deals 10"), Hit.Damage, 10.f);
	}
	AddInfo(FString::Printf(TEXT("wind-up at %.3f s; hits (s after wind-up):%s; %d attacks"), WindUpStart, *Times, Chaser->GetAttackCount()));
	TestTrue(TEXT("first hit 0.1 s after the wind-up starts (±1 frame)"), FMath::Abs(Hits[0].Time - WindUpStart - 0.1) <= FrameSeconds + 1e-3);
	for (int32 Index = 1; Index < Hits.Num(); ++Index)
	{
		TestTrue(FString::Printf(TEXT("hit %d at least 1 s after the previous"), Index + 1), Hits[Index].Time - Hits[Index - 1].Time >= 1.0 - 1e-3);
	}
	TestEqual(TEXT("Health = 100 - 10 per hit"), Attribute(Miner, URRHealthAttributeSet::GetHealthAttribute()), 100.f - 10.f * Hits.Num());
	TestTrue(TEXT("chaser stayed in range"), FVector::Dist2D(Chaser->GetActorLocation(), Miner->GetActorLocation()) <= 120.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyIFramesTest, "RogueRigs.Enemies.IFramesRefuseSecondChaser",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyIFramesTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	FTestWorld TestWorld(TEXT("RREnemyIFrames"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnUnarmedMiner(*this, TestWorld, Grid) : nullptr;
	if (!Miner)
	{
		return false;
	}
	TArray<FHit> Hits;
	const FDelegateHandle Handle = RecordHits(Miner, Hits);
	const FVector MinerFloor = Grid->GetCellCenter(MinerCell.X, MinerCell.Y);

	// Second chaser 6 frames (0.1 s) after the first: its attack lands inside the first hit's 0.25 s i-frames.
	ARRChaser* First = SpawnChaser(*this, TestWorld, MinerFloor + FVector(90.f, 0.f, 0.f));
	for (int32 Frame = 0; Frame < 6; ++Frame)
	{
		TestWorld.Tick();
	}
	ARRChaser* Second = SpawnChaser(*this, TestWorld, MinerFloor + FVector(60.f, 60.f, 0.f));
	if (!First || !Second)
	{
		return false;
	}

	bool bInvulnerableAtSecondAttack = false;
	double FirstAttackTime = -1.0;
	double SecondAttackTime = -1.0;
	for (int32 Frame = 0; Frame < 40 && SecondAttackTime < 0.0; ++Frame)
	{
		TestWorld.Tick();
		if (FirstAttackTime < 0.0 && First->GetAttackCount() > 0)
		{
			FirstAttackTime = First->GetLastAttackTime();
		}
		if (Second->GetAttackCount() > 0)
		{
			SecondAttackTime = Second->GetLastAttackTime();
			bInvulnerableAtSecondAttack = Miner->GetAbilitySystemComponent()->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable);
		}
	}
	Miner->GetHealthSet()->OnDamageReceived.Remove(Handle);

	AddInfo(FString::Printf(TEXT("first attack %.3f s, second attack %.3f s (gap %.3f s), hits %d"),
		FirstAttackTime, SecondAttackTime, SecondAttackTime - FirstAttackTime, Hits.Num()));
	TestTrue(TEXT("both chasers attacked"), FirstAttackTime >= 0.0 && SecondAttackTime >= 0.0);
	TestTrue(TEXT("second attack within the 0.25 s i-frames"), SecondAttackTime - FirstAttackTime < Miner->GetIFramesDuration());
	TestTrue(TEXT("miner invulnerable at the second attack"), bInvulnerableAtSecondAttack);
	TestEqual(TEXT("only one hit applied"), Hits.Num(), 1);
	TestEqual(TEXT("Health 90"), Attribute(Miner, URRHealthAttributeSet::GetHealthAttribute()), 90.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyDamageNumberTest, "RogueRigs.Enemies.DamageNumberSpawned",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyDamageNumberTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	FTestWorld TestWorld(TEXT("RREnemyDamageNumber"));
	UClass* ChaserClass = LoadTestClass<ARRChaser>(*this, ChaserClassPath);
	if (!ChaserClass)
	{
		return false;
	}
	ARRChaser* Target = TestWorld.Spawn<ARRChaser>(ChaserClass, FVector(0.f, 0.f, 200.f));
	ARRChaser* Source = TestWorld.Spawn<ARRChaser>(ChaserClass, FVector(1000.f, 0.f, 200.f));
	if (!TestNotNull(TEXT("target chaser spawned"), Target) || !TestNotNull(TEXT("source chaser spawned"), Source))
	{
		return false;
	}

	// One flat hit (no crit), below the chaser's 30 health.
	UAbilitySystemComponent* SourceASC = Source->GetAbilitySystemComponent();
	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(URRGE_DamageFlat::StaticClass(), 1.f, SourceASC->MakeEffectContext());
	Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, 5.f);
	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data, Target->GetAbilitySystemComponent());
	TestEqual(TEXT("target Health 25"), Attribute(Target, URRHealthAttributeSet::GetHealthAttribute()), 25.f);

	// The hit goes to the damage number subsystem (no actor spawned); its window closes 0.25 s later.
	URRDamageNumberSubsystem* Numbers = TestWorld.Get()->GetSubsystem<URRDamageNumberSubsystem>();
	if (!TestNotNull(TEXT("damage number subsystem"), Numbers))
	{
		return false;
	}
	TestEqual(TEXT("one pending number"), Numbers->GetPendingCount(), 1);
	// Read before ticking: without a grid the robot falls while the window runs; the number keeps the hit location.
	const float CapsuleTop = Target->GetActorLocation().Z + Target->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector HitLocation = Target->GetActorLocation();
	TArray<FRRDamageNumber> Pushed;
	const FDelegateHandle Handle = Numbers->OnNumbersPushed.AddLambda([&Pushed](const TArray<FRRDamageNumber>& Batch) { Pushed.Append(Batch); });
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	Numbers->OnNumbersPushed.Remove(Handle);
	if (!TestEqual(TEXT("exactly one damage number"), Pushed.Num(), 1))
	{
		return false;
	}
	const FVector TextLocation = Pushed[0].Location;
	AddInfo(FString::Printf(TEXT("capsule top %.1f, number at %s"), CapsuleTop, *TextLocation.ToString()));
	TestEqual(TEXT("value 5"), Pushed[0].Value, 5.f);
	TestFalse(TEXT("not critical"), Pushed[0].bCritical);
	TestTrue(TEXT("damage number above the capsule top"), TextLocation.Z > CapsuleTop);
	TestTrue(TEXT("damage number over the target (within the jitter)"), FVector::Dist2D(TextLocation, HitLocation) <= 50.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyCrowdOverlapTest, "RogueRigs.Enemies.RobotsOverlapRobots",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyCrowdOverlapTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	FTestWorld TestWorld(TEXT("RREnemyCrowdOverlap"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnUnarmedMiner(*this, TestWorld, Grid) : nullptr;
	if (!Miner)
	{
		return false;
	}
	const FVector Floor = Grid->GetCellCenter(TargetCell.X, TargetCell.Y);
	ARRChaser* Still = SpawnChaser(*this, TestWorld, Floor);
	ARRChaser* Mover = SpawnChaser(*this, TestWorld, Floor - FVector(100.f, 0.f, 0.f));
	if (!Still || !Mover)
	{
		return false;
	}
	TestTrue(TEXT("robots ignore robots by default"), Still->IsIgnoringOtherRobots() && Mover->IsIgnoringOtherRobots());

	// Swept move onto the other robot: no blocking hit, the capsules overlap.
	const FVector Target = Still->GetActorLocation() - FVector(10.f, 0.f, 0.f);
	FHitResult Hit;
	Mover->SetActorLocation(Target, /*bSweep*/ true, &Hit);
	TestFalse(TEXT("robot sweep through a robot: no blocking hit"), Hit.bBlockingHit);
	TestTrue(TEXT("robot reached 10 uu from the other robot"), FVector::Dist2D(Mover->GetActorLocation(), Target) <= 0.1f);

	// Crowd collision back on: the same move is blocked at contact (2 x radius).
	const FVector MoverStart = Target - FVector(90.f, 0.f, 0.f);
	Mover->SetActorLocation(MoverStart, /*bSweep*/ false);
	Mover->SetIgnoreOtherRobots(false);
	Still->SetIgnoreOtherRobots(false);
	Mover->SetActorLocation(Target, /*bSweep*/ true, &Hit);
	const float Contact = 2.f * Mover->GetCapsuleComponent()->GetScaledCapsuleRadius();
	TestTrue(TEXT("collision on: blocked by the other robot"), Hit.bBlockingHit && Hit.GetActor() == Still);
	AddInfo(FString::Printf(TEXT("collision on: stopped at %.2f uu (contact %.1f)"), FVector::Dist2D(Mover->GetActorLocation(), Still->GetActorLocation()), Contact));
	TestTrue(TEXT("collision on: stopped near contact"), FVector::Dist2D(Mover->GetActorLocation(), Still->GetActorLocation()) >= Contact - 1.f);
	Mover->SetIgnoreOtherRobots(true);
	Still->SetIgnoreOtherRobots(true);

	// The miner still blocks robots, and robots still block the miner.
	const FVector MinerLocation = Miner->GetActorLocation();
	const float MinerContact = Miner->GetCapsuleComponent()->GetScaledCapsuleRadius() + Mover->GetCapsuleComponent()->GetScaledCapsuleRadius();
	Mover->SetActorLocation(FVector(MinerLocation.X + 150.f, MinerLocation.Y, Mover->GetActorLocation().Z), /*bSweep*/ false);
	Mover->SetActorLocation(FVector(MinerLocation.X + 10.f, MinerLocation.Y, Mover->GetActorLocation().Z), /*bSweep*/ true, &Hit);
	TestTrue(TEXT("robot sweep into the miner: blocked by the miner"), Hit.bBlockingHit && Hit.GetActor() == Miner);
	TestTrue(TEXT("robot stopped at the miner contact"), FVector::Dist2D(Mover->GetActorLocation(), MinerLocation) >= MinerContact - 1.f);
	const FVector RobotLocation = Mover->GetActorLocation();
	Miner->SetActorLocation(FVector(RobotLocation.X, RobotLocation.Y, MinerLocation.Z), /*bSweep*/ true, &Hit);
	TestTrue(TEXT("miner sweep into a robot: blocked by the robot"), Hit.bBlockingHit && Hit.GetActor() == Mover);
	TestTrue(TEXT("miner stopped at the robot contact"), FVector::Dist2D(Miner->GetActorLocation(), RobotLocation) >= MinerContact - 1.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyLightMovementTest, "RogueRigs.Enemies.LightMovement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyLightMovementTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	FTestWorld TestWorld(TEXT("RREnemyLightMovement"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnUnarmedMiner(*this, TestWorld, Grid) : nullptr;
	ARRChaser* Chaser = Miner ? SpawnChaser(*this, TestWorld, Grid->GetCellCenter(TargetCell.X, TargetCell.Y)) : nullptr;
	if (!Chaser)
	{
		return false;
	}
	URRRobotMovementComponent* Movement = Cast<URRRobotMovementComponent>(Chaser->GetCharacterMovement());
	if (!TestNotNull(TEXT("robot CharacterMovement is URRRobotMovementComponent"), Movement))
	{
		return false;
	}
	USkeletalMeshComponent* CharacterMesh = Chaser->GetMesh();
	if (CharacterMesh && !CharacterMesh->GetSkinnedAsset() && CharacterMesh->GetAttachChildren().Num() == 0)
	{
		TestFalse(TEXT("empty character mesh does not tick"), CharacterMesh->IsComponentTickEnabled());
		TestTrue(TEXT("empty character mesh is absolute"), CharacterMesh->IsUsingAbsoluteLocation() && CharacterMesh->IsUsingAbsoluteRotation() && CharacterMesh->IsUsingAbsoluteScale());
	}

	// Walk toward the miner: floor results come from the cache, the robot stays on the floor at the same height.
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	const FVector Settled = Chaser->GetActorLocation();
	for (int32 Frame = 0; Frame < 60; ++Frame)
	{
		TestWorld.Tick();
	}
	const FVector Walked = Chaser->GetActorLocation();
	AddInfo(FString::Printf(TEXT("walked %.1f uu, height change %.3f uu, cached floor hits %d, light ticks %d, engine ticks %d"), FVector::Dist2D(Settled, Walked),
		Walked.Z - Settled.Z, Movement->GetCachedFloorHits(), Movement->GetLightMoveCount(), Movement->GetEngineMoveCount()));
	TestTrue(TEXT("robot walked"), FVector::Dist2D(Settled, Walked) > 10.f);
	TestTrue(TEXT("robot still walking"), Movement->IsMovingOnGround());
	TestTrue(TEXT("robot kept its height on the flat floor"), FMath::Abs(Walked.Z - Settled.Z) < 1.f);
	// Light step (rr.Enemy.LightMovement on): no floor search at all; engine path: floor checks answered from the cache.
	if (URRRobotMovementComponent::IsLightMovementEnabled())
	{
		TestTrue(TEXT("walked with the light step"), Movement->GetLightMoveCount() > Movement->GetEngineMoveCount());
	}
	else
	{
		TestTrue(TEXT("floor checks answered from the cache"), Movement->GetCachedFloorHits() > 0);
	}

	// Far movement: slower movement tick, the robot still moves and a knockback still launches it.
	TestFalse(TEXT("no far movement without a game viewport"), Chaser->IsFarMovementActive());
	Chaser->SetFarMovement(true);
	TestTrue(TEXT("far movement: CharacterMovement tick interval raised"), Movement->GetComponentTickInterval() > 0.f);
	Chaser->SetActorLocation(Grid->GetCellCenter(TargetCell.X, TargetCell.Y) + FVector(0.f, 0.f, Settled.Z - Grid->GetCellCenter(TargetCell.X, TargetCell.Y).Z));
	// Pushed away from the miner (as the shotgun does): the first slow movement tick (0.1 s) moves it away.
	const FVector MinerLocation = Miner->GetActorLocation();
	const float StartDistance = FVector::Dist2D(Chaser->GetActorLocation(), MinerLocation);
	Chaser->ApplyKnockback((Chaser->GetActorLocation() - MinerLocation).GetSafeNormal2D() * 1200.f);
	for (int32 Frame = 0; Frame < 8; ++Frame)
	{
		TestWorld.Tick();
	}
	const float PushedDistance = FVector::Dist2D(Chaser->GetActorLocation(), MinerLocation);
	AddInfo(FString::Printf(TEXT("far movement: knockback moved the robot %.1f uu away from the miner"), PushedDistance - StartDistance));
	TestTrue(TEXT("far movement: knockback pushed the robot away"), PushedDistance - StartDistance > 10.f);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestTrue(TEXT("far movement: robot back on the floor"), Movement->IsMovingOnGround() && FMath::Abs(Chaser->GetActorLocation().Z - Settled.Z) < 1.f);
	Chaser->SetFarMovement(false);
	TestEqual(TEXT("near movement: tick interval back to 0"), Movement->GetComponentTickInterval(), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRREnemyPassiveHitCapsuleTest, "RogueRigs.Enemies.ShotHitsMovingRobotAndDeathShowsParts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRREnemyPassiveHitCapsuleTest::RunTest(const FString& Parameters)
{
	using namespace RREnemyTests;

	// task-093-01: the hit capsule runs no overlap query when the robot moves; shots still hit a walking BP_Chaser, and the death
	// shrink keeps its Blueprint parts' rest visibility and shrinks them.
	FTestWorld TestWorld(TEXT("RREnemyPassiveHitCapsule"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	UClass* ChaserClass = Grid ? LoadTestClass<ARREnemy>(*this, ChaserClassPath) : nullptr;
	UClass* RifleClass = ChaserClass ? LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath) : nullptr;
	ARRWeapon* Rifle = RifleClass ? TestWorld.Spawn<ARRWeapon>(RifleClass, FVector::ZeroVector) : nullptr;
	if (!Rifle || !TestTrue(TEXT("rifle acquired"), Rifle->Acquire(nullptr)) || !SpawnUnarmedMiner(*this, TestWorld, Grid))
	{
		return false;
	}
	Rifle->GetAutoFireComponent()->StopFiring();
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);
	Rifle->GetAbilitySystemComponent()->SetNumericAttributeBase(URRWeaponAttributeSet::GetPierceAttribute(), 1.f);

	const FVector Floor = Grid->GetCellCenter(TargetCell.X, TargetCell.Y);
	const float HalfHeight = ChaserClass->GetDefaultObject<ARREnemy>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FTransform SpawnTransform(Floor + FVector(0.f, 0.f, HalfHeight + 2.f));
	ARREnemy* Chaser = TestWorld.Get()->SpawnActorDeferred<ARREnemy>(ChaserClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!TestNotNull(TEXT("BP_Chaser spawned"), Chaser))
	{
		return false;
	}
	FEnemyAccess::XPPickupClassOf(Chaser) = ARRTestPickupXP::StaticClass();
	Chaser->FinishSpawning(SpawnTransform);
	URRHitCapsuleComponent* HitCapsule = Cast<URRHitCapsuleComponent>(Chaser->GetHitCapsule());
	if (!TestNotNull(TEXT("hit capsule is a URRHitCapsuleComponent"), HitCapsule))
	{
		return false;
	}
	TestTrue(TEXT("hit capsule keeps overlap events (the projectile needs them)"), HitCapsule->GetGenerateOverlapEvents());

	// Walk toward the miner (west).
	const FVector Start = Chaser->GetActorLocation();
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	const float Walked = FVector::Dist2D(Start, Chaser->GetActorLocation());
	AddInfo(FString::Printf(TEXT("walked %.0f uu in 30 frames, hit capsule overlap queries %d"), Walked, HitCapsule->GetOverlapQueryCount()));
	TestTrue(TEXT("the chaser walks"), Walked > 50.f);
	if (URRHitCapsuleComponent::IsPassiveEnabled())
	{
		TestEqual(TEXT("no hit capsule overlap query while walking"), HitCapsule->GetOverlapQueryCount(), 0);
	}

	// Visual parts (BodyMesh, EyeMesh), their world scale at full size and their visibility at rest. EyeMesh is hidden in
	// BP_Chaser since task-152-02 (owner 2026-10-13: the eye is in the model), so the death must keep each part's rest visibility.
	TInlineComponentArray<UStaticMeshComponent*> Parts(Chaser);
	// The blob shadow (task-264-02) is not a body part: it hides when the death starts.
	Parts.RemoveAll([](const UStaticMeshComponent* Part) { return Part->IsA<URRBlobShadowComponent>(); });
	TestTrue(TEXT("BP_Chaser has at least 2 mesh parts"), Parts.Num() >= 2);
	TArray<float> FullScale;
	TArray<bool> VisibleAtRest;
	for (const UStaticMeshComponent* Part : Parts)
	{
		FullScale.Add(Part->GetComponentScale().X);
		VisibleAtRest.Add(Part->IsVisible());
	}
	TestTrue(TEXT("BP_Chaser shows at least one mesh part at rest"), VisibleAtRest.Contains(true));

	// Three shots of 10 into the walking chaser (30 health), fired 300 uu in front of it along +X while it walks west.
	const FVector MuzzleOffset = FWeaponAccess::MuzzleOf(Rifle)->GetComponentLocation() - Rifle->GetActorLocation();
	const float ShotHeight = Floor.Z + 0.5f * Grid->GetBlockHeight();
	const float ExpectedHealth[] = { 20.f, 10.f, 0.f };
	for (int32 Shot = 0; Shot < 3; ++Shot)
	{
		const FVector Robot = Chaser->GetActorLocation();
		const FVector MuzzleTarget(Robot.X - 300.f, Robot.Y, ShotHeight);
		Rifle->SetActorLocation(MuzzleTarget - MuzzleOffset, false, nullptr, ETeleportType::TeleportPhysics);
		const int32 Frame = TickUntilDestroyed(TestWorld, FireSingleShot(*this, Rifle), 30);
		TestTrue(FString::Printf(TEXT("shot %d stopped on the robot"), Shot + 1), Frame != INDEX_NONE);
		TestEqual(FString::Printf(TEXT("health after shot %d"), Shot + 1), Attribute(Chaser, URRHealthAttributeSet::GetHealthAttribute()), ExpectedHealth[Shot]);
	}
	if (!TestTrue(TEXT("dead after 3 hits"), Chaser->IsDead()))
	{
		return false;
	}

	// Death shrink: every part keeps its rest visibility (no hidden part lit, task-119-01), still attached to the robot and shrinking with it.
	TWeakObjectPtr<ARREnemy> WeakChaser(Chaser);
	for (int32 Frame = 0; Frame < 4; ++Frame)
	{
		TestWorld.Tick();
	}
	if (TestTrue(TEXT("still shrinking"), WeakChaser.IsValid() && !WeakChaser->IsActorBeingDestroyed() && !URRActorPoolSubsystem::IsPooledIdle(Chaser)))
	{
		for (int32 Index = 0; Index < Parts.Num(); ++Index)
		{
			const UStaticMeshComponent* Part = Parts[Index];
			TestEqual(FString::Printf(TEXT("%s keeps its rest visibility during the death"), *Part->GetName()), Part->IsVisible(), VisibleAtRest[Index]);
			TestTrue(FString::Printf(TEXT("%s attached to the robot"), *Part->GetName()), Part->GetAttachmentRootActor() == Chaser);
			TestTrue(FString::Printf(TEXT("%s shrinks"), *Part->GetName()), Part->GetComponentScale().X < 0.9f * FullScale[Index]);
		}
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
