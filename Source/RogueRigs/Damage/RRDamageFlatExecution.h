// Rogue Rigs - execution calculation of URRGE_DamageFlat: SetByCaller Data.Damage scaled by the source's AttackPower, written to IncomingDamage.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RRDamageFlatExecution.generated.h"

/**
 * ExecCalc of URRGE_DamageFlat (sys-damage). Reads the SetByCaller magnitude Data.Damage of the spec (missing = 0 and a
 * LogGameplayEffects error), multiplies it by the source ASC's AttackPower (URREnemyAttributeSet) when the source has that
 * set, else by 1, and outputs the result as an Additive modifier on the target's IncomingDamage. AttackPower is read at
 * execution, not captured: a source without the set (miner, trap, test) adds no capture warning. No critical roll.
 */
UCLASS()
class ROGUERIGS_API URRDamageFlatExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	/** Computes SetByCaller Data.Damage x source AttackPower (1 without the set) and writes it to IncomingDamage. */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
