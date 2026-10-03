// Rogue Rigs - shotgun: instant conical wave hitting every robot in the cone with a clear grid line of sight, with knockback.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RRWeapon.h"
#include "RRShotgun.generated.h"

class ARRArenaGrid;
class ARREnemy;

/** Parameters of the last shotgun wave, for the VFX and the tests. */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRShotgunWave
{
	GENERATED_BODY()

	/** Apex of the cone: the miner's axis at shot height (the Muzzle without a miner). */
	UPROPERTY(BlueprintReadOnly, Category = "Shotgun")
	FVector Origin = FVector::ZeroVector;

	/** Aim direction of the wave (unit, Z = 0). */
	UPROPERTY(BlueprintReadOnly, Category = "Shotgun")
	FVector Direction = FVector::ForwardVector;

	/** Half of the cone angle, in degrees. */
	UPROPERTY(BlueprintReadOnly, Category = "Shotgun")
	float HalfAngleDeg = 0.f;

	/** Range of the wave in uu (2D, from Origin). */
	UPROPERTY(BlueprintReadOnly, Category = "Shotgun")
	float Range = 0.f;

	/** Distance along Direction from Origin to the first block cell (grid line of sight), capped at Range. */
	UPROPERTY(BlueprintReadOnly, Category = "Shotgun")
	float ClippedLength = 0.f;

	/** Robots hit by the wave. */
	UPROPERTY(BlueprintReadOnly, Category = "Shotgun")
	int32 HitCount = 0;

	/** World time of the wave in seconds. */
	UPROPERTY(BlueprintReadOnly, Category = "Shotgun")
	double Time = 0.0;
};

/**
 * Shotgun (shotgun gdd, sys-weapons). Same weapon ASC, attributes, magazine and auto-fire as every ARRWeapon (CSV row
 * Shotgun: BurstCount 1). Each FireOnce is an instant conical wave, no projectile: every living ARREnemy whose center is
 * within Range (2D) of the wave origin and within ConeAngleDeg / 2 of the aim, with a clear grid line of sight from the
 * origin (ARRArenaGrid::HasLineOfSight: rock and every other block stop it), receives the weapon's damage spec (one spec per
 * wave, applied to each robot, so each robot gets its own critical roll in URRDamageExecution) and a knockback of
 * KnockbackForce uu/s outward from the origin (ARREnemy::ApplyKnockback scales it by 1 - KnockbackResistance). Then
 * OnWaveFired gives the cone parameters and the rock-clipped length to the Blueprint VFX. Blocks and barrels are not damaged.
 * Usable natively (StatsRowName Shotgun, no mesh): ARRMiner spawns it when no StartingWeapons entry is a shotgun. Subclass
 * in Blueprint for the mesh and the wave VFX.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRShotgun : public ARRWeapon
{
	GENERATED_BODY()

public:
	/** Sets StatsRowName to Shotgun. */
	ARRShotgun(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Full cone angle in degrees (CSV constant ConeAngleDeg). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	float GetConeAngleDeg() const { return ConeAngleDeg; }

	/** Knockback launch speed in uu/s before the robot's resistance (CSV constant KnockbackForce). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	float GetKnockbackForce() const { return KnockbackForce; }

	/** Apex of the cone now: the owning miner's axis at the shot height (GetShotOrigin moved back to the axis); the Muzzle without a miner. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	FVector GetWaveOrigin() const;

	/** The last wave fired (zeros before the first). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	FRRShotgunWave GetLastWave() const { return LastWave; }

	/** Total waves fired since BeginPlay. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	int32 GetWaveCount() const { return WaveCount; }

	/**
	 * Clear distance of the last wave along its aim rotated by YawOffsetDeg (grid line of sight, capped at its Range). For VFX
	 * that clip several rays of the cone; Range when there is no grid.
	 */
	UFUNCTION(BlueprintPure, Category = "Weapon|Shotgun")
	float GetWaveClearDistance(float YawOffsetDeg) const;

	/** Living robots a wave from Origin along Direction would hit now (range, angle and line of sight), nearest first. */
	TArray<ARREnemy*> FindWaveTargets(const FVector& Origin, const FVector& Direction) const;

protected:
	/** One wave: finds the targets, applies the damage spec and the knockback to each, records LastWave, calls OnWaveFired. */
	virtual void FireOnce() override;

	/** Copies the base columns, then ConeAngleDeg and KnockbackForce. */
	virtual void ApplyStatsRow(const FRRWeaponStatsRow& Row) override;

	/** Grid used for the line of sight: the owning miner's, else the first ARRArenaGrid of the world (cached); null when none. */
	ARRArenaGrid* ResolveGrid() const;

	/**
	 * Blueprint VFX hook, called after each wave (hits already applied). Origin: cone apex at shot height; Direction: aim (unit,
	 * Z = 0); HalfAngleDeg: half of the cone angle; Range: full range in uu; ClippedLength: distance along Direction to the first
	 * rock (<= Range). Use GetWaveClearDistance for other rays of the cone.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Shotgun")
	void OnWaveFired(FVector Origin, FVector Direction, float HalfAngleDeg, float WaveRange, float ClippedLength);

	/** Full cone angle in degrees, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	float ConeAngleDeg = 0.f;

	/** Knockback launch speed in uu/s, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	float KnockbackForce = 0.f;

	/** The last wave fired. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	FRRShotgunWave LastWave;

	/** Waves fired since BeginPlay. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 WaveCount = 0;

	/** Grid found by ResolveGrid when there is no owning miner. */
	mutable TWeakObjectPtr<ARRArenaGrid> CachedGrid;
};
