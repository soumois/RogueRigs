// Rogue Rigs - health attributes shared by the player and enemies.

#include "GAS/RRHealthAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GAS/RRGameplayTags.h"

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
			const float HealthBefore = GetHealth();
			SetHealth(FMath::Clamp(HealthBefore - Damage, 0.f, GetMaxHealth()));

			// Single death (sys-damage step 6): only the hit taking Health from above 0 to 0, only once per State.Dead.
			UAbilitySystemComponent* OwnerASC = GetOwningAbilitySystemComponent();
			const bool bDied = bDeathEnabled && OwnerASC && HealthBefore > 0.f && GetHealth() <= 0.f
				&& !OwnerASC->HasMatchingGameplayTag(RRGameplayTags::State_Dead);
			if (bDied)
			{
				OwnerASC->AddLooseGameplayTag(RRGameplayTags::State_Dead);
			}

			// The crit flag is a dynamic asset tag on the executed spec (sys-damage).
			const bool bIsCritical = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(RRGameplayTags::Effect_Damage_Critical);
			OnDamageReceived.Broadcast(Damage, bIsCritical, GetHealth());

			if (bDied)
			{
				OnDeath.Broadcast(Data.EffectSpec.GetEffectContext().GetOriginalInstigator());
			}
		}
	}
}
