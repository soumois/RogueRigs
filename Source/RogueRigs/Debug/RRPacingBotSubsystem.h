// Rogue Rigs - XP pacing bot: a scripted -game run (real director, real pickups) that aims at the nearest robot, stands still, walks to resting XP cubes or runs straight, picks the first card and logs the time of each level.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRPacingBotSubsystem.generated.h"

class ARRArenaGrid;
class ARREnemySpawner;
class ARRMiner;

/** How the pacing bot moves the miner (fourth argument of rr.Pacing.Run). */
UENUM()
enum class ERRPacingProfile : uint8
{
	/** 0: stands still; the magnet takes what comes in range. */
	Still,
	/** 1: walks to the nearest resting XP cube outside the pickup radius. */
	Collect,
	/** 2: runs in a straight line, turning only at walls (blocks or the arena edge); the magnet takes what it passes. */
	Run
};

/** One level reached during a pacing run. */
USTRUCT()
struct FRRPacingLevelTime
{
	GENERATED_BODY()

	/** Level reached (2 = first level-up). */
	UPROPERTY()
	int32 Level = 0;

	/** Spawner run clock (s of enabled spawning, the level time) when the level was reached. */
	UPROPERTY()
	float Seconds = 0.f;

	/** Robots alive at that moment. */
	UPROPERTY()
	int32 RobotsAlive = 0;

	/** XP cubes lying in the arena at that moment (not collected yet). */
	UPROPERTY()
	int32 CubesLeft = 0;
};

/**
 * Pacing bot (task-028-03, sys-progression): measures the time to each level with the real wave director and the real pickup
 * motion. Debug only: Game and PIE worlds, never in shipping.
 * - rr.Pacing.Run [Seconds=200] [Seed=1] [Quit=1] [Profile=1]: seeds the placed spawner stream and the global random (crits,
 *   cube scatter, run turns) with Seed, makes the player miner invulnerable (cheat count, removed at the end), then every frame
 *   aims the miner stick at the nearest live robot and moves by profile (ERRPacingProfile): 0 still; 1 collect, walks toward
 *   the nearest XP cube lying outside the pickup radius within rr.Pacing.CollectRange (else stands still); 2 run, straight
 *   from screen north, turning at a wall (free distance below rr.Pacing.RunLookahead, or stalled) toward the freest of 7
 *   other 45-degree headings. Picks card 0 of every level-up screen (ticks while paused).
 *   Level time is the spawner run clock (ARREnemySpawner::GetRunTime). Logs one "Pacing: level" line per level, a "Pacing: status" line every
 *   rr.Pacing.StatusInterval s (miner position, level, XP, robots, cubes), a summary
 *   at the end (after Seconds of run clock), writes Saved/Pacing/Pacing_<time>.csv and requests exit when Quit is 1.
 */
UCLASS()
class ROGUERIGS_API URRPacingBotSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Game and PIE worlds only; never created in shipping builds. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Drives the miner and records level times while a run is active. */
	virtual void Tick(float DeltaTime) override;

	/** Ticks while paused during a run, to pick the level-up card. */
	virtual bool IsTickableWhenPaused() const override { return bRunning; }

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

	/** Starts a run of DurationSeconds of spawner clock. False without a player miner or a spawner. */
	bool StartRun(float DurationSeconds, int32 Seed, bool bQuit, ERRPacingProfile InProfile);

	/** True while a run is active. */
	bool IsRunning() const { return bRunning; }

	/** Levels reached so far in the current or last run. */
	const TArray<FRRPacingLevelTime>& GetLevelTimes() const { return LevelTimes; }

protected:
	/** Ends the run: logs the summary, writes the CSV, removes the invulnerability, exits if asked. */
	void FinishRun();

	/** Turns the miner stick toward the nearest live robot (keeps the last aim when none). */
	void AimAtNearestRobot(ARRMiner* Miner) const;

	/** Walks the miner toward the nearest resting XP cube outside its pickup radius (within rr.Pacing.CollectRange). */
	void WalkToNearestCube(ARRMiner* Miner) const;

	/** Run profile: keeps RunHeading, turns at walls or when stalled, and pushes the stick along it. */
	void RunStraight(ARRMiner* Miner, float Now);

	/** Open floor (uu, capped at MaxDistance) from Origin along Direction before a block, void or off-grid cell; samples the miner's width. */
	float GetFreeDistance(const ARRArenaGrid& Grid, const FVector& Origin, const FVector& Direction, float MaxDistance) const;

	/** Pushes the miner stick along a world direction (projected on the camera's screen axes). */
	static void MoveAlong(ARRMiner* Miner, const FVector& Direction);

	/** The first spawner of the world (the one placed in the arena). */
	ARREnemySpawner* FindSpawner() const;

	/** Live robots and lying XP cubes now. */
	void CountWorld(int32& OutRobots, int32& OutCubes) const;

	/** True while a run is active. */
	bool bRunning = false;

	/** Exit the process at the end of the run. */
	bool bQuitWhenDone = false;

	/** How the miner moves this run. */
	ERRPacingProfile Profile = ERRPacingProfile::Collect;

	/** Run profile: current world heading (unit, XY). */
	FVector RunHeading = FVector::ForwardVector;

	/** Run profile: miner position and spawner clock at the last stall check. */
	FVector StallCheckLocation = FVector::ZeroVector;

	/** Run profile: spawner clock of the last stall check. */
	float StallCheckSeconds = 0.f;

	/** Run profile: number of turns taken. */
	int32 RunTurns = 0;

	/** The arena grid (walls for the run profile). */
	TWeakObjectPtr<ARRArenaGrid> Grid;

	/** Spawner clock at which the run ends. */
	float EndSeconds = 0.f;

	/** Spawner clock when the run started (should be close to 0: start the run from -ExecCmds). */
	float StartSeconds = 0.f;

	/** Seed of the run. */
	int32 RunSeed = 0;

	/** Miner level seen on the previous frame. */
	int32 LastLevel = 1;

	/** Spawner clock of the next status line (every rr.Pacing.StatusInterval s). */
	float NextStatusSeconds = 0.f;

	/** Level-up cards picked. */
	int32 CardsPicked = 0;

	/** Levels reached, in order. */
	TArray<FRRPacingLevelTime> LevelTimes;

	/** The spawner whose clock is the level time. */
	TWeakObjectPtr<ARREnemySpawner> Spawner;
};
