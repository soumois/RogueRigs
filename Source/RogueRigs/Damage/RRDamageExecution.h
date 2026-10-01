// Rogue Rigs - execution calculation of URRGE_Damage: weapon damage with a critical roll, written to IncomingDamage.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RRDamageExecution.generated.h"

/**
 * ExecCalc of URRGE_Damage (sys-damage). Captures the source weapon's Damage, CritChance and CritDamage
 * (URRWeaponAttributeSet, snapshot at spec creation = fire time), rolls the critical hit (FRand < CritChance),
 * and outputs Damage (x CritDamage on a critical) as an Additive modifier on the target's IncomingDamage
 * (URRHealthAttributeSet), consumed in PostGameplayEffectExecute. On a critical hit it adds the dynamic asset tag
 * Effect.Damage.Critical to the executed spec (the target ASC's own copy), so listeners read it from
 * Data.EffectSpec.GetDynamicAssetTags() in PostGameplayEffectExecute, or from the spec passed to cues.
 */
UCLASS()
class ROGUERIGS_API URRDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	/** Registers the three source captures (snapshot). */
	URRDamageExecution(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Rolls the critical hit and writes IncomingDamage; tags the executed spec with Effect.Damage.Critical on a crit. */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	/** Source weapon Damage, snapshot at spec creation. */
	FGameplayEffectAttributeCaptureDefinition DamageDef;

	/** Source weapon CritChance (0 to 1), snapshot at spec creation. */
	FGameplayEffectAttributeCaptureDefinition CritChanceDef;

	/** Source weapon CritDamage (multiplier), snapshot at spec creation. */
	FGameplayEffectAttributeCaptureDefinition CritDamageDef;
};
