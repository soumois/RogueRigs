// Rogue Rigs - debug log of where a miner shot is drawn on its first frames against the hand weapon Muzzle (task-413, console variable rr.Debug.ShotFirstFrame, off by default).

#pragma once

#include "CoreMinimal.h"

class ARRProjectile;
class USceneComponent;

/**
 * Shot first-frame measure (task-413). With rr.Debug.ShotFirstFrame 1, every miner shot spawned from the hand weapon Muzzle
 * logs one "fire" line (Muzzle at fire time, spawn point, location after the miner-to-Muzzle sweep) and, at the end of the world
 * tick (FWorldDelegates::OnWorldPostActorTick, the state sent to the renderer) of its first three frames, one "frame" line: the
 * shot mesh world location, the Muzzle location in that same frame, their horizontal distances, the frame time and whether the
 * mesh was rendered since the spawn. Lines start with "rr.Debug.ShotFirstFrame". Off by default; costs nothing when off.
 */
namespace RRShotFirstFrameDebug
{
	/** True when rr.Debug.ShotFirstFrame is on; check it before gathering the values TrackShot takes. */
	bool IsEnabled();

	/**
	 * Logs the fire line of Projectile and tracks it for its first three frames (see the namespace comment). Muzzle is the hand
	 * weapon Muzzle (read again on each frame line), FireMuzzleLocation its location at fire time, SpawnLocation where the projectile
	 * was spawned (miner axis) and SweptLocation where it stands after the sweep. Does nothing when the variable is off.
	 */
	void TrackShot(ARRProjectile* Projectile, const USceneComponent* Muzzle, const FVector& FireMuzzleLocation, const FVector& SpawnLocation, const FVector& SweptLocation);
}
