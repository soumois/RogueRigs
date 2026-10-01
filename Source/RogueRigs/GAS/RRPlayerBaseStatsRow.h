// Rogue Rigs - DataTable row holding the miner's base stats (PlayerBaseStats.csv).

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RRPlayerBaseStatsRow.generated.h"

/**
 * One row of Content/RogueRigs/Data/Tables/PlayerBaseStats.csv. Column names match the property
 * names; the first column is the row name. Applied as base values on the player's ASC by
 * FRRBaseStatsLoader::ApplyPlayerBaseStats (Health starts at MaxHealth).
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRPlayerBaseStatsRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Base MaxHealth (URRHealthAttributeSet), in health points. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats")
	float MaxHealth = 0.f;

	/** Base MoveSpeed (URRMovementAttributeSet), in uu/s. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats")
	float MoveSpeed = 0.f;

	/** Base PickupRadius (URRPlayerAttributeSet), in uu. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats")
	float PickupRadius = 0.f;

	/** Base XPGain multiplier (URRPlayerAttributeSet). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats")
	float XPGain = 0.f;

	/** Base MiningSpeed multiplier (URRPlayerAttributeSet). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats")
	float MiningSpeed = 0.f;

	/** Base Luck points (URRPlayerAttributeSet). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Stats")
	float Luck = 0.f;
};
