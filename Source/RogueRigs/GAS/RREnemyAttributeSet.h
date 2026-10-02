// Rogue Rigs - enemy-only attributes: AttackPower, the multiplier of the flat damage an enemy deals.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RREnemyAttributeSet.generated.h"

/**
 * Attributes only enemies have (sys-gas-core). AttackPower scales the flat damage of URRGE_DamageFlat specs made by
 * the enemy's ASC (URRDamageFlatExecution). Starts at 1 (neutral) and is clamped at >= 0 (base and current).
 * Add it to the enemy pawn as a default subobject next to its ASC.
 */
UCLASS()
class ROGUERIGS_API URREnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** Starts AttackPower at 1 (damage unscaled). */
	URREnemyAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ATTRIBUTE_ACCESSORS_BASIC(URREnemyAttributeSet, AttackPower)

	/** Clamps the base AttackPower to >= 0. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps the current AttackPower to >= 0. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

protected:
	/** Multiplier of the flat damage dealt by this enemy (1 = the SetByCaller amount as is). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Enemy")
	FGameplayAttributeData AttackPower;
};
