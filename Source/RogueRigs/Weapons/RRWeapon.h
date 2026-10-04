// Rogue Rigs - base weapon actor: own ASC and weapon attributes, CSV stats, auto-fire toward the owning miner's aim.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "RRWeapon.generated.h"

class ARRMiner;
class ARRProjectile;
class UAbilitySystemComponent;
class UGameplayEffect;
class URRAbilitySystemComponent;
class URRAutoFireComponent;
class URRWeaponAttributeSet;
class USceneComponent;
struct FRRWeaponStatsRow;

/**
 * Base class of the miner's weapons (sys-weapons). Owns its URRAbilitySystemComponent (owner and avatar: the
 * weapon) with URRWeaponAttributeSet, and a URRAutoFireComponent that decides when to fire; FireOnce decides
 * what is fired (default: one ProjectileClass from GetShotOrigin carrying a DamageEffectClass spec, with yaw
 * spread; overridden by weapons that fire differently). Shots start ShotHeight above the floor under the owning
 * miner, ShotForwardOffset ahead of its capsule, so they hit floor-standing targets and stop on blocks; the Muzzle is
 * only a visual reference (and the spawn point when there is no owning miner). The weapon actor has no collision
 * (bActorEnableCollision off): it is a visual carried inside the miner capsule. Acquisition (Acquire, called by the miner after spawn and
 * attach): InitAbilityActorInfo(weapon, weapon), base values from row StatsRowName of WeaponStats.csv, magazine
 * filled, auto-fire configured and started; the miner's global upgrades are replayed after the base values
 * (URRUpgradeComponent::ReplayGlobalUpgrades). The player registry is not implemented yet.
 * Aim comes from the owning miner's GetAimDirection. Subclass in Blueprint for the mesh, the muzzle position,
 * the projectile class and the stats row.
 */
UCLASS(Abstract)
class ROGUERIGS_API ARRWeapon : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/** Path of the weapon stats CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* WeaponStatsPath;

	/** Creates the root, the ASC, the weapon attribute set and the auto-fire component. */
	ARRWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ IAbilitySystemInterface
	/** Returns the weapon's ASC (owner and avatar: this weapon). */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Returns the weapon's ASC with its project type. */
	URRAbilitySystemComponent* GetRRAbilitySystemComponent() const { return AbilitySystemComponent; }

	/**
	 * Acquisition, after spawn and attach: stores InOwnerMiner, initializes the ASC actor info, applies the CSV
	 * row StatsRowName as base values, replays the miner's global upgrades, fills the magazine, configures the auto-fire (ticks after the miner) and
	 * starts it if bStartFiringOnAcquire. Returns false (logged) if the stats could not be loaded; the weapon then
	 * does not fire.
	 */
	bool Acquire(ARRMiner* InOwnerMiner);

	/** Miner that owns this weapon (null before Acquire). */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	ARRMiner* GetOwnerMiner() const { return OwnerMiner; }

	/**
	 * Fire direction in world space: the owning miner's aim, or the weapon's flat forward vector without a miner. Virtual so a
	 * weapon not held by a miner (turret) supplies its own aim.
	 */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	virtual FVector GetAimDirection() const;

	/**
	 * World location where FireOnce spawns its projectile. With an owning miner: the floor under the miner (capsule bottom
	 * minus the CharacterMovement floor distance when standing on a walkable floor) + ShotHeight, moved horizontally along
	 * GetAimDirection by the capsule radius + ShotForwardOffset. Without a miner: the Muzzle location. Virtual so a weapon not
	 * held by a miner (turret) supplies its own origin.
	 */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	virtual FVector GetShotOrigin() const;

	/** Height of the shots above the floor under the owning miner, in uu. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Projectile")
	float GetShotHeight() const { return ShotHeight; }

	/** Distance of the shot origin ahead of the owning miner's capsule surface, along the aim, in uu. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Projectile")
	float GetShotForwardOffset() const { return ShotForwardOffset; }

	/** The component that decides when this weapon fires. */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	URRAutoFireComponent* GetAutoFireComponent() const { return AutoFire; }

	/** Bullets per burst (CSV constant). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	int32 GetBurstCount() const { return BurstCount; }

	/** Share of the fire period used by a burst (CSV constant). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetBurstDuty() const { return BurstDuty; }

	/** Base reload duration in seconds before ReloadSpeed (CSV constant). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetReloadTime() const { return ReloadTime; }

	/** Projectile speed in uu/s (CSV constant). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetProjectileSpeed() const { return ProjectileSpeed; }

	/** Maximum range in uu (CSV constant). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetRange() const { return Range; }

	/** Half-angle of the random spread in degrees (CSV constant). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	float GetSpreadDeg() const { return SpreadDeg; }

	/** Name shown in the UI (CSV column DisplayName, or StatsRowName when empty or before acquisition). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Stats")
	FText GetDisplayName() const;

protected:
	/** Stops the auto-fire. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Fires one shot, called by the auto-fire for each bullet. Default (burst rifle): makes a DamageEffectClass spec
	 * on the weapon's ASC, spawns one ProjectileClass at GetShotOrigin, flat, toward GetAimDirection rotated by a random
	 * yaw in [-SpreadDeg, SpreadDeg], with speed ProjectileSpeed and lifetime Range / ProjectileSpeed (Owner: this
	 * weapon, Instigator: the miner; ARRProjectile::SpawnProjectile). Logged and skipped if ProjectileClass is unset or ProjectileSpeed or Range is <= 0.
	 * Weapons that fire differently (shotgun) override it.
	 */
	virtual void FireOnce();

	/** Makes the outgoing DamageEffectClass spec on the weapon's ASC (context instigator and causer: this weapon). */
	FGameplayEffectSpecHandle MakeDamageSpec() const;

	/** Loads row StatsRowName of WeaponStats.csv into OutRow. Returns false (logged) if the file or row is missing. */
	bool LoadStatsRow(FRRWeaponStatsRow& OutRow) const;

	/** Sets the attribute base values of Row on the ASC and copies the CSV constants onto this actor. Overrides call Super and copy their own columns. */
	virtual void ApplyStatsRow(const FRRWeaponStatsRow& Row);

	/** Root scene component; Blueprint subclasses attach the mesh and muzzle under it. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	/** The weapon's ability system component (owner and avatar: this weapon). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<URRAbilitySystemComponent> AbilitySystemComponent;

	/** Damage, FireRate, ReloadSpeed, CritChance, CritDamage, MagazineSize, Ammo. Registered on the ASC as a default subobject. */
	UPROPERTY()
	TObjectPtr<URRWeaponAttributeSet> WeaponSet;

	/** Decides when to fire (burst timing, magazine, reload). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRAutoFireComponent> AutoFire;

	/**
	 * Barrel tip, under Root; its relative location is set in the Blueprint subclass. Visual reference only (muzzle effects):
	 * shots of a weapon held by a miner start at GetShotOrigin. Spawn point of the shots only when there is no owning miner.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Muzzle;

	/** Projectile spawned by FireOnce. Must be set by the Blueprint subclass (a Blueprint of ARRProjectile). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile")
	TSubclassOf<ARRProjectile> ProjectileClass;

	/** Damage effect carried by each projectile. Defaults to URRGE_Damage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** Row of WeaponStats.csv applied at acquisition (e.g. BurstRifle). Must be set by the Blueprint subclass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	FName StatsRowName;

	/** Height of the shots above the floor under the owning miner, in uu. Must stay below the grid's BlockHeight (100 by default) so shots stop on blocks; checked at acquisition. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = 0, Units = "cm"))
	float ShotHeight = 50.f;

	/** Distance of the shot origin ahead of the owning miner's capsule surface, along the aim, in uu (origin = capsule radius + this value from the miner axis). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = 0, Units = "cm"))
	float ShotForwardOffset = 20.f;

	/** True = the auto-fire starts at acquisition. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bStartFiringOnAcquire = true;

	/** Bullets per burst, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	int32 BurstCount = 1;

	/** Share of the fire period used by a burst, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float BurstDuty = 0.f;

	/** Base reload duration in seconds, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float ReloadTime = 0.f;

	/** Projectile speed in uu/s, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float ProjectileSpeed = 0.f;

	/** Maximum range in uu, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float Range = 0.f;

	/** Half-angle of the random spread in degrees, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float SpreadDeg = 0.f;

	/** UI name, copied from the CSV column DisplayName at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	FString DisplayName;

	/** Miner that acquired this weapon. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ARRMiner> OwnerMiner;
};
