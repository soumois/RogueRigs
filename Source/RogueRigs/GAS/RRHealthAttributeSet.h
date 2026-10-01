// Rogue Rigs - health attributes shared by the player and enemies.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RRHealthAttributeSet.generated.h"

/**
 * Health of a combat actor (player, enemies). Health is kept in [0, MaxHealth]: base values are
 * clamped in PreAttributeBaseChange, current values in PreAttributeChange, and Health is brought
 * back under MaxHealth in PostAttributeChange when MaxHealth drops. Damage goes through the
 * IncomingDamage meta-attribute, which is consumed in PostGameplayEffectExecute.
 * Set MaxHealth before Health when initializing (Health is clamped against the current MaxHealth).
 */
UCLASS()
class ROGUERIGS_API URRHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	/** Starts every attribute at 0; base values come from the CSV loader. */
	URRHealthAttributeSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ATTRIBUTE_ACCESSORS_BASIC(URRHealthAttributeSet, Health)
	ATTRIBUTE_ACCESSORS_BASIC(URRHealthAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS_BASIC(URRHealthAttributeSet, IncomingDamage)

	/** Clamps base values: Health to [0, MaxHealth], MaxHealth and IncomingDamage to >= 0. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps current values with the same rules as the base values. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Brings Health back to MaxHealth when MaxHealth drops below it. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/** Consumes IncomingDamage: resets it to 0 and subtracts it from Health (clamped at 0). */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	/** Current health; the actor dies at 0 (death handling lives outside this set). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData Health;

	/** Maximum health; upper bound of Health. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData MaxHealth;

	/** Meta-attribute: damage to apply this execution. Never persists (reset to 0 after each execute). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData IncomingDamage;
};
