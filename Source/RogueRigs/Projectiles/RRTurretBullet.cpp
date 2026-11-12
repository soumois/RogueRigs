// Rogue Rigs - turret bullet: the concrete projectile the turret launcher gives its turrets by default (pierce set at spawn).

#include "Projectiles/RRTurretBullet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTurretBullet)

ARRTurretBullet::ARRTurretBullet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Turret sounds are not in the first sound events (task-511 Q1): a turret bullet impact stays silent.
	ImpactSoundEvent = NAME_None;
}
