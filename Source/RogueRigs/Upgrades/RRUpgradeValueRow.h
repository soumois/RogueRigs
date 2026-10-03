// Rogue Rigs - DataTable row of the upgrade value table (UpgradeValues.csv): fixed value of one stat at one rarity tier.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Upgrades/RRUpgradeCard.h"
#include "RRUpgradeValueRow.generated.h"

/**
 * One row of Content/RogueRigs/Data/Tables/UpgradeValues.csv, one row per (stat, tier); row name <Stat>_<Tier> (e.g.
 * Damage_Blue). GlobalValue is the value of a player-stat card or an all-weapons card; a one-weapon card gives
 * GlobalValue x TargetedMultiplier (unused for player stats). A stat is offered only when it has a row for every tier.
 * Read once by URRUpgradeComponent.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRUpgradeValueRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Stat of the row. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade Value")
	ERRUpgradeStat Stat = ERRUpgradeStat::Damage;

	/** Rarity tier of the row. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade Value")
	ERRUpgradeRarity Tier = ERRUpgradeRarity::Grey;

	/** Value in attribute units for a player or all-weapons card (0.12 = +12 % for a percentage stat). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade Value")
	float GlobalValue = 0.f;

	/** Factor applied to GlobalValue for a one-weapon card (upgrades: 1.5). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade Value")
	float TargetedMultiplier = 1.f;

	/** GAS operation: AddBase for points, MultiplyAdditive for percentages. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade Value")
	ERRUpgradeOperation Operation = ERRUpgradeOperation::AddBase;
};
