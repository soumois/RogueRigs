// Rogue Rigs - robot shot: a projectile that hurts only the miner and stops on rock without touching the terrain.

#include "Projectiles/RREnemyProjectile.h"

#include "Components/SphereComponent.h"
#include "Player/RRMiner.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RREnemyProjectile)

ARREnemyProjectile::ARREnemyProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HitActorClass = ARRMiner::StaticClass();
	bDamagesTerrain = false;
	// Robot shots keep the 10 uu sphere; the miner shots of the base class use 12 uu.
	CollisionSphere->InitSphereRadius(10.f);
}
