// Rogue Rigs - run state across the 4 levels of a run: level index, layout list, miner snapshot carried over a map travel.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Grid/RRLayoutData.h"
#include "Run/RRRunSnapshot.h"
#include "Run/RRLayoutProvider.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tasks/Task.h"
#include "RRRunSubsystem.generated.h"

class ARRMiner;

/** Broadcast when a run starts (BeginRun): level index (1) and its layout name. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnRunStartedSignature, int32, LevelIndex, const FString&, LayoutName);

/** Broadcast after CaptureLevelEndSnapshot stored a snapshot: the snapshot (recap data for the intermission). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnLevelSnapshotCapturedSignature, const FRRRunMinerSnapshot&, Snapshot);

/** Broadcast by AdvanceToNextLevel: the new level index and the layout to prepare and open (the async layout provider starts here). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLevelAdvancedSignature, int32, NewLevelIndex, const FString&, LayoutName);

/** Broadcast after the pending snapshot was re-applied to the miner of the new level. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnSnapshotAppliedSignature, ARRMiner*, Miner, int32, LevelIndex);

/**
 * Broadcast on the game thread when a requested layout is prepared (RequestLayout): its level, its name and whether it is
 * valid (false: file missing or parse error, the grid falls back to its own LayoutName).
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnLayoutReadySignature, int32, LevelIndex, const FString&, LayoutName, bool, bSuccess);

/** Broadcast by ResetRun (run lost or won, restart): no run in progress any more. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnRunResetSignature);

/**
 * State of the current run (run-structure, intermission): level index 1..N (N = URRRunSettings::LevelLayouts, 4), the layout of
 * each level, and the miner snapshot carried from one level to the next. Lives on the game instance, so it survives the
 * OpenLevel between two levels. No travel happens here: URRRunTransitionSubsystem (task-206-03) calls the entry points.
 *
 * Sequence: the miner calls HandleMinerBeginPlay at the end of its BeginPlay. No run in progress (PIE or -game started on a
 * map): BeginRun with the grid's LayoutName, level 1, nothing applied. Extraction success: the flow calls
 * CaptureLevelEndSnapshot (after URRRunFlowSubsystem::EndRun(true) secured the carried ore), then AdvanceToNextLevel, then
 * opens the map; the new miner's HandleMinerBeginPlay replays the snapshot once (ApplySnapshotToMiner). Loss or run won:
 * ResetRun, so the next map starts a new run at level 1.
 *
 * Layouts (task-206-02, task-496): AdvanceToNextLevel requests the layout of the new level at once (RequestLayout). The layout
 * provider (FRRGeneratorLayoutProvider: C++ generator, level seed derived from the run seed, hand JSON as fallback) builds it on a
 * worker thread; OnLayoutReady fires on the game thread and GetPreparedLayout returns it until the next request. It lives here,
 * so it survives the map travel, and ARRArenaGrid builds from it. Level 1 is requested by PrepareNewRunLayout (ResetRun, so New
 * Run, Retry and Main Menu) before the run begins; without that preparation (map played directly) the grid reads its own LayoutName.
 *
 * Ore: carried as secured ore. The extraction moves the carried ore into URROreBankSubsystem (game instance), which keeps it
 * across the travel; every level starts with 0 carried ore. The snapshot only records the recap figures: the ore secured by
 * this level's extraction and the bank balance held after it (task-391-04).
 */
UCLASS()
class ROGUERIGS_API URRRunSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Installs the JSON layout provider. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Waits for a layout request still in flight (its worker only holds the provider and the request). */
	virtual void Deinitialize() override;

	/** The run subsystem of WorldContext's game instance; null without an initialized game instance (bare test worlds). */
	static URRRunSubsystem* Get(const UObject* WorldContext);

	/**
	 * Starts a new run at level 1: clears any snapshot, records the secured ore at the run start. Level1LayoutName overrides
	 * the settings entry of level 1 for this run (the grid's own layout when a map is played directly); empty = settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void BeginRun(const FString& Level1LayoutName);

	/**
	 * Stores the level-end snapshot of Miner (cards, weapons, level, XP, health, ore recap) and marks it pending for the next
	 * level. Call after URRRunFlowSubsystem::EndRun(true). False (nothing stored) without a run in progress or a miner.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run")
	bool CaptureLevelEndSnapshot(ARRMiner* Miner);

	/**
	 * Moves to the next level (index + 1) and broadcasts OnLevelAdvanced with its layout; the pending snapshot is applied to
	 * the next miner that begins play. False on the last level or without a run in progress. Does not open the map.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run")
	bool AdvanceToNextLevel();

	/**
	 * Starts preparing the layout of run level InLevelIndex (its GetLayoutNameForLevel name) off the game thread; replaces any
	 * earlier request (an in-flight result is discarded). OnLayoutReady fires on the game thread when done. False when the
	 * level has no layout name.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run|Layout")
	bool RequestLayout(int32 InLevelIndex);

	/** RequestLayout with an explicit layout name for InLevelIndex (debug, or a layout outside the settings list). False for an empty name. */
	UFUNCTION(BlueprintCallable, Category = "Run|Layout")
	bool RequestLayoutByName(int32 InLevelIndex, const FString& InLayoutName);

	/** True when the last request was for InLevelIndex and its layout is prepared and valid. */
	UFUNCTION(BlueprintPure, Category = "Run|Layout")
	bool IsLayoutReady(int32 InLevelIndex) const;

	/** True while the request for InLevelIndex is still being built. */
	UFUNCTION(BlueprintPure, Category = "Run|Layout")
	bool IsLayoutPending(int32 InLevelIndex) const;

	/** Prepared valid layout of InLevelIndex; null when not requested, still pending or failed. */
	TSharedPtr<const FRRLayoutData> GetPreparedLayout(int32 InLevelIndex) const;

	/**
	 * GetPreparedLayout, but a request for InLevelIndex still in flight is first waited for on the calling (game) thread and
	 * delivered at once (OnLayoutReady fires). For a build that cannot wait for OnLayoutReady (the grid at BeginPlay).
	 */
	TSharedPtr<const FRRLayoutData> WaitForPreparedLayout(int32 InLevelIndex);

	/**
	 * Replaces the layout provider; null restores the default one (FRRGeneratorLayoutProvider, or FRRJsonLayoutProvider when
	 * URRRunSettings::bGenerateRunLayouts is false). Takes effect at the next request.
	 */
	void SetLayoutProvider(TSharedPtr<IRRLayoutProvider> InProvider);

	/**
	 * Prepares the next run (task-496): draws a new run seed and requests the level 1 layout off the game thread. The next
	 * BeginRun keeps that seed and that request, and the level 1 grid builds from it. Refused (false) while a run is in progress.
	 * ResetRun calls it unless a preparation is already waiting.
	 */
	bool PrepareNewRunLayout();

	/** True from PrepareNewRunLayout until the next BeginRun: a run seed and a level 1 request wait for the next run. */
	bool HasNewRunLayout() const { return bNewRunPrepared && RequestedLevelIndex == 1; }

	/** True when the last layout request is for InLevelIndex (pending, ready or failed). */
	bool IsLayoutRequested(int32 InLevelIndex) const { return InLevelIndex > 0 && RequestedLevelIndex == InLevelIndex; }

	/** Seed of the current (or prepared) run; every level seed derives from it. Log only, never shown to the player (task-496 Q4). */
	int32 GetRunSeed() const { return RunSeed; }

	/** Seed of level InLevelIndex of the run seed InRunSeed: the first seed the generator tries for that level. */
	static int32 DeriveLevelSeed(int32 InRunSeed, int32 InLevelIndex);

	/** Ends the run state (run lost, run won, restart, main menu New Run): no run in progress, snapshot cleared, secured ore bank cleared (ore lasts one run); the next map begins level 1. */
	UFUNCTION(BlueprintCallable, Category = "Run")
	void ResetRun();

	/**
	 * Called by ARRMiner at the end of its BeginPlay: begins a run when none is in progress (grid LayoutName as level 1),
	 * otherwise applies the pending snapshot once. Other miners of the same world are ignored.
	 */
	void HandleMinerBeginPlay(ARRMiner* Miner);

	/**
	 * Re-applies Snapshot to Miner: level and XP (no level-up), picked cards replayed in order through
	 * URRUpgradeComponent::ApplyUpgrade (a NewWeapon card adds its weapon, a weapon card finds its weapon by class), owned
	 * weapons missing after the replay added, then Health set (at most MaxHealth). Returns false without a valid snapshot.
	 */
	bool ApplySnapshotToMiner(ARRMiner* Miner, const FRRRunMinerSnapshot& Snapshot);

	/**
	 * Next run level started in the same world (task-318-01): the extracted miner itself carries the state (cards, weapons, level,
	 * XP, health; ore in the bank), so the pending snapshot is consumed without a replay. Logs the miner values next to the snapshot
	 * values (same line shape as ApplySnapshotToMiner) and broadcasts OnSnapshotApplied. False without a pending snapshot or miner.
	 */
	bool ConsumeSnapshotInPlace(ARRMiner* Miner);

	/**
	 * Adds Card (applied to Miner after the capture, e.g. bought in the intermission shop) to the end of the pending snapshot's
	 * picked cards, so the next level's miner replays it; refreshes the snapshot Health and MaxHealth from Miner (a MaxHealth
	 * card heals). False (nothing recorded) without a run, a pending snapshot or a miner. The recap ore figures are unchanged.
	 */
	bool AppendCardToPendingSnapshot(ARRMiner* Miner, const FRRUpgradeCard& Card);

	/** True between BeginRun and ResetRun. */
	UFUNCTION(BlueprintPure, Category = "Run")
	bool IsRunInProgress() const { return bRunInProgress; }

	/** Current level of the run, 1-based; 0 without a run in progress. */
	UFUNCTION(BlueprintPure, Category = "Run")
	int32 GetLevelIndex() const { return LevelIndex; }

	/** Number of levels of a run (URRRunSettings::LevelLayouts entries, at least 1). */
	UFUNCTION(BlueprintPure, Category = "Run")
	int32 GetLevelCount() const;

	/** True when the current level is the last of the run (its extraction wins the run). */
	UFUNCTION(BlueprintPure, Category = "Run")
	bool IsLastLevel() const { return bRunInProgress && LevelIndex >= GetLevelCount(); }

	/** Layout of level InLevelIndex (1-based): the level-1 override of this run for level 1, else the settings entry; empty out of range. */
	UFUNCTION(BlueprintPure, Category = "Run")
	FString GetLayoutNameForLevel(int32 InLevelIndex) const;

	/** Layout of the current level (GetLayoutNameForLevel(GetLevelIndex())). */
	UFUNCTION(BlueprintPure, Category = "Run")
	FString GetCurrentLayoutName() const { return GetLayoutNameForLevel(LevelIndex); }

	/** Last captured snapshot (bValid false before the first capture of the run); the intermission recap reads it. */
	UFUNCTION(BlueprintPure, Category = "Run")
	const FRRRunMinerSnapshot& GetLastSnapshot() const { return LastSnapshot; }

	/** True while a captured snapshot waits for the next level's miner. */
	UFUNCTION(BlueprintPure, Category = "Run")
	bool HasPendingSnapshot() const { return bSnapshotPending; }

	/** See FRROnRunStartedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnRunStartedSignature OnRunStarted;

	/** See FRROnLevelSnapshotCapturedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnLevelSnapshotCapturedSignature OnLevelSnapshotCaptured;

	/** See FRROnLevelAdvancedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnLevelAdvancedSignature OnLevelAdvanced;

	/** See FRROnSnapshotAppliedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnSnapshotAppliedSignature OnSnapshotApplied;

	/** See FRROnLayoutReadySignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Layout")
	FRROnLayoutReadySignature OnLayoutReady;

	/** See FRROnRunResetSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnRunResetSignature OnRunReset;

protected:
	/** Game-thread end of request Serial: takes the task result, stores it, logs and broadcasts OnLayoutReady. Ignored for a stale or delivered serial. */
	void CompleteLayoutRequest(uint32 Serial);

	/** Forgets the current request and its layout (an in-flight result is discarded). */
	void ClearLayoutRequest();

	/** Secured ore balance of the session bank, spending subtracted (0 without a bank). */
	int32 GetBankSecuredOre() const;

	/** True between BeginRun and ResetRun. */
	bool bRunInProgress = false;

	/** Current level, 1-based; 0 without a run. */
	int32 LevelIndex = 0;

	/** Level-1 layout of this run (grid LayoutName of the map the run started on); empty = settings entry. */
	FString Level1LayoutOverride;

	/**
	 * Last captured snapshot. A UPROPERTY so its Blueprint weapon classes stay referenced: without it the map travel lets the
	 * garbage collector unload them and the classes read as None in the next level. It holds no actor pointer.
	 */
	UPROPERTY()
	FRRRunMinerSnapshot LastSnapshot;

	/** True from CaptureLevelEndSnapshot until a miner of a later world received the snapshot. */
	bool bSnapshotPending = false;

	/** World whose miner already went through HandleMinerBeginPlay (one miner per world is handled). */
	TWeakObjectPtr<UWorld> HandledWorld;

	/** World the snapshot was captured in: a pending snapshot is applied only in another world (after the travel). */
	TWeakObjectPtr<UWorld> CaptureWorld;

	/** Builds the requested layouts on a worker thread; FRRJsonLayoutProvider unless SetLayoutProvider replaced it. */
	TSharedPtr<IRRLayoutProvider> LayoutProvider;

	/** Level of the last request (0: none). */
	int32 RequestedLevelIndex = 0;

	/** Layout name of the last request. */
	FString RequestedLayoutName;

	/** Id of the last request; a worker result with another serial is stale and dropped. */
	uint32 RequestSerial = 0;

	/** True from the request until its result is delivered on the game thread. */
	bool bLayoutPending = false;

	/** Worker task of the last request (valid while pending). */
	UE::Tasks::TTask<TSharedPtr<FRRLayoutData>> LayoutTask;

	/** Result of the last delivered request (may be invalid: Error set). */
	TSharedPtr<const FRRLayoutData> PreparedLayout;

	/** FPlatformTime::Seconds at the last request, for the ready log. */
	double RequestStartSeconds = 0.0;

	/** Seed of the current or prepared run (0 before the first one). */
	int32 RunSeed = 0;

	/** True from PrepareNewRunLayout until BeginRun. */
	bool bNewRunPrepared = false;

	/** Core ticker measuring the game-thread frames while a request is pending (task-496 A2: the generation must not stall them). */
	FTSTicker::FDelegateHandle PendingFrameTicker;

	/** Longest game-thread frame (ms) and frame count since the last request, logged with its result. */
	double LongestPendingFrameMs = 0.0;
	int32 PendingFrames = 0;
};
