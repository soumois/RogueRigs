// Rogue Rigs - instant effect removing one bullet from a weapon's magazine.

#include "Weapons/RRGE_ConsumeAmmo.h"

#include "Weapons/RRWeaponAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGE_ConsumeAmmo)

URRGE_ConsumeAmmo::URRGE_ConsumeAmmo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URRWeaponAttributeSet::GetAmmoAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-1.f));
	Modifiers.Add(Modifier);
}
