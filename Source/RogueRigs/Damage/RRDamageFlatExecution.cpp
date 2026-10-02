// Rogue Rigs - execution calculation of URRGE_DamageFlat: SetByCaller Data.Damage scaled by the source's AttackPower, written to IncomingDamage.

#include "Damage/RRDamageFlatExecution.h"

#include "AbilitySystemComponent.h"
#include "GAS/RREnemyAttributeSet.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRDamageFlatExecution)

void URRDamageFlatExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// A missing SetByCaller returns 0 and logs an error (gas-api-notes).
	float Damage = Spec.GetSetByCallerMagnitude(RRGameplayTags::Data_Damage, /*WarnIfNotFound*/ true, 0.f);

	float AttackPower = 1.f;
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	if (SourceASC && SourceASC->HasAttributeSetForAttribute(URREnemyAttributeSet::GetAttackPowerAttribute()))
	{
		AttackPower = SourceASC->GetNumericAttribute(URREnemyAttributeSet::GetAttackPowerAttribute());
	}

	Damage = FMath::Max(Damage * AttackPower, 0.f);
	if (Damage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URRHealthAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
	}
}
