// Rogue Rigs - shotgun pellet: the concrete projectile the shotgun fires by default (pierce and knockback set at spawn).

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/RRProjectile.h"
#include "RRShotgunPellet.generated.h"

/**
 * Pellet of the shotgun (shotgun gdd, sys-projectiles). Same flight, collision, pierce and rock stop as ARRProjectile; the
 * shotgun gives it at spawn its damage spec, MaxHits (the shotgun's Pierce) and KnockbackSpeed (its KnockbackForce), so each
 * robot it pierces is damaged (own critical roll), then pushed along the pellet's flight. Concrete, so the native shotgun
 * fires pellets without a Blueprint (no mesh: invisible); subclass in Blueprint for the mesh and the capsule radius (half height = radius by default; miner pellets get a taller capsule per shot, task-412).
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRShotgunPellet : public ARRProjectile
{
	GENERATED_BODY()

public:
	/** Default projectile setup (ARRProjectile). */
	ARRShotgunPellet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
