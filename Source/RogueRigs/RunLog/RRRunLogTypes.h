// Rogue Rigs - run log data: per-level counters, level entries and the run record (run-log-schema).

#pragma once

#include "CoreMinimal.h"
#include "RRRunLogTypes.generated.h"

/** How a level ended (level_outcome of run-log-schema). */
UENUM(BlueprintType)
enum class ERRRunLevelOutcome : uint8
{
	/** The miner boarded the shuttle (or a cheat extracted it). */
	Extracted,
	/** The miner died. */
	Death,
	/** The shuttle left without the miner. */
	MissedShuttle
};

/**
 * Counters of one level of a run (run-log-schema), as collected so far. Keys are run-log values: terrain sources
 * barrel / kamikaze / drone / shot, terrain types rock / ore / heal_ore / special_ore / barrel, weapon class names, robot types
 * (EnemyStats.csv row: Chaser, Kamikaze...), kill sources weapon / explosion / other, damage-taken sources robot type /
 * explosion / other. Copy taken by URRRunLogSubsystem::GetCurrentLevelStats; the writer serializes it.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRunLevelStats
{
	GENERATED_BODY()

	/** Run level index (1..N) of these counters; 0 when no run was in progress at level start. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	int32 LevelIndex = 0;

	/** Layout name of the level (URRRunSubsystem::GetCurrentLayoutName at level start). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	FString LayoutName;

	/** World time (seconds, pauses excluded) at which the level started: first miner BeginPlay, then OnLevelReady after a travel. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	double LevelStartWorldSeconds = 0.0;

	/** Destroyed blocks per destruction source (barrel, kamikaze, drone, shot). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> TerrainDestroyedBySource;

	/** Destroyed blocks per block type (rock, ore, heal_ore, special_ore, barrel). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> TerrainDestroyedByType;

	/** Raw damage dealt to robots per weapon class name (turret shots count for their launcher). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, float> DamageDealtByWeapon;

	/** Robots killed per robot type (EnemyStats.csv row name, class name when the row is empty). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> KillsByRobotType;

	/** Robots killed per kill source (weapon, explosion, other). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> KillsBySource;

	/** Upgrade cards picked during the level, in pick order (card ids, see URRRunLogSubsystem::MakeCardId). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TArray<FString> UpgradesPicked;

	/** Raw damage taken by the miner per source (robot type, explosion, other). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, float> DamageTakenBySource;

	/** XP gained by the miner during the level (after the XPGain multiplier; cheat grants included). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	float XPCollected = 0.f;
};

/** One alert event met during a level (events_met): what it was, which one (hordes), and how it ended. */
struct ROGUERIGS_API FRRRunLogEvent
{
	/** Event name: console, bonus_chest, horde. */
	FString Event;

	/** Horde index in the spawner director data; INDEX_NONE for the other events (not written). */
	int32 Index = INDEX_NONE;

	/** Outcome at level end: console hacked / not_hacked, bonus_chest opened / not_opened, horde lull / announced / spawning / done. */
	FString Outcome;
};

/** One level entry of a run: the collector counters plus the end-of-level fields, written at level end. */
struct ROGUERIGS_API FRRRunLogLevelEntry
{
	/** Collector counters of the level (URRRunLogSubsystem::GetCurrentLevelStats at level end). */
	FRRRunLevelStats Stats;

	/** level_outcome: extracted, death or missed_shuttle. */
	FString LevelOutcome;

	/** time_per_level: world seconds from level start to level end (pauses excluded: level-up screens and the end pause do not count). */
	float TimePerLevelSeconds = 0.f;

	/** True when the miner boarded the shuttle (time_left_at_shuttle is written; else null). */
	bool bReachedShuttle = false;

	/** time_left_at_shuttle: countdown seconds left when the miner boarded. */
	float TimeLeftAtShuttleSeconds = 0.f;

	/** ore_collected_in_risk_window: carried ore gained between the shuttle landing and the level end. */
	int32 OreCollectedInRiskWindow = 0;

	/** events_met: alert events that happened in the level, with their outcome. */
	TArray<FRRRunLogEvent> EventsMet;
};

/** One run: header fields and the level entries, serialized as one JSON file in Saved/RunLogs/. */
struct ROGUERIGS_API FRRRunLogRecord
{
	/** run_id: local start time yyyyMMdd-HHmmss (also the file name). */
	FString RunId;

	/** Absolute path of the run file. */
	FString FilePath;

	/** start_time (UTC, ISO 8601). */
	FDateTime StartTimeUtc;

	/** end_time (UTC, ISO 8601); written only once the run ended (null before). */
	FDateTime EndTimeUtc;

	/** True once the run ended (won, death, missed shuttle, abandoned). */
	bool bEnded = false;

	/** build_version: FApp::GetBuildVersion (engine branch and changelist of the build). */
	FString BuildVersion;

	/** build_configuration: Debug, Development, Shipping... */
	FString BuildConfiguration;

	/** layouts: layout name of each level started, in order. */
	TArray<FString> Layouts;

	/** outcome: in_progress, won, death, missed_shuttle, abandoned (restart before any end). */
	FString Outcome;

	/** levels: one entry per ended level, in order. */
	TArray<FRRRunLogLevelEntry> Levels;
};

