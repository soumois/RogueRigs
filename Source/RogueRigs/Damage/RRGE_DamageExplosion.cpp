// Rogue Rigs - instant explosion damage effect: SetByCaller Data.Damage on IncomingDamage, refused on dead or invulnerable targets.

#include "Damage/RRGE_DamageExplosion.h"

#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGE_DamageExplosion)

URRGE_DamageExplosion::URRGE_DamageExplosion(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = RRGameplayTags::Data_Damage;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URRHealthAttributeSet::GetIncomingDamageAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
	Modifiers.Add(Modifier);

	// GE components in a C++ constructor: CreateDefaultSubobject then GEComponents.Add (gas-api-notes).
	UTargetTagRequirementsGameplayEffectComponent* TargetRequirements = CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(TEXT("TargetTagRequirements"));
	TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(RRGameplayTags::State_Dead);
	TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(RRGameplayTags::State_Invulnerable);
	GEComponents.Add(TargetRequirements);
}
