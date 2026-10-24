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
class UMaterialInstanceDynamic;

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

/** Look of the console in one state (objective-console state visuals, owner 2026-10-23, provisional): lamps, antenna blink, screen, head turn. */
USTRUCT(BlueprintType)
struct FRRConsoleStateVisual
{
	GENERATED_BODY()

	FRRConsoleStateVisual() = default;

	FRRConsoleStateVisual(const FLinearColor& InLampColor, float InAntennaBlinkRate, const FLinearColor& InScreenColor, float InHeadYawSpeed)
		: LampColor(InLampColor), AntennaBlinkRate(InAntennaBlinkRate), ScreenColor(InScreenColor), HeadYawSpeed(InHeadYawSpeed)
	{
	}

	/** LampColor written on both lamp materials (antenna orange and the other orange parts). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console")
	FLinearColor LampColor = FLinearColor(1.f, 0.45f, 0.05f);

	/** BlinkRate (Hz) written on the antenna lamp only; 0 = steady. The other orange parts never blink. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0, Units = "Hz"))
	float AntennaBlinkRate = 0.f;

	/** ScreenColor written on the screen material (black = off). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console")
	FLinearColor ScreenColor = FLinearColor::Black;

	/** AntennaHead yaw speed about the mast axis, degrees per second; 0 = still. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0, Units = "DegreesPerSecond"))
	float HeadYawSpeed = 0.f;
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
 * block of MarkedCellTypes whose center lies within ZoneRadiusCells cells of the zone centre (the footprint centre, where the
 * console stands), so the hack zone never holds an unmarked rock (ARRArenaGrid::SetCellMarked: outline data on the block). State Locked until every marked block is destroyed (polled each tick: dig and explosions alike).
 * Then, each tick: when the miner is alive and within ZoneRadiusCells cells (2D) of the console, the console asks the miner's
 * drone to hack it (ARRDrone::StartHack); while the drone is linked, progress rises by DeltaSeconds / HackDuration (state
 * Hacking). Otherwise the drone is released (StopHack) and progress drains by DrainPerSecond per second (state Idle). At 1 the
 * state becomes Complete (final) and the drone is released. Display events for the Blueprint subclass: OnMarkedRocksChanged,
 * OnHackProgress, OnHackStateChanged. The run flow listens to the matching delegates. Not abstract, so a run without the
 * Blueprint (headless tests) still works; subclass in Blueprint for the mesh and the gauge. The zone is drawn on the ground by
 * GroundTelegraph (sys-ground-telegraph): radius = GetZoneRadius, mode ZoneBlocked while marked rocks remain else ZoneActive,
 * progress = the hack progress (drain included); shown from InitializeConsole, hidden at completion and at EndPlay.
 * State visuals (bDriveStateVisuals, task-302-01): on every state change the console writes the state FRRConsoleStateVisual
 * on dynamic instances of the lamp slots (Mesh OtherLampSlotName, AntennaHead AntennaLampSlotName) and the screen slot, and
 * turns the Blueprint component AntennaHeadName about its local Z while the state HeadYawSpeed is above 0.
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

	/**
	 * Cells this console would mark if it landed on anchor cell Center of InGrid: every block of MarkedCellTypes (ore deposits included)
	 * whose center lies within ZoneRadiusCells of the footprint centre (the zone centre). Works on the class default object.
	 */
	TArray<FIntPoint> FindMarkableCells(const ARRArenaGrid* InGrid, FIntPoint Center) const;

	/** Fewest markable blocks a landing spot may hold (MinMarkedCells); the drop picker filters on [Min, Max]. */
	int32 GetMinMarkedCells() const { return MinMarkedCells; }

	/** Most markable blocks a landing spot may hold (MaxMarkedCells). */
	int32 GetMaxMarkedCells() const { return MaxMarkedCells; }

	/** True when this console class marks blocks (MarkedCellTypes not empty), so the drop picker applies the marked-block range. */
	bool UsesMarkedCellRange() const { return !MarkedCellTypes.IsEmpty(); }

	/** Updates the marked rocks, the zone check, the drone link and the hack progress. */
	virtual void Tick(float DeltaSeconds) override;

	/** Look the console takes in State (LockedVisual, IdleVisual, HackingVisual, CompleteVisual). */
	const FRRConsoleStateVisual& GetStateVisual(ERRConsoleState State) const;

	/** Antenna head found by name at the first visual refresh (null when the Blueprint has none, or before it ran). */
	UStaticMeshComponent* GetAntennaHead() const { return AntennaHeadComp; }

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
	/** Builds the state visual instances and shows the current state. */
	virtual void BeginPlay() override;

	/** Finds AntennaHead and creates the dynamic lamp and screen instances once (no-op afterwards or when bDriveStateVisuals is false). */
	void SetupStateVisuals();

	/** Writes the current state visual on the lamp and screen instances (and sets up the instances first if needed). */
	void ApplyStateVisuals();

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

	/**
	 * Landing footprint in cells of the grid, rotation zero (2 x 2 cells of 67 uu = 134 uu for the 100 x 100 uu console mesh, task-026-00).
	 * The console lands only where every cell is reachable floor and stands at the footprint centre (RRLandingFootprint).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 1))
	FIntPoint FootprintCells = FIntPoint(2, 2);

	/** Block types that get marked inside the zone (every rock kind, ore included). Barrels are left out: they explode and are not rock. Empty = no marked rock. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console")
	TArray<ERRCellType> MarkedCellTypes = { ERRCellType::Rock, ERRCellType::Ore, ERRCellType::HealOre, ERRCellType::SpecialOre };

	/**
	 * Fewest markable blocks (FindMarkableCells) the zone may hold where the console lands; the drop picker keeps only spots in
	 * [MinMarkedCells, MaxMarkedCells], else the spot nearest the range with a warning (objective-console, owner 2026-10-23: 10, provisional).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0))
	int32 MinMarkedCells = 10;

	/** Most markable blocks the zone may hold where the console lands (objective-console, owner 2026-10-23: 40, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0))
	int32 MaxMarkedCells = 40;

	/** Seconds of hacking from 0 to complete (objective-console, owner 2026-10-23: 10 s, provisional; 12 s before task-305-01, 16 s before task-120-01, 20 s before task-042-01). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0.1, Units = "s"))
	float HackDuration = 10.f;

	/**
	 * Hack zone radius in cells, from the console center (footprint centre): the miner must be within it (2D) for the drone to hack,
	 * and every block of MarkedCellTypes within it is marked (objective-console, owner 2026-10-13: 7 cells of 67 uu, provisional; 9 before task-120-01).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0))
	float ZoneRadiusCells = 7.f;

	/** Gauge fraction lost per second while the miner is out of the zone (objective-console: 5 %/s = 0.05, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console", meta = (ClampMin = 0))
	float DrainPerSecond = 0.05f;

	/**
	 * True: the console drives its lamps, screen and antenna head from its state (task-302-01). ARRBonusChest sets it false
	 * so the chest keeps its own look.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	bool bDriveStateVisuals = true;

	/** Locked (marked rocks left): red, antenna blinks 2 Hz, screen black, head still (objective-console, owner 2026-10-23, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FRRConsoleStateVisual LockedVisual = { FLinearColor(1.f, 0.05f, 0.03f), 2.f, FLinearColor::Black, 0.f };

	/** Idle (rocks cleared, miner out of the zone): orange, antenna blinks 2 Hz, screen black, head 10 deg/s (owner 2026-10-23, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FRRConsoleStateVisual IdleVisual = { FLinearColor(1.f, 0.45f, 0.05f), 2.f, FLinearColor::Black, 10.f };

	/** Hacking: green, antenna blinks 2 Hz, screen green, head 45 deg/s (owner 2026-10-23, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FRRConsoleStateVisual HackingVisual = { FLinearColor(0.1f, 1.f, 0.15f), 2.f, FLinearColor(0.1f, 1.f, 0.15f), 45.f };

	/** Complete: steady green (no blink), screen green, head stopped (owner 2026-10-23, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FRRConsoleStateVisual CompleteVisual = { FLinearColor(0.1f, 1.f, 0.15f), 0.f, FLinearColor(0.1f, 1.f, 0.15f), 0.f };

	/** BlinkAmount written on the antenna lamp while its BlinkRate is above 0 (1 = full on/off pulse; the material ignores BlinkRate at 0). Implementation choice. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals", meta = (ClampMin = 0, ClampMax = 1))
	float AntennaBlinkAmount = 1.f;

	/** Name of the Blueprint StaticMeshComponent that turns (the dish head, pivot on the mast axis). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FName AntennaHeadName = TEXT("AntennaHead");

	/** Material slot of AntennaHead holding the blinking antenna lamp. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FName AntennaLampSlotName = TEXT("AntennaOrange");

	/** Material slot of Mesh holding the other (steady) orange parts. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FName OtherLampSlotName = TEXT("OtherOrange");

	/** Material slot of Mesh holding the screen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Console|Visuals")
	FName ScreenSlotName = TEXT("Screen");

	/** Turning head found by AntennaHeadName (Blueprint component). */
	UPROPERTY(Transient)
	TObjectPtr<UStaticMeshComponent> AntennaHeadComp;

	/** Dynamic instance of the antenna lamp slot (blinks). */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> AntennaLampMID;

	/** Dynamic instance of the other orange slot (steady). */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> OtherLampMID;

	/** Dynamic instance of the screen slot. */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ScreenMID;

	/** True once SetupStateVisuals ran. */
	bool bStateVisualsSetUp = false;

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
