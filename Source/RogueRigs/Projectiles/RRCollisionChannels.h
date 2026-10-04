// Rogue Rigs - named constants for the custom collision channels declared in Config/DefaultEngine.ini, and the collision mask filters.

#pragma once

#include "Engine/EngineTypes.h"

/** Custom collision channels. Each value must match its +DefaultChannelResponses entry in Config/DefaultEngine.ini. */
namespace RRCollisionChannels
{
	/** Object channel "Projectile" (default response Block): bullets; overlaps pawns, blocks world static and dynamic. */
	inline constexpr ECollisionChannel Projectile = ECC_GameTraceChannel1;

	/**
	 * Extra mask-filter bit of the robot capsules (FMaskFilter, NumExtraFilterBits = 6). A sweep whose ignore mask shares a bit
	 * with a shape skips it, whatever the channel responses: robots set it on their body and as their move-ignore mask, so only
	 * robot-robot move sweeps skip each other (sys-enemies).
	 */
	inline constexpr FMaskFilter RobotMaskFilter = 1 << 0;
}
