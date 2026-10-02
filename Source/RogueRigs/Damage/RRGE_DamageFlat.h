// Rogue Rigs - instant flat damage effect (enemy to player): SetByCaller Data.Damage x source AttackPower, refused on dead or invulnerable targets.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RRGE_DamageFlat.generated.h"

/**
 * Instant GE dealing flat damage, used by enemies on the player (sys-damage). The caller sets the amount with the
 * SetByCaller tag Data.Damage on the spec; the execution URRDamageFlatExecution scales it by the source's AttackPower
 * (URREnemyAttributeSet, 1 when absent) and writes IncomingDamage. A UTargetTagRequirementsGameplayEffectComponent
 * refuses the application when the target has State.Dead or State.Invulnerable (i-frames).
 */
UCLASS()
class ROGUERIGS_API URRGE_DamageFlat : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Sets the instant policy, the execution, and the target tag requirements. */
	URRGE_DamageFlat(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
