// Rogue Rigs - project ability system component, used on every actor that owns GAS state.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RRAbilitySystemComponent.generated.h"

/**
 * Ability system component for the player, weapons, turrets and enemies (one per actor, none on
 * projectiles). Single-player game: no project-specific replication logic. Add project-wide ASC
 * helpers here rather than on the owning actors.
 */
UCLASS(ClassGroup = (AbilitySystem), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/** Default constructor; keeps the engine defaults. */
	URRAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
