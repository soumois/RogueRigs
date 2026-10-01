// Rogue Rigs - base class of every project gameplay ability.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RRGameplayAbility.generated.h"

/**
 * Base class for every project ability (URRGA_<Name>). Instanced per actor: one instance per owning
 * ASC, reused across activations (the 5.8 default is InstancedPerExecution; NonInstanced is deprecated).
 */
UCLASS(Abstract)
class ROGUERIGS_API URRGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	/** Sets the instancing policy to InstancedPerActor. */
	URRGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
