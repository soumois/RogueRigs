// Rogue Rigs - pickup world subsystem: moves every live pickup in one loop per frame (no actor tick per pickup, no physics).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRPickupSubsystem.generated.h"

class ARRPickup;
class ARRMiner;

/**
 * Drives ARRPickup motion (sys-pickups). Pickups register at BeginPlay and unregister at EndPlay; each frame the subsystem
 * reads the player miner (player 0 pawn) and its PickupRadius once, then calls ARRPickup::UpdateMotion on every pickup.
 * Hordes split their XP over many cubes (task-027-01), so a per-actor tick would cost far more than this single loop.
 * Pickups collected (destroyed) during the loop are removed after it. Game and PIE worlds only; pauses with the world.
 */
UCLASS()
class ROGUERIGS_API URRPickupSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Adds Pickup to the update loop (called once, from its BeginPlay). */
	void RegisterPickup(ARRPickup* Pickup);

	/** Removes Pickup from the update loop (safe during the loop). */
	void UnregisterPickup(ARRPickup* Pickup);

	/** Number of registered pickups (tests, stress). */
	int32 GetPickupCount() const;

	/** Number of pickups currently magnetized toward the miner (tests, stress). */
	int32 GetMagnetizedCount() const;

	/** Moves every pickup. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

protected:
	/** Game worlds only (game, PIE). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Drops the list. */
	virtual void Deinitialize() override;

	/** Registered pickups; entries nulled during the loop are compacted after it. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<ARRPickup>> Pickups;

	/** True while Tick iterates Pickups (unregister then nulls instead of removing). */
	bool bUpdating = false;

	/** Index of the pickup being updated by the loop (INDEX_NONE outside it). */
	int32 UpdatingIndex = INDEX_NONE;

	/** True when an entry was nulled during the loop. */
	bool bNeedsCompaction = false;
};
