// Rogue Rigs - named constants for the custom collision channels declared in Config/DefaultEngine.ini.

#pragma once

#include "Engine/EngineTypes.h"

/** Custom collision channels. Each value must match its +DefaultChannelResponses entry in Config/DefaultEngine.ini. */
namespace RRCollisionChannels
{
	/** Object channel "Projectile" (default response Block): bullets; overlaps pawns, blocks world static and dynamic. */
	inline constexpr ECollisionChannel Projectile = ECC_GameTraceChannel1;
}
