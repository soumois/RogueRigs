// Rogue Rigs - run flow of a level: alert bar from time and kills, console drop and pause, extraction-ready event.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowConfig.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRRunFlowSubsystem.generated.h"

class ARRArenaGrid;
class ARRMiner;

/** Phase of the level run flow. */
UENUM(BlueprintType)
enum class ERRRunPhase : uint8
{
	/** The alert bar fills with time and kills. */
	Filling,
	/** A mandatory objective (the console) is pending: the alert bar is paused. */
	ObjectivePending,
	/** The bar is full and no objective is pending: extraction may start (OnExtractionReady fired). Final. */
	ExtractionReady
};

/** Fired when the alert value (0..1) or its paused flag changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnAlertChangedSignature, float, Alert, bool, bPaused);

/** Fired when the objective changes: console (null when none), its state, marked rocks left and marked at drop. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRROnObjectiveChangedSignature, ARRConsole*, Console, ERRConsoleState, State, int32, MarkedRocksLeft, int32, MarkedRocksTotal);

/** Fired when the pending console's hack progress (0..1) or hacking flag changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnObjectiveProgressChangedSignature, float, Progress, bool, bHacking);

/** Fired once when a console is dropped. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnConsoleDroppedSignature, ARRConsole*, Console);

/** Fired once when the alert bar is full and no mandatory objective is pending (the shuttle hooks here). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnExtractionReadySignature);

/**
 * Run flow of one level (level-flow M1, difficulty alert counter). Game and PIE worlds; active once the world has an
 * ARRArenaGrid and a living ARRMiner (player 0's pawn, else the first one).
 * Alert: AlertSeconds grows by DeltaSeconds x the cheat AlertSpeedMultiplier (URRCheatSettings) each tick and by
 * KillAlertSeconds x the multiplier per robot kill (NotifyEnemyKilled, from ARREnemy::HandleDeath; a kamikaze's own fuse is not
 * a kill); Alert = AlertSeconds / AlertFillSeconds, clamped to [0, 1]. Nothing grows while an objective is pending or while the
 * game is paused (the subsystem does not tick when paused).
 * Console: when Alert reaches ConsoleAlertThreshold the alert is clamped there and the console drops (ConsoleClass) on a random
 * Floor cell whose straight-line distance to the miner's cell is within [ConsoleMinDistanceCells, ConsoleMaxDistanceCells] and
 * that the flow field reaches from the miner, preferring cells with rocks to mark (bPreferCellsWithMarkedRocks); no candidate =
 * retried each tick (warning once). The alert stays paused until
 * the console is Complete.
 * Extraction: when Alert is 1 and no objective is pending, OnExtractionReady fires once (phase ExtractionReady); the shuttle
 * (task-016-02) binds to it. Display data: OnAlertChanged, OnObjectiveChanged, OnObjectiveProgressChanged (URRHUDWidget binds).
 */
UCLASS()
class ROGUERIGS_API URRRunFlowSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Seeds the console placement stream from the global random generator. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Game and PIE worlds only. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Advances the alert, drops the console at the threshold and fires the extraction event. */
	virtual void Tick(float DeltaTime) override;

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

	/** Replaces the tunables (tests); the game mode's RunFlowConfig is then no longer read. */
	void SetConfig(const FRRRunFlowConfig& InConfig);

	/** Current tunables. */
	const FRRRunFlowConfig& GetConfig() const { return Config; }

	/** Turns the whole flow on or off (off: nothing grows, nothing drops). On by default. */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	void SetRunFlowEnabled(bool bEnabled) { bRunFlowEnabled = bEnabled; }

	/** True while the flow runs. */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool IsRunFlowEnabled() const { return bRunFlowEnabled; }

	/** Alert value in [0, 1]. */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	float GetAlert() const;

	/** Alert in seconds of fill time (0..AlertFillSeconds). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	float GetAlertSeconds() const { return AlertSeconds; }

	/** True while a mandatory objective is pending (the alert does not grow). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool HasPendingObjective() const { return bConsoleTriggered && !bConsoleDone; }

	/** Current phase. */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	ERRRunPhase GetPhase() const;

	/** True once OnExtractionReady fired. */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool IsExtractionReady() const { return bExtractionReady; }

	/** The console of this level (null before the drop). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	ARRConsole* GetConsole() const { return Console; }

	/** Kills counted so far (those that added alert time or arrived while paused). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	int32 GetKillCount() const { return KillCount; }

	/**
	 * A robot died: adds KillAlertSeconds (x the cheat multiplier) unless an objective is pending or extraction is ready. A death
	 * whose instigator is the robot itself (kamikaze fuse) is not a kill. Called by ARREnemy::HandleDeath.
	 */
	void NotifyEnemyKilled(AActor* Enemy, AActor* DeathInstigator);

	/** Sets the alert value (0..1) directly (cheats, tests); the threshold and extraction checks run on the next tick. */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	void SetAlert(float NewAlert);

	/** Drops the console now if it has not dropped yet: the alert jumps to the threshold. Returns the console (null on failure). */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	ARRConsole* SkipToConsole();

	/** Completes the pending console at once (ARRConsole::ForceComplete). False when none is pending. */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	bool CompleteConsoleHack();

	/** Completes or skips the console and fills the alert: OnExtractionReady fires now. False when the flow has no grid or miner. */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	bool SkipToExtraction();

	/** Seeds the console placement stream (tests). */
	void SetRandomSeed(int32 Seed) { RandomStream.Initialize(Seed); }

	/** Broadcast when the alert value or its paused flag changes. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnAlertChangedSignature OnAlertChanged;

	/** Broadcast when the console drops, changes state or loses a marked rock. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnObjectiveChangedSignature OnObjectiveChanged;

	/** Broadcast when the console's hack progress or hacking flag changes. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnObjectiveProgressChangedSignature OnObjectiveProgressChanged;

	/** Broadcast once when the console drops. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnConsoleDroppedSignature OnConsoleDropped;

	/** Broadcast once when the bar is full and no objective is pending. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnExtractionReadySignature OnExtractionReady;

protected:
	/** Finds the grid (first ARRArenaGrid) and the miner (player 0's pawn, else the first living ARRMiner). False when either is missing. */
	bool ResolveRunActors();

	/** Reads RunFlowConfig from the auth game mode (ARRGameMode) once, unless SetConfig was called. */
	void ResolveConfig();

	/** Picks the console cell (see class comment). False when no cell qualifies. */
	bool FindConsoleCell(FIntPoint& OutCell);

	/** Spawns and initializes the console on a picked cell and binds its events. Null when no cell qualifies or the spawn failed. */
	ARRConsole* DropConsole();

	/** Fires OnExtractionReady once when the bar is full and nothing is pending. */
	void CheckExtraction();

	/** Fires OnAlertChanged when the value or the paused flag changed since the last broadcast. */
	void BroadcastAlertIfChanged();

	/** Alert gain multiplier from the cheat settings (1 when unset or not positive). */
	float GetAlertGainMultiplier() const;

	/** Console state callback: marks the objective done on Complete, relays OnObjectiveChanged. */
	UFUNCTION()
	void HandleConsoleStateChanged(ARRConsole* InConsole, ERRConsoleState NewState);

	/** Console progress callback: relays OnObjectiveProgressChanged. */
	UFUNCTION()
	void HandleConsoleProgressChanged(ARRConsole* InConsole, float Progress, bool bHacking);

	/** Console marked rocks callback: relays OnObjectiveChanged. */
	UFUNCTION()
	void HandleConsoleMarkedRocksChanged(ARRConsole* InConsole, int32 Remaining, int32 Total);

	/** Tunables. */
	UPROPERTY(Transient)
	FRRRunFlowConfig Config;

	/** Grid of the run. */
	UPROPERTY(Transient)
	TObjectPtr<ARRArenaGrid> Grid;

	/** Miner of the run. */
	UPROPERTY(Transient)
	TObjectPtr<ARRMiner> Miner;

	/** The dropped console. */
	UPROPERTY(Transient)
	TObjectPtr<ARRConsole> Console;

	/** Console placement stream. */
	FRandomStream RandomStream;

	/** Alert in seconds of fill time. */
	float AlertSeconds = 0.f;

	/** Kills counted. */
	int32 KillCount = 0;

	/** Last broadcast alert value and paused flag (-1 = never broadcast). */
	float LastBroadcastAlert = -1.f;
	bool bLastBroadcastPaused = false;

	/** True once the alert reached the console threshold (the objective is pending until bConsoleDone). */
	bool bConsoleTriggered = false;

	/** True once the console is complete (or skipped by SkipToExtraction). */
	bool bConsoleDone = false;

	/** True once OnExtractionReady fired. */
	bool bExtractionReady = false;

	/** True once Config was set by SetConfig or read from the game mode. */
	bool bConfigResolved = false;

	/** True once the "no console cell" warning was logged. */
	bool bDropFailureLogged = false;

	/** Flow on/off. */
	bool bRunFlowEnabled = true;
};
