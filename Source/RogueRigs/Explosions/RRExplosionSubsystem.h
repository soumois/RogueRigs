// Rogue Rigs - explosion world subsystem: queued explosions resolved one link at a time, terrain and character damage, FX.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectKey.h"
#include "Explosions/RRExplosionStatsRow.h"
#include "RRExplosionSubsystem.generated.h"

class AActor;
class ARRArenaGrid;
class ARRExplosionFX;
class UAbilitySystemComponent;
class UCameraShakeBase;
class UNiagaraSystem;
struct FRRExplosionBrokenBlock;

/**
 * What one explosion hands the wall debris effect (task-439-01): one entry per particle, sampled over the blocks it broke.
 * Positions and Colors have the same length, bounded by WallDebrisMaxParticles.
 */
struct FRRWallDebrisRequest
{
	/** Explosion centre, world: the effect throws each particle straight away from it. */
	FVector Center = FVector::ZeroVector;
	/** Start point of each particle, world, inside the cell of a broken block. */
	TArray<FVector> Positions;
	/** Colour of each particle: the colour of its block type (RRBlockColors). */
	TArray<FLinearColor> Colors;
};

/** Broadcast by URRExplosionSubsystem after each explosion resolved (terrain, characters and FX done). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnExplosionSignature, FVector, Center, FName, RowName);

/**
 * The single path of every explosion (sys-explosions). QueueExplosion never explodes in the caller's call stack: it adds
 * the explosion to a FIFO queue resolved in Tick, one explosion (link) per tick at most, and at least ChainDelay seconds
 * (world time) after the previous one. Each source object explodes once: a source already queued or exploded is refused.
 * An explosion, in order: destroys every block whose cell center is within the 2D radius (grid QueueDestroyCellsInRadius,
 * source from the row: barrels at once, the other blocks nearest first under the grid's per-frame budget, task-121-01); applies URRGE_DamageExplosion once to every actor whose ASC owns a URRHealthAttributeSet and whose
 * location is within the 2D radius (the row's MinerDamage to an ARRMiner, its Damage to any other character, task-294-01);
 * spawns the row's FX class; plays ExplosionShakeClass at the centre (task-418-01: URRCameraShakeLibrary::PlayCameraShakeAtLocation,
 * scale ShakeScale, max distance Radius x ShakeMaxDistanceFactor, once per resolved explosion, never on a fuse); broadcasts OnExplosion.
 * No-one-shot rule: when the target is an ARRMiner at full health (Health >= MaxHealth), the damage is capped at
 * Health - MinerHealthLeftAtFullHealth, so an explosion never takes a full-health miner to 0. Below full health the damage
 * is not capped. I-frames are respected: the GE is refused while the miner has State.Invulnerable.
 * Visual (explosions gdd, owner 2026-10-10): the FX radius is the collision radius x VisualRadiusScale (1.05) and the FX holds its
 * final state VisualHoldSeconds (0.1 s) before fading; the collision radius and the damage are not changed.
 * Wall debris (task-439-01, explosions gdd): an explosion that breaks blocks plays WallDebrisSystem once through
 * URRNiagaraPoolSubsystem, at its centre, with at most WallDebrisMaxParticles particles sampled over those blocks; none when no block
 * broke, none when the pool (WallDebrisPoolSize) has no free slot, so a chain stays bounded. Never one effect per block.
 * Stats come from ExplosionStats.csv (FRRExplosionStatsRow). ChainDelay, MinerHealthLeftAtFullHealth, VisualRadiusScale,
 * VisualHoldSeconds and ExplosionShakeClass are config
 * properties (DefaultGame.ini, section [/Script/RogueRigs.RRExplosionSubsystem]) with runtime setters.
 */
UCLASS(Config = Game)
class ROGUERIGS_API URRExplosionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Sets ExplosionShakeClass to URRExplosionCameraShake before the config is read. */
	URRExplosionSubsystem();

	/** Path of the explosion stats CSV, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* ExplosionStatsPath;

	/** Resolves the next queued explosion when ChainDelay has elapsed since the previous one. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

	/**
	 * Rendering game worlds only: loads the FX class of every ExplosionStats.csv row (kept in PreloadedFXClasses) and warms its
	 * shockwave material up (ARRDecalWarmUp), so the first explosion does not draw the engine default decal (task-201-01).
	 */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/**
	 * Queues an explosion of row RowName (ExplosionStats.csv) at Center. Instigator gets the kill credit (may be null);
	 * SourceObject is what explodes (barrel cell owner, kamikaze; may be null = never deduplicated). Returns false, without
	 * queuing, when the row is missing or SourceObject has already been queued or exploded.
	 */
	UFUNCTION(BlueprintCallable, Category = "Explosions")
	bool QueueExplosion(FVector Center, FName RowName, AActor* Instigator, UObject* SourceObject);

	/** Same as QueueExplosion with explicit stats instead of a CSV row (RowName only labels it). */
	bool QueueExplosionWithStats(FVector Center, FName RowName, const FRRExplosionStatsRow& Stats, AActor* Instigator, UObject* SourceObject);

	/** Copies row RowName of ExplosionStats.csv into OutStats. Returns false (logged) when the file or the row is missing. */
	UFUNCTION(BlueprintCallable, Category = "Explosions")
	bool GetExplosionStats(FName RowName, FRRExplosionStatsRow& OutStats);

	/** True when SourceObject has already been queued or has exploded (it will never explode again). */
	UFUNCTION(BlueprintPure, Category = "Explosions")
	bool IsSourceSpent(const UObject* SourceObject) const;

	/** True while an explosion of SourceObject waits in the queue (a pooled kamikaze is released only after its own blast). */
	bool IsSourcePending(const UObject* SourceObject) const;

	/**
	 * Next run level prepared in the same world (task-318-01): drops the queued explosions and forgets the spent sources, so the
	 * rebuilt terrain's barrels can explode again. Returns the count of queued explosions dropped.
	 */
	int32 ClearForNextLevel();

	/**
	 * Lets SourceObject explode again: removes it from the spent sources unless one of its explosions is still queued (returns
	 * false then). For pooled sources reused in a new life (kamikaze, sys-actor-pool).
	 */
	bool ForgetSource(const UObject* SourceObject);

	/** Number of explosions waiting in the queue. */
	UFUNCTION(BlueprintPure, Category = "Explosions")
	int32 GetPendingCount() const { return Pending.Num(); }

	/** Minimum world time between two resolved explosions (chain link delay), in seconds. */
	UFUNCTION(BlueprintPure, Category = "Explosions")
	float GetChainDelay() const { return ChainDelay; }

	/** Overrides ChainDelay for this world (clamped at 0). */
	UFUNCTION(BlueprintCallable, Category = "Explosions")
	void SetChainDelay(float InChainDelay) { ChainDelay = FMath::Max(InChainDelay, 0.f); }

	/** Visible radius over collision radius of every explosion FX (explosions gdd: about 5 % larger, 1.05). */
	UFUNCTION(BlueprintPure, Category = "Explosions")
	float GetVisualRadiusScale() const { return VisualRadiusScale; }

	/** Radius the FX of an explosion of collision radius CollisionRadius grows to: CollisionRadius x VisualRadiusScale. */
	UFUNCTION(BlueprintPure, Category = "Explosions")
	float GetVisualRadius(float CollisionRadius) const { return FMath::Max(CollisionRadius, 0.f) * VisualRadiusScale; }

	/** Seconds every explosion FX holds its final state (full size, opaque) before fading (explosions gdd: 0.1 s). */
	UFUNCTION(BlueprintPure, Category = "Explosions")
	float GetVisualHoldSeconds() const { return VisualHoldSeconds; }

	/** Camera shake class played at each resolved explosion (default URRExplosionCameraShake). */
	UFUNCTION(BlueprintPure, Category = "Explosions")
	TSubclassOf<UCameraShakeBase> GetExplosionShakeClass() const { return ExplosionShakeClass; }

	/** Uses System as the wall debris effect of this world instead of WallDebrisSystem (null = back to it); for tests. */
	void SetWallDebrisSystem(UNiagaraSystem* System) { ResolvedWallDebrisSystem = System; }

	/** Most wall debris effects playing at once (a chain beyond it shows no more of them). */
	int32 GetWallDebrisPoolSize() const { return WallDebrisPoolSize; }

	/** Particles per broken block while the total stays under WallDebrisMaxParticles. */
	int32 GetWallDebrisParticlesPerBlock() const { return WallDebrisParticlesPerBlock; }

	/** The last wall debris request sent to the effect in this world (tests, debug). */
	const FRRWallDebrisRequest& GetLastWallDebrisRequest() const { return LastWallDebrisRequest; }

	/** User parameter names of the wall debris effect (bp-explosion-wall-debris). */
	static const FName WallDebrisCenterParameter;
	static const FName WallDebrisPositionsParameter;
	static const FName WallDebrisColorsParameter;
	static const FName WallDebrisCountParameter;
	/** Sound event of every resolved explosion, faded from its edge (task-516, sys-audio). */
	static const FName ExplosionSoundEvent;

	/** Uses Grid as the terrain (instead of the first ARRArenaGrid of the world). */
	UFUNCTION(BlueprintCallable, Category = "Explosions")
	void SetGrid(ARRArenaGrid* InGrid) { Grid = InGrid; }

	/** Broadcast after each explosion is resolved, with its center and row name. */
	UPROPERTY(BlueprintAssignable, Category = "Explosions")
	FRROnExplosionSignature OnExplosion;

protected:
	/** One queued explosion. */
	struct FPendingExplosion
	{
		/** World center. */
		FVector Center = FVector::ZeroVector;
		/** Row name (label for logs and OnExplosion). */
		FName RowName;
		/** Stats copied when queued. */
		FRRExplosionStatsRow Stats;
		/** Kill credit. */
		TWeakObjectPtr<AActor> Instigator;
		/** What exploded (effect causer when it is an actor). */
		TWeakObjectPtr<UObject> SourceObject;
	};

	/** Resolves one explosion: terrain, characters, FX, OnExplosion. */
	void Explode(const FPendingExplosion& Explosion);

	/** Applies URRGE_DamageExplosion once to every ASC with a health set whose actor is inside the 2D radius. */
	void DamageCharacters(const FPendingExplosion& Explosion);

	/** Damage dealt to Target: Stats.MinerDamage after the no-one-shot cap for an ARRMiner, Stats.Damage for any other character. */
	float ComputeCharacterDamage(const AActor* Target, const UAbilitySystemComponent& TargetASC, const FRRExplosionStatsRow& Stats) const;

	/** Plays ExplosionShakeClass at the center (scale Stats.ShakeScale, max distance Stats.Radius x Stats.ShakeMaxDistanceFactor); nothing without a local camera. */
	void PlayShake(const FPendingExplosion& Explosion);

	/** Spawns the row's FX class at the center, sized to GetVisualRadius(radius) with VisualHoldSeconds; nothing when the class is empty or missing. */
	void SpawnFX(const FPendingExplosion& Explosion);

	/** One wall debris effect for Blocks (nothing when empty or without a system). */
	void PlayWallDebris(const FPendingExplosion& Explosion, const ARRArenaGrid& CurrentGrid, const TArray<FRRExplosionBrokenBlock>& Blocks);

	/** The system set by SetWallDebrisSystem, else WallDebrisSystem loaded once; null when unset or missing (logged once). */
	UNiagaraSystem* ResolveWallDebrisSystem();

	/** Loads (once) and returns a concrete FX class; null when empty, missing or abstract (logged once per path). */
	UClass* ResolveFXClass(const TSoftClassPtr<ARRExplosionFX>& FXClass);

	/** The grid set by SetGrid, else the first ARRArenaGrid of the world; null without one. */
	ARRArenaGrid* ResolveGrid();

	/** Minimum world time between two resolved explosions, in seconds (explosions gdd: 0.08 s, provisional). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions", meta = (ClampMin = "0"))
	float ChainDelay = 0.08f;

	/** Health a full-health miner keeps at least after one explosion (no-one-shot rule). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions", meta = (ClampMin = "0"))
	float MinerHealthLeftAtFullHealth = 1.f;

	/** Visible radius over collision radius of every explosion FX (explosions gdd, owner 2026-10-10: about 5 % larger). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions", meta = (ClampMin = "0"))
	float VisualRadiusScale = 1.05f;

	/** Seconds every explosion FX holds its final state before fading (explosions gdd, owner 2026-10-10: 0.1 s). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions", meta = (ClampMin = "0"))
	float VisualHoldSeconds = 0.1f;

	/**
	 * Camera shake played at the centre of each resolved explosion (task-418, owner 2026-11-02): default URRExplosionCameraShake
	 * (10 uu, 0.35 s, 25 Hz); strength and max distance per source come from ExplosionStats.csv. None = no explosion shake.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions")
	TSubclassOf<UCameraShakeBase> ExplosionShakeClass;

	/** Effect of the blocks an explosion breaks (bp-explosion-wall-debris). Empty = no wall debris. */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions|Wall Debris")
	TSoftObjectPtr<UNiagaraSystem> WallDebrisSystem;

	/** Most particles of one explosion, however many blocks it breaks (explosions gdd: 60, provisional). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions|Wall Debris", meta = (ClampMin = "0"))
	int32 WallDebrisMaxParticles = 60;

	/** Particles per broken block, so that a few blocks still read; the blocks are thinned evenly once the maximum is reached (provisional). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions|Wall Debris", meta = (ClampMin = "1"))
	int32 WallDebrisParticlesPerBlock = 4;

	/** Most wall debris effects playing at once: at the maximum, 480 particles over a chain (provisional; a later link mostly finds its blocks already gone). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions|Wall Debris", meta = (ClampMin = "0"))
	int32 WallDebrisPoolSize = 8;

	/** Random offset of a particle start around its cell centre, per axis, as a fraction of the cell size; below 0.5 it stays in the cell (provisional). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions|Wall Debris", meta = (ClampMin = "0", ClampMax = "0.5"))
	float WallDebrisCellJitter = 0.4f;

	/** Lowest start height of a particle above the floor, in uu (provisional; blocks are 100 uu high). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions|Wall Debris", meta = (ClampMin = "0"))
	float WallDebrisMinHeight = 20.f;

	/** Highest start height of a particle above the floor, in uu (provisional). */
	UPROPERTY(Config, EditAnywhere, Category = "Explosions|Wall Debris", meta = (ClampMin = "0"))
	float WallDebrisMaxHeight = 90.f;

	/** WallDebrisSystem once loaded, or the system of SetWallDebrisSystem. */
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraSystem> ResolvedWallDebrisSystem;

	/** True once WallDebrisSystem was found missing (logged once). */
	bool bWallDebrisSystemMissing = false;

	/** See GetLastWallDebrisRequest; its arrays are reused from one explosion to the next. */
	FRRWallDebrisRequest LastWallDebrisRequest;

	/** Terrain of the explosions. */
	TWeakObjectPtr<ARRArenaGrid> Grid;

	/** Explosions waiting, in queue order. */
	TArray<FPendingExplosion> Pending;

	/** Every source object ever queued in this world. */
	TSet<FObjectKey> SpentSources;

	/** FX classes loaded at OnWorldBeginPlay, referenced for the world's life (their shockwave materials stay loaded). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UClass>> PreloadedFXClasses;

	/** FX soft paths already reported as unusable (logged once). */
	TSet<FSoftObjectPath> UnusableFXPaths;

	/** World time before which the next explosion may not resolve. */
	double NextExplosionTime = -1.0;
};
