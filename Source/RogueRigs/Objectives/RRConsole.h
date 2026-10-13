// Rogue Rigs - command console objective: marks the rocks around it, then is hacked by the drone while the miner stays in its zone.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/RRArenaGrid.h"
#include "RRConsole.generated.h"

class ARRMiner;
class USceneComponent;
class UStaticMeshComponent;
class URRGroundTelegraphComponent;

/** Where a console stands in its objective. */
UENUM(BlueprintType)
enum class ERRConsoleState : uint8
{
	/** Marked rocks remain around it: hacking is impossible until they are destroyed. */
	Locked,
	/** Hackable, the miner is outside the zone: the drone is not linked and progress drains. */
	Idle,
	/** The miner is inside the zone and the drone is linked: progress rises. */
	Hacking,
	/** Hacked: the objective is done (the run flow unpauses the alert). Final. */
	Complete
};

class ARRConsole;

/** Fired when a console changes state. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnConsoleStateChangedSignature, ARRConsole*, Console, ERRConsoleState, NewState);

/** Fired when the hack progress (0..1) changes; bHacking is true while the drone is linked. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnConsoleProgressChangedSignature, ARRConsole*, Console, float, Progress, bool, bHacking);

/** Fired when the number of marked rocks left changes (Total = marked at drop). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnConsoleMarkedRocksChangedSignature, ARRConsole*, Console, int32, Remaining, int32, Total);

/**
 * Command console (objective-console). Spawned by URRRunFlowSubsystem on a floor cell, then InitializeConsole marks every
 * block of MarkedCellTypes whose center lies within MarkRadiusCells cells of the console cell (ARRArenaGrid::SetCellMarked:
 * outline data on the block). State Locked until every marked block is destroyed (polled each tick: dig and explosions alike).
 * Then, each tick: when the miner is alive and within ZoneRadiusCells cells (2D) of the console, the console asks the miner's
 * drone to hack it (ARRDrone::StartHack); while the drone is linked, progress rises by DeltaSeconds / HackDuration (state
 * Hacking). Otherwise the drone is released (StopHack) and progress drains by DrainPerSecond per second (state Idle). At 1 the
 * state becomes Complete (final) and the drone is released. Display events for the Blueprint subclass: OnMarkedRocksChanged,
 * OnHackProgress, OnHackStateChanged. The run flow listens to the matching delegates. Not abstract, so a run without the
 * Blueprint (headless tests) still works; subclass in Blueprint for the mesh and the gauge. The zone is drawn on the ground by
 * GroundTelegraph (sys-ground-telegraph): radius = GetZoneRadius, mode ZoneBlocked while marked rocks remain else ZoneActive,
 * progress = the hack progress (drain included); shown from InitializeConsole, hidden at completion and at EndPlay.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRConsole : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root and the mesh (no collision by default) and enables ticking. */
	ARRConsole();

	/**
	 * Places the console on Cell of Grid (centre of its FootprintCells around Cell, floor top), remembers Miner as the one whose presence drives the hack,
	 * marks the rocks around it and fires the first display events. Call once, right after spawning.
	 */
	void InitializeConsole(ARRArenaGrid* Grid, ARRMiner* Miner, FIntPoint Cell);

	/** Cells around Center of InGrid this console would mark (MarkedCellTypes within MarkRadiusCells, ore table deposit blocks excluded). Works on the class default object. */
	TArray<FIntPoint> FindMarkableCells(const ARRArenaGrid* InGrid, FIntPoint Center) const;

	/** Updates the marked rocks, the zone check, the drone link and the hack progress. */
	virtual void Tick(float DeltaSeconds) override;

	/** Current state. */
	UFUNCTION(BlueprintPure, Category = "Console")
	ERRConsoleState GetConsoleState() const { return ConsoleState; }

	/** Hack progress in [0, 1]. */
	UFUNCTION(BlueprintPure, Category = "Console")
	float GetHackProgress() const { return HackProgress; }

	/** True once the hack completed. */
	UFUNCTION(BlueprintPure, Category = "Console")
	bool IsComplete() const { return ConsoleState == ERRConsoleState::Complete; }

	/** True when no marked rock remains and the hack is not complete. */
	UFUNCTION(BlueprintPure, Category = "Console")
	bool IsHackable() const { return MarkedRocks.IsEmpty() && !IsComplete(); }

	/** Marked rocks still standing. */
	UFUNCTION(BlueprintPure, Category = "Console")
	int32 GetMarkedRockCount() const { return MarkedRocks.Num(); }

	/** Rocks marked when the console was dropped. */
	UFUNCTION(BlueprintPure, Category = "Console")
	int32 GetInitialMarkedRockCount() const { return InitialMarkedRockCount; }

	/** Cells of the marked rocks still standing. */
	UFUNCTION(BlueprintPure, Category = "Console")
	TArray<FIntPoint> GetMarkedRocks() const { return MarkedRocks; }

	/** Grid cell the console stands on (anchor of its footprint). */
	UFUNCTION(BlueprintPure, Category = "Console")
	FIntPoint GetCell() const { return Cell; }

	/** Landing footprint in cells (X along the grid X axis, Y along Y); every footprint cell must be open reachable floor. */
	UFUNCTION(BlueprintPure, Category = "Console")
	FIntPoint GetFootprintCells() const { return FootprintCells; }

	/** True when the miner was inside the zone at the last tick. */
	UFUNCTION(BlueprintPure, Category = "Console")
	bool IsMinerInZone() const { return bMinerInZone; }

	/** Zone radius in uu (ZoneRadiusCells x grid CellSize). */
	UFUNCTION(BlueprintPure, Category = "Console")
	float GetZoneRadius() const;

	/** Ground telegraph drawing the zone circle and the progress circle. */
	UFUNCTION(BlueprintPure, Category = "Console")
	URRGroundTelegraphComponent* GetGroundTelegraph() const { return GroundTelegraph; }

	/** Seconds of uninterrupted hacking from 0 to complete. */
	UFUNCTION(BlueprintPure, Category = "Console")
	float GetHackDuration() const { return HackDuration; }

	/** Progress lost per second while the miner is out of the zone (fraction of the gauge). */
	UFUNCTION(BlueprintPure, Category = "Console")
	float GetDrainPerSecond() const { return DrainPerSecond; }

	//~ Begin cheats (non-shipping: these entry points do nothing in shipping builds)
	/** Cheat and tests: completes the hack at once (CompleteHack). Does nothing in shipping builds. */
	UFUNCTION(BlueprintCallable, Category = "Cheats")
	void ForceComplete();
	//~ End cheats

	/** Broadcast on every state change (after OnHackStateChanged). */
	UPROPERTY(BlueprintAssignable, Category = "Console")
	FRROnConsoleStateChangedSignature OnConsoleStateChanged;

	/** Broadcast whenever the progress or the hacking flag changes (after OnHackProgress). */
	UPROPERTY(BlueprintAssignable, Category = "Console")
	FRROnConsoleProgressChangedSignature OnConsoleProgressChanged;

	/** Broadcast whenever the marked rock count changes, and once at initialization (after OnMarkedRocksChanged). */
	UPROPERTY(BlueprintAssignable, Category = "Console")
	FRROnConsoleMarkedRocksChangedSignature OnConsoleMarkedRocksChanged;

protected:
	/** Releases the drone and unmarks the rocks left when the console goes away. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Completes the hack: unmarks the rocks left, progress 1, state Complete, drone released. No-op once complete. Called by Tick when the gauge fills, and by ForceComplete. Virtual: ARRBonusChest grants its reward after it. */
	virtual void CompleteHack();

	/** Display event: marked rocks left (Remaining) out of Total; also called at initialization. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Console")
	void OnMarkedRocksChanged(int32 Remaining, int32 Total);

	/** Display event: the gauge changed (Progress 0..1; bHacking while the drone is linked). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Console")
	void OnHackProgress(float Progress, bool bHacking);

	/** Display event: the console entered NewState (Locked, Idle, Hacking, Complete). Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Console")
	void OnHackStateChanged(ERRConsoleState NewState);

	/** Marks every cell FindMarkableCells returns around Cell. */
	void MarkRocks();

	/** Drops the marked rocks that no longer hold a block; fires the count events when it changed. */
	void RefreshMarkedRocks();

	/** Sets the state and fires OnHackStateChanged and OnConsoleStateChanged when it changed. */
	void SetConsoleState(ERRConsoleState NewState);

	/** Sets the progress and hacking flag and fires OnHackProgress and OnConsoleProgressChanged when either changed. */
	void SetProgress(float NewProgress, bool bNewHacking);

	/** Fires OnMarkedRocksChanged and OnConsoleMarkedRocksChanged with the current counts. */
	void BroadcastMarkedRocks();

	/** Releases the miner's drone if it is hacking this console. */
	void ReleaseDrone();

	/** Writes the zone mode (ZoneBlocked while marked rocks remain, else ZoneActive) and the hack progress to GroundTelegraph. */
	void RefreshGroundTelegraph();

	/** Root: the console origin, on the floor at the cell center. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> ConsoleRoot;

	/** Console visual; mesh and material set in Blueprint. No collision by default (robots path through the flow field, which ignores it). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Ground signal of the hack zone (sharp zone circle, growing progress circle); hidden until InitializeConsole, hidden again at completion. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URRGroundTelegraphComponent> GroundTelegraph;

	/** Blocks whose center is within this many cells of the console cell are marked (objective-console: 2 cells of 200 uu = 6 cells of 67 uu, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0))
	float MarkRadiusCells = 6.f;

	/**
	 * Landing footprint in cells of the grid, rotation zero (2 x 2 cells of 67 uu = 134 uu for the 100 x 100 uu console mesh, task-026-00).
	 * The console lands only where every cell is reachable floor and stands at the footprint centre (RRLandingFootprint).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 1))
	FIntPoint FootprintCells = FIntPoint(2, 2);

	/** Block types that get marked. Barrels are left out: they explode and are not rock. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console")
	TArray<ERRCellType> MarkedCellTypes = { ERRCellType::Rock, ERRCellType::Ore, ERRCellType::HealOre, ERRCellType::SpecialOre };

	/** Seconds of hacking from 0 to complete (objective-console: 16 s, provisional; 20 s before task-042-01). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0.1, Units = "s"))
	float HackDuration = 16.f;

	/** The miner must be within this many cells (2D, from the console center) for the drone to hack (objective-console: 3 cells of 200 uu = 9 cells of 67 uu, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0))
	float ZoneRadiusCells = 9.f;

	/** Gauge fraction lost per second while the miner is out of the zone (objective-console: 5 %/s = 0.05, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0))
	float DrainPerSecond = 0.05f;

	/** Grid the console stands on. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Console")
	TObjectPtr<ARRArenaGrid> Grid;

	/** Miner whose presence drives the hack (its drone does the hacking). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Console")
	TObjectPtr<ARRMiner> Miner;

	/** Grid cell the console stands on. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Console")
	FIntPoint Cell = FIntPoint(-1, -1);

	/** Marked rocks still standing. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Console")
	TArray<FIntPoint> MarkedRocks;

	/** Rocks marked at initialization. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Console")
	int32 InitialMarkedRockCount = 0;

	/** Current state. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Console")
	ERRConsoleState ConsoleState = ERRConsoleState::Locked;

	/** Hack progress in [0, 1]. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Console")
	float HackProgress = 0.f;

	/** True while the drone is linked (progress rising). */
	bool bDroneLinked = false;

	/** True when the miner was inside the zone at the last tick. */
	bool bMinerInZone = false;

	/** True once InitializeConsole ran. */
	bool bInitialized = false;
};
