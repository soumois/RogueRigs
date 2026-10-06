// Rogue Rigs - extraction shuttle: lands far from the miner, counts down, ends the run with a win (miner on board) or a loss (countdown over).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRShuttle.generated.h"

class ARRArenaGrid;
class ARRMiner;
class USceneComponent;
class UStaticMeshComponent;
class ARRShuttle;

/** Fired every ticked frame of the countdown with the seconds left (also once at landing). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnShuttleCountdownChangedSignature, ARRShuttle*, Shuttle, float, RemainingSeconds);

/** Fired once when the shuttle is done: bExtracted true = the miner boarded, false = the countdown ran out. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnShuttleFinishedSignature, ARRShuttle*, Shuttle, bool, bExtracted);

/**
 * Extraction shuttle (extraction gdd). Spawned by URRRunFlowSubsystem on a reachable floor cell once extraction is ready, then
 * InitializeShuttle places it on the cell center and starts the countdown. Each tick (not while paused): when the miner is alive
 * and its 2D distance to the shuttle is at most BoardRadius, the miner boards (OnExtracted, OnShuttleFinished true); otherwise the
 * countdown drops by DeltaSeconds and at 0 the shuttle leaves without the miner (OnMissed, OnShuttleFinished false). A dead miner
 * freezes the countdown (the game over owns the end). Finished is final. The run flow turns OnShuttleFinished into the run end.
 * Not abstract, so headless tests run without the Blueprint; subclass in Blueprint for the mesh and the landing effects.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRShuttle : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root and the mesh (no collision by default) and enables ticking. */
	ARRShuttle();

	/**
	 * Places the shuttle on Cell of Grid (centre of its FootprintCells around Cell, floor top), remembers Miner as the one who must board, starts a countdown
	 * of CountdownSeconds and sets the boarding radius (cm). Fires OnLanded and the first countdown events. Call once, right after
	 * spawning.
	 */
	void InitializeShuttle(ARRArenaGrid* Grid, ARRMiner* Miner, FIntPoint Cell, float CountdownSeconds, float BoardRadius);

	/** Boarding check and countdown. */
	virtual void Tick(float DeltaSeconds) override;

	/** Seconds left before the shuttle leaves. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	float GetRemainingSeconds() const { return RemainingSeconds; }

	/** Countdown length given at landing. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	float GetCountdownSeconds() const { return CountdownSeconds; }

	/** Boarding radius in cm (2D distance between the miner and the shuttle). */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	float GetBoardRadius() const { return BoardRadius; }

	/** Grid cell the shuttle landed on (anchor of its footprint). */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	FIntPoint GetCell() const { return Cell; }

	/** Landing footprint in cells (X along the grid X axis, Y along Y); every footprint cell must be open reachable floor. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	FIntPoint GetFootprintCells() const { return FootprintCells; }

	/** True once the miner boarded or the countdown ran out. */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	bool IsFinished() const { return bFinished; }

	/** True when the miner boarded (valid once finished). */
	UFUNCTION(BlueprintPure, Category = "Shuttle")
	bool WasExtracted() const { return bExtracted; }

	/** Broadcast every countdown frame and once at landing. */
	UPROPERTY(BlueprintAssignable, Category = "Shuttle")
	FRROnShuttleCountdownChangedSignature OnShuttleCountdownChanged;

	/** Broadcast once when the miner boards or the countdown runs out. */
	UPROPERTY(BlueprintAssignable, Category = "Shuttle")
	FRROnShuttleFinishedSignature OnShuttleFinished;

protected:
	/** Display event: the shuttle touched down (landing effects). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnLanded();

	/** Display event: the countdown changed (every ticked frame, and at landing). Implement in Blueprint (gauge, beacon). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnCountdown(float Remaining);

	/** Display event: the miner boarded (run won). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnExtracted();

	/** Display event: the countdown ran out, the shuttle leaves without the miner (run lost). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shuttle")
	void OnMissed();

	/** Ends the shuttle once: fires OnExtracted or OnMissed, then OnShuttleFinished. */
	void Finish(bool bInExtracted);

	/** Fires OnCountdown and OnShuttleCountdownChanged with the current value. */
	void BroadcastCountdown();

	/** Root of the actor (the landing point, floor top). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<USceneComponent> ShuttleRoot;

	/** Shuttle mesh, set in the Blueprint subclass. No collision by default. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shuttle")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Grid the shuttle landed on. */
	UPROPERTY(Transient)
	TObjectPtr<ARRArenaGrid> Grid;

	/** Miner who must board. */
	UPROPERTY(Transient)
	TObjectPtr<ARRMiner> Miner;

	/** Landing cell. */
	FIntPoint Cell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/**
	 * Landing footprint in cells of the grid, rotation zero (4 x 3 cells of 67 uu = 268 x 201 uu for the 240 x 160 uu shuttle mesh,
	 * task-026-00). The shuttle lands only where every cell is reachable floor and stands at the footprint centre (RRLandingFootprint).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shuttle", meta = (ClampMin = 1))
	FIntPoint FootprintCells = FIntPoint(4, 3);

	/** Countdown length given at landing (s). */
	float CountdownSeconds = 0.f;

	/** Seconds left. */
	float RemainingSeconds = 0.f;

	/** Boarding radius (cm). */
	float BoardRadius = 0.f;

	/** True once InitializeShuttle ran. */
	bool bInitialized = false;

	/** True once finished (final). */
	bool bFinished = false;

	/** True when the miner boarded. */
	bool bExtracted = false;
};
