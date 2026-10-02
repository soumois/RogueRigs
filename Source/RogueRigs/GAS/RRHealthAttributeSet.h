// Rogue Rigs - health attributes shared by the player and enemies.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RRHealthAttributeSet.generated.h"

/**
 * Fired by URRHealthAttributeSet each time IncomingDamage is consumed with a positive value.
 * Params: Damage (raw amount from the execution, before the Health clamp), bIsCritical (the executed spec
 * carries the dynamic asset tag Effect.Damage.Critical), HealthLeft (Health after the hit).
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FRROnDamageReceived, float /*Damage*/, bool /*bIsCritical*/, float /*HealthLeft*/);

/**
 * Fired once by URRHealthAttributeSet when a hit takes Health from above 0 to 0 (single death).
 * Param: Instigator (original instigator of the killing effect context; may be null).
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FRROnDeath, AActor* /*Instigator*/);

/**
 * Health of a combat actor (player, enemies). Health is kept in [0, MaxHealth]: base values are
 * clamped in PreAttributeBaseChange, current values in PreAttributeChange, and Health is brought
 * back under MaxHealth in PostAttributeChange when MaxHealth drops. Damage goes through the
 * IncomingDamage meta-attribute, which is consumed in PostGameplayEffectExecute.
 * Single death: when a hit takes Health from above 0 to 0 and the owner has no State.Dead yet, the set adds the loose
 * tag State.Dead to the owning ASC (damage GEs then refuse the actor) and broadcasts OnDeath once. Owners that must never
 * die (target dummy) call SetDeathEnabled(false) in their constructor.
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

	/**
	 * Consumes IncomingDamage: resets it to 0, subtracts it from Health (clamped at 0), detects the death (State.Dead added),
	 * then broadcasts OnDamageReceived, then OnDeath on a death.
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/**
	 * Per-hit damage event (amount, critical flag, health left), broadcast from PostGameplayEffectExecute after
	 * Health is updated. Bind for hit feedback (flash, damage numbers, logs). Handlers run inside the GE execution:
	 * applying a GE or setting an attribute is safe there; removing effects or cancelling abilities is not.
	 */
	FRROnDamageReceived OnDamageReceived;

	/**
	 * Death event, broadcast once from PostGameplayEffectExecute after OnDamageReceived, when a hit takes Health from above 0
	 * to 0; State.Dead is already on the owning ASC. Same safety rules as OnDamageReceived.
	 */
	FRROnDeath OnDeath;

	/** Enables or disables the death detection (State.Dead + OnDeath). Enabled by default; call from the owner constructor. */
	void SetDeathEnabled(bool bEnabled) { bDeathEnabled = bEnabled; }

protected:
	/** Current health; the actor dies when a hit takes it to 0 (see OnDeath). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData Health;

	/** Maximum health; upper bound of Health. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData MaxHealth;

	/** Meta-attribute: damage to apply this execution. Never persists (reset to 0 after each execute). */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData IncomingDamage;

	/** When false, reaching 0 Health adds no State.Dead and fires no OnDeath (never-dying debug targets). */
	bool bDeathEnabled = true;
};
