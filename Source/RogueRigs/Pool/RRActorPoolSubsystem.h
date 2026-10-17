// Rogue Rigs - generic actor pool world subsystem (sys-actor-pool): reuses released actors and spreads the creation of missing ones over a capped number of frames.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/ObjectKey.h"
#include "RRActorPoolSubsystem.generated.h"

class AActor;
class APawn;

/** Per-request options of URRActorPoolSubsystem::Acquire (plain C++, game thread). */
struct FRRPoolAcquireParams
{
	/** Owner given to the actor (SpawnActorDeferred on creation, SetOwner on reuse); may be null. */
	TWeakObjectPtr<AActor> Owner;

	/** Instigator given to the actor (creation and reuse); may be null. */
	TWeakObjectPtr<APawn> Instigator;

	/**
	 * Collision handling of a creation. On reuse, AdjustIfPossible* moves the actor with UWorld::FindTeleportSpot (placed at
	 * the transform when no spot is found); the DontSpawnIfColliding variants are not honoured on reuse.
	 */
	ESpawnActorCollisionHandlingMethod CollisionHandling = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	/**
	 * Frames (subsystem ticks after the request) within which a missing actor is created; at least 1. Creations per tick =
	 * the smallest rate that meets every pending deadline (ceil(pending / MaxFrames) for one batch). Ignored on reuse.
	 */
	int32 MaxFrames = 1;

	/**
	 * Optional: when set and the object dies (destroyed, garbage) before delivery, the request is dropped silently. Use the
	 * caller itself (spawner, dying robot's owner, miner) so a request never outlives it.
	 */
	TWeakObjectPtr<UObject> Requester;

	/**
	 * Per-call setup (XP amount, ore value, pop yaw...). Runs before BeginPlay on a creation (deferred spawn) and before
	 * IRRPoolable::OnPoolAcquired on a reuse. Optional.
	 */
	TFunction<void(AActor*)> OnInit;

	/** Called once the actor is active and owned by the caller (after OnPoolAcquired). Optional. */
	TFunction<void(AActor*)> OnReady;
};

/** Identifies a pending Acquire request; valid (non-zero) only while the request waits for a creation. */
struct FRRPoolRequestHandle
{
	/** Request id; 0 = no pending request (served at once, cancelled, delivered or failed). */
	uint32 Id = 0;

	/** True when the handle names a request (it may have been served since: see URRActorPoolSubsystem::IsRequestPending). */
	bool IsValid() const { return Id != 0; }

	/** Forgets the request id without cancelling it. */
	void Reset() { Id = 0; }
};

/** Counters of the pool, for tests and stress logs (all classes, or one class with GetStats(Class)). */
struct FRRActorPoolStats
{
	/** Actors created by the pool and still alive (idle + acquired). */
	int32 PoolSize = 0;

	/** Idle actors ready for reuse. */
	int32 Idle = 0;

	/** Actors handed to callers and not released. */
	int32 Acquired = 0;

	/** Requests waiting for a creation (prewarm included). */
	int32 Pending = 0;

	/** Actors created during the current engine frame (GFrameCounter). */
	int32 CreatedThisFrame = 0;

	/** Longest wait, in subsystem ticks, of a delivered request since the last ResetStats (0 = served at once). */
	int32 LongestWaitFrames = 0;

	/** Actors created since the last ResetStats. */
	int32 TotalCreated = 0;

	/** Deliveries of a reused idle actor since the last ResetStats. */
	int32 TotalReused = 0;
};

/**
 * Generic actor pool of a game world (sys-actor-pool, task-090-01). Acquire hands out an idle actor of the class at once
 * (reset, moved, OnInit, IRRPoolable::OnPoolAcquired, OnReady) or queues a creation; Tick serves the queue: reuses actors
 * released meanwhile, then creates (deferred spawn, OnInit before BeginPlay) the smallest number per tick that still meets
 * every request's MaxFrames deadline, earliest deadline first. Release parks an actor: OnPoolReleased, hidden, collision,
 * actor and component ticks off, timers cleared, owner cleared. An actor is never handed out twice: only idle actors are
 * reused and a double Release is refused. Idle actors stay in the world, so systems that enumerate actors or count live
 * ones skip them with IsPooledIdle. Ticks after the actor tick groups (UWorld::Tick), so a request made from an actor tick
 * with MaxFrames 1 is served in the same frame. Pauses with the world. Game thread only. Game and PIE worlds only.
 */
UCLASS()
class ROGUERIGS_API URRActorPoolSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Requests an actor of Class at Transform. An idle pooled actor is delivered before returning (OnReady already called,
	 * the returned handle is invalid); otherwise the creation is queued and the returned handle names it until delivery.
	 * Invalid handle and nothing queued when Class is null or abstract.
	 */
	FRRPoolRequestHandle Acquire(TSubclassOf<AActor> Class, const FTransform& Transform, FRRPoolAcquireParams Params);

	/**
	 * Returns an actor now: an idle one, else a creation outside the frame budget (counted in the stats). For callers that
	 * need the actor synchronously. Params.MaxFrames, Requester and OnReady are ignored. Null on failure.
	 */
	AActor* AcquireImmediate(TSubclassOf<AActor> Class, const FTransform& Transform, const FRRPoolAcquireParams& Params);

	/** Typed AcquireImmediate. */
	template <typename T>
	T* AcquireImmediate(TSubclassOf<T> Class, const FTransform& Transform, const FRRPoolAcquireParams& Params)
	{
		return Cast<T>(AcquireImmediate(TSubclassOf<AActor>(Class.Get()), Transform, Params));
	}

	/**
	 * Queues Count creations of idle actors of Class within MaxFrames ticks (parked at once, OnPoolReleased called; no
	 * callbacks). Deadlines are staggered (creation i due on tick ceil((i + 1) * MaxFrames / Count)) and each prewarm creation
	 * runs on its deadline tick, not earlier: the batch spans MaxFrames ticks at most ceil(Count / MaxFrames) per tick (5 over
	 * 60 = one every 12 ticks), also when several batches run together (task-111-01). Default pools are empty:
	 * prewarm only where a caller asks for it (ARREnemySpawner at level start).
	 */
	void Prewarm(TSubclassOf<AActor> Class, int32 Count, int32 MaxFrames);

	/** Prewarm creations still queued (all classes); 0 once every Prewarm batch is served. */
	int32 GetPendingPrewarmCount() const;

	/** Drops a pending request (its callbacks never run) and resets Handle. False when it was not pending. */
	bool CancelRequest(FRRPoolRequestHandle& Handle);

	/** True while the request named by Handle waits for a creation. */
	bool IsRequestPending(const FRRPoolRequestHandle& Handle) const;

	/**
	 * Returns an acquired pool actor to the pool (see the class comment). False, with a warning, when Actor is null, not
	 * created by this pool, already idle or being destroyed; destroy such actors instead.
	 */
	bool Release(AActor* Actor);

	/** True when Actor was created by this pool and is idle. */
	bool IsIdle(const AActor* Actor) const;

	/** True when Actor was created by this pool (idle or acquired). */
	bool IsPoolMember(const AActor* Actor) const;

	/**
	 * True when Actor is parked idle in the pool of its world. Systems that enumerate or count live actors (spawner alive
	 * counts, pickup lists, actor iterators) skip actors for which it returns true. Null-safe; false outside a pool.
	 */
	static bool IsPooledIdle(const AActor* Actor);

	/** Counters over every class (Class null) or one class. */
	FRRActorPoolStats GetStats(TSubclassOf<AActor> Class = nullptr) const;

	/** Clears LongestWaitFrames, TotalCreated and TotalReused (all classes). */
	void ResetStats();

	/**
	 * Empties the pool: pending requests dropped (no callback), idle actors destroyed when bDestroyIdle, acquired actors
	 * forgotten (they stay with their callers; Release then refuses them). Called with bDestroyIdle false on Deinitialize
	 * (world end: the world destroys its actors).
	 */
	void ClearPool(bool bDestroyIdle);

	/** Serves pending requests. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

protected:
	/** One request waiting for a creation. */
	struct FPendingRequest
	{
		/** Request id (handle value). */
		uint32 Id = 0;
		/** Class to create. */
		TWeakObjectPtr<UClass> Class;
		/** Spawn transform. */
		FTransform Transform;
		/** Caller options and callbacks. */
		FRRPoolAcquireParams Params;
		/** True when the requester was set (a dead requester drops the request). */
		bool bHasRequester = false;
		/** True for a prewarm creation (parked idle, no callbacks). */
		bool bPrewarm = false;
		/** Ticks left before the deadline (1 = must be served in the next tick). */
		int32 FramesLeft = 1;
		/**
		 * Exact deadline minus FramesLeft, in (-1, 0]: 0 for Acquire, the fraction of a staggered prewarm deadline otherwise
		 * (tie-break between requests due the same tick, task-111-01).
		 */
		float DeadlineOffset = 0.f;
		/** Ticks this request has been through. */
		int32 FramesWaited = 0;
		/** TickCount when queued (requests queued during a tick are not aged by it). */
		uint32 QueuedTick = 0;
	};

	/** Pool of one class. */
	struct FClassPool
	{
		/** Idle actors, last released on top (destroyed ones removed by HandleMemberDestroyed). */
		TArray<TWeakObjectPtr<AActor>> Idle;
		/** Actors created and alive (idle or acquired). */
		int32 MemberCount = 0;
		/** Creations during CreatedFrame. */
		int32 CreatedThisFrame = 0;
		/** GFrameCounter of CreatedThisFrame. */
		uint64 CreatedFrame = 0;
		/** See FRRActorPoolStats. */
		int32 LongestWaitFrames = 0;
		/** See FRRActorPoolStats. */
		int32 TotalCreated = 0;
		/** See FRRActorPoolStats. */
		int32 TotalReused = 0;
	};

	/** Game worlds only (game, PIE). */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Forgets everything (ClearPool(false)). */
	virtual void Deinitialize() override;

	/** Pool of Class, created on demand. */
	FClassPool& GetClassPool(UClass* Class);

	/** Forgets a member destroyed by anyone (bound to its OnDestroyed). */
	UFUNCTION()
	void HandleMemberDestroyed(AActor* DestroyedActor);

	/** Pops a live idle actor of Class (destroyed entries skipped), null when none. */
	AActor* PopIdle(UClass* Class);

	/** Creates an actor of Class (deferred spawn, OnInit before BeginPlay), records it as a member. Null on failure. */
	AActor* CreateActor(UClass* Class, const FTransform& Transform, const FRRPoolAcquireParams& Params);

	/** Turns an idle actor back on at Transform (owner, instigator, visibility, collision, default ticks) and runs OnInit. */
	void ReactivateActor(AActor* Actor, const FTransform& Transform, const FRRPoolAcquireParams& Params);

	/** Marks Actor acquired and runs OnPoolAcquired then OnReady; records the wait. */
	void DeliverActor(AActor* Actor, const FRRPoolAcquireParams& Params, int32 FramesWaited);

	/** Runs OnPoolReleased and parks Actor idle (hidden, collision and ticks off, timers and owner cleared). */
	void ParkActor(AActor* Actor);

	/** Serves one pending request (reuse when an idle actor exists, else creation); prewarm requests park their actor. */
	void ServeRequest(FPendingRequest& Request, bool bCreate);

	/** Index of the pending request Id, INDEX_NONE when absent. */
	int32 FindPending(uint32 Id) const;

	/** True when the request lost its class or its requester. */
	static bool IsRequestDead(const FPendingRequest& Request);

	/** Pools by class. */
	TMap<TObjectKey<UClass>, FClassPool> ClassPools;

	/** Idle actors of every class (IsIdle). */
	TSet<TObjectKey<AActor>> IdleActors;

	/** Class of every member actor (Release, IsPoolMember). */
	TMap<TObjectKey<AActor>, TObjectKey<UClass>> MemberClasses;

	/**
	 * Template root scale of every member (task-095-01): root relative scale at creation divided by the spawn scale. A spawn
	 * multiplies the requested scale with the root template scale (ESpawnActorScaleMethod::MultiplyWithRoot, e.g. the 0.3
	 * PickupMesh of BP_PickupXP); a reuse must do the same, else the root takes the requested scale alone (3.3x cubes).
	 */
	TMap<TObjectKey<AActor>, FVector> MemberRootScales;

	/** Requests waiting for a creation, earliest deadline first after each sort. */
	TArray<FPendingRequest> Pending;

	/** Last request id given (0 never used). */
	uint32 LastRequestId = 0;

	/** Ticks served so far (ages the requests). */
	uint32 TickCount = 0;
};
