// Rogue Rigs - instant heal effect: adds a SetByCaller amount (Data.Heal) to Health, clamped to MaxHealth by the health set.

#include "GAS/RRGE_Heal.h"

#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGE_Heal)

URRGE_Heal::URRGE_Heal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat HealByCaller;
	HealByCaller.DataTag = RRGameplayTags::Data_Heal;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URRHealthAttributeSet::GetHealthAttribute();
	Modifier.ModifierOp = EGameplayModOp::AddBase;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealByCaller);
	Modifiers.Add(Modifier);
}
