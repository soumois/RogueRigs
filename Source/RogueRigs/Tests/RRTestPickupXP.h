// Rogue Rigs - test-only XP pickup: a native, non-abstract ARRPickup of type XP for the enemy tests (no Blueprint needed).

#pragma once

#include "CoreMinimal.h"
#include "Pickups/RRPickup.h"
#include "RRTestPickupXP.generated.h"

/**
 * ARRPickup with PickupType XP and no mesh, used by RogueRigs.Enemies.* as the enemy XPPickupClass until BP_PickupXP
 * exists. A UCLASS, so compiled in every configuration (UHT ignores WITH_DEV_AUTOMATION_TESTS); never used by gameplay.
 */
UCLASS(NotBlueprintable, NotPlaceable, HideDropdown)
class ROGUERIGS_API ARRTestPickupXP : public ARRPickup
{
	GENERATED_BODY()

public:
	/** Sets the type to XP. */
	ARRTestPickupXP();
};
