// Rogue Rigs - instant damage effect of player weapons: URRDamageExecution, refused on dead or invulnerable targets.

#include "Damage/RRGE_Damage.h"

#include "Damage/RRDamageExecution.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
#include "GAS/RRGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGE_Damage)

URRGE_Damage::URRGE_Damage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition Execution;
	Execution.CalculationClass = URRDamageExecution::StaticClass();
	Executions.Add(Execution);

	// GE components in a C++ constructor: CreateDefaultSubobject then GEComponents.Add (gas-api-notes).
	UTargetTagRequirementsGameplayEffectComponent* TargetRequirements = CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(TEXT("TargetTagRequirements"));
	TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(RRGameplayTags::State_Dead);
	TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(RRGameplayTags::State_Invulnerable);
	GEComponents.Add(TargetRequirements);
}
