// Rogue Rigs - instant effect filling a weapon's magazine.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RRGE_RefillAmmo.generated.h"

/**
 * Instant GE: overrides Ammo with the target's current MagazineSize (attribute-based magnitude, captured
 * on the target, not snapshotted). Applied to self by ARRWeapon at acquisition and by URRAutoFireComponent
 * at the end of a reload.
 */
UCLASS()
class ROGUERIGS_API URRGE_RefillAmmo : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Builds the single Override modifier on Ammo backed by MagazineSize. */
	URRGE_RefillAmmo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
