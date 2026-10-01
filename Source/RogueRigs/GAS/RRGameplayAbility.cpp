// Rogue Rigs - base class of every project gameplay ability.

#include "GAS/RRGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGameplayAbility)

URRGameplayAbility::URRGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}
