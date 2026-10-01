// Rogue Rigs - runtime CSV loader: reads stat tables from Content into DataTables and applies base values on an ASC.

#pragma once

#include "CoreMinimal.h"

class UAbilitySystemComponent;
class UDataTable;
class UObject;
class UScriptStruct;
struct FRRPlayerBaseStatsRow;

/**
 * Reads CSV files under the project Content directory at runtime (UDataTable::CreateTableFromCSVString,
 * no imported asset) and applies rows as attribute base values. The CSV folder must be staged as a
 * non-asset directory to exist in packaged builds. Problems are logged on LogRogueRigs.
 */
class ROGUERIGS_API FRRBaseStatsLoader
{
public:
	/** Path of the player base stats CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* PlayerBaseStatsPath;

	/**
	 * Reads the CSV at ContentRelativePath into a new transient DataTable of RowStruct (must derive from
	 * FTableRowBase). Returns nullptr if the file is unreadable or yields no row; import problems are logged.
	 * The table is not rooted: keep it in a UPROPERTY or copy the rows out before the next GC.
	 */
	static UDataTable* LoadTableFromCSV(const FString& ContentRelativePath, UScriptStruct* RowStruct, UObject* Outer = nullptr);

	/** Loads PlayerBaseStats.csv and copies row RowName into OutRow. Returns false (logged) if the file or row is missing. */
	static bool LoadPlayerBaseStats(FName RowName, FRRPlayerBaseStatsRow& OutRow);

	/**
	 * Sets Row as base values on ASC: MaxHealth, then Health = MaxHealth, MoveSpeed, PickupRadius, XPGain,
	 * MiningSpeed, Luck. The ASC must already own URRHealthAttributeSet, URRMovementAttributeSet and
	 * URRPlayerAttributeSet; attributes of a missing set are skipped and the call returns false (logged).
	 */
	static bool ApplyPlayerBaseStats(UAbilitySystemComponent& ASC, const FRRPlayerBaseStatsRow& Row);

	/** Convenience: LoadPlayerBaseStats then ApplyPlayerBaseStats. Returns false if either step fails. */
	static bool LoadAndApplyPlayerBaseStats(UAbilitySystemComponent& ASC, FName RowName);

protected:
	/** Sets the base value of Attribute on ASC if the ASC owns its set; logs and returns false otherwise. */
	static bool SetBaseIfPresent(UAbilitySystemComponent& ASC, const struct FGameplayAttribute& Attribute, float Value);
};
