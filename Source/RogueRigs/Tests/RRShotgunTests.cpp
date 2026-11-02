// Rogue Rigs - automation tests of the shotgun (RogueRigs.Shotgun.*): CSV stats, cadence and M1 loadout, pellet count and spread, pierce, rock stop, knockback per pellet hit, crit per pellet hit, upgrade cards.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "Enemies/RRChaser.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Projectiles/RRShotgunPellet.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPickupXP.h"
#include "Tests/RRTestStatsRows.h"
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

	/** Rock test: miner on (36, 40) aiming -X; rock (34, 40) 300 uu ahead (its face); floor (33, 40) behind it. */
	const FIntPoint RockMinerCell(36, 40);
	const FIntPoint RockCellWest(34, 40);
	const FIntPoint BehindRockCell(33, 40);

	// Expected stats: WeaponStats.csv row Shotgun, read through Tests/RRTestStatsRows.h (no literal copy, task-420).

	// Protected-member accessors: Tests/RRTestAccess.h.
	using namespace RRTestAccess;

	/** The test scene: grid, BP_Miner (weapons stopped, aimed), its shotgun, the shot origin and aim, the Shotgun CSV row. */
	struct FShotgunSetup
	{
		ARRArenaGrid* Grid = nullptr;
		ARRMiner* Miner = nullptr;
		ARRShotgun* Shotgun = nullptr;
		FVector Origin = FVector::ZeroVector;
		FVector Aim = FVector::ForwardVector;
		FRRWeaponStatsRow Row;
	};

	/** Shotgun row of WeaponStats.csv (test error when missing). */
	FRRWeaponStatsRow ReadShotgunRow(FAutomationTestBase& Test)
	{
		return RRTestStats::WeaponRow(Test, RRTestStats::ShotgunRow);
	}

	/** Pellet count of the row as an int. */
	int32 PelletsOf(const FRRWeaponStatsRow& Row)
	{
		return Row.PelletCount;
	}

	/** Pierce of the row as robots per pellet. */
	int32 PierceOf(const FRRWeaponStatsRow& Row)
	{
		return FMath::RoundToInt32(Row.Pierce);
	}

	/** Health that survives Hits pellet hits of the row, critical ones included, with one hit to spare. */
	float SurvivingHealth(const FRRWeaponStatsRow& Row, int32 Hits)
	{
		return (Hits * FMath::Max(Row.CritDamage, 1.f) + 1.f) * Row.Damage;
	}

	/** Points the miner's stick aim along Direction (camera yaw 0: screen up = world +X, screen right = world +Y). */
	void AimAlong(ARRMiner* Miner, const FVector& Direction)
	{
		const FVector Dir = Direction.GetSafeNormal2D();
		Miner->ApplyStickAimInput(FVector2D(Dir.Y, Dir.X));
	}

	/** Spawns the grid and BP_Miner on Cell, stops every weapon, aims along Aim, finds the shotgun and records its shot origin. */
	bool SetUp(FAutomationTestBase& Test, const FTestWorld& TestWorld, FIntPoint Cell, const FVector& Aim, FShotgunSetup& Out)
	{
		Out.Row = ReadShotgunRow(Test);
		Out.Grid = SpawnGrid(Test, TestWorld);
		Out.Miner = Out.Grid ? SpawnMiner(Test, TestWorld, Out.Grid, Cell, M1TestLoadout) : nullptr;
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
		Out.Aim = Aim.GetSafeNormal2D();
		Test.TestTrue(TEXT("miner aims as asked"), Out.Miner->GetAimDirection().Equals(Out.Aim, 1e-4f));
		Out.Shotgun = FindShotgun(Out.Miner);
		if (!Test.TestNotNull(TEXT("miner holds a shotgun"), Out.Shotgun))
		{
			return false;
		}
		Out.Origin = Out.Shotgun->GetShotOrigin();
		return true;
	}

	/** Spawns a native ARRChaser with its feet on the floor at FloorLocation, ticking off (it neither moves nor consumes a pending launch); XP drop ARRTestPickupXP. */
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

	/** Unit flat direction of the pellet ray at AngleDeg (yaw, toward +Y seen from above) from the aim. */
	FVector RayDirection(const FShotgunSetup& S, float AngleDeg)
	{
		return S.Aim.RotateAngleAxis(AngleDeg, FVector::UpVector);
	}

	/** Spawns a frozen chaser on the pellet ray AngleDeg at Distance uu from the shot origin (feet on the floor). */
	ARRChaser* SpawnOnRay(FAutomationTestBase& Test, const FTestWorld& TestWorld, const FShotgunSetup& S, float Distance, float AngleDeg)
	{
		const FVector Floor = S.Grid->GetCellCenter(0, 0);
		const FVector Location = S.Origin + RayDirection(S, AngleDeg) * Distance;
		return SpawnFrozenChaser(Test, TestWorld, FVector(Location.X, Location.Y, Floor.Z));
	}

	/** Fires exactly one shot now through the auto-fire (started, advanced 1 ms, stopped). */
	void FireOneShot(FAutomationTestBase& Test, ARRShotgun* Shotgun)
	{
		const int32 Before = Shotgun->GetShotCount();
		URRAutoFireComponent* AutoFire = Shotgun->GetAutoFireComponent();
		AutoFire->StartFiring();
		AutoFire->AdvanceTime(0.001f);
		AutoFire->StopFiring();
		Test.TestEqual(TEXT("exactly one shot fired"), Shotgun->GetShotCount(), Before + 1);
	}

	/** Living shotgun pellets of the world. */
	TArray<ARRShotgunPellet*> GetPellets(const FTestWorld& TestWorld)
	{
		TArray<ARRShotgunPellet*> Result;
		for (TActorIterator<ARRShotgunPellet> It(TestWorld.Get()); It; ++It)
		{
			if (IsValid(*It))
			{
				Result.Add(*It);
			}
		}
		return Result;
	}

	/** Ticks until no pellet is left (at most MaxFrames); returns the frames ticked. */
	int32 TickUntilPelletsGone(const FTestWorld& TestWorld, int32 MaxFrames = 60)
	{
		int32 Frame = 0;
		while (Frame < MaxFrames && GetPellets(TestWorld).Num() > 0)
		{
			TestWorld.Tick();
			++Frame;
		}
		return Frame;
	}

	/** Current Health of a robot. */
	float GetHealth(const ARREnemy* Enemy)
	{
		return Enemy->GetAbilitySystemComponent()->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	}

	/** Sets MaxHealth and Health of a robot to Health (a scenario value derived from the row). */
	void SetRobotHealth(ARRChaser* Chaser, float Health)
	{
		UAbilitySystemComponent* ASC = Chaser->GetAbilitySystemComponent();
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), Health);
		ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), Health);
	}

	/** Sets a base attribute of the shotgun ASC. */
	void SetShotgunAttribute(ARRShotgun* Shotgun, const FGameplayAttribute& Attribute, float Value)
	{
		Shotgun->GetAbilitySystemComponent()->SetNumericAttributeBase(Attribute, Value);
	}

	/** Counts the hits a robot receives. */
	void CountHits(ARRChaser* Chaser, int32& Counter)
	{
		Chaser->GetHealthSet()->OnDamageReceived.AddLambda([&Counter](float, bool, float) { ++Counter; });
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunStatsAndLoadoutTest, "RogueRigs.Shotgun.StatsAndLoadout",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunStatsAndLoadoutTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunLoadout"));
	ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell, M1TestLoadout) : nullptr;
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

	// CSV row Shotgun: every attribute and constant of the weapon equals its column.
	const FRRWeaponStatsRow Row = ReadShotgunRow(*this);
	const FRRWeaponStatsRow RifleRow = RRTestStats::WeaponRow(*this, RRTestStats::BurstRifleRow);
	const UAbilitySystemComponent* ASC = Shotgun->GetAbilitySystemComponent();
	TestEqual(TEXT("Damage per pellet = CSV Damage"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetDamageAttribute()), Row.Damage);
	TestEqual(TEXT("FireRate = CSV FireRate"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetFireRateAttribute()), Row.FireRate, 1e-5f);
	TestEqual(TEXT("MagazineSize = CSV MagazineSize"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetMagazineSizeAttribute()), Row.MagazineSize);
	TestEqual(TEXT("Ammo full"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()), Row.MagazineSize);
	TestEqual(TEXT("ReloadSpeed = CSV ReloadSpeed"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetReloadSpeedAttribute()), Row.ReloadSpeed);
	TestEqual(TEXT("CritChance = CSV CritChance"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritChanceAttribute()), Row.CritChance, 1e-5f);
	TestEqual(TEXT("CritDamage = CSV CritDamage"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetCritDamageAttribute()), Row.CritDamage, 1e-5f);
	TestEqual(TEXT("Pierce = CSV Pierce"), Shotgun->GetPierceCount(), PierceOf(Row));
	TestEqual(TEXT("BurstCount = CSV BurstCount"), Shotgun->GetBurstCount(), Row.BurstCount);
	TestEqual(TEXT("ReloadTime = CSV ReloadTime"), Shotgun->GetReloadTime(), Row.ReloadTime);
	TestEqual(TEXT("Range = CSV Range"), Shotgun->GetRange(), Row.Range);
	TestEqual(TEXT("spread = CSV SpreadDeg"), Shotgun->GetSpreadDeg(), Row.SpreadDeg);
	TestEqual(TEXT("pellets = CSV PelletCount"), Shotgun->GetPelletCount(), PelletsOf(Row));
	TestEqual(TEXT("knockback per pellet hit = CSV KnockbackForce"), Shotgun->GetKnockbackForce(), Row.KnockbackForce);
	TestTrue(TEXT("pellet speed set"), Shotgun->GetProjectileSpeed() > 0.f);
	TestEqual(TEXT("display name = CSV DisplayName"), Shotgun->GetDisplayName().ToString(), Row.DisplayName);
	TestEqual(TEXT("rifle display name = CSV DisplayName"), Rifle->GetDisplayName().ToString(), RifleRow.DisplayName);
	if (!TestTrue(TEXT("CSV shotgun: single shots, FireRate and MagazineSize positive"), Row.BurstCount == 1 && Row.FireRate > 0.f && Row.MagazineSize >= 1.f))
	{
		return false;
	}

	// Cadence and magazine on the accumulator alone, derived from the row: shots 1 / FireRate apart, a ReloadTime / ReloadSpeed
	// reload from the last shot of a magazine, the next magazine opening at its end. Two magazines and one shot are recorded.
	const int32 Magazine = FMath::RoundToInt32(Row.MagazineSize);
	const double Period = 1.0 / Row.FireRate;
	const double Reload = Row.ReloadTime / Row.ReloadSpeed;
	TArray<double> Expected;
	for (int32 Shot = 0; Shot < 2 * Magazine + 1; ++Shot)
	{
		const int32 MagazineIndex = Shot / Magazine;
		Expected.Add(MagazineIndex * ((Magazine - 1) * Period + Reload) + (Shot % Magazine) * Period);
	}
	URRAutoFireComponent* AutoFire = Shotgun->GetAutoFireComponent();
	constexpr float Step = 0.01f;
	const int32 MaxSteps = FMath::CeilToInt32(Expected.Last() / Step) + 100;
	TArray<double> Times;
	bool bReloadSeen = false;
	AutoFire->StartFiring();
	double Elapsed = 0.0;
	int32 LastCount = Shotgun->GetShotCount();
	for (int32 Index = 0; Index <= MaxSteps && Times.Num() < Expected.Num(); ++Index)
	{
		AutoFire->AdvanceTime(Index == 0 ? 0.f : Step);
		Elapsed += Index == 0 ? 0.0 : Step;
		if (Shotgun->GetShotCount() != LastCount)
		{
			Times.Add(Elapsed);
			LastCount = Shotgun->GetShotCount();
			TestEqual(TEXT("CSV PelletCount pellets per shot"), Shotgun->GetLastPelletsSpawned(), PelletsOf(Row));
			if (Times.Num() == Magazine)
			{
				TestEqual(TEXT("magazine empty after MagazineSize shots"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()), 0.f);
				TestTrue(TEXT("reloading after MagazineSize shots"), AutoFire->IsReloading());
				TestEqual(TEXT("reload lasts ReloadTime / ReloadSpeed"), AutoFire->GetReloadDuration(), static_cast<float>(Reload), 1e-4f);
				bReloadSeen = true;
			}
		}
	}
	AutoFire->StopFiring();
	TestTrue(TEXT("reload observed"), bReloadSeen);
	if (TestEqual(TEXT("two magazines and one shot recorded"), Times.Num(), Expected.Num()))
	{
		for (int32 Index = 0; Index < Expected.Num(); ++Index)
		{
			TestEqual(FString::Printf(TEXT("shot %d at %.2f s"), Index + 1, Expected[Index]), Times[Index], Expected[Index], Step + 1e-4);
		}
		TestEqual(TEXT("magazine refilled minus one"), ASC->GetNumericAttribute(URRWeaponAttributeSet::GetAmmoAttribute()), Row.MagazineSize - 1.f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunPelletFanTest, "RogueRigs.Shotgun.PelletFan",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunPelletFanTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunFan"));
	FShotgunSetup S;
	if (!SetUp(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, S))
	{
		return false;
	}
	TestEqual(TEXT("no pellet before the shot"), GetPellets(TestWorld).Num(), 0);

	const int32 Pellets = PelletsOf(S.Row);
	const float Spread = S.Row.SpreadDeg;
	FireOneShot(*this, S.Shotgun);
	TArray<ARRShotgunPellet*> Shot = GetPellets(TestWorld);
	if (!TestEqual(TEXT("CSV PelletCount pellets spawned"), Shot.Num(), Pellets) || !TestTrue(TEXT("CSV PelletCount above 1"), Pellets > 1))
	{
		return false;
	}
	const float AimYaw = S.Aim.Rotation().Yaw;
	TArray<float> Offsets;
	for (const ARRShotgunPellet* Pellet : Shot)
	{
		Offsets.Add(FRotator::NormalizeAxis(Pellet->GetActorRotation().Yaw - AimYaw));
		TestTrue(TEXT("pellet spawned at the shot origin"), Pellet->GetActorLocation().Equals(S.Origin, 0.5f));
		TestEqual(TEXT("pellet flies flat"), Pellet->GetActorRotation().Pitch, 0.0, 1e-3);
		TestEqual(TEXT("pellet pierces CSV Pierce robots"), Pellet->GetMaxHits(), PierceOf(S.Row));
		TestEqual(TEXT("pellet knockback = CSV KnockbackForce"), Pellet->GetKnockbackSpeed(), S.Row.KnockbackForce);
		TestEqual(TEXT("pellet owner: the shotgun"), Pellet->GetOwner(), static_cast<AActor*>(S.Shotgun));
		TestEqual(TEXT("pellet instigator: the miner"), Pellet->GetInstigator(), static_cast<APawn*>(S.Miner));
		TestEqual(TEXT("pellet lifetime = range / speed"), Pellet->GetLifeSpan(), S.Shotgun->GetRange() / S.Shotgun->GetProjectileSpeed(), 0.02f);
	}
	Offsets.Sort();
	FString OffsetText;
	for (int32 Index = 0; Index < Offsets.Num(); ++Index)
	{
		const float Expected = -Spread + Index * (2.f * Spread / (Pellets - 1));
		TestEqual(FString::Printf(TEXT("pellet %d yaw offset %.1f deg"), Index, Expected), Offsets[Index], Expected, 0.01f);
		TestEqual(FString::Printf(TEXT("GetPelletYawOffset(%d)"), Index), S.Shotgun->GetPelletYawOffset(Index), Expected, 1e-4f);
		OffsetText += FString::Printf(TEXT("%.1f "), Offsets[Index]);
	}
	AddInfo(FString::Printf(TEXT("pellet yaw offsets from the aim: %s"), *OffsetText));

	// Pellets expire at the range: none left after range / speed.
	const int32 Frames = TickUntilPelletsGone(TestWorld);
	TestEqual(TEXT("all pellets gone"), GetPellets(TestWorld).Num(), 0);
	TestTrue(TEXT("pellets lived about range / speed"), Frames * FrameSeconds <= S.Shotgun->GetRange() / S.Shotgun->GetProjectileSpeed() + 3.f * FrameSeconds);

	// Pierce is read at fire time: an upgrade (CSV Pierce + 2) reaches the next shot's pellets.
	const int32 UpgradedPierce = PierceOf(S.Row) + 2;
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetPierceAttribute(), static_cast<float>(UpgradedPierce));
	// Second shot of the magazine, 1 / FireRate after the first.
	URRAutoFireComponent* AutoFire = S.Shotgun->GetAutoFireComponent();
	const int32 ShotsBefore = S.Shotgun->GetShotCount();
	AutoFire->StartFiring();
	AutoFire->AdvanceTime(1.f / S.Row.FireRate + 1e-4f);
	AutoFire->StopFiring();
	TestEqual(TEXT("second shot fired"), S.Shotgun->GetShotCount(), ShotsBefore + 1);
	Shot = GetPellets(TestWorld);
	TestEqual(TEXT("CSV PelletCount pellets on the second shot"), Shot.Num(), Pellets);
	for (const ARRShotgunPellet* Pellet : Shot)
	{
		TestEqual(TEXT("upgraded pellet pierces CSV Pierce + 2"), Pellet->GetMaxHits(), UpgradedPierce);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunPierceTest, "RogueRigs.Shotgun.Pierce",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunPierceTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunPierce"));
	FShotgunSetup S;
	if (!SetUp(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, S))
	{
		return false;
	}
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);

	// Four robots on the center ray from 460 uu: the +/-5 deg pellets pass > 40 uu beside them (chaser radius 24 + pellet 10), so only the center pellet meets them.
	const float Distances[] = { 460.f, 515.f, 570.f, 625.f };
	const int32 Pierce = PierceOf(S.Row);
	TestTrue(TEXT("CSV Pierce leaves the last robot of the line untouched"), Pierce < UE_ARRAY_COUNT(Distances));
	const float StartHealth = SurvivingHealth(S.Row, 1);
	TArray<ARRChaser*> Line;
	TArray<int32> Hits;
	Hits.Init(0, UE_ARRAY_COUNT(Distances));
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Distances); ++Index)
	{
		ARRChaser* Chaser = SpawnOnRay(*this, TestWorld, S, Distances[Index], 0.f);
		if (!Chaser)
		{
			return false;
		}
		SetRobotHealth(Chaser, StartHealth);
		CountHits(Chaser, Hits[Index]);
		Line.Add(Chaser);
	}
	FireOneShot(*this, S.Shotgun);
	TickUntilPelletsGone(TestWorld);

	for (int32 Index = 0; Index < Line.Num(); ++Index)
	{
		const bool bPierced = Index < Pierce;
		TestEqual(FString::Printf(TEXT("robot %d at %.0f uu: %s"), Index, Distances[Index], bPierced ? TEXT("hit once") : TEXT("not hit (CSV Pierce spent)")),
			Hits[Index], bPierced ? 1 : 0);
		TestEqual(FString::Printf(TEXT("robot %d health"), Index), GetHealth(Line[Index]), bPierced ? StartHealth - S.Row.Damage : StartHealth);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunRockStopsPelletsTest, "RogueRigs.Shotgun.RockStopsPellets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunRockStopsPelletsTest::RunTest(const FString& Parameters)
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
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.f);

	// Rock face 300 uu ahead of the miner, 100 uu half-width: the +/-15 deg pellets meet it within 85 uu of its center line.
	const FVector MinerLocation = S.Miner->GetActorLocation();
	TestEqual(TEXT("grid: clear distance west = 300 (rock face)"), Grid->GetClearDistance(MinerLocation, -FVector::ForwardVector, S.Shotgun->GetRange()), 300.f, 0.01f);
	ARRChaser* Hidden = SpawnFrozenChaser(*this, TestWorld, Grid->GetCellCenter(BehindRockCell.X, BehindRockCell.Y));
	ARRChaser* Front = SpawnOnRay(*this, TestWorld, S, 120.f, 0.f);
	if (!Hidden || !Front)
	{
		return false;
	}
	// Front takes every pellet at 120 uu (PelletCount x Damage): health raised from the row keeps it alive for the check.
	const int32 Pellets = PelletsOf(S.Row);
	const float FrontHealth = SurvivingHealth(S.Row, Pellets);
	SetRobotHealth(Front, FrontHealth);
	const float HiddenHealth = GetHealth(Hidden);
	TestTrue(TEXT("hidden robot within the range"), FVector::Dist2D(S.Origin, Hidden->GetActorLocation()) < S.Shotgun->GetRange());

	FireOneShot(*this, S.Shotgun);
	const int32 Frames = TickUntilPelletsGone(TestWorld);
	AddInfo(FString::Printf(TEXT("pellets gone after %d frames"), Frames));
	TestTrue(TEXT("pellets stopped on the rock before their range"), Frames * FrameSeconds < S.Shotgun->GetRange() / S.Shotgun->GetProjectileSpeed());
	TestEqual(TEXT("robot behind the rock not hit"), GetHealth(Hidden), HiddenHealth);
	TestTrue(TEXT("robot behind the rock not pushed"), Hidden->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	TestEqual(TEXT("robot in front of the rock hit by every pellet"), GetHealth(Front), FrontHealth - Pellets * S.Row.Damage);
	TestTrue(TEXT("the rock is not destroyed by pellets"), Grid->GetCellType(RockCellWest) == ERRCellType::Rock);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunCritPerPelletTest, "RogueRigs.Shotgun.CritPerPellet",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunCritPerPelletTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	FTestWorld TestWorld(TEXT("RRShotgunCrits"));
	FShotgunSetup S;
	if (!SetUp(*this, TestWorld, OpenMinerCell, FVector::ForwardVector, S))
	{
		return false;
	}
	SetShotgunAttribute(S.Shotgun, URRWeaponAttributeSet::GetCritChanceAttribute(), 0.5f);

	// One robot on each pellet ray at 560 uu (rays 49 uu apart there): each robot takes exactly its own pellet.
	const int32 Pellets = PelletsOf(S.Row);
	const float StartHealth = SurvivingHealth(S.Row, 1);
	TArray<ARRChaser*> Chasers;
	TArray<int32> HitCounts;
	TArray<bool> CritFlags;
	TArray<float> Amounts;
	HitCounts.Init(0, Pellets);
	CritFlags.Init(false, Pellets);
	Amounts.Init(0.f, Pellets);
	for (int32 Index = 0; Index < Pellets; ++Index)
	{
		ARRChaser* Chaser = SpawnOnRay(*this, TestWorld, S, 560.f, S.Shotgun->GetPelletYawOffset(Index));
		if (!Chaser)
		{
			return false;
		}
		SetRobotHealth(Chaser, StartHealth);
		Chaser->GetHealthSet()->OnDamageReceived.AddLambda([&HitCounts, &CritFlags, &Amounts, Index](float Damage, bool bIsCritical, float)
		{
			++HitCounts[Index];
			CritFlags[Index] = bIsCritical;
			Amounts[Index] = Damage;
		});
		Chasers.Add(Chaser);
	}

	// Seeded global stream: URRDamageExecution rolls FMath::FRand once per pellet hit.
	FMath::RandInit(20261005);
	FireOneShot(*this, S.Shotgun);
	TickUntilPelletsGone(TestWorld);
	int32 Crits = 0;
	FString Pattern;
	for (int32 Index = 0; Index < Chasers.Num(); ++Index)
	{
		TestEqual(FString::Printf(TEXT("robot %d hit by one pellet"), Index), HitCounts[Index], 1);
		const float Expected = CritFlags[Index] ? S.Row.Damage * S.Row.CritDamage : S.Row.Damage;
		TestEqual(FString::Printf(TEXT("robot %d damage matches its own roll (%s %g)"), Index, CritFlags[Index] ? TEXT("crit") : TEXT("normal"), Expected),
			Amounts[Index], Expected, 1e-3f);
		TestEqual(FString::Printf(TEXT("robot %d health"), Index), GetHealth(Chasers[Index]), StartHealth - Expected, 1e-3f);
		Crits += CritFlags[Index] ? 1 : 0;
		Pattern += CritFlags[Index] ? TEXT("C") : TEXT("n");
	}
	AddInfo(FString::Printf(TEXT("crit pattern (ray order) %s: %d / %d"), *Pattern, Crits, Chasers.Num()));
	TestTrue(TEXT("independent rolls: some pellet hits crit, some do not"), Crits > 0 && Crits < Chasers.Num());
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

	// Rays of the CSV fan (PelletCount over +/-SpreadDeg): the center pellet, its neighbours, the second neighbours and the outer one.
	const int32 Center = PelletsOf(S.Row) / 2;
	const float CenterRay = S.Shotgun->GetPelletYawOffset(Center);
	const float LeftRay = S.Shotgun->GetPelletYawOffset(Center - 1);
	const float RightRay = S.Shotgun->GetPelletYawOffset(Center + 1);
	const float SecondRightRay = S.Shotgun->GetPelletYawOffset(Center + 2);
	const float SecondLeftRay = S.Shotgun->GetPelletYawOffset(Center - 2);
	const float OuterRay = S.Shotgun->GetPelletYawOffset(0);
	AddInfo(FString::Printf(TEXT("rays: center %.1f, neighbours %.1f / %.1f, second %.1f / %.1f, outer %.1f deg"), CenterRay, LeftRay, RightRay,
		SecondLeftRay, SecondRightRay, OuterRay));

	// Far robots (560 uu) each take the one pellet of their ray; Close (250 uu, center) takes the center pellet and its two neighbours
	// (the second neighbours pass 43 uu beside it with the 5 deg spacing of the CSV fan).
	ARRChaser* Full = SpawnOnRay(*this, TestWorld, S, 560.f, CenterRay);
	ARRChaser* Quarter = SpawnOnRay(*this, TestWorld, S, 560.f, SecondRightRay);
	ARRChaser* Immune = SpawnOnRay(*this, TestWorld, S, 560.f, SecondLeftRay);
	ARRChaser* Doomed = SpawnOnRay(*this, TestWorld, S, 560.f, OuterRay);
	ARRChaser* Close = SpawnOnRay(*this, TestWorld, S, 250.f, CenterRay);
	if (!Full || !Quarter || !Immune || !Doomed || !Close)
	{
		return false;
	}
	TestEqual(TEXT("CSV knockback resistance 0"), Full->GetKnockbackResistance(), 0.f);
	FEnemyAccess::StatsOf(Quarter).KnockbackResistance = 0.25f;
	FEnemyAccess::StatsOf(Immune).KnockbackResistance = 1.f;
	// Health from the row: one pellet leaves Full alive, three leave Close alive, one kills Doomed.
	const float Knockback = S.Row.KnockbackForce;
	const float FullHealth = SurvivingHealth(S.Row, 1);
	const float CloseHealth = SurvivingHealth(S.Row, 3);
	SetRobotHealth(Full, FullHealth);
	SetRobotHealth(Quarter, FullHealth);
	SetRobotHealth(Immune, FullHealth);
	SetRobotHealth(Close, CloseHealth);
	Doomed->GetAbilitySystemComponent()->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 0.5f * S.Row.Damage);
	int32 CloseHits = 0;
	CountHits(Close, CloseHits);
	TWeakObjectPtr<ARRChaser> WeakDoomed(Doomed);

	FireOneShot(*this, S.Shotgun);
	TickUntilPelletsGone(TestWorld);

	// Frozen robots keep their pending launch (CharacterMovement does not tick).
	const FVector FullLaunch = Full->GetCharacterMovement()->PendingLaunchVelocity;
	const FVector QuarterLaunch = Quarter->GetCharacterMovement()->PendingLaunchVelocity;
	const FVector CloseLaunch = Close->GetCharacterMovement()->PendingLaunchVelocity;
	AddInfo(FString::Printf(TEXT("launch full %s, quarter-resistant %s, close (%d hits) %s"), *FullLaunch.ToCompactString(), *QuarterLaunch.ToCompactString(),
		CloseHits, *CloseLaunch.ToCompactString()));
	TestTrue(TEXT("resistance 0: CSV KnockbackForce along its pellet"), FullLaunch.Equals(RayDirection(S, CenterRay) * Knockback, 0.05f));
	TestTrue(TEXT("resistance 0.25: 0.75 x CSV KnockbackForce along its pellet"), QuarterLaunch.Equals(RayDirection(S, SecondRightRay) * Knockback * 0.75f, 0.05f));
	TestTrue(TEXT("resistance 1: no knockback"), Immune->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	TestEqual(TEXT("robot hit by one pellet: CSV Damage"), GetHealth(Full), FullHealth - S.Row.Damage);
	if (WeakDoomed.IsValid())
	{
		TestTrue(TEXT("robot killed by its pellet: dead"), WeakDoomed->IsDead());
		TestTrue(TEXT("robot killed by its pellet: not pushed"), WeakDoomed->GetCharacterMovement()->PendingLaunchVelocity.IsNearlyZero());
	}

	// Several pellets in the same frame stack their pushes: the center pellet and its two neighbours.
	TestEqual(TEXT("close robot hit by 3 pellets"), CloseHits, 3);
	const FVector CloseExpected = (RayDirection(S, CenterRay) + RayDirection(S, RightRay) + RayDirection(S, LeftRay)) * Knockback;
	TestTrue(TEXT("close robot: pushes of the 3 pellets add up"), CloseLaunch.Equals(CloseExpected, 0.1f));
	TestEqual(TEXT("close robot took 3 x CSV Damage"), GetHealth(Close), CloseHealth - 3.f * S.Row.Damage);

	// The launch is applied by CharacterMovement once the robot ticks again: it moves away from the miner.
	const double StartDistance = FVector::Dist2D(Full->GetActorLocation(), S.Origin);
	Full->GetCharacterMovement()->SetComponentTickEnabled(true);
	for (int32 Frame = 0; Frame < 3; ++Frame)
	{
		TestWorld.Tick();
	}
	const double EndDistance = FVector::Dist2D(Full->GetActorLocation(), S.Origin);
	AddInfo(FString::Printf(TEXT("knocked robot: %.1f -> %.1f uu from the shot origin after 3 frames"), StartDistance, EndDistance));
	TestTrue(TEXT("knocked robot moved away from the miner"), EndDistance > StartDistance + 5.0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRShotgunCardsOnlyWhenEquippedTest, "RogueRigs.Shotgun.CardsOnlyWhenEquipped",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRShotgunCardsOnlyWhenEquippedTest::RunTest(const FString& Parameters)
{
	using namespace RRShotgunTests;

	constexpr int32 Offers = 200;
	const FRRWeaponStatsRow Row = ReadShotgunRow(*this);
	for (int32 Pass = 0; Pass < 2; ++Pass)
	{
		const bool bEquipped = Pass == 1;
		FTestWorld TestWorld(TEXT("RRShotgunCards"));
		ARRArenaGrid* Grid = SpawnGrid(*this, TestWorld);
		ARRMiner* Miner = nullptr;
		if (Grid)
		{
			Miner = bEquipped ? SpawnMiner(*this, TestWorld, Grid, OpenMinerCell, M1TestLoadout) : SpawnMiner(*this, TestWorld, Grid, OpenMinerCell, KeepRifleOnly);
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
						TestEqual(TEXT("shotgun card target text = CSV DisplayName"), Card.TargetText.ToString(), Row.DisplayName);
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
		const float Expected = Card.Operation == ERRUpgradeOperation::MultiplyAdditive ? Row.Damage * (1.f + Card.Value) : Row.Damage + Card.Value;
		TestEqual(TEXT("shotgun damage raised"), Shotgun->GetAbilitySystemComponent()->GetNumericAttribute(Damage), Expected, 1e-3f);
		TestEqual(TEXT("rifle damage unchanged"), Rifle->GetAbilitySystemComponent()->GetNumericAttribute(Damage), RifleBefore);
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
