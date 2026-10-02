// Rogue Rigs - i-frames effect: grants State.Invulnerable for a SetByCaller duration (Data.Duration).

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RRGE_IFrames.generated.h"

/**
 * Duration GE granting State.Invulnerable to its target (sys-damage step 7). The duration in seconds comes from the
 * SetByCaller tag Data.Duration, set by the pawn that applies it (ARRMiner::IFramesDuration). Applied to self from the
 * damage post-execute (same call stack), so a second hit in the same frame is already refused by the damage GEs' tag
 * requirements. No stacking: a new application while active adds a second, independent effect.
 */
UCLASS()
class ROGUERIGS_API URRGE_IFrames : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Sets the duration policy (SetByCaller Data.Duration) and the granted State.Invulnerable tag. */
	URRGE_IFrames(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
