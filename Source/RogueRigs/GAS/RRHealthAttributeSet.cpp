// Rogue Rigs - health attributes shared by the player and enemies.

#include "GAS/RRHealthAttributeSet.h"

#include "GameplayEffectExtension.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRHealthAttributeSet)

URRHealthAttributeSet::URRHealthAttributeSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitHealth(0.f);
	InitMaxHealth(0.f);
	InitIncomingDamage(0.f);
}

void URRHealthAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute() || Attribute == GetIncomingDamageAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void URRHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute() || Attribute == GetIncomingDamageAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void URRHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && GetHealth() > NewValue)
	{
		SetHealth(NewValue);
	}
}

void URRHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float Damage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (Damage > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth() - Damage, 0.f, GetMaxHealth()));
		}
	}
}
