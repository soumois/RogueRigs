// Rogue Rigs - turret launcher: a miner weapon whose magazine is turret charges; a button places a turret on the aimed floor cell, charges recharge one at a time.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/RRWeapon.h"
#include "RRTurretLauncher.generated.h"

class ARRArenaGrid;
class ARRTurret;

/** Fired when the charge count, the maximum or the recharge state changes: charges ready, maximum, duration of the running recharge (0 = none). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnTurretChargesChangedSignature, int32, Charges, int32, MaxCharges, float, RechargeDuration);

/** Fired after a turret was placed and initialized. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnTurretPlacedSignature, ARRTurret*, Turret);

/**
 * Turret launcher (turret gdd, sys-weapons). A weapon of the miner (in ARRMiner::GetWeapons, so all-weapon and targeted upgrades
 * reach it), CSV row TurretLauncher. It never auto-fires (bStartFiringOnAcquire off): the player places a turret with a button
 * (ARRMiner::PlaceTurret, input action PlaceTurret), never automatically. Charges = the Ammo attribute, maximum = MagazineSize.
 * TryPlaceTurret spends one charge (URRGE_ConsumeAmmo) and spawns TurretClass on the placement cell (FindPlacementLocation),
 * which copies the launcher's stats (ARRTurret::InitFromLauncher). Recharge (Tick, AdvanceRecharge): while Charges <
 * MaxCharges one charge recharges at a time over ReloadTime / ReloadSpeed seconds (ReloadSpeed <= 0 = never), then Ammo + 1.
 * No cap on active turrets. ProjectileClass (native default ARRTurretBullet) is the turrets' bullet. Subclass in Blueprint for
 * the mesh, the turret Blueprint and the bullet Blueprint.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRTurretLauncher : public ARRWeapon
{
	GENERATED_BODY()

public:
	/** StatsRowName TurretLauncher, TurretClass ARRTurret, ProjectileClass ARRTurretBullet; ticks (recharge); no auto-fire. */
	ARRTurretLauncher(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Places a turret if a charge is ready: finds the placement location, spawns TurretClass there (facing the aim; owner: this
	 * launcher, instigator: the miner), initializes it from this launcher, spends one charge, starts the recharge if none runs,
	 * broadcasts OnTurretChargesChanged and OnTurretPlaced. Returns the turret, or null (no charge, no TurretClass, no placement,
	 * spawn failure; nothing spent).
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Turret")
	ARRTurret* TryPlaceTurret();

	/**
	 * Placement of the next turret: the floor cell under the aimed point (the mouse cursor point with mouse aim, clamped to
	 * PlacementMaxDistance; else PlacementDistance ahead of the miner along the aim) when it is floor and in grid line of sight
	 * from the miner; otherwise the floor cell in line of sight sampled along the aim (every half cell from the miner up to
	 * PlacementMaxDistance) nearest to that point. Out: the cell center on the floor top. Without a grid: the aimed point at the
	 * miner's feet. False when no cell qualifies.
	 */
	bool FindPlacementLocation(FVector& OutLocation) const;

	/** Charges ready now (Ammo rounded down). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	int32 GetCharges() const;

	/** Maximum charges (MagazineSize rounded down). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	int32 GetMaxCharges() const;

	/** True while a charge is recharging. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	bool IsRecharging() const { return bRecharging; }

	/** Seconds left on the running recharge (0 when none). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	float GetRechargeRemaining() const { return bRecharging ? RechargeRemaining : 0.f; }

	/** Duration of one charge's recharge: ReloadTime / ReloadSpeed (0 when ReloadSpeed <= 0: never recharges). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	float GetRechargeDuration() const;

	/** Progress of the running recharge, 0..1 (0 when none). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	float GetRechargeProgress() const;

	/** Lifetime of the turrets placed now, in seconds (CSV constant TurretLifetime). */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	float GetTurretLifetime() const { return TurretLifetime; }

	/** Turrets placed since BeginPlay. */
	UFUNCTION(BlueprintPure, Category = "Weapon|Turret")
	int32 GetTurretsPlaced() const { return TurretsPlaced; }

	/** Advances the recharge by DeltaSeconds (one charge at a time; leftover time carries to the next charge). Called by Tick; tests may call it. */
	void AdvanceRecharge(float DeltaSeconds);

	/** Advances the recharge. */
	virtual void Tick(float DeltaSeconds) override;

	/** Broadcast when the charges, their maximum or the recharge state change (HUD). */
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Turret")
	FRROnTurretChargesChangedSignature OnTurretChargesChanged;

	/** Broadcast after each placement. */
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Turret")
	FRROnTurretPlacedSignature OnTurretPlaced;

protected:
	/** Copies the base columns, then TurretLifetime. */
	virtual void ApplyStatsRow(const FRRWeaponStatsRow& Row) override;

	/** Starts a recharge when charges are missing and none runs; stops it when full (no broadcast). */
	void RefreshRecharge();

	/** Broadcasts OnTurretChargesChanged when the charges, the maximum or the recharge state differ from the last broadcast (bForce: always). */
	void BroadcastChargesIfChanged(bool bForce = false);

	/** Arena grid of the miner, or the first of the world. */
	ARRArenaGrid* GetGrid() const;

	/** Turret spawned by TryPlaceTurret (a Blueprint of ARRTurret for the mesh). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Turret")
	TSubclassOf<ARRTurret> TurretClass;

	/** Distance of the aimed point ahead of the miner along the aim when aiming with a stick (no cursor point), in uu. Provisional. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Turret", meta = (ClampMin = 0, Units = "cm"))
	float PlacementDistance = 200.f;

	/** Farthest placement from the miner along the aim, in uu (task-024-03: 400). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Turret", meta = (ClampMin = 0, Units = "cm"))
	float PlacementMaxDistance = 400.f;

	/** Turret lifetime in seconds, copied from the CSV at acquisition. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon|Turret")
	float TurretLifetime = 0.f;

	/** True while a charge is recharging. */
	bool bRecharging = false;

	/** Seconds left on the running recharge. */
	float RechargeRemaining = 0.f;

	/** Charges sent by the last OnTurretChargesChanged (-1 before the first). */
	int32 LastBroadcastCharges = -1;

	/** Maximum sent by the last OnTurretChargesChanged (-1 before the first). */
	int32 LastBroadcastMaxCharges = -1;

	/** Recharge state sent by the last OnTurretChargesChanged. */
	bool bLastBroadcastRecharging = false;

	/** Turrets placed since BeginPlay. */
	int32 TurretsPlaced = 0;
};
