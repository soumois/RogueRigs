// Rogue Rigs - tunables of the level run flow (alert bar, console drop, bonus chest drop, extraction shuttle), edited on the game mode Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "RRRunFlowConfig.generated.h"

class ARRBonusChest;
class ARRConsole;
class ARRShuttle;

/**
 * Tunables of URRRunFlowSubsystem. Defaults are the provisional M1 values (level-flow, objective-console, owner 2026-10-03).
 * Edited on the game mode Blueprint (ARRGameMode::RunFlowConfig); the subsystem reads them from the auth game mode on its first
 * tick unless SetConfig was called (tests).
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRunFlowConfig
{
	GENERATED_BODY()

	/** Seconds of time alone to fill the alert bar from 0 to 1 (M1: 240 s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow", meta = (ClampMin = 1, Units = "s"))
	float AlertFillSeconds = 240.f;

	/** Alert seconds added by each robot kill (M1: 0.5 s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow", meta = (ClampMin = 0, Units = "s"))
	float KillAlertSeconds = 0.5f;

	/**
	 * When true, kills add no alert until the front of the first horde of the run's spawner has finished spawning (its state is
	 * Done), so the bar follows time alone and the horde icons stay still; afterwards each kill adds KillAlertSeconds again. No
	 * spawner, director off or no horde = kills add alert from the start (level-flow M1, owner 2026-11-05, provisional, task-454).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow")
	bool bKillsWaitForFirstHorde = true;

	/** Alert value (0..1) at which the console drops; the bar pauses there until it is hacked (M1: 0.4). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow", meta = (ClampMin = 0, ClampMax = 1))
	float ConsoleAlertThreshold = 0.4f;

	/** Minimum straight-line distance, in cells, between the miner's cell and the console cell (objective-console: 12 cells of 200 uu = 36 cells of 67 uu, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow", meta = (ClampMin = 0))
	float ConsoleMinDistanceCells = 36.f;

	/** Maximum straight-line distance, in cells, between the miner's cell and the console cell (objective-console: 20 cells of 200 uu = 60 cells of 67 uu, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow", meta = (ClampMin = 0))
	float ConsoleMaxDistanceCells = 60.f;

	/**
	 * When true the console drops only where its zone holds between the console class's MinMarkedCells and MaxMarkedCells markable
	 * blocks (FindMarkableCells: ZoneRadiusCells, MarkedCellTypes); no such spot = the spot nearest the range, warning
	 * (objective-console, owner 2026-10-23, provisional; a plain "has marked rocks" preference before task-304-01). False = any candidate.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow")
	bool bPreferCellsWithMarkedRocks = true;

	/** Console actor spawned at the threshold (the console Blueprint). None = the native ARRConsole (no mesh). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow")
	TSubclassOf<ARRConsole> ConsoleClass;

	/** Alert value (0..1) at which the bonus chest drops, once per level; optional, the bar keeps filling (objective-bonus-chest: 0.6, provisional). 0 = no chest. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Bonus Chest", meta = (ClampMin = 0, ClampMax = 1))
	float BonusChestAlertThreshold = 0.6f;

	/** Minimum straight-line distance, in cells, between the miner's cell and the chest cell (objective-bonus-chest: 30 cells of 67 uu, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Bonus Chest", meta = (ClampMin = 0))
	float BonusChestMinDistanceCells = 30.f;

	/** Maximum straight-line distance, in cells, between the miner's cell and the chest cell (objective-bonus-chest: 45 cells of 67 uu, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Bonus Chest", meta = (ClampMin = 0))
	float BonusChestMaxDistanceCells = 45.f;

	/** Bonus chest actor dropped at its threshold (the chest Blueprint). None = the native ARRBonusChest (no mesh). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Bonus Chest")
	TSubclassOf<ARRBonusChest> BonusChestClass;

	/** Minimum straight-line distance, in cells, between the miner's cell and the shuttle cell (extraction: 37 cells of 67 uu, provisional; 75 before task-049-01). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Extraction", meta = (ClampMin = 0))
	float ShuttleMinDistanceCells = 37.f;

	/** Maximum straight-line distance, in cells, between the miner's cell and the shuttle cell (extraction: 50 cells of 67 uu, provisional, task-049-01). Below ShuttleMinDistanceCells it is treated as equal to it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Extraction", meta = (ClampMin = 0))
	float ShuttleMaxDistanceCells = 50.f;

	/** Seconds between the shuttle landing and its departure without the miner (extraction: 45 s, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Extraction", meta = (ClampMin = 0.1, Units = "s"))
	float ShuttleCountdownSeconds = 45.f;

	/** Shuttle actor landed when extraction is ready (the shuttle Blueprint). None = the native ARRShuttle (no mesh). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Extraction")
	TSubclassOf<ARRShuttle> ShuttleClass;

	/**
	 * Arrival extractor (level-flow step 1, task-371): cell distance along the screen up axis from the miner's start cell to the
	 * nearest row of the arrival footprint (1 = the footprint starts on the next row, the miner stands just outside it). The
	 * footprint (ShuttleClass FootprintCells) is centred on the start cell across that axis. Provisional, owner 2026-10-29 ("1 cell").
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Extraction", meta = (ClampMin = 1))
	int32 ArrivalDistanceCells = 1;

	/** A pending horde icon moving by more than this (bar fraction) re-sends OnAlertBarEventsChanged; smaller moves wait (HUD). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|HUD", meta = (ClampMin = 0))
	float AlertEventPositionTolerance = 0.002f;
};
