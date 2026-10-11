// Rogue Rigs - reset hooks of pooled actors (sys-actor-pool): URRActorPoolSubsystem calls them each time an actor leaves or enters the pool.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RRPoolable.generated.h"

/** UObject side of IRRPoolable (reflection only). */
UINTERFACE(MinimalAPI, Blueprintable)
class URRPoolable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Reset hooks of an actor reused by URRActorPoolSubsystem. Optional: an actor without it is pooled with the generic reset only
 * (hidden, collision and tick off, timers cleared, owner cleared). BeginPlay runs once per actor, at its first creation: a
 * pooled class moves its per-life setup (health, death flags, scale, registrations in subsystems) into OnPoolAcquired and
 * its per-life teardown (unregistrations, effects, movement) into OnPoolReleased. Call through IRRPoolable::Execute_*.
 */
class ROGUERIGS_API IRRPoolable
{
	GENERATED_BODY()

public:
	/**
	 * Called on every delivery to a caller, after the pool restored visibility, collision and tick and after the caller's
	 * init callback; on a fresh actor it runs after BeginPlay. Reset per-life state here.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Pool")
	void OnPoolAcquired();

	/**
	 * Called when the actor returns to the pool (Release, or a prewarmed actor parked after creation), before the pool hides
	 * it and turns collision and tick off. Stop activity and leave gameplay registries here.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Pool")
	void OnPoolReleased();
};
