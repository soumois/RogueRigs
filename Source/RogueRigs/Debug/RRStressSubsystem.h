// Rogue Rigs - stress test tooling: cheat commands that spawn robots in bulk around the miner and a timed frame-time sequencer.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRStressSubsystem.generated.h"

class ARREnemy;

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
};

/**
 * Stress test tool of the world (stress-test-m1). Development builds only in practice: the console commands are cheats.
 * - rr.Stress.Spawn <N> [Class]: spawns N robots (default: the chaser class of the first ARREnemySpawner) on walkable cells
 *   reachable in the flow field, nearest first from rr.Stress.MinCells flow-field steps away from the miner, ignoring MaxAlive.
 * - rr.Stress.Clear: destroys every robot the tool spawned (no death, no XP).
 * - rr.Stress.Peace <0|1>: 1 makes the miner invulnerable (loose State.Invulnerable) and stops its weapons, so counts hold.
 * - rr.Stress.Run [Counts] [SettleSeconds] [MeasureSeconds] [Quit]: disables timed spawners, turns peace on, then for each
 *   count (list separated by commas or +, e.g. 50+100+200) tops the stress robots up to it, waits the settle time, samples every frame for the
 *   measure time and logs average and p99 frame, game, draw and GPU ms; writes Saved/Stress/Stress_<time>.csv at the end and
 *   requests exit when Quit is 1.
 */
UCLASS()
class ROGUERIGS_API URRStressSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Game and PIE worlds only. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Advances the sequencer and samples frame times while a run is active. */
	virtual void Tick(float DeltaTime) override;

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

	/** Starts a timed run over Counts (ascending recommended); Settle and Measure in seconds; bQuit exits when done. */
	void StartRun(const TArray<int32>& Counts, float SettleSeconds, float MeasureSeconds, bool bQuit);

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
};
