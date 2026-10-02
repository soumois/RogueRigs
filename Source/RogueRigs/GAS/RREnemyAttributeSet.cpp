// Rogue Rigs - enemy-only attributes: AttackPower, the multiplier of the flat damage an enemy deals.

#include "GAS/RREnemyAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RREnemyAttributeSet)

URREnemyAttributeSet::URREnemyAttributeSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitAttackPower(1.f);
}

void URREnemyAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetAttackPowerAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void URREnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetAttackPowerAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}
