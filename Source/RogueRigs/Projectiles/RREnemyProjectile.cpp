// Rogue Rigs - robot shot: a projectile that hurts only the miner and stops on rock without touching the terrain.

#include "Projectiles/RREnemyProjectile.h"

#include "Components/CapsuleComponent.h"
#include "Player/RRMiner.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RREnemyProjectile)

ARREnemyProjectile::ARREnemyProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HitActorClass = ARRMiner::StaticClass();
	bDamagesTerrain = false;
	// Impact sparks are for player shots only (task-432).
	ImpactSparkSystem = nullptr;
	// Robot shots keep the 10 uu sphere (capsule half height = radius); the miner shots of the base class use 14 uu.
	CollisionCapsule->InitCapsuleSize(10.f, 10.f);
}
