// Rogue Rigs - instant heal effect: adds a SetByCaller amount (Data.Heal) to Health, clamped to MaxHealth by the health set.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RRGE_Heal.generated.h"

/**
 * Instant GE restoring Health on its target. The amount comes from the SetByCaller tag Data.Heal, computed by the caller
 * (the level-up heal of URRLevelUpComponent: HealFraction x MaxHealth). It modifies the Health base value, so
 * URRHealthAttributeSet::PreAttributeBaseChange clamps the result to MaxHealth. Callers skip dead targets.
 */
UCLASS()
class ROGUERIGS_API URRGE_Heal : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Sets the instant policy and the Health AddBase modifier reading SetByCaller Data.Heal. */
	URRGE_Heal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
