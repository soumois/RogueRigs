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
	 * When true and at least one candidate cell has blocks to mark around it (the console class's MarkRadiusCells and
	 * MarkedCellTypes), the console drops only on such a cell, so there are rocks to clear (objective-console: "the rocks around
	 * it are marked"). False = any candidate. Provisional choice of task-016-01.
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

	/** Boarding radius in cells: the miner boards when its 2D distance to the shuttle is at most this (1 cell of 200 uu = 3 cells of 67 uu, provisional choice of task-016-02). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Extraction", meta = (ClampMin = 0))
	float ShuttleBoardRadiusCells = 3.f;

	/** Shuttle actor landed when extraction is ready (the shuttle Blueprint). None = the native ARRShuttle (no mesh). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow|Extraction")
	TSubclassOf<ARRShuttle> ShuttleClass;
};
