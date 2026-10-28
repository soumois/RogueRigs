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
 * from the targeting origin (GetTargetingOrigin: ShotHeight above the actor, the turret centre); otherwise picks the nearest
 * robot meeting those rules (searched at most every TargetSearchInterval while none is found). Base and head (task-350-01,
 * owner Q3 and Q4 2026-10-28): the actor keeps the yaw it got at placement (the base); only HeadPivot turns, snapped each tick
 * to the yaw toward the locked robot (no turn rate). The Muzzle hangs under HeadPivot, so it follows the canon. Fires only with
 * a target: GetShotOrigin is the Muzzle's horizontal position at ShotHeight above the actor, GetAimDirection points from it at
 * the robot. Bullets are ARRWeapon::FireOnce shots (MaxHits = Pierce, stop on rock, crit rolled per hit; the damage
 * spec's instigator and causer are the turret). Invincible and untargetable: no actor collision (inherited), no health set, so
 * robots, projectiles and the miner pass through it. Destroyed after the launcher's TurretLifetime (actor life span).
 * Blueprintable for the meshes (base under Root, head under HeadPivot), the Muzzle position and the muzzle flash (OnTurretFired).
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRTurret : public ARRWeapon
{
	GENERATED_BODY()

public:
	/** Ticks (targeting); creates HeadPivot under Root and moves the inherited Muzzle under it. The auto-fire does not start before InitFromLauncher. */
	ARRTurret(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Copies Launcher's stats (see the class comment), sets the life span to Launcher's TurretLifetime, finds the arena grid,
	 * configures the auto-fire without magazine and arms it (it fires once a target is locked). Returns false (logged) when the
	 * auto-fire cannot be configured; the turret then never fires.
	 */
	bool InitFromLauncher(const ARRTurretLauncher& Launcher);

	/**
	 * Direction from the shot origin to the locked robot (flat, unit); from the targeting origin when the robot is not ahead of the
	 * Muzzle (closer than the canon tip). Without a target: HeadPivot's flat forward (the canon).
	 */
	virtual FVector GetAimDirection() const override;

	/**
	 * Where bullets and OnTurretFired start: the Muzzle's horizontal position (it turns with HeadPivot) at ShotHeight above the actor
	 * location (the floor top of the turret's cell). Equals GetTargetingOrigin while the Muzzle sits on the yaw axis.
	 */
	virtual FVector GetShotOrigin() const override;

	/** ShotHeight above the actor location (the turret centre): origin of the range and line-of-sight checks, never moved by the head. */
	UFUNCTION(BlueprintPure, Category = "Turret")
	FVector GetTargetingOrigin() const;

	/** Component the head turns with (relative yaw only); the head mesh and the Muzzle are attached under it. */
	UFUNCTION(BlueprintPure, Category = "Turret")
	USceneComponent* GetHeadPivot() const { return HeadPivot; }

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
	 * True when Enemy can be locked from this turret now: alive, not being destroyed, within GetRange (2D) of the targeting origin
	 * and, with an arena grid, in grid line of sight from the targeting origin.
	 */
	bool IsValidTarget(const ARREnemy* Enemy) const;

	/** Keeps the locked target if still valid, else searches the nearest valid robot (throttled by TargetSearchInterval while none). Called by Tick; tests may call it. */
	void UpdateTarget(float DeltaSeconds);

	/** Updates the target, turns HeadPivot toward it (the actor keeps its placement yaw) and starts or pauses the auto-fire. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Turns HeadPivot toward the locked robot, then one bullet from the canon tip (ARRWeapon::FireOnce), then OnTurretFired; skipped without a valid target. */
	virtual void FireOnce() override;

	/** Nearest valid robot (IsValidTarget) of the world, or null. */
	ARREnemy* FindNearestTarget() const;

	/**
	 * With a locked robot: sets HeadPivot's relative yaw so its world yaw points from HeadPivot at the robot (flat; relative yaw =
	 * target yaw minus actor yaw, snapped, pitch and roll kept). Without a target the head keeps its last yaw.
	 */
	void UpdateHeadYaw();

	/** Blueprint hook for the muzzle flash, after each bullet: Origin = shot origin (canon tip), Direction = flat unit direction to the target. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Turret")
	void OnTurretFired(FVector Origin, FVector Direction);

	/**
	 * Yawing head, under Root (identity relative transform by default; BP_Turret may move it onto the base's yaw axis). Only its
	 * relative yaw is driven (UpdateHeadYaw). BP_Turret attaches the head mesh under it; the inherited Muzzle is attached under it.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> HeadPivot;

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
