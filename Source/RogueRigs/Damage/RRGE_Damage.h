// Rogue Rigs - instant damage effect of player weapons: URRDamageExecution, refused on dead or invulnerable targets.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RRGE_Damage.generated.h"

/**
 * Instant GE dealing weapon damage (sys-damage). Its execution, URRDamageExecution, reads the source weapon's
 * Damage, CritChance, CritDamage, rolls the critical hit and writes IncomingDamage on the target. A
 * UTargetTagRequirementsGameplayEffectComponent refuses the application when the target has State.Dead or
 * State.Invulnerable. The spec is made by the weapon's ASC at fire time (instigator and causer: the weapon) and
 * carried by the projectile to the impact.
 */
UCLASS()
class ROGUERIGS_API URRGE_Damage : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Sets the instant policy, the execution, and the target tag requirements. */
	URRGE_Damage(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
