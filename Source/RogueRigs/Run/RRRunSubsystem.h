// Rogue Rigs - run state across the 4 levels of a run: level index, layout list, miner snapshot carried over a map travel.

#pragma once

#include "CoreMinimal.h"
#include "Run/RRRunSnapshot.h"
#include "Subsystems/GameInstanceSubsystem.h"
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

/** Broadcast by ResetRun (run lost or won, restart): no run in progress any more. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnRunResetSignature);

/**
 * State of the current run (run-structure, intermission): level index 1..N (N = URRRunSettings::LevelLayouts, 4), the layout of
 * each level, and the miner snapshot carried from one level to the next. Lives on the game instance, so it survives the
 * OpenLevel between two levels. No travel happens here: the transition flow (task-206-03) calls the entry points.
 *
 * Sequence: the miner calls HandleMinerBeginPlay at the end of its BeginPlay. No run in progress (PIE or -game started on a
 * map): BeginRun with the grid's LayoutName, level 1, nothing applied. Extraction success: the flow calls
 * CaptureLevelEndSnapshot (after URRRunFlowSubsystem::EndRun(true) secured the carried ore), then AdvanceToNextLevel, then
 * opens the map; the new miner's HandleMinerBeginPlay replays the snapshot once (ApplySnapshotToMiner). Loss or run won:
 * ResetRun, so the next map starts a new run at level 1.
 *
 * Ore: carried as secured ore. The extraction moves the carried ore into URROreBankSubsystem (game instance), which keeps it
 * across the travel; every level starts with 0 carried ore. The snapshot only records the recap figures.
 */
UCLASS()
class ROGUERIGS_API URRRunSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
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

	/** Ends the run state (run lost, run won, restart): no run in progress, snapshot cleared; the next map begins level 1. */
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

	/** Ore secured since the run started (bank total minus the total at BeginRun). */
	UFUNCTION(BlueprintPure, Category = "Run")
	int32 GetRunSecuredOre() const;

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

	/** See FRROnRunResetSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run")
	FRROnRunResetSignature OnRunReset;

protected:
	/** Secured ore total of the session bank (0 without a bank). */
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

	/** Bank total at BeginRun. */
	int32 SecuredOreAtRunStart = 0;

	/** Bank total at the start of the current level. */
	int32 SecuredOreAtLevelStart = 0;
};
