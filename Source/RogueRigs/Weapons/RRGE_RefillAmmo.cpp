// Rogue Rigs - instant effect filling a weapon's magazine.

#include "Weapons/RRGE_RefillAmmo.h"

#include "Weapons/RRWeaponAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGE_RefillAmmo)

URRGE_RefillAmmo::URRGE_RefillAmmo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FAttributeBasedFloat MagazineSizeMagnitude;
	MagazineSizeMagnitude.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
		URRWeaponAttributeSet::GetMagazineSizeAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = URRWeaponAttributeSet::GetAmmoAttribute();
	Modifier.ModifierOp = EGameplayModOp::Override;
	Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(MagazineSizeMagnitude);
	Modifiers.Add(Modifier);
}
