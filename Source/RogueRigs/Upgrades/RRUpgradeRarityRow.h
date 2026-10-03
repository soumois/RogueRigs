// Rogue Rigs - DataTable row of the rarity draw table (UpgradeRarities.csv): base weight and luck factor of one tier.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RRUpgradeRarityRow.generated.h"

/**
 * One row of Content/RogueRigs/Data/Tables/UpgradeRarities.csv; row name = tier (Grey, Green, Blue, Purple, Yellow).
 * Draw weight of the tier = BaseWeight x max(0, 1 + LuckFactor x Luck)^rank, rank 0 (Grey) to 4 (Yellow), then
 * normalized (rarities: one shared k = 0.1, kept per row so each tier can be tuned). A missing tier has weight 0.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRUpgradeRarityRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Weight of the tier at luck 0 (>= 0). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade Rarity")
	float BaseWeight = 0.f;

	/** k of the draw formula: how much each luck point raises the tier, compounded by its rank. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Upgrade Rarity")
	float LuckFactor = 0.f;
};
