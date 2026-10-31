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
struct FOnAttributeChangeData;

/**
 * Where and how one shot of a weapon is spawned (ARRWeapon::GetShotPlacement, task-412). Origin: the projectile location (its shot
 * mesh and capsule centre). CapsuleHalfHeight: the projectile capsule half height in uu, 0 = keep half height = radius (a sphere).
 * bSweepFromMiner: the projectile is spawned at SweepStart (the miner axis, at the Origin height) and swept to Origin
 * (ARRProjectile::SweepTo) so a rock between the miner and the Muzzle stops it and a robot there is hit.
 */
struct FRRShotPlacement
{
	/** Projectile spawn location (shot mesh and capsule centre). */
	FVector Origin = FVector::ZeroVector;

	/** Projectile capsule half height in uu; 0 = half height = radius (sphere). */
	float CapsuleHalfHeight = 0.f;

	/** True = spawn at SweepStart and sweep to Origin before the flight. */
	bool bSweepFromMiner = false;

	/** Sweep start: the owning miner's axis at the Origin height (used when bSweepFromMiner). */
	FVector SweepStart = FVector::ZeroVector;
};

/** Fired when the weapon magazine shown on the HUD changed: Ammo or MagazineSize rounded down (shot, refill, upgrade). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnWeaponAmmoChangedSignature, ARRWeapon*, Weapon, int32, Rounds, int32, MagazineSize);

/**
 * Base class of the miner's weapons (sys-weapons). Owns its URRAbilitySystemComponent (owner and avatar: the
 * weapon) with URRWeaponAttributeSet, and a URRAutoFireComponent that decides when to fire; FireOnce decides
 * what is fired (default: one ProjectileClass from GetShotOrigin carrying a DamageEffectClass spec, with yaw
 * spread; overridden by weapons that fire differently). Shot origin (GetShotPlacement, task-412): when the owning miner holds a
 * weapon on its hand socket (the shown weapon, the burst rifle), every weapon of that miner (the hand weapon and every weapon
 * without a hand socket, the shotgun) shoots from the hand weapon's Muzzle read at fire time, with a vertical projectile capsule
 * centred on the Muzzle whose bottom is ShotFloorClearance above the floor (so it reaches rocks and robots and never touches the
 * floor), swept from the miner axis to the Muzzle first; a Muzzle too low for that capsule falls back to a sphere ShotHeight above
 * the floor under the Muzzle (warning). Without a hand weapon: ShotHeight above the floor, ShotForwardOffset ahead of the miner
 * capsule, a sphere. Without a miner: the own Muzzle (turrets override GetShotOrigin). The weapon actor has no collision
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
	 * World location where FireOnce spawns its projectile (its shot mesh). With an owning miner: GetShotPlacement().Origin (the
	 * hand weapon Muzzle, its low-Muzzle fallback, or the floor + ShotHeight point without a hand weapon). Without a miner: the
	 * Muzzle location. Virtual so a weapon not held by a miner (turret) supplies its own origin.
	 */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	virtual FVector GetShotOrigin() const;

	/**
	 * Placement of a shot fired now (task-412), read at fire time. With an owning miner holding a hand weapon (FindHandWeapon):
	 * Origin = that weapon's Muzzle world location; capsule half height = Muzzle height - (floor + ShotFloorClearance), so the
	 * capsule centred on the Muzzle has its bottom ShotFloorClearance above the floor and its top as far above the Muzzle; sweep from
	 * the miner axis. When the Muzzle is lower than floor + ShotFloorClearance + the projectile radius: Origin under the Muzzle at
	 * floor + ShotHeight, a sphere, still swept from the miner axis, with a warning (logged once per weapon). With an owning miner
	 * and no hand weapon: the floor + ShotHeight point ShotForwardOffset ahead of the miner capsule, a sphere, no sweep. Without a
	 * miner: GetShotOrigin (own Muzzle; turret override), a sphere, no sweep. Floor: the miner capsule bottom minus the
	 * CharacterMovement floor distance on a walkable floor.
	 */
	FRRShotPlacement GetShotPlacement() const;

	/** True when this weapon has a hand socket (HandSocketName) and is attached to its owning miner's mesh at that socket: the shown weapon. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Attach")
	bool IsHeldOnHandSocket() const;

	/** First valid weapon of the owning miner held on its hand socket (IsHeldOnHandSocket), this weapon included; null without one or without a miner. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Attach")
	ARRWeapon* FindHandWeapon() const;

	/** Clearance in uu between the floor and the bottom of the projectile capsule of shots fired from the hand weapon Muzzle. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Projectile")
	float GetShotFloorClearance() const { return ShotFloorClearance; }

	/** Height of the shots above the floor under the owning miner, in uu. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Projectile")
	float GetShotHeight() const { return ShotHeight; }

	/** Distance of the shot origin ahead of the owning miner's capsule surface, along the aim, in uu. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Projectile")
	float GetShotForwardOffset() const { return ShotForwardOffset; }

	/**
	 * Socket of the miner mesh this weapon attaches to (e.g. WeaponSocket_R on the right hand), or None to keep the miner's
	 * shared attachment (ARRMiner WeaponAttachSocketName, or the capsule at WeaponAttachOffset). Read by ARRMiner::AddWeapon.
	 */
	UFUNCTION(BlueprintPure, Category = "Weapon|Attach")
	FName GetHandSocketName() const { return HandSocketName; }

	/** Barrel tip component (see Muzzle): shot origin of the miner's weapons when this weapon is held on the hand socket. Read by URRMinerAnimInstance to counter-move it with the rifle mesh (task-411-01). */
	USceneComponent* GetMuzzle() const { return Muzzle; }

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

	/**
	 * UI name of a weapon class before any instance exists (new-weapon cards): CSV column DisplayName of the class default
	 * object's StatsRowName, or that row name when the column is empty; empty text when WeaponClass is unset.
	 */
	static FText GetClassDisplayName(TSubclassOf<ARRWeapon> WeaponClass);

	/** Projectile class FireOnce spawns (a turret copies its launcher's). */
	TSubclassOf<ARRProjectile> GetProjectileClass() const { return ProjectileClass; }

	/** Damage effect carried by the projectiles (a turret copies its launcher's). */
	TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }

	/** Distinct robots one projectile fired now damages: the Pierce attribute rounded down, at least 1 (sys-projectiles). */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetPierceCount() const;

	/** Rounds in the magazine as shown on the HUD (Ammo rounded down, at least 0). */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetRounds() const;

	/** Magazine size as shown on the HUD (MagazineSize rounded down, at least 0). */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetMagazineRounds() const;

	/**
	 * Broadcast when GetRounds or GetMagazineRounds changed (ASC Ammo or MagazineSize change: shot, reload refill, upgrade, CSV
	 * stats at acquisition), once per change of the rounded values; never when they stay the same. Bound from Acquire on.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FRROnWeaponAmmoChangedSignature OnAmmoChanged;

protected:
	/** ASC callback for Ammo and MagazineSize: broadcasts OnAmmoChanged when the rounded values changed. */
	void HandleAmmoAttributeChanged(const FOnAttributeChangeData& ChangeData);

	/** Handle of the Ammo change binding on the ASC. */
	FDelegateHandle AmmoChangedHandle;

	/** Handle of the MagazineSize change binding on the ASC. */
	FDelegateHandle MagazineSizeChangedHandle;

	/** Rounds last sent by OnAmmoChanged (-1 before the first). */
	int32 LastBroadcastRounds = INDEX_NONE;

	/** Magazine size last sent by OnAmmoChanged (-1 before the first). */
	int32 LastBroadcastMagazine = INDEX_NONE;

	/** Stops the auto-fire. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Turns shadow casting off on every primitive component, Blueprint weapon meshes included (task-263-01: only rocks cast). */
	virtual void PostInitializeComponents() override;

	/**
	 * Fires one shot, called by the auto-fire for each bullet. Default (burst rifle): makes a DamageEffectClass spec
	 * on the weapon's ASC, spawns one ProjectileClass placed by GetShotPlacement (SpawnShot), flat, toward GetAimDirection rotated
	 * by a random yaw in [-SpreadDeg, SpreadDeg], with speed ProjectileSpeed and lifetime Range / ProjectileSpeed (Owner: this
	 * weapon, Instigator: the miner). Logged and skipped if ProjectileClass is unset or ProjectileSpeed or Range is <= 0.
	 * Weapons that fire differently (shotgun) override it.
	 */
	virtual void FireOnce();

	/**
	 * Spawns one ProjectileClass for Placement with ShotRotation (ARRProjectile::SpawnProjectile with Placement's capsule half
	 * height): at Placement.SweepStart then swept to Placement.Origin when bSweepFromMiner (a rock on the way stops it there, robots
	 * on the way are hit), else at Placement.Origin. Owner: this weapon, Instigator: the miner (or this weapon's instigator).
	 * Returns the spawned projectile (possibly already destroyed by the sweep), null when the spawn failed.
	 */
	ARRProjectile* SpawnShot(const FRRShotPlacement& Placement, const FRotator& ShotRotation, const FGameplayEffectSpecHandle& Spec, int32 MaxHits, float KnockbackSpeed);

	/** Floor height under the owning miner: capsule bottom, lowered by the CharacterMovement floor distance on a walkable floor. False without a miner capsule. */
	bool GetFloorZUnderMiner(double& OutFloorZ) const;

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
	 * Barrel tip, under Root (ARRTurret: under its HeadPivot); its relative location is set in the Blueprint subclass (BP_BurstRifle:
	 * placed by hand by the owner). Not visual only since task-412: when this weapon is held on the miner hand socket, its world
	 * location at fire time is the shot origin of every weapon of that miner (GetShotPlacement). Also the spawn point of the shots
	 * when there is no owning miner (a turret uses its horizontal position).
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

	/**
	 * Height of the shots above the floor under the owning miner, in uu, for shots without a hand weapon and for the low-Muzzle
	 * fallback. Must stay below the grid's BlockHeight (100 by default) so shots stop on blocks; checked at acquisition.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = 0, Units = "cm"))
	float ShotHeight = 50.f;

	/**
	 * Gap in uu between the floor under the miner and the bottom of the projectile capsule of shots fired from the hand weapon Muzzle
	 * (task-412 Q1, owner 2026-10-31, provisional: 20). Above 0 so shots never stop on the floor, below the grid BlockHeight so they
	 * never pass over a rock; checked at acquisition.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = 0, Units = "cm"))
	float ShotFloorClearance = 20.f;

	/** True once the low-Muzzle fallback warning was logged for this weapon (logged once, not per shot). */
	mutable bool bLowMuzzleWarned = false;

	/** Distance of the shot origin ahead of the owning miner's capsule surface, along the aim, in uu (origin = capsule radius + this value from the miner axis). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Projectile", meta = (ClampMin = 0, Units = "cm"))
	float ShotForwardOffset = 20.f;

	/**
	 * Per-weapon socket of the miner mesh (task-400): when set and present on the miner mesh, ARRMiner::AddWeapon snaps the
	 * weapon to it (the burst rifle: WeaponSocket_R in the right hand). None (default) = the miner's shared attachment
	 * (WeaponAttachSocketName, or the capsule at WeaponAttachOffset). A weapon held on it is the shown weapon: its Muzzle is the shot
	 * origin of every weapon of the miner (GetShotPlacement, task-412).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attach")
	FName HandSocketName;

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
