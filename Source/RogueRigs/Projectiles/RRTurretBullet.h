// Rogue Rigs - turret bullet: the concrete projectile the turret launcher gives its turrets by default (pierce set at spawn).

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/RRProjectile.h"
#include "RRTurretBullet.generated.h"

/**
 * Bullet of a placed turret (turret gdd, sys-projectiles). Same flight, collision, pierce and rock stop as ARRProjectile; the
 * turret gives it at spawn its damage spec (made on the turret's ASC) and MaxHits (the turret's Pierce). Concrete, so the native
 * turret launcher works without a Blueprint (no mesh: invisible); subclass in Blueprint for the mesh and the capsule radius (half height = radius: a sphere, as before task-412).
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRTurretBullet : public ARRProjectile
{
	GENERATED_BODY()

public:
	/** Default projectile setup (ARRProjectile). */
	ARRTurretBullet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
