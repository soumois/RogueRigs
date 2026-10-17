// Rogue Rigs - named constants for the custom collision channels declared in Config/DefaultEngine.ini, and the collision mask filters.

#pragma once

#include "Engine/EngineTypes.h"

/** Custom collision channels. Each value must match its +DefaultChannelResponses entry in Config/DefaultEngine.ini. */
namespace RRCollisionChannels
{
	/** Object channel "Projectile" (default response Block): bullets; overlaps pawns, blocks world static and dynamic. */
	inline constexpr ECollisionChannel Projectile = ECC_GameTraceChannel1;

	/**
	 * Object channel "Pickup" (task-229-02, default response Block so the BlockAll grid floor and rock collision block it): the
	 * landed pickups. Profile "Pickup" (PickupProfile): physics only, blocks WorldStatic, ignores every other channel, itself included.
	 */
	inline constexpr ECollisionChannel Pickup = ECC_GameTraceChannel2;

	/** Collision profile of ARRPickup::PickupMesh (Config/DefaultEngine.ini, task-229-02). */
	inline const FName PickupProfile(TEXT("Pickup"));

	/**
	 * Extra mask-filter bit of the robot capsules (FMaskFilter, NumExtraFilterBits = 6). A sweep whose ignore mask shares a bit
	 * with a shape skips it, whatever the channel responses: robots set it on their body and as their move-ignore mask, so only
	 * robot-robot move sweeps skip each other (sys-enemies).
	 */
	inline constexpr FMaskFilter RobotMaskFilter = 1 << 0;
}
