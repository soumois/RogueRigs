// Rogue Rigs - Niagara component pool world subsystem (sys-actor-pool): capped per-system pools of reusable one-shot Niagara components (robot death puff, debris).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRNiagaraPoolSubsystem.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UFXSystemComponent;

/** Components of one Niagara system held by URRNiagaraPoolSubsystem: free ones wait inactive, in-use ones are playing. */
USTRUCT()
struct FRRNiagaraSystemPool
{
	GENERATED_BODY()

	/** Inactive components ready to play again (last in, first out). */
	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> Free;

	/** Components handed out and not finished yet. */
	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> InUse;

	/** Most components this pool may own (free + in use); the last cap passed to Spawn or Prewarm. */
	int32 Cap = 0;

	/** Components created for this pool since the world began (tests, debug). */
	int32 CreatedCount = 0;

	/** Spawn calls refused because the pool was full (tests, debug). */
	int32 SkippedCount = 0;

	/** Spawn calls with this system, played or not (tests, debug). */
	int32 RequestedCount = 0;

	/** Spawn calls whose component is playing or waiting to start after Activate (effects played; one that completed inside
	 *  Activate is not counted) since the world began (stress logs, task-181-01, task-194-01). */
	int32 PlayedCount = 0;
};

/**
 * Pools of one-shot Niagara components, one pool per UNiagaraSystem asset, each capped by its caller (enemy-death: puff and
 * debris, 40 each at most). Spawn hands out a free component (or creates one while the pool is below its cap), places it at
 * the given world transform, lets the caller set user parameters, then activates it with a reset; when the pool is full and
 * nothing is free it returns null and the effect is skipped. A component goes back to the free list when its system
 * finishes (OnSystemFinished); components are never destroyed per effect. Components that never started or got stuck
 * (headless runs without rendering, scalability) are reclaimed when the pool is full: an in-use component that is not
 * active and not waiting to start (IsWaitingToStart: system still compiling or precaching PSOs) is free again.
 * Prewarm also starts the deferred editor compile of the system so the first effect does not wait. No tick: all work
 * happens in Spawn, Prewarm and the finish callback.
 * Components are owned by the world settings, absolute, auto-destroy off, not attached (same set-up as
 * URRDamageNumberSubsystem). The Niagara systems used must be one-shot (non-looping), otherwise they never return.
 * Game thread only; game and PIE worlds.
 */
UCLASS()
class ROGUERIGS_API URRNiagaraPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Default cap of a pool when the caller has none of its own (enemy-death: 40 per system). */
	static constexpr int32 DefaultCap = 40;

	// UWorldSubsystem
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	virtual void Deinitialize() override;

	/**
	 * Plays System once at Location / Rotation through its pool, capped at Cap components (Cap <= 0 = no effect).
	 * BeforeActivate, when set, runs on the placed, inactive component just before activation (set user parameters there).
	 * Returns the playing component, or null when System is null or the pool is full (effect skipped). Do not keep the
	 * returned pointer past the end of the effect: the component is reused.
	 */
	UNiagaraComponent* Spawn(UNiagaraSystem* System, const FVector& Location, const FRotator& Rotation = FRotator::ZeroRotator,
		int32 Cap = DefaultCap, TFunctionRef<void(UNiagaraComponent&)> BeforeActivate = [](UNiagaraComponent&) {});

	/**
	 * Creates free components of System until its pool owns Count of them (at most Cap); call at load so deaths create none.
	 * Editor builds: also starts the deferred compile of System in the background (non-blocking), so the first Spawn finds
	 * it ready instead of waiting.
	 */
	void Prewarm(UNiagaraSystem* System, int32 Count, int32 Cap = DefaultCap);

	/** Components of System currently playing (handed out, not finished). */
	int32 GetInUseCount(const UNiagaraSystem* System) const;

	/** Components owned by the pool of System (free + in use). */
	int32 GetComponentCount(const UNiagaraSystem* System) const;

	/** Components created for System since the world began. */
	int32 GetCreatedCount(const UNiagaraSystem* System) const;

	/** Spawn calls for System refused because its pool was full. */
	int32 GetSkippedCount(const UNiagaraSystem* System) const;

	/** Spawn calls for System since the world began, played or not. */
	int32 GetRequestedCount(const UNiagaraSystem* System) const;

	/** Spawn calls for System that activated a component (effects played) since the world began. */
	int32 GetPlayedCount(const UNiagaraSystem* System) const;

	/** Appends the Niagara systems that have a pool (stress logs iterate them with the counters above). */
	void GetPooledSystems(TArray<UNiagaraSystem*>& OutSystems) const;

	/**
	 * Next level in the same world (task-389-01, ClearingLevel step): stops at once every pooled effect still playing or waiting
	 * to start (robot death puffs and debris: the world is paused behind the intermission screens, so they would resume in the
	 * next level) and puts it back in its free list, then stops every tracked world effect (TrackWorldEffect: shield debris).
	 * Logs the count per system. Returns how many effects were still alive.
	 */
	int32 ClearForNextLevel();

	/** Pooled effects playing or waiting to start plus tracked world effects still alive (level start check). */
	int32 CountLiveEffects() const;

	/**
	 * Registers an effect spawned outside the pools by a robot (Blueprint SpawnSystemAtLocation: world-owned, so not removed
	 * with the robot) so ClearForNextLevel stops it. Weak reference; finished or destroyed entries are dropped on the next call.
	 */
	void TrackWorldEffect(UFXSystemComponent* Effect);

	/**
	 * Fills Out with the effect components outered to World or its world settings (SpawnSystemAtLocation, engine FX pool) that are
	 * alive: active, or waiting for their system to compile (editor builds, first use) before they start.
	 */
	static void CollectWorldOwnedEffects(const UWorld& World, TSet<const UFXSystemComponent*>& Out);

	/** Tracks (TrackWorldEffect) every active world-owned effect of this world not in Before; returns how many. */
	int32 TrackWorldEffectsSpawnedSince(const TSet<const UFXSystemComponent*>& Before);

	/** World effects tracked by TrackWorldEffect (alive or not, until the next prune). */
	int32 GetTrackedWorldEffectCount() const { return TrackedWorldEffects.Num(); }

protected:
	/** Creates one registered, inactive component of System owned by the world settings; null on failure. */
	UNiagaraComponent* CreateComponent(UNiagaraSystem* System);

	/**
	 * Moves every in-use component of Pool that is not active and not waiting to start (finished without callback, never
	 * started) back to Free. A component waiting to start keeps its slot: it plays as soon as its system is ready.
	 */
	void ReclaimInactive(FRRNiagaraSystemPool& Pool);

	/**
	 * True when Component was activated but waits for its system to be ready (compile, PSO precache): inactive with its tick
	 * enabled, the engine tick activates it later. Such a component is playing as far as the pool is concerned.
	 */
	static bool IsWaitingToStart(const UNiagaraComponent& Component);

	/** True when Effect is playing, or is a Niagara component waiting to start (IsWaitingToStart): both would show later. */
	static bool IsEffectAlive(const UFXSystemComponent& Effect);

	/** OnSystemFinished of a pooled component: back to the free list of its system. */
	UFUNCTION()
	void HandleSystemFinished(UNiagaraComponent* Component);

	/** One pool per Niagara system asset. */
	UPROPERTY()
	TMap<TObjectPtr<UNiagaraSystem>, FRRNiagaraSystemPool> Pools;

	/** Robot effects spawned outside the pools (TrackWorldEffect), stopped by ClearForNextLevel. Weak: auto-destroyed one-shots go on their own. */
	TArray<TWeakObjectPtr<UFXSystemComponent>> TrackedWorldEffects;
};
