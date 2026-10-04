// Rogue Rigs - automation tests of the shotgun (RogueRigs.Shotgun.*): CSV stats and M1 loadout, cone range and angle, rock line of sight, independent crits, knockback, upgrade cards.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Enemies/RRChaser.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Upgrades/RRUpgradeCard.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRShotgun.h"
#include "Weapons/RRWeaponAttributeSet.h"

namespace RRShotgunTests
{
	using namespace RRTestHarness;

	/** Burst rifle Blueprint (first starting weapon of BP_Miner). */
	const TCHAR* const BurstRifleClassPath = TEXT("/Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle.BP_BurstRifle_C");

	/** Open floor of arena_m1: rows 41-43 are floor from x 32 to 45 (rock (31, 41) is behind the miner). */
	const FIntPoint OpenMinerCell(32, 42);

	/** Rock test: miner on (36, 40) aiming -X; rock (34, 40) between it and floor (33, 40); floor (34, 41) seen past (35, 40-41). */
	const FIntPoint RockMinerCell(36, 40);
	const FIntPoint RockCellWest(34, 40);
	const FIntPoint BehindRockCell(33, 40);
	const FIntPoint SideFloorCell(34, 41);

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** The test scene: grid, BP_Miner (weapons stopped, aimed), its shotgun. */
	struct FShotgunSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRShotgun* Shotgun = nullptr;
	};

	/** Points the miner's stick aim along Direction (camera yaw 0: screen up = world +X, screen right = world +Y). */
	void AimAlong(ARRMiner* Miner, const FVector& Direction)
	{
		const FVector Dir = Direction.GetSafeNormal2D();
		Miner->ApplyStickAimInput(FVector2D(Dir.Y, Dir.X));
	}

	/** Spawns the grid and BP_Miner on Cell, stops every weapon, aims along Aim and finds the shotgun. */
	bool SetUp(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, const FVector& Aim, FShotgunSetup& Out)
	{
		Out.Grid = SpawnGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? SpawnMiner(Test, TestWorld, Out.Grid, Cell) : nullptr;
		if (!Out.Miner)
		{
			return false;
		}
		for (ARRWeapon* Weapon : Out.Miner->GetWeapons())
		{
			if (IsValid(Weapon))
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		AimAlong(Out.Miner, Aim);
		Test.TestTrue(TEXT("miner aims as asked"), Out.Miner->GetAimDirection().Equals(Aim.GetSafeNormal2D(), 1e-4f));
		Out.Shotgun = FindShotgun(Out.Miner);
		return Test.TestNotNull(TEXT("miner holds a shotgun"), Out.Shotgun);
	}

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

	/** Spawns a chaser at Distance uu and AngleDeg (yaw, counter-clockwise seen from above = toward +Y) from the miner's axis around Aim. */
	ARRChaser* SpawnChaserPolar(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FShotgunSetup& S, float Distance, float AngleDeg)
	{
		const FVector Floor = S.Grid->GetCellCenter(0, 0);
		const FVector Dir = S.Miner->GetAimDirection().RotateAngleAxis(AngleDeg, FVector::UpVector);
		const FVector Location = S.Miner->GetActorLocation() + Dir * Distance;
		return SpawnChaser(Test, TestWorld, FVector(Location.X, Location.Y, Floor.Z));
	}

	/** Fires exactly one wave now through the auto-fire (started, advanced 1 ms, stopped). */
	void FireOneWave(FAutomationTestBase& Test, ARRShotgun* Shotgun)
	{
		const int32 Before = Shotgun->GetWaveCount();
		URRAutoFireComponent* AutoFire = Shotgun->GetAutoFireComponent();
		AutoFire->StartFiring();
		AutoFire->AdvanceTime(0.001f);
		AutoFire->StopFiring();
		Test.TestEqual(TEXT("exactly one wave fired"), Shotgun->GetWaveCount(), Before + 1);
	}

	/** Current Health of a robot. */
	float GetHealth(const ARREnemy* Enemy)
	{
		return Enemy->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Sets a base attribute of the shotgun ASC. */
	void SetShotgunAttribute(ARRShotgun* Shotgun, const FGameplayAttribute& Attribute, float Value)
	{
		Shotgun->GetAbilitySystemComponent()->SetNumericAttributeBase(Attribute, Value);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunStatsAndLoadoutTest, "RogueRigs.Shotgun.StatsAndLoadout",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunStatsAndLoadoutTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunLoadout"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell) : nullptr;
	UClass* RifleClass = LoadTestClass<ARRWeapon>(*this, BurstRifleClassPath);
	if (!Miner || !RifleClass)
	{
		return false;
	}

	// M1 loadout: BP_BurstRifle (StartingWeapons) then the shotgun (native fallback, or a shotgun Blueprint once listed).
	if (!TestEqual(TEXT("two starting weapons"), Miner->GetWeapons().Num(), 2))
	{
		return false;
	}
	ARRWeapon* Rifle = Miner->GetWeapons()[0];
	ARRShotgun* Shotgun = Cast<ARRShotgun>(Miner->GetWeapons()[1].Get());
	TestTrue(TEXT("weapon 0 is the burst rifle"), IsValid(Rifle) && Rifle->IsA(RifleClass));
	if (!TestNotNull(TEXT("weapon 1 is a shotgun"), Shotgun))
	{
		return false;
	}
	AddInfo(FString::Printf(TEXT("shotgun class %s"), *Shotgun->GetClass()->GetName()));
	TestEqual(TEXT("shotgun acquired by the miner"), Shotgun->GetOwnerMiner(), Miner);
	TestEqual(TEXT("shotgun attached to the miner"), Shotgun->GetAttachParentActor(), static_cast<AActor*>(Miner));
	TestFalse(TEXT("shotgun has no actor collision"), Shotgun->GetActorEnableCollision());
	TestTrue(TEXT("shotgun firing from acquisition"), Shotgun->GetAutoFireComponent()->IsFiring());
	TestTrue(TEXT("rifle firing from acquisition"), Rifle->GetAutoFireComponent()->IsFiring());
	Rifle->GetAutoFireComponent()->StopFiring();
	Shotgun->GetAutoFireComponent()->StopFiring();

	// CSV row Shotgun (shotgun gdd, provisional owner values 2026-10-03).
	const UAbilitySystemComponent* ASC = Shotgun->GetAbilitySystemComponent();
	TestEqual(TEXT("Damage 18"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetDamageAttribute()), 18.f);
	TestEqual(TEXT("FireRate 0.8"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetFireRateAttribute()), 0.8f, 1e-5f);
	TestEqual(TEXT("MagazineSize 6"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetMagazineSizeAttribute()), 6.f);
	TestEqual(TEXT("Ammo full"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()), 6.f);
	TestEqual(TEXT("ReloadSpeed 1"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetReloadSpeedAttribute()), 1.f);
	TestEqual(TEXT("CritChance 5 %"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritChanceAttribute()), 0.05f, 1e-5f);
	TestEqual(TEXT("CritDamage x1.5"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritDamageAttribute()), 1.5f, 1e-5f);
	TestEqual(TEXT("BurstCount 1"), Shotgun->GetBurstCount(), 1);
	TestEqual(TEXT("ReloadTime 2 s"), Shotgun->GetReloadTime(), 2.f);
	TestEqual(TEXT("Range 700"), Shotgun->GetRange(), 700.f);
	TestEqual(TEXT("cone 60 deg"), Shotgun->GetConeAngleDeg(), 60.f);
	TestEqual(TEXT("knockback 600"), Shotgun->GetKnockbackForce(), 600.f);
	TestEqual(TEXT("display name"), Shotgun->GetDisplayName().ToString(), FString(TEXT("Fusil à pompe")));
	TestEqual(TEXT("rifle keeps no cone"), Rifle->GetDisplayName().ToString(), FString(TEXT("Fusil à rafales")));

	// Cadence and magazine on the accumulator alone: waves at k / 0.8 s for k = 0..5, then a 2 s reload, wave 7 at 6.25 + 2 = 8.25 s.
	URRAutoFireComponent* AutoFire = Shotgun->GetAutoFireComponent();
	constexpr float Step = 0.01f;
	const double Expected[] = { 0.0, 1.25, 2.5, 3.75, 5.0, 6.25, 8.25 };
	TArray<double> Times;
	bool bReloadSeen = false;
	AutoFire->StartFiring();
	double Elapsed = 0.0;
	int32 LastCount = Shotgun->GetWaveCount();
	for (int32 Index = 0; Index <= 900 && Times.Num() < 7; ++Index)
	{
		AutoFire->AdvanceTime(Index == 0 ? 0.f : Step);
		Elapsed += Index == 0 ? 0.0 : Step;
		if (Shotgun->GetWaveCount() != LastCount)
		{
			Times.Add(Elapsed);
			LastCount = Shotgun->GetWaveCount();
			if (Times.Num() == 6)
			{
				TestEqual(TEXT("magazine empty after 6 waves"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()), 0.f);
				TestTrue(TEXT("reloading after 6 waves"), AutoFire->IsReloading());
				TestEqual(TEXT("reload lasts 2 s"), AutoFire->GetReloadDuration(), 2.f, 1e-4f);
				bReloadSeen = true;
			}
		}
	}
	AutoFire->StopFiring();
	TestTrue(TEXT("reload observed"), bReloadSeen);
	if (TestEqual(TEXT("7 waves in 9 s"), Times.Num(), 7))
	{
		for (int32 Index = 0; Index < 7; ++Index)
		{
			TestEqual(FString::Printf(TEXT("wave %d at %.2f s"), Index + 1, Expected[Index]), Times[Index], Expected[Index], Step + 1e-4);
		}
		TestEqual(TEXT("magazine refilled minus one"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()), 5.f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunConeHitsTest, "RogueRigs.Shotgun.ConeHits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunConeHitsTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunCone"));
	FShotgunSetup S;
	if (!SetUp(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, S))
	{
		return false;
	}
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);

	// (distance, angle from the aim): inside = within 700 uu and 30 deg; outside by angle, range or behind.
	struct FCase { float Distance; float Angle; bool bHit; };
	const FCase Cases[] = {
		{ 500.f, 0.f, true }, { 500.f, 28.f, true }, { 690.f, -20.f, true }, { 100.f, 29.f, true },
		{ 450.f, 32.f, false }, { 500.f, -32.f, false }, { 710.f, 0.f, false }, { 300.f, 180.f, false },
	};
	TArray<ARRChaser*> Chasers;
	for (const FCase& Case : Cases)
	{
		ARRChaser* Chaser = SpawnChaserPolar(*this, TestWorld, S, Case.Distance, Case.Angle);
		if (!Chaser)
		{
			return false;
		}
		Chasers.Add(Chaser);
	}
	const float FullHealth = GetHealth(Chasers[0]);
	TestEqual(TEXT("chaser full health (CSV 30)"), FullHealth, 30.f);

	FireOneWave(*this, S.Shotgun);
	int32 Hits = 0;
	for (int32 Index = 0; Index < Chasers.Num(); ++Index)
	{
		const FCase& Case = Cases[Index];
		const float Expected = Case.bHit ? FullHealth - 18.f : FullHealth;
		TestEqual(FString::Printf(TEXT("chaser at %.0f uu, %.0f deg: %s"), Case.Distance, Case.Angle, Case.bHit ? TEXT("hit for 18") : TEXT("not hit")),
			GetHealth(Chasers[Index]), Expected);
		Hits += Case.bHit ? 1 : 0;
	}
	const FRRShotgunWave Wave = S.Shotgun->GetLastWave();
	TestEqual(TEXT("wave hit count"), Wave.HitCount, Hits);
	TestEqual(TEXT("wave half angle 30"), Wave.HalfAngleDeg, 30.f);
	TestEqual(TEXT("wave range 700"), Wave.Range, 700.f);
	TestTrue(TEXT("wave direction = aim"), Wave.Direction.Equals(FVector::ForwardVector, 1e-4f));
	TestEqual(TEXT("wave origin on the miner axis"), FVector::Dist2D(Wave.Origin, S.Miner->GetActorLocation()), 0.0, 0.01);
	TestEqual(TEXT("open floor: wave not clipped"), Wave.ClippedLength, 700.f, 0.01f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunRockBlocksTest, "RogueRigs.Shotgun.RockBlocks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunRockBlocksTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunRock"));
	FShotgunSetup S;
	if (!SetUp(*this, TestWorld, RockMinerCell, -FVector::ForwardVector, S))
	{
		return false;
	}
	ARRArenaGrid* Grid = S.Grid;
	TestTrue(TEXT("(34, 40) is rock"), Grid->GetCellType(RockCellWest) == ERRCellType::Rock);
	TestFalse(TEXT("(33, 40) is open"), Grid->IsBlockCell(BehindRockCell));
	TestFalse(TEXT("(34, 41) is open"), Grid->IsBlockCell(SideFloorCell));
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);

	ARRChaser* Hidden = SpawnChaser(*this, TestWorld, Grid->GetCellCenter(BehindRockCell.X, BehindRockCell.Y));
	ARRChaser* Seen = SpawnChaser(*this, TestWorld, Grid->GetCellCenter(SideFloorCell.X, SideFloorCell.Y));
	if (!Hidden || !Seen)
	{
		return false;
	}
	const FVector MinerLocation = S.Miner->GetActorLocation();
	TestFalse(TEXT("grid: no line of sight through the rock"), Grid->HasLineOfSight(MinerLocation, Hidden->GetActorLocation()));
	TestTrue(TEXT("grid: line of sight to the side cell"), Grid->HasLineOfSight(MinerLocation, Seen->GetActorLocation()));
	TestEqual(TEXT("grid: clear distance west = 300 (rock face)"), Grid->GetClearDistance(MinerLocation, -FVector::ForwardVector, 700.f), 300.f, 0.01f);
	TestEqual(TEXT("grid: clear distance from inside a rock = 0"), Grid->GetClearDistance(Grid->GetCellCenter(RockCellWest.X, RockCellWest.Y), FVector::ForwardVector, 700.f), 0.f);
	TestTrue(TEXT("both robots inside range and angle"),
		FVector::Dist2D(MinerLocation, Hidden->GetActorLocation()) <= 700.f && FVector::Dist2D(MinerLocation, Seen->GetActorLocation()) <= 700.f);

	FireOneWave(*this, S.Shotgun);
	TestEqual(TEXT("robot behind the rock not hit"), GetHealth(Hidden), 30.f);
	TestTrue(TEXT("robot behind the rock not pushed"), Hidden->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	TestEqual(TEXT("robot in sight hit for 18"), GetHealth(Seen), 12.f);
	const FRRShotgunWave Wave = S.Shotgun->GetLastWave();
	TestEqual(TEXT("one robot hit"), Wave.HitCount, 1);
	TestEqual(TEXT("wave clipped at the rock face (300 uu)"), Wave.ClippedLength, 300.f, 0.01f);
	TestEqual(TEXT("clear distance of the center ray"), S.Shotgun->GetWaveClearDistance(0.f), 300.f, 0.01f);
	TestTrue(TEXT("edge ray toward the open side reaches farther"), S.Shotgun->GetWaveClearDistance(-30.f) > 300.f);
	AddInfo(FString::Printf(TEXT("clear distance at -30 / +30 deg: %.1f / %.1f uu"), S.Shotgun->GetWaveClearDistance(-30.f), S.Shotgun->GetWaveClearDistance(30.f)));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunIndependentCritsTest, "RogueRigs.Shotgun.IndependentCrits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunIndependentCritsTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunCrits"));
	FShotgunSetup S;
	if (!SetUp(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, S))
	{
		return false;
	}
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.5f);

	const FVector2D Polar[] = { { 250.f, 0.f }, { 350.f, 15.f }, { 450.f, -15.f }, { 550.f, 0.f }, { 400.f, 25.f }, { 600.f, 10.f } };
	TArray<ARRChaser*> Chasers;
	TArray<int32> HitCounts;
	HitCounts.Init(0, UE_ARRAY_COUNT(Polar));
	TArray<bool> CritFlags;
	CritFlags.Init(false, UE_ARRAY_COUNT(Polar));
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Polar); ++Index)
	{
		ARRChaser* Chaser = SpawnChaserPolar(*this, TestWorld, S, Polar[Index].X, Polar[Index].Y);
		if (!Chaser)
		{
			return false;
		}
		Chaser->GetHealthSet()->OnDamageReceived.AddLambda([&HitCounts, &CritFlags, Index](float, bool bIsCritical, float)
		{
			++HitCounts[Index];
			CritFlags[Index] = bIsCritical;
		});
		Chasers.Add(Chaser);
	}

	// Seeded global stream: URRDamageExecution rolls FMath::FRand once per robot.
	FMath::RandInit(20261003);
	FireOneWave(*this, S.Shotgun);
	int32 Crits = 0;
	FString Pattern;
	for (int32 Index = 0; Index < Chasers.Num(); ++Index)
	{
		TestEqual(FString::Printf(TEXT("robot %d hit once"), Index), HitCounts[Index], 1);
		const float Expected = CritFlags[Index] ? 30.f - 18.f * 1.5f : 30.f - 18.f;
		TestEqual(FString::Printf(TEXT("robot %d health matches its own roll (%s)"), Index, CritFlags[Index] ? TEXT("crit 27") : TEXT("normal 18")),
			GetHealth(Chasers[Index]), Expected, 1e-3f);
		Crits += CritFlags[Index] ? 1 : 0;
		Pattern += CritFlags[Index] ? TEXT("C") : TEXT("n");
	}
	AddInfo(FString::Printf(TEXT("crit pattern (spawn order) %s: %d / %d"), *Pattern, Crits, Chasers.Num()));
	TestEqual(TEXT("six robots hit by one wave"), S.Shotgun->GetLastWave().HitCount, 6);
	TestTrue(TEXT("independent rolls: some robots crit, some do not"), Crits > 0 && Crits < Chasers.Num());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunKnockbackTest, "RogueRigs.Shotgun.Knockback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunKnockbackTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunKnockback"));
	FShotgunSetup S;
	if (!SetUp(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, S))
	{
		return false;
	}
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);

	ARRChaser* Full = SpawnChaserPolar(*this, TestWorld, S, 300.f, 20.f);
	ARRChaser* Quarter = SpawnChaserPolar(*this, TestWorld, S, 400.f, -10.f);
	ARRChaser* Immune = SpawnChaserPolar(*this, TestWorld, S, 500.f, 5.f);
	ARRChaser* Doomed = SpawnChaserPolar(*this, TestWorld, S, 350.f, -25.f);
	if (!Full || !Quarter || !Immune || !Doomed)
	{
		return false;
	}
	TestEqual(TEXT("CSV knockback resistance 0"), Full->GetKnockbackResistance(), 0.f);
	FEnemyAccess::StatsOf(Quarter).KnockbackResistance = 0.25f;
	FEnemyAccess::StatsOf(Immune).KnockbackResistance = 1.f;
	Doomed->GetAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 10.f);

	const FVector Origin = S.Shotgun->GetWaveOrigin();
	const double FullStartDistance = FVector::Dist2D(Full->GetActorLocation(), Origin);
	FireOneWave(*this, S.Shotgun);

	const FVector FullOut = (Full->GetActorLocation() - Origin).GetSafeNormal2D();
	const FVector QuarterOut = (Quarter->GetActorLocation() - Origin).GetSafeNormal2D();
	const FVector FullLaunch = Full->GetCharacterMovement()->PendingLaunchVelocity;
	const FVector QuarterLaunch = Quarter->GetCharacterMovement()->PendingLaunchVelocity;
	AddInfo(FString::Printf(TEXT("launch full %s, quarter-resistant %s"), *FullLaunch.ToCompactString(), *QuarterLaunch.ToCompactString()));
	TestTrue(TEXT("resistance 0: 600 uu/s outward from the miner"), FullLaunch.Equals(FullOut * 600.f, 0.05f));
	TestTrue(TEXT("resistance 0.25: 450 uu/s outward from the miner"), QuarterLaunch.Equals(QuarterOut * 450.f, 0.05f));
	TestTrue(TEXT("resistance 1: no knockback"), Immune->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	TestTrue(TEXT("robot killed by the wave: dead"), Doomed->IsDead());
	TestTrue(TEXT("robot killed by the wave: not pushed"), Doomed->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	TestEqual(TEXT("knocked robot still damaged"), GetHealth(Full), 12.f);

	// The launch is applied by CharacterMovement: the robot moves away from the miner although it chases it.
	for (int32 Frame = 0; Frame < 3; ++Frame)
	{
		TestWorld.Tick();
	}
	const double FullEndDistance = FVector::Dist2D(Full->GetActorLocation(), Origin);
	AddInfo(FString::Printf(TEXT("full-knockback robot: %.1f -> %.1f uu from the miner after 3 frames"), FullStartDistance, FullEndDistance));
	TestTrue(TEXT("knocked robot moved away from the miner"), FullEndDistance > FullStartDistance + 10.0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunCardsOnlyWhenEquippedTest, "RogueRigs.Shotgun.CardsOnlyWhenEquipped",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunCardsOnlyWhenEquippedTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	constexpr int32 Offers = 200;
	for (int32 Pass = 0; Pass < 2; ++Pass)
	{
		const bool bEquipped = Pass == 1;
		FTestWorld TestWorld(TEXT("RRShotgunCards"));
		ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
		ARRMiner* Miner = nullptr;
		if (Grid)
		{
			Miner = bEquipped ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell) : SpawnMiner(*this, TestWorld, Grid, OpenMinerCell, KeepRifleOnly);
		}
		URRUpgradeComponent* Upgrades = Miner ? Miner->GetUpgrades() : nullptr;
		if (!Miner || !TestNotNull(TEXT("upgrade component"), Upgrades))
		{
			return false;
		}
		for (ARRWeapon* Weapon : Miner->GetWeapons())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
		ARRShotgun* Shotgun = FindShotgun(Miner);
		ARRWeapon* Rifle = FindRifle(Miner);
		TestNotNull(TEXT("rifle owned"), Rifle);
		TestEqual(bEquipped ? TEXT("shotgun equipped") : TEXT("shotgun not equipped"), Shotgun != nullptr, bEquipped);
		TestEqual(TEXT("weapon count"), Miner->GetWeapons().Num(), bEquipped ? 2 : 1);

		Upgrades->SetRandomSeed(20261003);
		int32 ShotgunCards = 0;
		int32 RifleCards = 0;
		for (int32 Offer = 0; Offer < Offers; ++Offer)
		{
			for (const FRRUpgradeCard& Card : Upgrades->BuildOffer(3, static_cast<float>(Offer % 11)))
			{
				if (Card.Target != ERRUpgradeTarget::Weapon)
				{
					continue;
				}
				TestTrue(TEXT("targeted card on an owned weapon"), Miner->GetWeapons().Contains(Card.TargetWeapon.Get()));
				if (Card.TargetWeapon && Card.TargetWeapon->IsA<ARRShotgun>())
				{
					++ShotgunCards;
					if (ShotgunCards == 1)
					{
						TestEqual(TEXT("shotgun card target text"), Card.TargetText.ToString(), FString(TEXT("Fusil à pompe")));
					}
				}
				else
				{
					++RifleCards;
				}
			}
		}
		AddInfo(FString::Printf(TEXT("%s: %d offers, %d shotgun cards, %d rifle cards"), bEquipped ? TEXT("rifle + shotgun") : TEXT("rifle only"), Offers, ShotgunCards, RifleCards));
		TestTrue(TEXT("rifle cards offered"), RifleCards > 0);
		if (!bEquipped)
		{
			TestEqual(TEXT("no shotgun card without a shotgun"), ShotgunCards, 0);
			continue;
		}
		TestTrue(TEXT("shotgun cards offered when equipped"), ShotgunCards > 0);

		// A damage card targeted at the shotgun raises the shotgun only.
		FRRUpgradeCard Card;
		if (!TestTrue(TEXT("shotgun damage card made"), Upgrades->MakeCard(ERRUpgradeStat::Damage, ERRUpgradeRarity::Grey, ERRUpgradeTarget::Weapon, Shotgun, Card)))
		{
			return false;
		}
		const FGameplayAttribute Damage = URRWeaponAttributeSet::GetDamageAttribute();
		const float RifleBefore = Rifle->GetAbilitySystemComponent()->GetNumericAttribute(Damage);
		Upgrades->BuildOffer(3, 0.f);
		TestTrue(TEXT("shotgun damage card applied"), Upgrades->ApplyUpgrade(Card));
		const float Expected = Card.Operation == ERRUpgradeOperation::MultiplyAdditive ? 18.f * (1.f + Card.Value) : 18.f + Card.Value;
		TestEqual(TEXT("shotgun damage raised"), Shotgun->GetAbilitySystemComponent()->GetNumericAttribute(Damage), Expected, 1e-3f);
		TestEqual(TEXT("rifle damage unchanged"), Rifle->GetAbilitySystemComponent()->GetNumericAttribute(Damage), RifleBefore);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
