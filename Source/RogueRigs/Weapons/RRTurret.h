// Rogue Rigs - placed turret: copies its launcher's stats at placement, locks the nearest robot in range with grid line of sight, fires piercing bullets until its lifetime ends.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RRWeapon.h"
#include "RRTurret.generated.h"

class ARRArenaGrid;
class ARREnemy;
class ARRTurretLauncher;

/**
 * Turret placed by ARRTurretLauncher (turret gdd, sys-weapons). An ARRWeapon with its own ASC and URRWeaponAttributeSet, not held
 * by a miner and never in the miner's weapon list: InitFromLauncher copies every current attribute of the launcher as base values
 * (plus its owned gameplay tags as loose tags, its projectile and damage effect classes and its CSV constants), so upgrades taken
 * later reach only turrets placed later. The auto-fire runs without magazine (MagazineSize is the launcher's charge count) at the
 * copied FireRate. Targeting (Tick): keeps the locked robot while it is alive, within GetRange (2D) and in grid line of sight
 * from the shot origin; otherwise picks the nearest robot meeting those rules (searched at most every TargetSearchInterval while
 * none is found). Fires only with a target: GetAimDirection points at it, GetShotOrigin is ShotHeight above the actor (the floor
 * top of its cell). Bullets are ARRWeapon::FireOnce shots (MaxHits = Pierce, stop on rock, crit rolled per hit; the damage
 * spec's instigator and causer are the turret). Invincible and untargetable: no actor collision (inherited), no health set, so
 * robots, projectiles and the miner pass through it. Destroyed after the launcher's TurretLifetime (actor life span).
 * Blueprintable for the mesh and the muzzle flash (OnTurretFired).
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRTurret : public ARRWeapon
{
	GENERATED_BODY()

public:
	/** Ticks (targeting); the auto-fire does not start before InitFromLauncher. */
	ARRTurret(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Copies Launcher's stats (see the class comment), sets the life span to Launcher's TurretLifetime, finds the arena grid,
	 * configures the auto-fire without magazine and arms it (it fires once a target is locked). Returns false (logged) when the
	 * auto-fire cannot be configured; the turret then never fires.
	 */
	bool InitFromLauncher(const ARRTurretLauncher& Launcher);

	/** Direction to the locked robot (flat, unit); the actor's flat forward without a target. */
	virtual FVector GetAimDirection() const override;

	/** ShotHeight above the actor location (the floor top of the turret's cell). */
	virtual FVector GetShotOrigin() const override;

	/** Locked robot, or null. */
	UFUNCTION(BlueprintPure, Category = "Turret")
	ARREnemy* GetTarget() const { return Target.Get(); }

	/** Bullets fired since placement. */
	UFUNCTION(BlueprintPure, Category = "Turret")
	int32 GetShotCount() const { return ShotCount; }

	/** Lifetime set at placement in seconds (the launcher's TurretLifetime). */
	UFUNCTION(BlueprintPure, Category = "Turret")
	float GetLifetime() const { return Lifetime; }

	/**
	 * True when Enemy can be locked from this turret now: alive, not being destroyed, within GetRange (2D) of the shot origin and,
	 * with an arena grid, in grid line of sight from the shot origin.
	 */
	bool IsValidTarget(const ARREnemy* Enemy) const;

	/** Keeps the locked target if still valid, else searches the nearest valid robot (throttled by TargetSearchInterval while none). Called by Tick; tests may call it. */
	void UpdateTarget(float DeltaSeconds);

	/** Updates the target, turns the actor toward it and starts or pauses the auto-fire. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** One bullet toward the locked robot (ARRWeapon::FireOnce), then OnTurretFired; skipped without a valid target. */
	virtual void FireOnce() override;

	/** Nearest valid robot (IsValidTarget) of the world, or null. */
	ARREnemy* FindNearestTarget() const;

	/** Blueprint hook for the muzzle flash, after each bullet: Origin = shot origin, Direction = flat unit direction to the target. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Turret")
	void OnTurretFired(FVector Origin, FVector Direction);

	/** Seconds between two searches for a target while none is locked (a lost target triggers an immediate search). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret", meta = (ClampMin = 0, Units = "s"))
	float TargetSearchInterval = 0.1f;

	/** Locked robot. */
	TWeakObjectPtr<ARREnemy> Target;

	/** Arena grid used for line of sight; none = no sight check. */
	TWeakObjectPtr<ARRArenaGrid> ArenaGrid;

	/** Seconds until the next search while no target is locked. */
	float TimeToNextSearch = 0.f;

	/** True once InitFromLauncher succeeded. */
	bool bArmed = false;

	/** Bullets fired since placement. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret")
	int32 ShotCount = 0;

	/** Lifetime set at placement in seconds. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret")
	float Lifetime = 0.f;
};
