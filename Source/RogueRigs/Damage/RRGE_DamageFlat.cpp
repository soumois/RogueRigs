// Rogue Rigs - instant flat damage effect (enemy to player): SetByCaller Data.Damage x source AttackPower, refused on dead or invulnerable targets.

#include "Damage/RRGE_DamageFlat.h"

#include "Damage/RRDamageFlatExecution.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
#include "GAS/RRGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGE_DamageFlat)

URRGE_DamageFlat::URRGE_DamageFlat(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition Execution;
	Execution.CalculationClass = URRDamageFlatExecution::StaticClass();
	Executions.Add(Execution);

	// GE components in a C++ constructor: CreateDefaultSubobject then GEComponents.Add (gas-api-notes).
	UTargetTagRequirementsGameplayEffectComponent* TargetRequirements = CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(TEXT("TargetTagRequirements"));
	TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(RRGameplayTags::State_Dead);
	TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(RRGameplayTags::State_Invulnerable);
	GEComponents.Add(TargetRequirements);
}
