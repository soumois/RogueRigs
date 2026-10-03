// Rogue Rigs - base projectile: flies straight, carries a damage spec, hurts the first actor with an ASC, stops on rock.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "RRProjectile.generated.h"

class UPrimitiveComponent;
class UProjectileMovementComponent;
class USphereComponent;
struct FHitResult;

/**
 * Base projectile (sys-projectiles). No ASC: it carries the damage spec made by the weapon's ASC at fire time and
 * applies it on impact. Collision: a sphere on the Projectile object channel (RRCollisionChannels::Projectile)
 * that overlaps pawns and blocks world static and dynamic. On overlap with an actor that has an ASC (other than
 * its owner weapon and the instigating miner), it applies the spec once and is destroyed; on a blocking hit (rock
 * blocks, walls) it is destroyed, after damaging the cell when it is a barrel of the arena grid (carried Damage, no GAS on barrels). Straight flight, no gravity, no sub-stepping; speed and lifetime set by
 * InitProjectile. Spawn with SpawnActorDeferred (Owner = weapon, Instigator = miner), call InitProjectile, then
 * FinishSpawning. Subclass in Blueprint for the mesh and the sphere radius.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRProjectile : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the collision sphere (root) and the projectile movement. */
	ARRProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Sets the carried damage spec, the speed (uu/s) and the lifetime (s, 0 = unlimited). Call between
	 * SpawnActorDeferred and FinishSpawning; called after BeginPlay it sets the velocity and life span directly.
	 */
	void InitProjectile(const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime);

	/** The collision sphere (root component). */
	USphereComponent* GetCollisionSphere() const { return CollisionSphere; }

	/** The movement component. */
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	/** Binds the overlap and stop events and ignores the owner weapon and the instigator while moving. */
	virtual void PostInitializeComponents() override;

	/** Pawn overlap: applies the spec once to an actor with an ASC (owner and instigator excluded), then destroys this projectile. */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Blocking hit (rock, wall): applies the carried damage to a barrel of the arena grid (ARRArenaGrid::ApplyShotDamage), then destroys this projectile. */
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	/**
	 * Per-bullet damage carried by the spec, read without a target ASC: the fire-time snapshot of the source weapon's
	 * URRWeaponAttributeSet Damage captured in the spec (critical hits not rolled). 0 when the spec has no such capture.
	 */
	float GetCarriedDamage() const;

	/** True for actors this projectile never hits: itself, its owner weapon, the instigator, and the owner's owner. */
	bool IsIgnoredActor(const AActor* OtherActor) const;

	/** Collision sphere on the Projectile channel; root component. Radius editable in the Blueprint subclass. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	/** Straight flight without gravity, bounce or sub-stepping; stops on the first blocking hit. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** Damage spec made by the weapon's ASC at fire time; applied on impact (the target ASC works on its own copy). */
	FGameplayEffectSpecHandle DamageSpec;

	/** True once the spec has been applied; guards against a second application before destruction. */
	bool bHasHit = false;
};
