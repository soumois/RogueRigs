// Rogue Rigs - test helper: reads the expected stat values of an automation test from the CSV row the game reads (Content/RogueRigs/Data/Tables), so a value lives only in its CSV (task-420).

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Enemies/RREnemy.h"
#include "Enemies/RREnemyStatsRow.h"
#include "Explosions/RRExplosionStatsRow.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RRPlayerBaseStatsRow.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RROreTypeRow.h"
#include "Progression/RRProgressionComponent.h"
#include "Progression/RRXPCurveRow.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Upgrades/RRUpgradeRarityRow.h"
#include "Upgrades/RRUpgradeValueRow.h"
#include "Weapons/RRWeapon.h"
#include "Weapons/RRWeaponStatsRow.h"

/**
 * Test access to the stat CSV rows through the runtime cache (FRRBaseStatsLoader::FindRow, the same parse the game uses).
 * A test never repeats a CSV value: it reads the row, checks the actor against it and derives the rest from it
 * (crit hit = Damage x CritDamage, shot times from FireRate, health raised so a target survives N hits). A hand edit of
 * a CSV value then keeps the tests passing.
 */
namespace RRTestStats
{
	/** Row name of the burst rifle in WeaponStats.csv. */
	constexpr const TCHAR* BurstRifleRow = TEXT("BurstRifle");

	/** Row name of the shotgun in WeaponStats.csv. */
	constexpr const TCHAR* ShotgunRow = TEXT("Shotgun");

	/** Row name of the turret launcher in WeaponStats.csv. */
	constexpr const TCHAR* TurretLauncherRow = TEXT("TurretLauncher");

	/** Row name of the chaser in EnemyStats.csv. */
	constexpr const TCHAR* ChaserRow = TEXT("Chaser");

	/** Row name of the kamikaze in EnemyStats.csv and ExplosionStats.csv (its blast). */
	constexpr const TCHAR* KamikazeRow = TEXT("Kamikaze");

	/** Row name of the shooter in EnemyStats.csv. */
	constexpr const TCHAR* ShooterRow = TEXT("Shooter");

	/** Row name of the heavy in EnemyStats.csv. */
	constexpr const TCHAR* HeavyRow = TEXT("Heavy");

	/** Row name of the shield chaser in EnemyStats.csv. */
	constexpr const TCHAR* ShieldChaserRow = TEXT("ShieldChaser");

	/** Row name of the miner in PlayerBaseStats.csv. */
	constexpr const TCHAR* MinerRow = TEXT("Miner");

	/** Row name of the barrel blast in ExplosionStats.csv. */
	constexpr const TCHAR* BarrelRow = TEXT("Barrel");

	/** Row name of the duskite ore in OreTypes.csv. */
	constexpr const TCHAR* DuskiteRow = TEXT("Duskite");

	/** Row name of the heal ore in OreTypes.csv. */
	constexpr const TCHAR* HealOreRow = TEXT("Heal");

	/**
	 * Copies row RowName of the CSV at ContentRelativePath (relative to the project Content folder) into OutRow. RowType is
	 * the USTRUCT row of that table. Adds an error to Test and returns false when the file or the row is missing.
	 */
	template <typename RowType>
	bool ReadRow(FAutomationTestBase& Test, const FString& ContentRelativePath, FName RowName, RowType& OutRow)
	{
		if (FRRBaseStatsLoader::FindRow(ContentRelativePath, RowName, OutRow, TEXT("automation test")))
		{
			return true;
		}
		Test.AddError(FString::Printf(TEXT("CSV row '%s' missing in %s"), *RowName.ToString(), *ContentRelativePath));
		return false;
	}

	/** Row RowName of WeaponStats.csv (ARRWeapon::WeaponStatsPath); a default row (zeros) and a test error when missing. */
	inline FRRWeaponStatsRow WeaponRow(FAutomationTestBase& Test, FName RowName)
	{
		FRRWeaponStatsRow Row;
		ReadRow(Test, ARRWeapon::WeaponStatsPath, RowName, Row);
		return Row;
	}

	/** Row RowName of EnemyStats.csv (ARREnemy::EnemyStatsPath); a default row (zeros) and a test error when missing. */
	inline FRREnemyStatsRow EnemyRow(FAutomationTestBase& Test, FName RowName)
	{
		FRREnemyStatsRow Row;
		ReadRow(Test, ARREnemy::EnemyStatsPath, RowName, Row);
		return Row;
	}

	/** Row RowName of PlayerBaseStats.csv (default Miner); a default row (zeros) and a test error when missing. */
	inline FRRPlayerBaseStatsRow PlayerRow(FAutomationTestBase& Test, FName RowName = MinerRow)
	{
		FRRPlayerBaseStatsRow Row;
		ReadRow(Test, FRRBaseStatsLoader::PlayerBaseStatsPath, RowName, Row);
		return Row;
	}

	/** Row RowName of ExplosionStats.csv (Barrel, Kamikaze); a default row (zeros) and a test error when missing. */
	inline FRRExplosionStatsRow ExplosionRow(FAutomationTestBase& Test, FName RowName)
	{
		FRRExplosionStatsRow Row;
		ReadRow(Test, URRExplosionSubsystem::ExplosionStatsPath, RowName, Row);
		return Row;
	}

	/** Row RowName of OreTypes.csv (Duskite, Heal); a default row and a test error when missing. */
	inline FRROreTypeRow OreRow(FAutomationTestBase& Test, FName RowName)
	{
		FRROreTypeRow Row;
		ReadRow(Test, ARRArenaGrid::OreTypesPath, RowName, Row);
		return Row;
	}

	/** Row RowName of UpgradeValues.csv (e.g. Damage_Grey, Pierce_Blue); a default row and a test error when missing. */
	inline FRRUpgradeValueRow UpgradeValueRow(FAutomationTestBase& Test, FName RowName)
	{
		FRRUpgradeValueRow Row;
		ReadRow(Test, URRUpgradeComponent::UpgradeValuesPath, RowName, Row);
		return Row;
	}

	/** Row "<Stat>_<Tier>" of UpgradeValues.csv (e.g. Damage_Blue); a default row and a test error when missing. */
	inline FRRUpgradeValueRow UpgradeValueRow(FAutomationTestBase& Test, ERRUpgradeStat Stat, ERRUpgradeRarity Tier)
	{
		const FString Name = StaticEnum<ERRUpgradeStat>()->GetNameStringByValue(static_cast<int64>(Stat)) + TEXT("_")
			+ StaticEnum<ERRUpgradeRarity>()->GetNameStringByValue(static_cast<int64>(Tier));
		return UpgradeValueRow(Test, FName(*Name));
	}

	/** Card value of Stat at Tier read from UpgradeValues.csv: GlobalValue, times TargetedMultiplier for a one-weapon card. */
	inline float UpgradeCardValue(FAutomationTestBase& Test, ERRUpgradeStat Stat, ERRUpgradeRarity Tier, bool bTargeted)
	{
		const FRRUpgradeValueRow Row = UpgradeValueRow(Test, Stat, Tier);
		return bTargeted ? Row.GlobalValue * Row.TargetedMultiplier : Row.GlobalValue;
	}

	/** Row RowName of UpgradeRarities.csv (Grey to Yellow); a default row (zeros) and a test error when missing. */
	inline FRRUpgradeRarityRow RarityRow(FAutomationTestBase& Test, FName RowName)
	{
		FRRUpgradeRarityRow Row;
		ReadRow(Test, URRUpgradeComponent::UpgradeRaritiesPath, RowName, Row);
		return Row;
	}

	/** Row of UpgradeRarities.csv for Tier (row name = tier name); a default row (zeros) and a test error when missing. */
	inline FRRUpgradeRarityRow RarityRow(FAutomationTestBase& Test, ERRUpgradeRarity Tier)
	{
		return RarityRow(Test, FName(*StaticEnum<ERRUpgradeRarity>()->GetNameStringByValue(static_cast<int64>(Tier))));
	}

	/** XP needed to leave Level (XPCurve.csv row named by the level number, 1-based); 0 and a test error when missing. */
	inline float XPToNextLevel(FAutomationTestBase& Test, int32 Level)
	{
		FRRXPCurveRow Row;
		ReadRow(Test, URRProgressionComponent::XPCurvePath, FName(*FString::FromInt(Level)), Row);
		return Row.XPToNextLevel;
	}
}

#endif // WITH_DEV_AUTOMATION_TESTS
