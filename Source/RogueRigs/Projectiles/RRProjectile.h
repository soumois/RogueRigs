// Rogue Rigs - base projectile: flies straight, carries a damage spec, hurts up to MaxHits distinct hittable actors with an ASC (pierce), optional knockback per hit, stops on rock.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "RRProjectile.generated.h"

class APawn;
class UPrimitiveComponent;
class UProjectileMovementComponent;
class USphereComponent;
struct FHitResult;

/**
 * Base projectile (sys-projectiles). No ASC: it carries the damage spec made by the weapon's ASC at fire time and
 * applies it on impact. Collision: a sphere on the Projectile object channel (RRCollisionChannels::Projectile)
 * that overlaps pawns and blocks world static and dynamic. On overlap with an actor that has an ASC, is not ignored
 * (owner, owner's owner, instigator), passes CanHit (default: an instance of HitActorClass, any actor when unset) and was
 * not hit yet by this projectile, it applies the spec once to it; after MaxHits distinct actors (pierce, set by
 * InitProjectile, default 1) it is destroyed; other actors are passed through. With a KnockbackSpeed > 0 (shotgun pellets) each robot
 * hit is also pushed along the flight direction (ARREnemy::ApplyKnockback, after the damage: a robot killed by the hit is not pushed). On a blocking hit (rock blocks, walls) it is
 * destroyed, after damaging the cell when bDamagesTerrain and it is a barrel of the arena grid (carried Damage, no GAS on
 * barrels). Straight flight, no gravity, no sub-stepping; speed and lifetime set by InitProjectile. Spawn with
 * SpawnProjectile (deferred spawn + InitProjectile + FinishSpawning; Owner = the firing actor, Instigator = its pawn).
 * Sphere radius 14 uu (miner shots: rifle, pellet, turret bullet); robot shots 10 uu. Robot shots: HitActorClass = the miner
 * class (robots are passed through) and bDamagesTerrain off. Subclass in Blueprint
 * for the mesh (its collision is turned off at spawn: only the sphere collides) and the sphere radius.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRProjectile : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the collision sphere (root) and the projectile movement. */
	ARRProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Sets the carried damage spec, the speed (uu/s), the lifetime (s, 0 = unlimited) and the number of distinct actors it
	 * damages before it is destroyed (InMaxHits, clamped to >= 1; the firing weapon's Pierce) and the knockback launch speed
	 * given to each robot hit (InKnockbackSpeed uu/s along the flight, before its resistance; 0 = none). Call between
	 * SpawnActorDeferred and FinishSpawning; called after BeginPlay it sets the velocity and life span directly.
	 */
	void InitProjectile(const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime, int32 InMaxHits = 1, float InKnockbackSpeed = 0.f);

	/**
	 * Spawns one projectile of Class at SpawnTransform (deferred, always spawned), sets its damage spec, speed (uu/s) and
	 * lifetime (s, 0 = unlimited), MaxHits (distinct actors damaged, >= 1) and KnockbackSpeed (uu/s per robot hit, 0 = none) with InitProjectile, then finishes the spawn. InOwner and InInstigator are ignored by its
	 * collision. Returns null (logged) when World or Class is missing or the spawn failed.
	 */
	static ARRProjectile* SpawnProjectile(UWorld* World, TSubclassOf<ARRProjectile> Class, const FTransform& SpawnTransform,
		AActor* InOwner, APawn* InInstigator, const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime, int32 MaxHits = 1, float KnockbackSpeed = 0.f);

	/**
	 * True when this projectile may hit OtherActor (checked on overlap after the ignored actors, before the ASC lookup).
	 * Default: OtherActor is an instance of HitActorClass, or any actor when HitActorClass is unset. Override for other filters.
	 */
	virtual bool CanHit(const AActor* OtherActor) const;

	/** True when a blocking hit on a barrel cell applies the carried damage to it. */
	bool DamagesTerrain() const { return bDamagesTerrain; }

	/** Distinct actors this projectile damages before it is destroyed (>= 1). */
	int32 GetMaxHits() const { return MaxHits; }

	/** Knockback launch speed in uu/s given to each robot hit along the flight direction, before its resistance (0 = none). */
	float GetKnockbackSpeed() const { return KnockbackSpeed; }

	/** Distinct actors damaged so far. */
	int32 GetHitCount() const { return HitCount; }

	/** The collision sphere (root component). */
	USphereComponent* GetCollisionSphere() const { return CollisionSphere; }

	/** The movement component. */
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	/** Turns off the collision of the visual components (DisableVisualCollision), binds the overlap and stop events and ignores the owner weapon and the instigator while moving. */
	virtual void PostInitializeComponents() override;

	/**
	 * Sets every primitive component other than CollisionSphere (the Blueprint shot mesh) to NoCollision without overlap
	 * events: only the sphere collides. A shot mesh left with a blocking collision moved with the bullet into robot capsules,
	 * whose next move resolved the penetration by pushing the robot (task-180-01), and stopped other bullets.
	 */
	void DisableVisualCollision();

	/**
	 * Pawn overlap: applies the spec once to a hittable actor with an ASC (ignored actors, CanHit false and actors already hit
	 * excluded), then the knockback to a robot when KnockbackSpeed > 0; destroys this projectile once MaxHits distinct actors were hit.
	 */
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Blocking hit (rock, wall): when bDamagesTerrain, applies the carried damage once to a barrel of the arena grid (ARRArenaGrid::ApplyShotDamage); then destroys this projectile. */
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

	/** Only instances of this class are hit (CanHit); others are passed through. None = any actor with an ASC (miner shots). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AActor> HitActorClass;

	/** A blocking hit on a barrel cell damages it (miner shots). Off for shots that must not touch the terrain (robot shots). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool bDamagesTerrain = true;

	/** Damage spec made by the weapon's ASC at fire time; applied on impact (the target ASC works on its own copy). */
	FGameplayEffectSpecHandle DamageSpec;

	/** Distinct actors this projectile damages before it is destroyed (pierce; >= 1). Set by InitProjectile. */
	int32 MaxHits = 1;

	/** Knockback launch speed in uu/s per robot hit, along the flight direction, before the robot's resistance (0 = none). Set by InitProjectile. */
	float KnockbackSpeed = 0.f;

	/**
	 * Actors already damaged by this projectile, with the pool life of a robot (ARREnemy::GetPoolLifeCount, 0 otherwise): a
	 * pierced robot is never hit twice by the same bullet, but the same actor reused from the actor pool is a new target.
	 */
	TMap<TWeakObjectPtr<AActor>, int32> HitActors;

	/** Distinct hits so far (MaxHits counts them). */
	int32 HitCount = 0;

	/** Pool life of OtherActor when it is a robot, else 0 (key of HitActors). */
	static int32 GetHitLife(const AActor* OtherActor);

	/** True once the projectile is spent (MaxHits reached or stopped); guards against any application before destruction. */
	bool bSpent = false;
};
