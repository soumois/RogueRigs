// Rogue Rigs - tunables of the intermission shop (run-shop, task-316-01): number of cards and ore price of a card per rarity tier.

#include "Run/RRShopSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRShopSettings)

int32 URRShopSettings::GetPriceForTier(ERRUpgradeRarity Tier) const
{
	int32 Price = PriceGrey;
	switch (Tier)
	{
	case ERRUpgradeRarity::Grey:   Price = PriceGrey; break;
	case ERRUpgradeRarity::Green:  Price = PriceGreen; break;
	case ERRUpgradeRarity::Blue:   Price = PriceBlue; break;
	case ERRUpgradeRarity::Purple: Price = PricePurple; break;
	case ERRUpgradeRarity::Yellow: Price = PriceYellow; break;
	}
	return FMath::Max(0, Price);
}
