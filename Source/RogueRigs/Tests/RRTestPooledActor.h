// Rogue Rigs - test-only pooled actor for RogueRigs.ActorPool.*: records the pool reset hooks, BeginPlay and the init callback order.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pool/RRPoolable.h"
#include "RRTestPooledActor.generated.h"

/**
 * Plain actor (scene root, actor tick allowed) implementing IRRPoolable natively; counts its hooks so RogueRigs.ActorPool.*
 * can check when the pool calls them. A UCLASS, so compiled in every configuration; never used by gameplay.
 */
UCLASS(NotBlueprintable, NotPlaceable, HideDropdown)
class ROGUERIGS_API ARRTestPooledActor : public AActor, public IRRPoolable
{
	GENERATED_BODY()

public:
	/** Creates the scene root; the actor can tick (starts enabled) so the pool's tick toggling is visible. */
	ARRTestPooledActor();

	/** OnPoolAcquired calls so far. */
	int32 AcquiredCount = 0;

	/** OnPoolReleased calls so far. */
	int32 ReleasedCount = 0;

	/** BeginPlay calls so far (1 for a pooled actor's whole life). */
	int32 BeginPlayCount = 0;

	/** Value written by a test's OnInit callback. */
	int32 InitValue = 0;

	/** InitValue seen by the last BeginPlay (proves OnInit ran before it). */
	int32 InitValueAtBeginPlay = -1;

	/** InitValue seen by the last OnPoolAcquired (proves OnInit ran before it). */
	int32 InitValueAtAcquire = -1;

	/** Records the hook. */
	virtual void OnPoolAcquired_Implementation() override;

	/** Records the hook. */
	virtual void OnPoolReleased_Implementation() override;

protected:
	/** Records BeginPlay. */
	virtual void BeginPlay() override;
};
