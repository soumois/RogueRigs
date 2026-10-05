// Rogue Rigs - DataTable row holding one weapon's base stats and constants (WeaponStats.csv).

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RRWeaponStatsRow.generated.h"

/**
 * One row of Content/RogueRigs/Data/Tables/WeaponStats.csv, one row per weapon (row name = weapon id,
 * e.g. BurstRifle). Column names match the property names. Damage, FireRate, ReloadSpeed, CritChance,
 * CritDamage, MagazineSize and Pierce become base values of URRWeaponAttributeSet (Ammo starts at MagazineSize);
 * the other columns are constants copied onto the weapon actor (ARRWeapon) because nothing changes them during a run.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRWeaponStatsRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Base Damage per bullet (attribute). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float Damage = 0.f;

	/** Base FireRate in bursts per second (attribute). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float FireRate = 0.f;

	/** Bullets per burst (constant). 1 = single shots, BurstDuty ignored. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	int32 BurstCount = 1;

	/** Share of the fire period used by the burst, 0 to 1 (constant). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float BurstDuty = 0.f;

	/** Base CritChance, 0 to 1 (attribute). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float CritChance = 0.f;

	/** Base CritDamage multiplier (attribute). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float CritDamage = 0.f;

	/** Base MagazineSize in bullets (attribute); Ammo starts full. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float MagazineSize = 0.f;

	/** Base Pierce: distinct robots one projectile damages before it is destroyed (attribute; 0 for weapons without projectiles hits at least 1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float Pierce = 0.f;

	/** Base reload duration in seconds (constant); actual duration = ReloadTime / ReloadSpeed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float ReloadTime = 0.f;

	/** Base ReloadSpeed multiplier (attribute). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float ReloadSpeed = 0.f;

	/** Projectile speed in uu/s (constant). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float ProjectileSpeed = 0.f;

	/** Maximum range in uu (constant). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float Range = 0.f;

	/** Half-angle of the random spread in degrees (constant). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float SpreadDeg = 0.f;

	/** Name shown in the UI (upgrade card target), French (constant). Empty = the row name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	FString DisplayName;

	/** Projectiles fired per shot, spread evenly over +/-SpreadDeg (constant; read by the shotgun, 1 elsewhere). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	int32 PelletCount = 1;

	/** Knockback launch speed in uu/s given by each projectile hit to a robot, before its KnockbackResistance (constant); 0 = no knockback. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float KnockbackForce = 0.f;

	/** Seconds a placed turret lives before it disappears (constant; read by the turret launcher, 0 elsewhere). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float TurretLifetime = 0.f;
};
