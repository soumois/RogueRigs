// Rogue Rigs - reroll cost rule shared by the level-up reroll and the intermission shop reroll: base cost plus a step per reroll.

#include "Upgrades/RRRerollCost.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRRerollCost)

int32 URRRerollCostLibrary::GetRerollCostForCount(int32 RerollCount)
{
	const URRRerollSettings* Settings = GetDefault<URRRerollSettings>();
	return FMath::Max(0, Settings->RerollBaseCost + FMath::Max(0, RerollCount) * Settings->RerollCostStep);
}
