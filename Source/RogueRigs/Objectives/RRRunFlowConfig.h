// Rogue Rigs - tunables of the level run flow (alert bar and console drop), edited on the game mode Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "RRRunFlowConfig.generated.h"

class ARRConsole;

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

	/** Minimum straight-line distance, in cells, between the miner's cell and the console cell (12, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow", meta = (ClampMin = 0))
	float ConsoleMinDistanceCells = 12.f;

	/** Maximum straight-line distance, in cells, between the miner's cell and the console cell (20, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Run Flow", meta = (ClampMin = 0))
	float ConsoleMaxDistanceCells = 20.f;

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
};
