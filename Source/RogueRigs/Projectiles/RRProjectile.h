// Rogue Rigs - base projectile: vertical capsule collision, flies straight, carries a damage spec, hurts up to MaxHits distinct hittable actors with an ASC (pierce), optional knockback per hit, stops on rock.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "RRProjectile.generated.h"

class APawn;
class UPrimitiveComponent;
class UCapsuleComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UWorld;
struct FHitResult;

/**
 * Base projectile (sys-projectiles). No ASC: it carries the damage spec made by the weapon's ASC at fire time and
 * applies it on impact. Collision: a vertical capsule (CollisionCapsule, root) on the Projectile object channel (RRCollisionChannels::Projectile)
 * that overlaps pawns and blocks world static and dynamic. On overlap with an actor that has an ASC, is not ignored
 * (owner, owner's owner, instigator), passes CanHit (default: an instance of HitActorClass, any actor when unset) and was
 * not hit yet by this projectile, it applies the spec once to it; after MaxHits distinct actors (pierce, set by
 * InitProjectile, default 1) it is destroyed; other actors are passed through. With a KnockbackSpeed > 0 (shotgun pellets) each robot
 * hit is also pushed along the flight direction (ARREnemy::ApplyKnockback, after the damage: a robot killed by the hit is not pushed). On a blocking hit (rock blocks, walls) it is
 * destroyed, after damaging the cell when bDamagesTerrain and it is a barrel of the arena grid (carried Damage, no GAS on
 * barrels). Straight flight, no gravity, no sub-stepping; speed and lifetime set by InitProjectile. Spawn with
 * SpawnProjectile (deferred spawn + InitProjectile + capsule half height + FinishSpawning; Owner = the firing actor, Instigator = its pawn).
 * Capsule radius 14 uu (miner shots: rifle, pellet, turret bullet); robot shots 10 uu. By default the half height equals the
 * radius (the capsule is a sphere: turret bullets, robot shots, shots without a hand weapon). Miner shots leaving from the hand
 * weapon Muzzle get a taller capsule per shot (SpawnProjectile CapsuleHalfHeight, task-412): centred on the Muzzle, its bottom
 * just above the floor, so it reaches down to rock blocks and robots while the shot mesh flies at the Muzzle height; the flat,
 * yaw-only flight keeps it vertical. SweepTo moves a just-spawned shot with a sweep (miner axis to Muzzle, task-412 Q3) and
 * HoldFirstMove keeps it at the Muzzle for its first drawn frame (task-413). Robot
 * shots: HitActorClass = the miner class (robots are passed through) and bDamagesTerrain off. Subclass in Blueprint for the mesh
 * (its collision is turned off at spawn: only the capsule collides) and the capsule radius.
 * Impact sparks (task-432, task-437, art rule A8): a hit on a robot or a stop on a grid block (ARRArenaGrid::IsBlockCell, the cells
 * the drone can dig: rock, every ore, barrel) plays ImpactSparkSystem once at the impact, through URRNiagaraPoolSubsystem (cap
 * ImpactSparkPoolSize; skipped when no slot is free). A stop on anything else shows nothing; robot shots have no system.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRProjectile : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the collision capsule (root, half height = radius) and the projectile movement. */
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
	 * lifetime (s, 0 = unlimited), MaxHits (distinct actors damaged, >= 1) and KnockbackSpeed (uu/s per robot hit, 0 = none) with InitProjectile,
	 * the capsule half height (CapsuleHalfHeight in uu, see SetCapsuleHalfHeight; 0 = keep half height = radius), then finishes the spawn.
	 * The capsule is centred on SpawnTransform. InOwner and InInstigator are ignored by its collision. Returns null (logged) when World
	 * or Class is missing or the spawn failed.
	 */
	static ARRProjectile* SpawnProjectile(UWorld* World, TSubclassOf<ARRProjectile> Class, const FTransform& SpawnTransform,
		AActor* InOwner, APawn* InInstigator, const FGameplayEffectSpecHandle& InDamageSpec, float Speed, float Lifetime, int32 MaxHits = 1, float KnockbackSpeed = 0.f,
		float CapsuleHalfHeight = 0.f);

	/**
	 * Sets the half height of the collision capsule in uu, clamped to at least its radius (half height = radius is a sphere); <= 0
	 * restores half height = radius. The capsule stays centred on the actor (the shot mesh). Call between SpawnActorDeferred and
	 * FinishSpawning (SpawnProjectile does), or before the first move.
	 */
	void SetCapsuleHalfHeight(float HalfHeight);

	/**
	 * Moves this projectile to TargetLocation (actor location) with a sweep of its capsule, right after its spawn (task-412 Q3:
	 * from the miner axis to the Muzzle). Hittable actors overlapped along the way are hit through the normal overlap path (pierce
	 * counted, knockback); a blocking hit (rock, barrel, wall) stops it there as a blocking hit in flight does (OnProjectileStop:
	 * barrel damage, then destroyed). Returns true when the projectile is still flying at TargetLocation, false when spent or stopped.
	 */
	bool SweepTo(const FVector& TargetLocation);

	/**
	 * Holds this projectile where it stands for the rest of the frame it was spawned in, so its first drawn frame shows it there
	 * (task-413: at the Muzzle), and starts its flight on the next frame. Without it, a projectile spawned during the world tick
	 * moves one full frame (speed x frame time: 27 uu at 110 fps, 50 uu at 60 fps) before it is first drawn. The life span is
	 * extended by the held frame time so the range flown is unchanged. Does nothing outside the world tick (the flight then starts
	 * on the next tick anyway), on a spent projectile or once the flight started. Called by ARRWeapon::SpawnShot for Muzzle shots.
	 */
	void HoldFirstMove();

	/** True while HoldFirstMove keeps the projectile in place (its movement starts on the next frame). */
	bool IsHoldingFirstMove() const { return HeldFrameTime >= 0.0; }

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

	/** The collision capsule (root component; half height = radius unless set per shot). */
	UCapsuleComponent* GetCollisionCapsule() const { return CollisionCapsule; }

	/** Radius in uu of the collision capsule of Class's default object (its Blueprint value); 0 when Class is unset. */
	static float GetDefaultCollisionRadius(TSubclassOf<ARRProjectile> Class);

	/** Creates the pooled impact spark components of Class's default system, so the first hits create none (ARRWeapon::Acquire). No system: nothing. */
	static void PrewarmImpactSparks(UWorld* World, TSubclassOf<ARRProjectile> Class);

	/** The movement component. */
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	/** Turns off the collision of the visual components (DisableVisualCollision), binds the overlap and stop events and ignores the owner weapon and the instigator while moving. */
	virtual void PostInitializeComponents() override;

	/**
	 * Only enabled by HoldFirstMove; runs once in TG_PostUpdateWork of the held frame, after that frame's movement tick (which did
	 * not move the projectile): turns the movement simulation back on, so the next frame moves it, and turns this tick off again.
	 * Projectiles otherwise never tick (the movement component moves them).
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * Sets every primitive component other than CollisionCapsule (the Blueprint shot mesh) to NoCollision without overlap
	 * events: only the capsule collides. A shot mesh left with a blocking collision moved with the bullet into robot capsules,
	 * whose next move resolved the penetration by pushing the robot (task-180-01), and stopped other bullets.
	 */
	void DisableVisualCollision();

	/**
	 * Pawn overlap: applies the spec once to a hittable actor with an ASC (ignored actors, CanHit false and actors already hit
	 * excluded), then the knockback to a robot when KnockbackSpeed > 0; destroys this projectile once MaxHits distinct actors were hit.
	 */
	UFUNCTION()
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * Blocking hit (rock, wall): plays the impact spark on a block of the arena grid; when bDamagesTerrain, applies the carried damage
	 * once to a barrel (ARRArenaGrid::ApplyShotDamage); then destroys this projectile.
	 */
	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	/**
	 * Per-bullet damage carried by the spec, read without a target ASC: the fire-time snapshot of the source weapon's
	 * URRWeaponAttributeSet Damage captured in the spec (critical hits not rolled). 0 when the spec has no such capture.
	 */
	float GetCarriedDamage() const;

	/** True for actors this projectile never hits: itself, its owner weapon, the instigator, and the owner's owner. */
	bool IsIgnoredActor(const AActor* OtherActor) const;

	/**
	 * Plays ImpactSparkSystem once through the Niagara pool, at the shot's height: on the horizontal impact point of Impact, or at
	 * this projectile when Impact is null or started inside the target (no impact point). Skipped when no slot is free.
	 */
	void PlayImpactSpark(const FHitResult* Impact);

	/**
	 * Collision capsule on the Projectile channel; root component (the sphere CollisionSphere before task-412). Radius editable in
	 * the Blueprint subclass; half height = radius by default, set per shot by SpawnProjectile (miner shots from the Muzzle).
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CollisionCapsule;

	/** Straight flight without gravity, bounce or sub-stepping; stops on the first blocking hit. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** Only instances of this class are hit (CanHit); others are passed through. None = any actor with an ASC (miner shots). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AActor> HitActorClass;

	/** A blocking hit on a barrel cell damages it (miner shots). Off for shots that must not touch the terrain (robot shots). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool bDamagesTerrain = true;

	/**
	 * One-shot system played at each hit on a robot or a grid block (task-432, task-437, art rule A8). Default
	 * /Game/RogueRigs/FX/NS_ShotImpactSpark (constructor reference); None = no sparks (robot shots).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact")
	TObjectPtr<UNiagaraSystem> ImpactSparkSystem;

	/** Most impact sparks playing at once, all projectile classes (cap of the Niagara pool, 24 x at most 8 sprites); a hit beyond it shows none. Provisional, owner 2026-11-05. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact", meta = (ClampMin = "0"))
	int32 ImpactSparkPoolSize = 24;

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

	/** World time (s) of the frame HoldFirstMove holds (movement simulation off until Tick at its end); -1 when not holding. */
	double HeldFrameTime = -1.0;
};
