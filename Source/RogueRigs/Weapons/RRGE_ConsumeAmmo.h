// Rogue Rigs - instant effect removing one bullet from a weapon's magazine.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "RRGE_ConsumeAmmo.generated.h"

/**
 * Instant GE: Ammo -= 1 on URRWeaponAttributeSet (base value). Applied to self by
 * URRAutoFireComponent after each shot. The attribute set clamps Ammo to [0, MagazineSize].
 */
UCLASS()
class ROGUERIGS_API URRGE_ConsumeAmmo : public UGameplayEffect
{
	GENERATED_BODY()

public:
	/** Builds the single Add modifier on Ammo. */
	URRGE_ConsumeAmmo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
