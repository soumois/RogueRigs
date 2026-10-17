// Rogue Rigs - pickup world subsystem: moves every live pickup in one loop per frame (no actor tick per pickup; landed pickups simulate Chaos physics, task-229-02) and acquires grid and robot drops from the actor pool.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "RRPickupSubsystem.generated.h"

class AActor;
class ARRPickup;
class ARRMiner;
class UPhysicalMaterial;

/** One collected XP cube request (task-110-01), resolved by URRPickupSubsystem::FlushXPCubes. */
struct FRRPendingXPCube
{
	/** Small cube class requested. */
	TSubclassOf<ARRPickup> PickupClass;
	/** Requested drop point. */
	FVector Location = FVector::ZeroVector;
	/** Raw XP of the cube. */
	float XPAmount = 0.f;
	/** Ejection yaw in degrees. */
	float PopYawDegrees = 0.f;
};

/**
 * Drives ARRPickup motion (sys-pickups). Pickups register at BeginPlay and unregister at EndPlay; each frame the subsystem
 * reads the player miner (player 0 pawn) and its PickupRadius once, then calls ARRPickup::UpdateMotion on every pickup.
 * Hordes split their XP over many cubes (task-027-01), so a per-actor tick would cost far more than this single loop.
 * Pickups collected (destroyed) during the loop are removed after it. Game and PIE worlds only; pauses with the world.
 * Drops (task-092-01, replaces the 4-per-frame spawn queue of task-070-01): RequestSpawn (grid ore and heal) and
 * RequestXPCube (robot XP) acquire the pickup from URRActorPoolSubsystem with MaxFrames = SpawnMaxFrames (3): an idle pickup
 * is reused at once, missing ones are created within 3 frames, so a mass kill or a big explosion never creates all its
 * pickups in one frame. The requester is this subsystem (a drop outlives the robot or grid that asked for it). SpawnMaxFrames
 * 0 = AcquireImmediate (tests that read drops in the same frame). Without a pool the pickup is spawned at once. Pooled idle
 * pickups are unregistered, so GetPickupCount never counts them; actor iterators skip them with IsPooledIdle.
 * Physics (task-229-02): FindOrAddPhysicalMaterial shares one physical material per pickup class among its pickups.
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
	 * Acquires a pickup of PickupClass at Location from the pool (owner Owner, ore value OreValue, heal amount HealAmount when
	 * above 0, else the class default). Returns the pickup when delivered now (reused idle pickup, SpawnMaxFrames 0 or no
	 * pool), nullptr when its creation is queued (within SpawnMaxFrames frames) or on failure.
	 */
	ARRPickup* RequestSpawn(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, AActor* Owner, int32 OreValue, float HealAmount = 0.f);

	/**
	 * Acquires an XP cube of PickupClass at Location from the pool, giving XPAmount raw XP, ejected along PopYawDegrees (world
	 * yaw). Same delivery rules and return value as RequestSpawn. Used by ARREnemy::DropXP.
	 * Big XP block (task-110-01): when the PickupClass default has an XPBigPickupClass, the request is collected instead (returns
	 * nullptr) and resolved by FlushXPCubes at the end of the frame (this subsystem's Tick).
	 */
	ARRPickup* RequestXPCube(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, float XPAmount, float PopYawDegrees);

	/**
	 * Resolves the XP cube requests collected since the last flush (task-110-01): grouped per small class and per XPMergeBucketSize
	 * square cell of the request location; a bucket of more than XPMergeThreshold cubes turns each full group of XPMergeGroupSize
	 * cubes (in request order) into one XPBigPickupClass block worth the group's XP sum, at the group's mean location, ejected
	 * along the first cube's yaw; the remainder stays small cubes. Total XP is preserved. Called by Tick; tests call it directly.
	 */
	void FlushXPCubes();

	/** XP cube requests collected and not yet flushed (tests). */
	int32 GetPendingXPCubeCount() const { return PendingXPCubes.Num(); }

	/** Big XP blocks requested since the world started (tests, stress). */
	int32 GetTotalXPBigBlocksRequested() const { return TotalXPBigBlocksRequested; }

	/** Pickup creations still waiting in the pool (tests, stress). */
	int32 GetQueuedSpawnCount() const;

	/** Pool counters summed over every pickup class requested through this subsystem (tests, stress). */
	FRRActorPoolStats GetPoolStats() const;

	/** XP cubes requested since the world started (stress spike tag). */
	int32 GetTotalXPCubesRequested() const { return TotalXPCubesRequested; }

	/** Frames within which the pool creates a missing pickup; 0 = synchronous creation. */
	int32 GetSpawnMaxFrames() const { return SpawnMaxFrames; }

	/** Overrides SpawnMaxFrames for this world (tests that read drops in the same frame use 0). */
	void SetSpawnMaxFrames(int32 InMaxFrames) { SpawnMaxFrames = FMath::Max(InMaxFrames, 0); }

	/**
	 * Physical material of PickupClass (task-229-02), built once from its class default (ARRPickup::MakePhysicalMaterial) and
	 * shared by every pickup of that class. Null for a null class.
	 */
	UPhysicalMaterial* FindOrAddPhysicalMaterial(TSubclassOf<ARRPickup> PickupClass);

	/** Moves every pickup. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

protected:
	/** Game worlds only (game, PIE). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Drops the list. */
	virtual void Deinitialize() override;

	/**
	 * Shared path of RequestSpawn and RequestXPCube: Init sets the per-drop values (before BeginPlay on a creation, before
	 * OnPoolAcquired on a reuse).
	 */
	ARRPickup* AcquirePickup(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, AActor* Owner, TFunction<void(ARRPickup*)> Init);

	/** Frames within which the pool creates a missing pickup (task-092-01: 3). Config (Game), default 3; 0 = synchronous. */
	UPROPERTY(Config, EditAnywhere, Category = "Pickups", meta = (ClampMin = "0"))
	int32 SpawnMaxFrames = 3;

	/** Size in uu of the square cells XP cube requests are grouped by (task-110-01, provisional 200). Config (Game). */
	UPROPERTY(Config, EditAnywhere, Category = "Pickups|XP Big Block", meta = (ClampMin = "1", Units = "cm"))
	float XPMergeBucketSize = 200.f;

	/** A bucket merges only when it holds more than this many cubes in a frame (task-110-01, provisional 8). Config (Game). */
	UPROPERTY(Config, EditAnywhere, Category = "Pickups|XP Big Block", meta = (ClampMin = "0"))
	int32 XPMergeThreshold = 8;

	/** Cubes per big block (task-110-01, provisional 8); below 2 nothing merges. Config (Game). */
	UPROPERTY(Config, EditAnywhere, Category = "Pickups|XP Big Block", meta = (ClampMin = "2"))
	int32 XPMergeGroupSize = 8;

	/** Physical material per pickup class (FindOrAddPhysicalMaterial); held as UObject because the module does not link PhysicsCore. */
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UObject>> PhysicalMaterials;

	/** XP cube requests collected since the last FlushXPCubes. */
	TArray<FRRPendingXPCube> PendingXPCubes;

	/** See GetTotalXPBigBlocksRequested. */
	int32 TotalXPBigBlocksRequested = 0;

	/** Pickup classes requested through the pool (GetPoolStats, GetQueuedSpawnCount). */
	TArray<TWeakObjectPtr<UClass>> RequestedClasses;

	/** See GetTotalXPCubesRequested. */
	int32 TotalXPCubesRequested = 0;

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
