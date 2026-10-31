// Rogue Rigs - robot shot: a projectile that hurts only the miner and stops on rock without touching the terrain.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/RRProjectile.h"
#include "RREnemyProjectile.generated.h"

/**
 * Shot of a ranged robot (enemy-shooter, sys-projectiles). Same flight and collision as ARRProjectile; its defaults make it a
 * robot shot: HitActorClass = ARRMiner (robots and every other actor are passed through) and bDamagesTerrain off (a blocking
 * hit on a rock or a barrel only stops it); collision capsule of radius 10 uu with half height = radius, a sphere (miner shots 14 uu). Spawned by ARRShooter through ARRProjectile::SpawnProjectile with the robot's flat
 * damage spec. Subclass in Blueprint for the red mesh.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARREnemyProjectile : public ARRProjectile
{
	GENERATED_BODY()

public:
	/** Sets the robot-shot defaults (miner only, no terrain damage). */
	ARREnemyProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
