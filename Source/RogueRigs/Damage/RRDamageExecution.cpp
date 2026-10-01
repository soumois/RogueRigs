// Rogue Rigs - execution calculation of URRGE_Damage: weapon damage with a critical roll, written to IncomingDamage.

#include "Damage/RRDamageExecution.h"

#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Weapons/RRWeaponAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRDamageExecution)

URRDamageExecution::URRDamageExecution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Snapshot: the weapon's stats at fire time, even if an upgrade lands while the bullet flies.
	DamageDef = FGameplayEffectAttributeCaptureDefinition(URRWeaponAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	CritChanceDef = FGameplayEffectAttributeCaptureDefinition(URRWeaponAttributeSet::GetCritChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	CritDamageDef = FGameplayEffectAttributeCaptureDefinition(URRWeaponAttributeSet::GetCritDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);

	RelevantAttributesToCapture.Add(DamageDef);
	RelevantAttributesToCapture.Add(CritChanceDef);
	RelevantAttributesToCapture.Add(CritDamageDef);
}

void URRDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Damage = 0.f;
	float CritChance = 0.f;
	float CritDamage = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageDef, EvaluateParameters, Damage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritChanceDef, EvaluateParameters, CritChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CritDamageDef, EvaluateParameters, CritDamage);

	Damage = FMath::Max(Damage, 0.f);
	if (Damage <= 0.f)
	{
		return;
	}

	// FRand is in [0, 1): CritChance 0 never crits, CritChance >= 1 always crits.
	const bool bIsCritical = FMath::FRand() < CritChance;
	if (bIsCritical)
	{
		Damage *= FMath::Max(CritDamage, 0.f);

		// The executed spec is the target ASC's copy: the carried spec handle keeps no crit tag for the next hit.
		if (FGameplayEffectSpec* MutableSpec = ExecutionParams.GetOwningSpecForPreExecuteMod())
		{
			MutableSpec->AddDynamicAssetTag(RRGameplayTags::Effect_Damage_Critical);
		}
	}

	if (Damage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URRHealthAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage));
	}
}
