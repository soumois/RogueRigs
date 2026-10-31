// Rogue Rigs - shotgun: each shot fires PelletCount piercing pellets spread evenly over +/-SpreadDeg around the aim, each with knockback.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RRWeapon.h"
#include "RRShotgun.generated.h"

/**
 * Shotgun (shotgun gdd, sys-weapons). Same weapon ASC, attributes, magazine and auto-fire as every ARRWeapon (CSV row
 * Shotgun: BurstCount 1). Each FireOnce fires PelletCount projectiles of ProjectileClass (native default ARRShotgunPellet)
 * placed by GetShotPlacement (task-412 Q4: held by a miner with a shown hand weapon, from that weapon's Muzzle with the same
 * vertical capsule and miner-to-Muzzle sweep as the burst rifle) through SpawnShot, flat, at yaw offsets spread evenly from -SpreadDeg to +SpreadDeg
 * around the aim (no random), speed ProjectileSpeed, lifetime Range / ProjectileSpeed. All pellets of a shot carry one damage
 * spec (MakeDamageSpec; each application rolls its own critical in URRDamageExecution), MaxHits = GetPierceCount() and
 * KnockbackSpeed = KnockbackForce (each robot hit is pushed along the pellet's flight; ARREnemy::ApplyKnockback scales it by
 * 1 - KnockbackResistance). Pellets stop on rock and damage barrels like every projectile. Then one native muzzle flash per
 * volley, not per pellet (ARRWeapon::SpawnMuzzleFlash along the aim, task-414), and OnShotFired for any extra Blueprint effect. Usable natively (StatsRowName Shotgun, ProjectileClass ARRShotgunPellet, no mesh): ARRMiner spawns it when no
 * StartingWeapons entry is a shotgun. Subclass in Blueprint for the mesh, the pellet Blueprint and the muzzle flash.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRShotgun : public ARRWeapon
{
	GENERATED_BODY()

public:
	/** Sets StatsRowName to Shotgun and ProjectileClass to ARRShotgunPellet. */
	ARRShotgun(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Pellets fired per shot (CSV constant PelletCount, at least 1). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	int32 GetPelletCount() const { return PelletCount; }

	/** Knockback launch speed in uu/s given by each pellet hit, before the robot's resistance (CSV constant KnockbackForce). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	float GetKnockbackForce() const { return KnockbackForce; }

	/** Yaw offset in degrees from the aim of pellet PelletIndex (0 .. PelletCount - 1): evenly from -SpreadDeg to +SpreadDeg; 0 for a single pellet. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	float GetPelletYawOffset(int32 PelletIndex) const;

	/** Shots fired since BeginPlay (one per FireOnce that spawned its pellets). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	int32 GetShotCount() const { return ShotCount; }

	/** Pellets actually spawned by the last shot (PelletCount, or fewer if a spawn failed). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	int32 GetLastPelletsSpawned() const { return LastPelletsSpawned; }

protected:
	/** One shot: PelletCount pellets around the aim (see the class comment), one muzzle flash when a pellet spawned, then OnShotFired. Skipped (logged) without ProjectileClass or with ProjectileSpeed or Range <= 0. */
	virtual void FireOnce() override;

	/** Copies the base columns, then PelletCount and KnockbackForce. */
	virtual void ApplyStatsRow(const FRRWeaponStatsRow& Row) override;

	/**
	 * Blueprint hook for the muzzle flash, called after the pellets of a shot were spawned. Origin: GetShotOrigin at the shot;
	 * Direction: aim (unit, Z = 0); FanSpreadDeg: half-angle of the pellet fan in degrees (SpreadDeg; named apart from the member).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Shotgun")
	void OnShotFired(FVector Origin, FVector Direction, float FanSpreadDeg);

	/** Pellets per shot, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 PelletCount = 1;

	/** Knockback launch speed in uu/s per pellet hit, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	float KnockbackForce = 0.f;

	/** Shots fired since BeginPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 ShotCount = 0;

	/** Pellets spawned by the last shot. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 LastPelletsSpawned = 0;
};
