// Rogue Rigs - attributes owned by every weapon ASC (player weapons, turret launcher, turrets).

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RRWeaponAttributeSet.generated.h"

/**
 * Weapon stats changed by upgrades or effects during a run: Damage, FireRate, ReloadSpeed, CritChance,
 * CritDamage, MagazineSize, and the current magazine content Ammo. Every attribute is clamped to >= 0
 * (base and current); Ammo is also clamped to <= MagazineSize and lowered when MagazineSize drops (never
 * raised when it grows). All start at 0; base values come from WeaponStats.csv (ARRWeapon acquisition).
 */
UCLASS()
class ROGUERIGS_API URRWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** Starts every attribute at 0; base values come from the CSV. */
	URRWeaponAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ATTRIBUTE_ACCESSORS_BASIC(URRWeaponAttributeSet, Damage)
	ATTRIBUTE_ACCESSORS_BASIC(URRWeaponAttributeSet, FireRate)
	ATTRIBUTE_ACCESSORS_BASIC(URRWeaponAttributeSet, ReloadSpeed)
	ATTRIBUTE_ACCESSORS_BASIC(URRWeaponAttributeSet, CritChance)
	ATTRIBUTE_ACCESSORS_BASIC(URRWeaponAttributeSet, CritDamage)
	ATTRIBUTE_ACCESSORS_BASIC(URRWeaponAttributeSet, MagazineSize)
	ATTRIBUTE_ACCESSORS_BASIC(URRWeaponAttributeSet, Ammo)

	/** Clamps every base value to >= 0, and base Ammo to <= MagazineSize. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps every current value to >= 0, and current Ammo to <= MagazineSize. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Lowers Ammo when MagazineSize drops below it. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

protected:
	/** Damage dealt per bullet (or per target for the shotgun), before critical hits. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData Damage;

	/** Bursts per second; drives the auto-fire period (1 / FireRate). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData FireRate;

	/** Reload speed multiplier: reload duration = CSV ReloadTime / ReloadSpeed. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData ReloadSpeed;

	/** Chance of a critical hit per bullet, 0 to 1. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData CritChance;

	/** Damage multiplier of a critical hit. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData CritDamage;

	/** Bullets per magazine (turret launcher: turret charges). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData MagazineSize;

	/** Bullets left in the magazine, in [0, MagazineSize]. Consumed per shot, refilled by the reload. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Weapon")
	FGameplayAttributeData Ammo;

	/** Applies the clamps shared by base and current values to NewValue. */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
