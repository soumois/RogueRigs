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

/** Fired once by URRHealthAttributeSet when a hit takes Shield from above 0 to 0 (shield chaser shell breaks). */
DECLARE_MULTICAST_DELEGATE(FRROnShieldBroken);

/**
 * One consumed damage hit, as seen by URRHealthAttributeSet::OnAnyDamageReceived (every health set of every world).
 * Read the source from the effect context: weapon shots carry the weapon as instigator, robot hits the robot, explosions the
 * explosion instigator with EffectClass URRGE_DamageExplosion.
 */
struct FRRDamageEventInfo
{
	/** Avatar actor of the damaged ASC (miner, robot); never null when broadcast. */
	AActor* Target = nullptr;

	/** Original instigator of the effect context (weapon, robot, kamikaze, grid); may be null. */
	AActor* Instigator = nullptr;

	/** Effect causer of the context (projectile owner weapon, explosion source actor); may be null. */
	AActor* EffectCauser = nullptr;

	/** Source object of the context (explosion source such as URRBarrelSource); may be null. */
	const UObject* SourceObject = nullptr;

	/** Class of the executed effect (URRGE_Damage, URRGE_DamageFlat, URRGE_DamageExplosion...); may be null. */
	const UClass* EffectClass = nullptr;

	/** Raw damage of the hit (IncomingDamage, before the Shield and Health clamp), same value as OnDamageReceived. */
	float Damage = 0.f;

	/** True when the spec carries Effect.Damage.Critical. */
	bool bIsCritical = false;

	/** True when this hit killed the target (the hit that broadcast OnDeath). */
	bool bKilled = false;
};

/** Fired by every URRHealthAttributeSet after its own per-hit events (OnDamageReceived, OnShieldBroken, OnDeath). */
DECLARE_MULTICAST_DELEGATE_OneParam(FRROnAnyDamageReceived, const FRRDamageEventInfo& /*Info*/);

/**
 * Health of a combat actor (player, enemies). Health is kept in [0, MaxHealth]: base values are
 * clamped in PreAttributeBaseChange, current values in PreAttributeChange, and Health is brought
 * back under MaxHealth in PostAttributeChange when MaxHealth drops. Damage goes through the
 * IncomingDamage meta-attribute, which is consumed in PostGameplayEffectExecute.
 * Shield (0 for most actors) absorbs a hit before Health: the part of the damage up to Shield is taken from it, the rest
 * from Health in the same hit; the hit taking Shield from above 0 to 0 broadcasts OnShieldBroken once.
 * Single death: when a hit takes Health from above 0 to 0 and the owner has no State.Dead yet, the set adds the loose
 * tag State.Dead to the owning ASC (damage GEs then refuse the actor) and broadcasts OnDeath once. Owners that must never
 * die call SetDeathEnabled(false) in their constructor (no such owner since the target dummy was removed, task-031-01).
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
	ATTRIBUTE_ACCESSORS_BASIC(URRHealthAttributeSet, Shield)

	/** Clamps base values: Health to [0, MaxHealth], MaxHealth, IncomingDamage and Shield to >= 0. */
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Clamps current values with the same rules as the base values. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** Brings Health back to MaxHealth when MaxHealth drops below it. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/**
	 * Consumes IncomingDamage: resets it to 0, takes it from Shield first then the rest from Health (clamped at 0), detects the
	 * death (State.Dead added), then broadcasts OnDamageReceived, then OnShieldBroken on a shield break, then OnDeath on a death.
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

	/**
	 * Shield break event, broadcast once from PostGameplayEffectExecute after OnDamageReceived (before OnDeath when the same hit
	 * kills), when a hit takes Shield from above 0 to 0. Same safety rules as OnDamageReceived.
	 */
	FRROnShieldBroken OnShieldBroken;

	/**
	 * Global per-hit event of every health set (all worlds), broadcast last from PostGameplayEffectExecute with the effect context
	 * read out (FRRDamageEventInfo). For observers that cannot bind each actor (run log, URRRunLogSubsystem); filter by world.
	 * Same safety rules as OnDamageReceived.
	 */
	static FRROnAnyDamageReceived OnAnyDamageReceived;

	/** Enables or disables the death detection (State.Dead + OnDeath). Enabled by default; call from the owner constructor. */
	void SetDeathEnabled(bool bEnabled) { bDeathEnabled = bEnabled; }

	/** Class of the gameplay effect whose execution caused the last death (set just before OnDeath; null before any death). Lets OnDeath listeners tell the damage path (e.g. URRGE_DamageExplosion). */
	TSubclassOf<UGameplayEffect> GetLethalEffectClass() const { return LethalEffectClass; }

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

	/** Damage absorbed before Health (robots: CSV column Shield, 0 for all but the shield chaser). Never regenerates. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
	FGameplayAttributeData Shield;

	/** When false, reaching 0 Health adds no State.Dead and fires no OnDeath (never-dying debug targets). */
	bool bDeathEnabled = true;

	/** Effect class of the last killing hit (GetLethalEffectClass). */
	UPROPERTY(Transient)
	TSubclassOf<UGameplayEffect> LethalEffectClass;
};
