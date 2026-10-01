// Rogue Rigs - base weapon actor: own ASC and weapon attributes, CSV stats, auto-fire toward the owning miner's aim.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "RRWeapon.generated.h"

class ARRMiner;
class UAbilitySystemComponent;
class URRAbilitySystemComponent;
class URRAutoFireComponent;
class URRWeaponAttributeSet;
class USceneComponent;
struct FRRWeaponStatsRow;

/**
 * Base class of the miner's weapons (sys-weapons). Owns its URRAbilitySystemComponent (owner and avatar: the
 * weapon) with URRWeaponAttributeSet, and a URRAutoFireComponent that decides when to fire; FireOnce decides
 * what is fired (empty here, overridden per weapon). Acquisition (Acquire, called by the miner after spawn and
 * attach): InitAbilityActorInfo(weapon, weapon), base values from row StatsRowName of WeaponStats.csv, magazine
 * filled, auto-fire configured and started. Upgrade replay and the player registry are not implemented yet.
 * Aim comes from the owning miner's GetAimDirection. Subclass in Blueprint for the mesh and the stats row.
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
	 * row StatsRowName as base values, fills the magazine, configures the auto-fire (ticks after the miner) and
	 * starts it if bStartFiringOnAcquire. Returns false (logged) if the stats could not be loaded; the weapon then
	 * does not fire.
	 */
	bool Acquire(ARRMiner* InOwnerMiner);

	/** Miner that owns this weapon (null before Acquire). */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	ARRMiner* GetOwnerMiner() const { return OwnerMiner; }

	/** Fire direction in world space: the owning miner's aim, or the weapon's flat forward vector without a miner. */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	FVector GetAimDirection() const;

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

protected:
	/** Stops the auto-fire. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Fires one bullet (or one pattern) toward GetAimDirection. Called by the auto-fire for each shot. Empty here. */
	virtual void FireOnce();

	/** Loads row StatsRowName of WeaponStats.csv into OutRow. Returns false (logged) if the file or row is missing. */
	bool LoadStatsRow(FRRWeaponStatsRow& OutRow) const;

	/** Sets the attribute base values of Row on the ASC and copies the CSV constants onto this actor. */
	void ApplyStatsRow(const FRRWeaponStatsRow& Row);

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

	/** Row of WeaponStats.csv applied at acquisition (e.g. BurstRifle). Must be set by the Blueprint subclass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	FName StatsRowName;

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

	/** Miner that acquired this weapon. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ARRMiner> OwnerMiner;
};
