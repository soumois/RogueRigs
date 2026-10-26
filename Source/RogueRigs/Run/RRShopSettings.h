// Rogue Rigs - tunables of the intermission shop (run-shop, task-316-01): number of cards and ore price of a card per rarity tier.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
// RRUpgradeCard.h converts TSubclassOf<ARRWeapon> inline (IsSameChoice): the complete type is needed by every includer.
#include "Upgrades/RRUpgradeCard.h"
#include "Weapons/RRWeapon.h"
#include "RRShopSettings.generated.h"

/**
 * Data of the intermission shop (run-shop; prices owner 2026-10-26, provisional): the card count of an offer and the ore price of
 * a card by its rarity tier. The reroll cost is not here: the shop shares URRRerollSettings with the level-up reroll. Read with
 * GetDefault; not a UDeveloperSettings (same choice as URRRerollSettings and URRRunSettings). Override without code in
 * Config/DefaultGame.ini, section [/Script/RogueRigs.RRShopSettings].
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRShopSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Cards of a shop offer (run-shop: 5). */
	UPROPERTY(Config, EditAnywhere, Category = "Shop", meta = (ClampMin = 1))
	int32 ShopCardCount = 5;

	/** Ore price of a grey card ("blanche"). */
	UPROPERTY(Config, EditAnywhere, Category = "Shop|Prices", meta = (ClampMin = 0))
	int32 PriceGrey = 25;

	/** Ore price of a green card ("verte"). */
	UPROPERTY(Config, EditAnywhere, Category = "Shop|Prices", meta = (ClampMin = 0))
	int32 PriceGreen = 40;

	/** Ore price of a blue card ("bleue"). */
	UPROPERTY(Config, EditAnywhere, Category = "Shop|Prices", meta = (ClampMin = 0))
	int32 PriceBlue = 60;

	/** Ore price of a purple card ("épique"). */
	UPROPERTY(Config, EditAnywhere, Category = "Shop|Prices", meta = (ClampMin = 0))
	int32 PricePurple = 80;

	/** Ore price of a yellow card ("légendaire"). */
	UPROPERTY(Config, EditAnywhere, Category = "Shop|Prices", meta = (ClampMin = 0))
	int32 PriceYellow = 100;

	/** Ore price of a card of Tier (the matching Price* value, never negative). */
	int32 GetPriceForTier(ERRUpgradeRarity Tier) const;
};
