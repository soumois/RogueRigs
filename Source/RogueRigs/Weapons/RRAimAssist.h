// Rogue Rigs - light aim assist of the miner's shots (task-463): predicts what a shot hits first and turns it a few degrees toward a robot.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"

class AActor;
class ARREnemy;
class UWorld;

/**
 * Light aim assist (task-463, owner 2026-11-06, values provisional). Stateless helpers used by ARRWeapon::FireOnce:
 * - PredictFirstHit sweeps the real shot (its capsule, its collision responses, from the miner axis to the Muzzle then along the
 *   direction up to Range) and says what it meets first: a living robot, a barrel cell, another block (rock, ore, wall) or nothing.
 * - FindAssistedDirection: when the straight shot meets a block or nothing, picks among the living robots within Range, within
 *   MaxAngleDeg of the shot direction (2D), the one needing the smallest turn whose own turned shot meets a robot first (so a rock
 *   or a barrel in the way, with the real shot width, rules it out); the shot then aims straight at its current position.
 *   A shot whose straight path meets a robot or a barrel first is never turned; barrels never attract a shot.
 */
namespace RRAimAssist
{
	/** What a shot meets first along its path. */
	enum class EFirstHit : uint8
	{
		Nothing,
		Robot,
		Barrel,
		Block
	};

	/** Geometry of one shot as fired (ARRWeapon::GetShotPlacement and the projectile class). */
	struct FShotQuery
	{
		/** Projectile spawn point, swept to Origin first when bSweepFromMiner. */
		FVector SweepStart = FVector::ZeroVector;
		bool bSweepFromMiner = false;

		/** Flight start (the Muzzle). */
		FVector Origin = FVector::ZeroVector;

		/** Projectile capsule (vertical): radius and half height (>= radius). */
		float Radius = 0.f;
		float HalfHeight = 0.f;

		/** Flight length in uu from Origin. */
		float Range = 0.f;

		/** Collision responses of the projectile capsule (its default object), so the sweep meets what the shot meets. */
		FCollisionResponseParams Responses;

		/** Actors the shot ignores (weapon, miner, instigator). */
		TArray<const AActor*, TInlineAllocator<4>> IgnoredActors;
	};

	/** First thing the shot of Query meets flying along Direction (flat); OutRobot set when it is a robot. */
	ROGUERIGS_API EFirstHit PredictFirstHit(const UWorld& World, const FShotQuery& Query, const FVector& Direction, const ARREnemy** OutRobot = nullptr);

	/**
	 * Direction of an assisted shot. Returns false (OutDirection = ShotDirection) when MaxAngleDeg <= 0, when the straight shot meets a
	 * robot or a barrel first, or when no robot qualifies. True: OutDirection points flat at the chosen robot, OutTarget is it.
	 */
	ROGUERIGS_API bool FindAssistedDirection(const UWorld& World, const FShotQuery& Query, const FVector& ShotDirection, float MaxAngleDeg,
		FVector& OutDirection, const ARREnemy** OutTarget = nullptr);
}
