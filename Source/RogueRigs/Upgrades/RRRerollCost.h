// Rogue Rigs - reroll cost rule shared by the level-up reroll and the intermission shop reroll: base cost plus a step per reroll.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Object.h"
#include "RRRerollCost.generated.h"

/**
 * Tunables of the ore-paid reroll (xp-level-up, run-shop; owner 2026-10-26, provisional): the first reroll costs
 * RerollBaseCost, each further reroll RerollCostStep more (15, 20, 25...). Each caller keeps its own reroll count (the level-up
 * screen, the intermission shop) and asks URRRerollCostLibrary::GetRerollCostForCount. Read with GetDefault; not a
 * UDeveloperSettings (same choice as URRIconSettings and URRRunSettings). Override without code in Config/DefaultGame.ini,
 * section [/Script/RogueRigs.RRRerollSettings].
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRRerollSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Ore cost of the first reroll (count 0). */
	UPROPERTY(Config, EditAnywhere, Category = "Reroll", meta = (ClampMin = 0))
	int32 RerollBaseCost = 15;

	/** Extra ore for each further reroll without a reset of the count. */
	UPROPERTY(Config, EditAnywhere, Category = "Reroll", meta = (ClampMin = 0))
	int32 RerollCostStep = 5;
};

/** Blueprint-readable reroll cost rule, the single place that turns a reroll count into an ore cost. */
UCLASS()
class ROGUERIGS_API URRRerollCostLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Ore cost of the next reroll after RerollCount rerolls since the last reset: RerollBaseCost + RerollCount x RerollCostStep. */
	UFUNCTION(BlueprintPure, Category = "Rogue Rigs|Reroll")
	static int32 GetRerollCostForCount(int32 RerollCount);
};
