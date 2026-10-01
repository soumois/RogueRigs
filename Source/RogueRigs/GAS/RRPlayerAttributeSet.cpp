// Rogue Rigs - attributes owned by the miner only.

#include "GAS/RRPlayerAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRPlayerAttributeSet)

URRPlayerAttributeSet::URRPlayerAttributeSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitPickupRadius(0.f);
	InitXPGain(0.f);
	InitMiningSpeed(0.f);
	InitLuck(0.f);
}

bool URRPlayerAttributeSet::IsNonNegativeAttribute(const FGameplayAttribute& Attribute)
{
	return Attribute == GetPickupRadiusAttribute()
		|| Attribute == GetXPGainAttribute()
		|| Attribute == GetMiningSpeedAttribute();
}

void URRPlayerAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (IsNonNegativeAttribute(Attribute))
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void URRPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (IsNonNegativeAttribute(Attribute))
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}
