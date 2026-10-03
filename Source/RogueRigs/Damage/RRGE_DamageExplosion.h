// Rogue Rigs - instant explosion damage effect: SetByCaller Data.Damage on IncomingDamage, refused on dead or invulnerable targets.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RRGE_DamageExplosion.generated.h"

/**
 * Instant GE dealt by URRExplosionSubsystem to every character in an explosion radius (sys-explosions, sys-damage).
 * One Additive modifier on IncomingDamage whose magnitude is the SetByCaller Data.Damage (the subsystem has already
 * applied the miner no-one-shot cap). No attribute scaling, no critical. A UTargetTagRequirementsGameplayEffectComponent
 * refuses the application when the target has State.Dead or State.Invulnerable, so explosions respect the miner i-frames.
 */
UCLASS()
class ROGUERIGS_API URRGE_DamageExplosion : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Sets the instant policy, the SetByCaller modifier and the target tag requirements. */
	URRGE_DamageExplosion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
