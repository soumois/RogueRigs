// Rogue Rigs - pickup world subsystem: moves every live pickup in one loop per frame (no actor tick per pickup, no physics) and spawns queued grid drops under a per-frame cap.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRPickupSubsystem.generated.h"

class AActor;
class ARRPickup;
class ARRMiner;

/** One pickup waiting to be spawned by URRPickupSubsystem (grid drops over the per-frame cap). */
USTRUCT()
struct FRRPickupSpawnRequest
{
	GENERATED_BODY()

	/** Pickup class to spawn. */
	UPROPERTY()
	TSubclassOf<ARRPickup> PickupClass;

	/** World spawn location (the pop starts here). */
	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	/** Owner of the spawned pickup (the grid); may be null. */
	UPROPERTY()
	TWeakObjectPtr<AActor> Owner;

	/** Ore value given to an Ore pickup (ARRPickup::SetOreValue); ignored by other types. */
	UPROPERTY()
	int32 OreValue = 1;

	/** Health given by a Heal pickup (ARRPickup::SetHealAmount) when above 0; 0 keeps the pickup default. */
	UPROPERTY()
	float HealAmount = 0.f;
};

/**
 * Drives ARRPickup motion (sys-pickups). Pickups register at BeginPlay and unregister at EndPlay; each frame the subsystem
 * reads the player miner (player 0 pawn) and its PickupRadius once, then calls ARRPickup::UpdateMotion on every pickup.
 * Hordes split their XP over many cubes (task-027-01), so a per-actor tick would cost far more than this single loop.
 * Pickups collected (destroyed) during the loop are removed after it. Game and PIE worlds only; pauses with the world.
 * Spawn queue (ore-in-rock-plan, performance budget): RequestSpawn spawns at once while fewer than MaxSpawnsPerFrame pickups
 * were spawned this frame (a frame = one world time value), else queues; Tick spawns the queue first, under the same cap. An
 * explosion that breaks many ore blocks therefore spreads its drops over several frames.
 */
UCLASS(Config = Game)
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

	/**
	 * Spawns a pickup of PickupClass at Location (owner Owner, ore value OreValue, heal amount HealAmount when above 0) now when the frame budget allows, else
	 * queues it for the next frames (FIFO). Returns the pickup when spawned now, nullptr when queued or on failure.
	 */
	ARRPickup* RequestSpawn(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, AActor* Owner, int32 OreValue, float HealAmount = 0.f);

	/** Number of spawn requests still waiting in the queue (tests, stress). */
	int32 GetQueuedSpawnCount() const { return SpawnQueue.Num(); }

	/** Pickups spawned at most per frame by RequestSpawn and the queue. */
	int32 GetMaxSpawnsPerFrame() const { return MaxSpawnsPerFrame; }

	/** Overrides MaxSpawnsPerFrame for this world (at least 1). */
	void SetMaxSpawnsPerFrame(int32 InMaxSpawnsPerFrame) { MaxSpawnsPerFrame = FMath::Max(InMaxSpawnsPerFrame, 1); }

	/** Moves every pickup. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

protected:
	/** Game worlds only (game, PIE). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Drops the list and the spawn queue. */
	virtual void Deinitialize() override;

	/** Starts a new frame budget when the world time moved since the last spawn. */
	void RefreshFrameBudget();

	/** Spawns one request (deferred spawn, ore value set before BeginPlay). Counts toward the frame budget. */
	ARRPickup* SpawnNow(const FRRPickupSpawnRequest& Request);

	/** Spawns queued requests, oldest first, until the frame budget is used. */
	void SpawnQueued();

	/** Pickups spawned at most per frame (ore-in-rock-plan: 4, about 0.4 ms of SpawnActor). Config (Game), default 4. */
	UPROPERTY(Config, EditAnywhere, Category = "Pickups", meta = (ClampMin = "1"))
	int32 MaxSpawnsPerFrame = 4;

	/** Spawn requests over the frame cap, oldest first. */
	UPROPERTY(Transient)
	TArray<FRRPickupSpawnRequest> SpawnQueue;

	/** Pickups spawned during the current frame budget. */
	int32 SpawnedThisFrame = 0;

	/** World time of the current frame budget (-1 before the first spawn). */
	double BudgetWorldTime = -1.0;

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
