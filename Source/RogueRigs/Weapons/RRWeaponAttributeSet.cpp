// Rogue Rigs - attributes owned by every weapon ASC (player weapons, turret launcher, turrets).

#include "Weapons/RRWeaponAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRWeaponAttributeSet)

URRWeaponAttributeSet::URRWeaponAttributeSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitDamage(0.f);
	InitFireRate(0.f);
	InitReloadSpeed(0.f);
	InitCritChance(0.f);
	InitCritDamage(0.f);
	InitMagazineSize(0.f);
	InitAmmo(0.f);
}

void URRWeaponAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, FMath::Max(GetMagazineSize(), 0.f));
	}
	else
	{
		// Every weapon attribute is non-negative.
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void URRWeaponAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void URRWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void URRWeaponAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMagazineSizeAttribute() && GetAmmo() > NewValue)
	{
		SetAmmo(NewValue);
	}
}
