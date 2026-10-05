// Rogue Rigs - stress test tooling: cheat commands that spawn robots in bulk around the miner and a timed frame-time sequencer.

#pragma once

#include "CoreMinimal.h"
#include "Grid/RRArenaGrid.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRStressSubsystem.generated.h"

class ARRArenaGrid;
class ARREnemy;

/** What the miner and the robots do during a stress run (stress-test-m1 scenarios). */
UENUM()
enum class ERRStressMode : uint8
{
	/** S0: miner invulnerable, weapons stopped; robots crowd the miner. */
	Peace,
	/** S2: miner invulnerable, every weapon firing; robots get rr.Stress.FireHealth so the count holds under fire. */
	Fire,
	/** S7: each step clears, spawns the robots, and at the end of the settle time kills them all and destroys every barrel at once. */
	Burst,
	/** Grid stress (task-023-01): peace, tough robots; at the end of the settle time queues rr.Stress.ChainCount explosions on block points around the miner. */
	Chain,
	/** Grid stress (task-023-01): peace, tough robots; during the measure window destroys the block cells nearest the miner at rr.Stress.DigRate. */
	Dig,
	/** Flow-field stress (task-029-02): peace, tough robots; during the measure window the miner is teleported along a circle (rr.Stress.WalkRadius, rr.Stress.WalkSpeed), only onto walkable cells, so the goal keeps changing cell. */
	Walk
};

/** Frame-time averages of one stress step, in milliseconds (same sources as stat unit). */
USTRUCT()
struct FRRStressStepResult
{
	GENERATED_BODY()

	/** Robots alive (spawned by the stress tool) during the step. */
	UPROPERTY()
	int32 EnemyCount = 0;

	/** Frames sampled during the measure window. */
	UPROPERTY()
	int32 FrameCount = 0;

	/** Average and 99th percentile frame time (wall clock delta). */
	UPROPERTY()
	float FrameAvg = 0.f;
	UPROPERTY()
	float FrameP99 = 0.f;

	/** Average and 99th percentile game thread time (idle excluded). */
	UPROPERTY()
	float GameAvg = 0.f;
	UPROPERTY()
	float GameP99 = 0.f;

	/** Average and 99th percentile render thread time (stat unit "Draw"). */
	UPROPERTY()
	float DrawAvg = 0.f;
	UPROPERTY()
	float DrawP99 = 0.f;

	/** Average and 99th percentile GPU frame time (GPU 0). */
	UPROPERTY()
	float GpuAvg = 0.f;
	UPROPERTY()
	float GpuP99 = 0.f;

	/** Worst frame and game thread time of the measure window. */
	UPROPERTY()
	float FrameMax = 0.f;
	UPROPERTY()
	float GameMax = 0.f;

	/** Burst mode: robots killed and barrels destroyed by the burst. */
	UPROPERTY()
	int32 BurstKills = 0;
	UPROPERTY()
	int32 BurstBarrels = 0;

	/** Explosions resolved during the measure window. */
	UPROPERTY()
	int32 Explosions = 0;

	/** Grid blocks destroyed during the measure window (any source). */
	UPROPERTY()
	int32 BlocksDestroyed = 0;

	/** Event frames (an explosion resolved or a block destroyed): count, average and worst of the two frames after each event (ms). */
	UPROPERTY()
	int32 EventFrames = 0;
	UPROPERTY()
	float EventFrameAvg = 0.f;
	UPROPERTY()
	float EventFrameMax = 0.f;

	/** Frames with flow-field work during the measure window (rebuilds, openings, goal rebuild slices), average and worst flow-field ms of those frames. */
	UPROPERTY()
	int32 FlowComputes = 0;
	UPROPERTY()
	float FlowAvg = 0.f;
	UPROPERTY()
	float FlowMax = 0.f;
};

/**
 * Stress test tool of the world (stress-test-m1). Development builds only in practice: the console commands are cheats.
 * - rr.Stress.Spawn <N> [Class]: spawns N robots (default: the chaser class of the first ARREnemySpawner) on walkable cells
 *   reachable in the flow field, nearest first from rr.Stress.MinCells flow-field steps away from the miner, ignoring MaxAlive.
 * - rr.Stress.Clear: destroys every robot the tool spawned (no death, no XP).
 * - rr.Stress.Peace <0|1>: 1 makes the miner invulnerable (loose State.Invulnerable) and stops its weapons, so counts hold.
 * - rr.Stress.Run [Counts] [SettleSeconds] [MeasureSeconds] [Quit] [Mode] [Class]: Mode peace (S0, default), fire (S2),
 *   burst (S7), chain, dig or walk, see ERRStressMode; Class as rr.Stress.Spawn. Disables timed spawners, applies the mode, then for each
 *   count (list separated by commas or +, e.g. 50+100+200) tops the stress robots up to it, waits the settle time, samples every frame for the
 *   measure time and logs average and p99 frame, game, draw and GPU ms; writes Saved/Stress/Stress_<time>.csv at the end and
 *   requests exit when Quit is 1.
 */
UCLASS()
class ROGUERIGS_API URRStressSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Game and PIE worlds only; never created in shipping builds. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Advances the sequencer and samples frame times while a run is active. */
	virtual void Tick(float DeltaTime) override;

	/** Ticks while paused during a run, to dismiss the level-up screen. */
	virtual bool IsTickableWhenPaused() const override { return bRunning; }

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

	/**
	 * Spawns Count robots of EnemyClass (null = default chaser class) around the miner, ignoring any alive cap.
	 * Returns the number spawned.
	 */
	int32 SpawnStressEnemies(int32 Count, TSubclassOf<ARREnemy> EnemyClass = nullptr);

	/** Destroys every robot spawned by SpawnStressEnemies. Returns how many were destroyed. */
	int32 ClearStressEnemies();

	/** Stress robots alive (valid, not being destroyed, not dead). Prunes the list. */
	int32 GetStressEnemyCount();

	/** Peace on: miner invulnerable and weapons stopped. Off: tag removed and weapons fire again. */
	void SetPeace(bool bInPeace);

	/** Adds (or removes) the loose State.Invulnerable tag on every miner of the world. */
	void SetMinersInvulnerable(bool bInvulnerable);

	/** Starts (or stops) the auto-fire of every weapon of every miner. */
	void SetMinerWeaponsFiring(bool bFiring);

	/**
	 * Starts a timed run over Counts (ascending recommended); Settle and Measure in seconds; bQuit exits when done.
	 * Mode: see ERRStressMode. EnemyClass: robots of the run (null = default chaser class).
	 */
	void StartRun(const TArray<int32>& Counts, float SettleSeconds, float MeasureSeconds, bool bQuit,
		ERRStressMode Mode = ERRStressMode::Peace, TSubclassOf<ARREnemy> EnemyClass = nullptr);

	/** Kills Enemy now through its health set (instant explosion-damage GE on itself, far above its health): death, XP, kamikaze explosion. */
	static bool KillEnemy(ARREnemy* Enemy);

	/** Destroys every barrel cell of Grid (each queues its explosion). Returns the number destroyed. */
	static int32 DestroyAllBarrels(ARRArenaGrid* Grid);

	/** Parses a run mode: "peace"/"0", "fire"/"1", "burst"/"2" (case insensitive). False when unknown. */
	static bool ParseMode(const FString& Text, ERRStressMode& OutMode);

	/** True while a run is active. */
	bool IsRunning() const { return bRunning; }

	/** Results of the last (or current) run, one entry per finished step. */
	const TArray<FRRStressStepResult>& GetResults() const { return Results; }

	/**
	 * Robot class named by Name: a spawnable class of an ARREnemySpawner whose name contains Name (case insensitive), else a
	 * class path loaded with LoadClass. Empty Name = default chaser class. Null when none matches.
	 */
	TSubclassOf<ARREnemy> ResolveEnemyClass(const FString& Name) const;

protected:
	/** First spawnable class of the first ARREnemySpawner that derives from ARRChaser, else its first class. */
	TSubclassOf<ARREnemy> GetDefaultEnemyClass() const;

	/** Begins the step at StepIndex: top up the robots and reset the timer; finishes the run past the last step. */
	void BeginStep(int32 StepIndex);

	/** Computes the averages and p99 of the samples of the current step and logs them. */
	void FinishStep();

	/** Writes Results to Saved/Stress/Stress_<time>.csv and logs the table. */
	void WriteResults() const;

	/** Fire mode: raises the health of the stress robots to rr.Stress.FireHealth so shots never kill them. */
	void MakeStressEnemiesTough();

	/** Burst mode: kills every stress robot and destroys every barrel in the same frame. */
	void TriggerBurst();

	/** Counts the explosions resolved during a measure window. */
	UFUNCTION()
	void HandleExplosion(FVector Center, FName RowName);

	/** Counts the grid blocks destroyed during a measure window. */
	UFUNCTION()
	void HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source);

	/** Chain mode: queues rr.Stress.ChainCount explosions (row rr.Stress.ChainRow) on block points around the miner. */
	void TriggerChain();

	/** Dig mode: lists the non-barrel block cells by distance from the miner (the sweep order). */
	void BuildDigQueue();

	/** Dig mode: destroys the next cells of the sweep at rr.Stress.DigRate for DeltaSeconds. */
	void TickDig(float DeltaSeconds);

	/** Walk mode: moves the miner along its circle for DeltaSeconds (teleport, skipped when the target cell is not walkable). */
	void TickWalk(float DeltaSeconds);

	/** Measure window: records the event frame costs and the flow-field computations of this frame. */
	void SampleGridEvents(float FrameMs);

	/** Robots spawned by the tool. */
	TArray<TWeakObjectPtr<ARREnemy>> StressEnemies;

	/** Steps of the active run. */
	TArray<int32> RunCounts;

	/** Results of finished steps. */
	TArray<FRRStressStepResult> Results;

	/** Per-frame samples of the current step (ms). */
	TArray<float> FrameSamples;
	TArray<float> GameSamples;
	TArray<float> DrawSamples;
	TArray<float> GpuSamples;

	/** Index in RunCounts of the current step. */
	int32 StepIndex = INDEX_NONE;

	/** Seconds since the current step began (real time). */
	double StepElapsed = 0.0;

	/** Settle and measure durations of the run (s). */
	float RunSettleSeconds = 5.f;
	float RunMeasureSeconds = 20.f;

	/** True while a run is active. */
	bool bRunning = false;

	/** Request exit at the end of the run. */
	bool bQuitWhenDone = false;

	/** Peace mode state. */
	bool bPeace = false;

	/** Mode and robot class of the active run. */
	ERRStressMode RunMode = ERRStressMode::Peace;
	TSubclassOf<ARREnemy> RunEnemyClass;

	/** Burst results of the current step. */
	int32 StepBurstKills = 0;
	int32 StepBurstBarrels = 0;

	/** Explosions resolved since the current measure window began. */
	int32 StepExplosions = 0;

	/** True once HandleExplosion is bound to the explosion subsystem. */
	bool bExplosionBound = false;

	/** Grid blocks destroyed since the current measure window began. */
	int32 StepBlocksDestroyed = 0;

	/** Event counter (explosions + blocks) at the last sample, frames left in the current event window and its worst frame (ms). */
	int32 LastEventCounter = 0;
	int32 PendingEventFrames = 0;
	float PendingEventMax = 0.f;

	/** Event frame costs and flow-field BFS durations of the current measure window (ms). */
	TArray<float> EventSamples;
	TArray<float> FlowSamples;

	/** Flow-field work frame count at the last sample. */
	int32 LastFlowCount = 0;

	/** Dig mode: sweep order, next index and fractional cells owed. */
	TArray<FIntPoint> DigQueue;
	int32 DigIndex = 0;
	float DigAccumulator = 0.f;

	/** Walk mode: circle center (the miner location at the start of the measure window) and current angle in radians. */
	FVector WalkCenter = FVector::ZeroVector;
	float WalkAngle = 0.f;

	/** Grid whose OnCellDestroyed is bound to HandleCellDestroyed. */
	TWeakObjectPtr<ARRArenaGrid> BoundGrid;
};
