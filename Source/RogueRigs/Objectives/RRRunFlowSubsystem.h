// Rogue Rigs - run flow of a level: alert bar from time and kills, console drop and pause, extraction shuttle and run end.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"
#include "Objectives/RRConsole.h"
#include "Enemies/RREnemySpawner.h"
#include "Objectives/RRRunFlowConfig.h"
#include "UI/RRHUDTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRRunFlowSubsystem.generated.h"

class ARRArenaGrid;
class ARRBonusChest;
class ARRMiner;
class ARRShuttle;
class URRFlowFieldSubsystem;
class URRRunTransitionSubsystem;
enum class ERRTransitionFade : uint8;

/** Phase of the level run flow. */
UENUM(BlueprintType)
enum class ERRRunPhase : uint8
{
	/** The alert bar fills with time and kills. */
	Filling,
	/** A mandatory objective (the console) is pending: the alert bar is paused. */
	ObjectivePending,
	/** The bar is full and no objective is pending: OnExtractionReady fired, the shuttle lands and counts down. */
	ExtractionReady,
	/** The run ended: the miner boarded the shuttle (win) or the countdown ran out (loss). Final. */
	Ended
};

/** Fired when the alert value (0..1) or its paused flag changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnAlertChangedSignature, float, Alert, bool, bPaused);

/** Fired when the objective changes: console (null when none), its state, marked rocks left and marked at drop. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRROnObjectiveChangedSignature, ARRConsole*, Console, ERRConsoleState, State, int32, MarkedRocksLeft, int32, MarkedRocksTotal);

/** Fired when the pending console's hack progress (0..1) or hacking flag changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnObjectiveProgressChangedSignature, float, Progress, bool, bHacking);

/** Fired once when a console is dropped. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnConsoleDroppedSignature, ARRConsole*, Console);

/** Fired once when the bonus chest is dropped. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnBonusChestDroppedSignature, ARRBonusChest*, BonusChest);

/** Fired once when the alert bar is full and no mandatory objective is pending (the shuttle hooks here). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnExtractionReadySignature);

/** Fired once when the shuttle lands. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnShuttleLandedSignature, ARRShuttle*, Shuttle);

/** Fired every countdown frame (and at landing) with the seconds left and the shuttle location. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnShuttleCountdownSignature, float, RemainingSeconds, FVector, ShuttleLocation);

/** Fired once when the run ends: bSuccess true = extracted, false = the shuttle left without the miner. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnRunEndedSignature, bool, bSuccess);

/** Fired when the alert-bar icons changed (console, bonus chest, the next horde not over, shuttle). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnAlertBarEventsChangedSignature, const TArray<FRRAlertBarEvent>&, Events);

/** Relay of the registered spawner's horde announcement: horde HordeIndex starts in SecondsUntil seconds. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnRunFlowHordeIncomingSignature, int32, HordeIndex, float, SecondsUntil);

/** Fired when the objective locator target changed (None = hide the arrow). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLocatorTargetChangedSignature, ERRLocatorTarget, Target, FVector, WorldLocation);

/** Fired when a miner registered at its BeginPlay (late HUD binding). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnMinerRegisteredSignature, ARRMiner*, Miner);

/** Fired when the carried or the secured ore count changed (ore-economy): both current counts. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnOreChangedSignature, int32, CarriedOre, int32, SecuredOre);

/** Fired when an enemy spawner registered at its BeginPlay and became the run's spawner. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnSpawnerRegisteredSignature, ARREnemySpawner*, Spawner);

/**
 * Run flow of one level (level-flow M1, difficulty alert counter). Game and PIE worlds; active once the world has an
 * ARRArenaGrid and a living ARRMiner (player 0's pawn, else the first one).
 * Alert: AlertSeconds grows by DeltaSeconds x the cheat AlertSpeedMultiplier (URRCheatSettings) each tick and by
 * KillAlertSeconds x the multiplier per robot kill once the first horde front is over (DoKillsAddAlert, bKillsWaitForFirstHorde;
 * NotifyEnemyKilled, from ARREnemy::HandleDeath; a kamikaze's own fuse is not
 * a kill); Alert = AlertSeconds / AlertFillSeconds, clamped to [0, 1]. Nothing grows while an objective is pending or while the
 * game is paused (the subsystem does not tick when paused).
 * Console: when Alert reaches ConsoleAlertThreshold the alert is clamped there and the console drops (ConsoleClass) on a random
 * Floor cell whose straight-line distance to the miner's cell is within [ConsoleMinDistanceCells, ConsoleMaxDistanceCells] and
 * that the flow field reaches from the miner, keeping cells whose zone holds [MinMarkedCells, MaxMarkedCells] markable blocks (ARRConsole; bPreferCellsWithMarkedRocks; none = nearest count, warning); no candidate =
 * retried each tick (warning once). The alert stays paused until
 * the console is Complete. Console and shuttle land only where every cell of their FootprintCells is reachable floor
 * (RRLandingFootprint); when no such spot exists the fallback is logged as a warning.
 * Bonus chest (objective-bonus-chest): once per level, when Alert reaches BonusChestAlertThreshold (> 0) before extraction is
 * ready, the chest (BonusChestClass) drops on a random reachable Floor cell within [BonusChestMinDistanceCells,
 * BonusChestMaxDistanceCells] of the miner's cell whose footprint is reachable floor (same picker as the console, no marked-block
 * range; no candidate = retried each tick). Optional: the alert keeps filling, nothing waits for it. An alert that jumps
 * straight to full (cheat) skips it when extraction becomes ready in that same tick.
 * Extraction: when Alert is 1 and no objective is pending, OnExtractionReady fires once (phase ExtractionReady) and the shuttle
 * (ShuttleClass) lands on a random reachable Floor cell within [ShuttleMinDistanceCells, ShuttleMaxDistanceCells] of the miner's
 * cell (none = the fitting spot nearest to that band, then the reachable floor cell nearest to it, warning; no reachable cell =
 * retried each tick), with a ShuttleCountdownSeconds countdown (the
 * cheat ShuttleCountdownOverride replaces it when > 0). Robots keep coming. The shuttle finishing ends the run (EndRun): the
 * miner's level-up offer is cancelled, player 0's controller pauses the game with UI input and the cursor shown, OnRunEnded
 * fires once (phase Ended). RestartLevel reopens the map. Display data: OnAlertChanged, OnObjectiveChanged, OnObjectiveProgressChanged (URRHUDWidget binds).
 * Ore counter (ore-economy): an Ore pickup collected adds its OreValue to the carried count (AddCarriedOre, from
 * ARRPickup::Collect; the GrantOre cheat too). EndRun(true) adds the carried count to the secured total of URROreBankSubsystem
 * (game instance: survives RestartLevel) and sets carried to 0; EndRun(false) (shuttle missed) and the death of a registered
 * miner set carried to 0, secured unchanged. After either, carried stays 0 for this world. OnOreChanged sends both counts on
 * every change; a HUD panel binds it in BindToRunFlow and reads GetCarriedOre / GetSecuredOre there.
 */
UCLASS()
class ROGUERIGS_API URRRunFlowSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Seeds the console placement stream from the global random generator. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Unbinds the arrival extractor's wait on the transition fade. */
	virtual void Deinitialize() override;

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

	/**
	 * Turns the arrival extractor placement on or off (task-494-01: the generator test map has no shuttle). On by default; off
	 * before the first tick: no arrival is placed (TickArrival and PlaceArrivalShuttle do nothing).
	 */
	void SetArrivalEnabled(bool bEnabled) { bArrivalEnabled = bEnabled; }

	/** True when the arrival extractor is placed at level start (SetArrivalEnabled). */
	bool IsArrivalEnabled() const { return bArrivalEnabled; }

	/**
	 * Turns the arrival footprint clear (task-372-01) on or off; on by default. Off: the arrival extractor is still placed but the
	 * blocks under it stay (task-371 behaviour). The automation test worlds turn it off (RRTestHarness::FTestWorld) so the
	 * scenes they build next to the miner are not cleared; the extraction landing clear is not affected.
	 */
	void SetArrivalClearsFootprint(bool bClears) { bArrivalClearsFootprint = bClears; }

	/** True when the arrival placement clears the blocks of its footprint (SetArrivalClearsFootprint). */
	bool DoesArrivalClearFootprint() const { return bArrivalClearsFootprint; }

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

	/** The bonus chest of this level (null before its drop or when disabled). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	ARRBonusChest* GetBonusChest() const { return BonusChest; }

	/** True once the alert reached the bonus chest threshold (the chest drops, or its drop is being retried). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool IsBonusChestTriggered() const { return bBonusChestTriggered; }

	/** Kills counted so far (those that added alert time or arrived while paused). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	int32 GetKillCount() const { return KillCount; }

	/**
	 * True when a kill adds alert time now (pending objective and extraction aside): always when bKillsWaitForFirstHorde is off or
	 * the run has no spawner, no enabled director or no horde; otherwise once the first horde (index 0) is Done.
	 */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool DoKillsAddAlert() const;

	/**
	 * A robot died: counts it, and adds KillAlertSeconds (x the cheat multiplier) unless an objective is pending, extraction is
	 * ready or the first horde front is not over yet (DoKillsAddAlert). A death
	 * whose instigator is the robot itself (kamikaze fuse) is not a kill. Called by ARREnemy::HandleDeath.
	 */
	void NotifyEnemyKilled(AActor* Enemy, AActor* DeathInstigator);

	//~ Begin cheats (cheats and tests; non-shipping: in shipping builds they do nothing and return null or false)
	/** Sets the alert value (0..1) directly; the threshold and extraction checks run on the next tick. */
	UFUNCTION(BlueprintCallable, Category = "Cheats")
	void SetAlert(float NewAlert);

	/** Drops the console now if it has not dropped yet: the alert jumps to the threshold. Returns the console (null on failure). */
	UFUNCTION(BlueprintCallable, Category = "Cheats")
	ARRConsole* SkipToConsole();

	/** Completes the pending console at once (ARRConsole::ForceComplete). False when none is pending. */
	UFUNCTION(BlueprintCallable, Category = "Cheats")
	bool CompleteConsoleHack();

	/** Completes or skips the console and fills the alert: OnExtractionReady fires now. False when the flow has no grid or miner. */
	UFUNCTION(BlueprintCallable, Category = "Cheats")
	bool SkipToExtraction();
	//~ End cheats

	/** The landed shuttle (null before landing). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	ARRShuttle* GetShuttle() const { return Shuttle; }

	/** The arrival extractor of the level start (null before its placement and once it left the view and was removed). Not an exit. */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	ARRShuttle* GetArrivalShuttle() const;

	/**
	 * Arrival extractor (level-flow step 1, task-371-01), once per level: spawns ShuttleClass in arrival mode
	 * (ARRShuttle::InitializeArrival: no board, no countdown, not the run's shuttle, so no locator, minimap or drone use) with its
	 * footprint ArrivalDistanceCells from the miner's cell along the screen up axis (camera yaw snapped to the nearest grid axis),
	 * centred on that cell across it; the blocks of that footprint are destroyed first (ARRArenaGrid::ClearExtractorFootprint,
	 * task-372-01). Takeoff (ARRShuttle::StartTakeoff): when a level transition is running or fading in
	 * (URRRunTransitionSubsystem phase not None, or fade FadingIn), in the frame OnScreenFadeFinished(FadingIn) fires; otherwise
	 * (level 1 from the menu, direct start) on the first run flow tick, when the level plays. Called by
	 * URRRunTransitionSubsystem::PlaceMinerAtStart (next level prepared in the same world) and by the first tick that resolves the
	 * grid and the miner (new world). Returns the arrival (null without grid, miner or world; the existing one when already placed).
	 */
	ARRShuttle* PlaceArrivalShuttle();

	/**
	 * Hides or shows the arrival extractor in game (task-397-01). The next level preload places it during the cards screen
	 * (URRRunTransitionSubsystem::PlaceMinerAtStart), when the paused level is still drawn behind the intermission screens: it is
	 * hidden there and shown again by URRRunTransitionSubsystem::StartPreparedLevel, under black, before the fade in. Logs the change.
	 * Returns false when there is no arrival extractor.
	 */
	bool SetArrivalShuttleHidden(bool bHidden, const TCHAR* Reason);

	/** True once the run ended (win or loss). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool IsRunEnded() const { return bRunEnded; }

	/** True when the run ended with the miner extracted (valid once ended). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool WasRunSuccessful() const { return bRunSucceeded; }

	/**
	 * Ends the run once: cancels the miner's level-up offer (the end screen wins), pauses the game with UI input and the cursor
	 * shown (player 0's controller, when there is one) and broadcasts OnRunEnded. Refused (false) when already ended or when the
	 * miner is dead (the game over owns that end). Called when the shuttle finishes. A success during a run
	 * (URRRunTransitionSubsystem::HandleLevelExtracted) opens the stats screen instead: OnRunEnded is not broadcast, the phase is
	 * still Ended (task-206-03); on the last level the stats screen's button calls ShowRunWonEndScreen (task-317-01). A loss (shuttle
	 * missed) also shows the run-lost screen after OnRunEnded (URRRunLostSubsystem::ShowShuttleMissed, task-484).
	 */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	bool EndRun(bool bSuccess);

	/**
	 * Run won end screen after the last level's stats screen (URRRunTransitionSubsystem::NextLevel, task-317-01): broadcasts
	 * OnRunEnded(true). Refused (false) unless the run ended with a success.
	 */
	bool ShowRunWonEndScreen();

	/** Reopens the current map (end screen restart button); resets the run first (URRRunSubsystem::ResetRun), so it begins level 1. */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	void RestartLevel();

	/**
	 * Run won end screen "Main Menu" button (task-390-01, main-menu gdd), pause menu Exit and run-lost Main Menu: resets the run (URRRunSubsystem::ResetRun: level 1 next,
	 * secured ore 0), then opens URRRunSettings::MainMenuMapName directly (no loading screen, no fade). The menu controller sets
	 * its own UI input and cursor at BeginPlay. False (warning, nothing reset) without a world or with an empty map name.
	 */
	UFUNCTION(BlueprintCallable, Category = "RunFlow")
	bool ReturnToMainMenu();

	/**
	 * Next run level prepared in the same world (task-318-01): destroys the console, bonus chest and shuttle and puts the flow back
	 * to its level start (alert 0, kills 0, no objective, run not ended, carried ore 0 and open again). Config and the registered
	 * miner and spawner are kept. Nothing is broadcast (the HUD is recreated by the controller).
	 */
	void ResetForNextLevel();

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

	/** Broadcast once when the bonus chest drops. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnBonusChestDroppedSignature OnBonusChestDropped;

	/** Broadcast once when the bar is full and no objective is pending. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnExtractionReadySignature OnExtractionReady;

	/** Broadcast once when the shuttle lands. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnShuttleLandedSignature OnShuttleLanded;

	/** Broadcast every countdown frame and at landing (seconds left, shuttle location). */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnShuttleCountdownSignature OnShuttleCountdown;

	/** Broadcast once when the run ends (true = extracted). */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnRunEndedSignature OnRunEnded;

	/**
	 * Broadcast when the alert-bar icons changed: count, type, done flag, or a pending horde's estimated position moved by more
	 * than Config.AlertEventPositionTolerance. Recomputed on the run flow events, the spawner horde events and the run flow tick
	 * (the estimate moves while the bar is paused); sent only on change. Read GetAlertBarEvents when binding.
	 */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnAlertBarEventsChangedSignature OnAlertBarEventsChanged;

	/** Relay of the registered spawner's OnHordeIncoming (banner without a spawner reference); once per announcement. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnRunFlowHordeIncomingSignature OnHordeIncoming;

	/**
	 * Broadcast when the objective locator target (GetLocatorTarget) changed: console dropped, console hacked, shuttle landed, run
	 * ended. The arrow needs per-frame placement only while Target is not None.
	 */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnLocatorTargetChangedSignature OnLocatorTargetChanged;

	/** Broadcast when a miner registered (RegisterMiner, at the end of its BeginPlay: base stats applied). */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnMinerRegisteredSignature OnMinerRegistered;

	/** Broadcast when the run's spawner registered (RegisterSpawner: the first one of the level). */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow")
	FRROnSpawnerRegisteredSignature OnSpawnerRegistered;

	/** Broadcast when the carried or secured ore count changed (both counts); never when nothing changed. */
	UPROPERTY(BlueprintAssignable, Category = "RunFlow|Ore")
	FRROnOreChangedSignature OnOreChanged;

	/** Ore carried by the miner in this level: lost on a missed shuttle or death, secured on extraction. */
	UFUNCTION(BlueprintPure, Category = "RunFlow|Ore")
	int32 GetCarriedOre() const { return CarriedOre; }

	/** Ore secured this session (URROreBankSubsystem; a world-local total when the world has no initialized game instance). */
	UFUNCTION(BlueprintPure, Category = "RunFlow|Ore")
	int32 GetSecuredOre() const;

	/** Ore this level's successful extraction moved from carried to secured (0 before it and after ResetForNextLevel); recap OreSecuredThisLevel. */
	int32 GetExtractionSecuredOre() const { return ExtractionSecuredOre; }

	/**
	 * Adds Amount (> 0) to the carried ore and broadcasts OnOreChanged. Refused (false) once the run ended or the miner died.
	 * Called by ARRPickup::Collect for Ore pickups (their OreValue) and by the GrantOre cheat.
	 */
	bool AddCarriedOre(int32 Amount);

	/** Ore that can pay a purchase now: carried plus secured. */
	UFUNCTION(BlueprintPure, Category = "RunFlow|Ore")
	int32 GetSpendableOre() const { return CarriedOre + GetSecuredOre(); }

	/** True when Amount (>= 0) can be paid now (GetSpendableOre() >= Amount). */
	UFUNCTION(BlueprintPure, Category = "RunFlow|Ore")
	bool CanSpendOre(int32 Amount) const { return Amount >= 0 && GetSpendableOre() >= Amount; }

	/**
	 * Pays Amount (> 0) from the carried ore first, then the rest from the secured ore (URROreBankSubsystem, or the world-local
	 * total), and broadcasts OnOreChanged once. Refused (false, nothing spent) when Amount is not positive or exceeds
	 * GetSpendableOre. Used by the paid level-up reroll and reusable by the intermission shop.
	 */
	bool SpendOre(int32 Amount);

	/** Called by ARRMiner at the end of its BeginPlay: becomes the run's miner when none is valid, then OnMinerRegistered. */
	void RegisterMiner(ARRMiner* InMiner);

	/**
	 * Called by ARREnemySpawner at its BeginPlay: the first valid one becomes the run's spawner (horde relay and alert-bar icons),
	 * then OnSpawnerRegistered. Later spawners are ignored while it is valid.
	 */
	void RegisterSpawner(ARREnemySpawner* InSpawner);

	/** The run's miner (registered or resolved by the tick); null before. */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	ARRMiner* GetMiner() const { return Miner; }

	/** The run's enemy spawner (RegisterSpawner); null in a level without spawner. */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	ARREnemySpawner* GetSpawner() const { return Spawner.Get(); }

	/** Alert-bar icons last sent by OnAlertBarEventsChanged (computed now when never sent). */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	const TArray<FRRAlertBarEvent>& GetAlertBarEvents();

	/**
	 * Objective locator target: the console while it exists and is not hacked, else the landed shuttle; None before the drop,
	 * between the hack and the landing, and after the run end. False for None.
	 */
	UFUNCTION(BlueprintPure, Category = "RunFlow")
	bool GetLocatorTarget(ERRLocatorTarget& OutTarget, FVector& OutWorldLocation) const;

	/** Recomputes the alert-bar icons and broadcasts OnAlertBarEventsChanged when they changed (see the event). */
	void RefreshAlertBarEvents();

	/**
	 * Alert-bar icons of RunFlow (may be null: defaults) and Spawner (may be null: no horde icons): console, bonus chest (threshold
	 * > 0), one horde (the first one not Done, i.e. whose front has not finished spawning; none after the last, task-525), shuttle.
	 * InOutDoneHordePositions freezes a started horde at the alert of that moment.
	 */
	static void ComputeAlertBarEvents(const URRRunFlowSubsystem* RunFlow, const ARREnemySpawner* InSpawner, TMap<int32, float>& InOutDoneHordePositions, TArray<FRRAlertBarEvent>& OutEvents);

	/**
	 * Estimated bar position of an event SecondsUntil seconds away: Alert + SecondsUntil / AlertFillSeconds (the base fill rate,
	 * without kills or pause), clamped to [Alert, 1].
	 */
	static float EstimateBarPosition(float Alert, float SecondsUntil, float AlertFillSeconds);

protected:
	/** Extraction success: adds the carried ore to the secured total, sets carried to 0, closes the counter, broadcasts on change. */
	void SecureCarriedOre();

	/** Missed shuttle or death: sets carried to 0 (secured unchanged), closes the counter, broadcasts on change. Reason is logged. */
	void LoseCarriedOre(const TCHAR* Reason);

	/** Registered miner OnDeath callback: the carried ore is lost and the run-lost death sequence starts (URRRunLostSubsystem, task-484). */
	UFUNCTION()
	void HandleMinerDeath(AActor* DeathInstigator);

	/** Ore carried in this level. */
	int32 CarriedOre = 0;

	/** Secured total used when the world has no ore bank (bare test worlds without an initialized game instance). */
	int32 LocalSecuredOre = 0;

	/** Ore secured by this level's extraction (SecureCarriedOre); cleared by ResetForNextLevel. */
	int32 ExtractionSecuredOre = 0;

	/** True once the run ended or the miner died: the carried count no longer grows. */
	bool bCarriedOreClosed = false;

	/** Run flow work of a tick (alert fill, drops, extraction); Tick then refreshes the alert-bar icons. */
	void TickRunFlow(float DeltaTime);

	/** Computes GetLocatorTarget and broadcasts OnLocatorTargetChanged when the target or its location changed. */
	void UpdateLocatorTarget();

	/** Spawner OnHordeIncoming callback: relays OnHordeIncoming and refreshes the icons. */
	UFUNCTION()
	void HandleHordeIncoming(int32 HordeIndex, float SecondsUntil);

	/** Spawner OnHordeLull callback: refreshes the icons. */
	UFUNCTION()
	void HandleHordeLull(int32 HordeIndex);

	/** Spawner OnHordeStarted callback: freezes the horde icon at the current alert and refreshes. */
	UFUNCTION()
	void HandleHordeStarted(int32 HordeIndex, ERRSpawnSide Side);

	/** Spawner OnHordeFinished callback: refreshes the icons. */
	UFUNCTION()
	void HandleHordeFinished(int32 HordeIndex, int32 Spawned);

	/** The run's spawner (RegisterSpawner). */
	TWeakObjectPtr<ARREnemySpawner> Spawner;

	/** Alert-bar icons last sent by OnAlertBarEventsChanged. */
	UPROPERTY(Transient)
	TArray<FRRAlertBarEvent> AlertBarEvents;

	/** True once OnAlertBarEventsChanged was sent. */
	bool bAlertBarEventsSent = false;

	/** Bar position of each started horde, frozen at the alert of that moment (key: horde index). */
	TMap<int32, float> DoneHordePositions;

	/** Locator target last sent by OnLocatorTargetChanged. */
	ERRLocatorTarget LastLocatorTarget = ERRLocatorTarget::None;

	/** Locator location last sent by OnLocatorTargetChanged. */
	FVector LastLocatorLocation = FVector::ZeroVector;

	/** Finds the grid (first ARRArenaGrid) and the miner (player 0's pawn, else the first living ARRMiner). False when either is missing. */
	bool ResolveRunActors();

	/** Reads RunFlowConfig from the auth game mode (ARRGameMode) once, unless SetConfig was called. */
	void ResolveConfig();

	/** Picks the console cell (see class comment). False when no cell qualifies. */
	bool FindConsoleCell(FIntPoint& OutCell);

	/**
	 * Shared console and chest picker: a random reachable Floor cell straight-line [MinCells, MaxCells] from the miner's cell whose
	 * Defaults footprint is reachable floor (none fits: any ring cell, warning naming What); with bApplyMarkedRange and a Defaults
	 * that marks blocks, only cells whose FindMarkableCells count is in [MinMarkedCells, MaxMarkedCells] (none: the cells nearest the
	 * range, warning). False when the ring is empty.
	 */
	bool FindObjectiveCell(const ARRConsole* Defaults, float MinCells, float MaxCells, bool bApplyMarkedRange, const TCHAR* What, FIntPoint& OutCell);

	/** Spawns and initializes the bonus chest on a picked cell. Null when no cell qualifies or the spawn failed. */
	ARRBonusChest* DropBonusChest();

	/** Spawns and initializes the console on a picked cell and binds its events. Null when no cell qualifies or the spawn failed. */
	ARRConsole* DropConsole();

	/** Every Floor cell the flow field reaches from the miner (OutCells reset first). Landing footprints are checked against it. */
	static void GatherReachableFloor(URRFlowFieldSubsystem& FlowField, TSet<FIntPoint>& OutCells);

	/** Footprint (MinCell, Size) grown by Margin cells on every side and clamped to the grid: the rectangle handed to ARRArenaGrid::ClearExtractorFootprint (task-376). */
	void GetClearRect(FIntPoint MinCell, FIntPoint Size, int32 Margin, FIntPoint& OutMinCell, FIntPoint& OutSize) const;

	/** True when every cell of the Footprint (in cells, RRLandingFootprint) around Anchor is in ReachableFloor. */
	static bool FootprintFits(const TSet<FIntPoint>& ReachableFloor, FIntPoint Anchor, FIntPoint Footprint);

	/** Fires OnExtractionReady once when the bar is full and nothing is pending. */
	void CheckExtraction();

	/** Picks the shuttle cell (see class comment). False when no reachable floor cell exists. */
	bool FindShuttleCell(FIntPoint& OutCell);

	/** Clears the landing footprint (ARRArenaGrid::ClearExtractorFootprint, task-372-01), then spawns and initializes the shuttle and binds its events. Null when no cell qualifies or the spawn failed. */
	ARRShuttle* LandShuttle();

	/**
	 * Picks the Kessler-Voss line of alert event EventId and shows it as a subtitle (URRSubtitleSubsystem::ShowAlertLine) just before
	 * its event is raised; the line also stays readable with URRTextContentSubsystem::GetCurrentAlertLine. No line = no subtitle.
	 */
	void PickAlertLine(const TCHAR* EventId);

	/** Asks the subtitles for a bark of key event TriggerId (URRSubtitleSubsystem::ShowBark: cooldown, alert priority). */
	void ShowBark(const TCHAR* TriggerId);

	/** Pauses the game through player 0's controller and switches it to game-and-UI input with the cursor shown. */
	void EnterEndPause();

	/** Arrival step of each tick (run even while the flow is held): places the arrival once, then starts its takeoff when no fade is awaited. */
	void TickArrival();

	/** Starts the arrival takeoff once and logs Trigger (fade in finished, level start); stops listening to the fade. */
	void StartArrivalTakeoff(const TCHAR* Trigger);

	/** Removes the OnScreenFadeFinished binding of the arrival, if any. */
	void StopWaitingForArrivalFade();

	/** Transition OnScreenFadeFinished callback: a finished FadingIn starts the arrival takeoff. */
	UFUNCTION()
	void HandleScreenFadeFinished(ERRTransitionFade Fade);

	/** Shuttle countdown callback: relays OnShuttleCountdown. */
	UFUNCTION()
	void HandleShuttleCountdownChanged(ARRShuttle* InShuttle, float RemainingSeconds);

	/** Shuttle finished callback: ends the run. */
	UFUNCTION()
	void HandleShuttleFinished(ARRShuttle* InShuttle, bool bExtracted);

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

	/** The dropped bonus chest. */
	UPROPERTY(Transient)
	TObjectPtr<ARRBonusChest> BonusChest;

	/** The landed shuttle. */
	UPROPERTY(Transient)
	TObjectPtr<ARRShuttle> Shuttle;

	/** The arrival extractor of this level (PlaceArrivalShuttle); destroyed by its own takeoff or by ResetForNextLevel. */
	UPROPERTY(Transient)
	TObjectPtr<ARRShuttle> ArrivalShuttle;

	/** Transition whose OnScreenFadeFinished the arrival waits for (bound only while waiting). */
	TWeakObjectPtr<URRRunTransitionSubsystem> ArrivalFadeSource;

	/** True once this level's arrival was placed (or its placement failed for good). */
	bool bArrivalPlaced = false;

	/** True while the placed arrival waits for the end of the transition fade in. */
	bool bArrivalWaitsForFade = false;

	/** True once this level's arrival takeoff was started. */
	bool bArrivalTakeoffStarted = false;

	/** True when PlaceArrivalShuttle clears the blocks of the footprint (SetArrivalClearsFootprint; off in automation test worlds). */
	bool bArrivalClearsFootprint = true;

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

	/** True once the alert reached the bonus chest threshold. */
	bool bBonusChestTriggered = false;

	/** True once the "no chest cell" warning was logged. */
	bool bBonusChestFailureLogged = false;

	/** True once the run ended. */
	bool bRunEnded = false;

	/** True when the run ended with the miner extracted. */
	bool bRunSucceeded = false;

	/** True once the "no shuttle cell" warning was logged. */
	bool bShuttleFailureLogged = false;

	/** True once Config was set by SetConfig or read from the game mode. */
	bool bConfigResolved = false;

	/** True once the "no console cell" warning was logged. */
	bool bDropFailureLogged = false;

	/** Flow on/off. */
	bool bRunFlowEnabled = true;

	/** Arrival extractor placement on/off (SetArrivalEnabled). */
	bool bArrivalEnabled = true;
};
