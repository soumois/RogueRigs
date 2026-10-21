// Rogue Rigs - shadow casting rule shared by every non-terrain actor (task-263-01): only the grid's rocks cast shadows.

#pragma once

#include "CoreMinimal.h"

class AActor;

/**
 * Shadow casting helpers (sys-grid, lights-no-shadows). The player's light is the only shadow-casting light and only the grid
 * blocks cast; robots, pickups, the drone, the miner, weapons and projectiles never do. Their classes call
 * DisableCastShadows from PostInitializeComponents, so the rule holds for every Blueprint subclass and its added components,
 * and survives pool reuse (the pool never touches CastShadow). Receiving shadows is unchanged.
 */
namespace RRShadows
{
	/** Sets CastShadow false on every primitive component Actor holds at call time (C++ and Blueprint components). Null-safe. */
	ROGUERIGS_API void DisableCastShadows(const AActor* Actor);
}
