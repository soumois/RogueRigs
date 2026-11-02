// Rogue Rigs - test helper: reads the expected stat values of an automation test from the CSV row the game reads (Content/RogueRigs/Data/Tables), so a value lives only in its CSV (task-420).

#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Enemies/RREnemy.h"
#include "Enemies/RREnemyStatsRow.h"
#include "GAS/RRBaseStatsLoader.h"
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
}

#endif // WITH_DEV_AUTOMATION_TESTS
