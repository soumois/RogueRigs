// Rogue Rigs - movement attributes shared by the player and enemies.

#include "GAS/RRMovementAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRMovementAttributeSet)

URRMovementAttributeSet::URRMovementAttributeSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitMoveSpeed(0.f);
}

void URRMovementAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void URRMovementAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}
