// Rogue Rigs - level-to-level transition of a run: intermission after an extraction, Continue, map travel and the loading hold
// until the next level is ready (task-206-03).

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RRRunTransitionSubsystem.generated.h"

class ARREnemySpawner;
class ARRMiner;
class URRRunFlowSubsystem;
class URRRunSubsystem;

/** Phase of the transition between two run levels. */
UENUM(BlueprintType)
enum class ERRRunTransitionPhase : uint8
{
	/** No transition: a level is being played (or no run). */
	None,
	/** Level extracted (not the last one): game paused, recap shown, next layout prepared in the background. */
	Intermission,
	/** Continue pressed: loading screen up until the next level is ready (layout, map, grid, snapshot, pool prewarm). */
	Loading
};

/** Step of the loading phase, for the loading screen text. */
UENUM(BlueprintType)
enum class ERRRunLoadingStage : uint8
{
	/** Not loading. */
	None,
	/** The next layout is still being built off the game thread; the map opens on OnLayoutReady. */
	WaitingForLayout,
	/** The map travel is requested (the old world is still shown until the engine loads the new map). */
	OpeningMap,
	/** New map loaded, grid built: gameplay held while the pool prewarm runs. */
	PreparingLevel
};

/** Recap of a finished level shown by the intermission (intermission gdd: level recap, run total). */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRIntermissionRecap
{
	GENERATED_BODY()

	/** Level just extracted, 1-based. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 FinishedLevelIndex = 0;

	/** Level that Continue starts (FinishedLevelIndex + 1). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 NextLevelIndex = 0;

	/** Levels of a run (4). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 LevelCount = 0;

	/** Ore secured by the extraction of this level. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 OreSecuredThisLevel = 0;

	/** Ore secured since the run started (this level included). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 RunSecuredOre = 0;

	/** Layout of the next level (already requested). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	FString NextLayoutName;
};

/** Fired when the intermission starts (game paused, next layout requested): the recap to show. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnIntermissionStartedSignature, const FRRIntermissionRecap&, Recap);

/** Fired when Continue is accepted: the loading screen must show now; level and layout to load. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLoadingStartedSignature, int32, LevelIndex, const FString&, LayoutName);

/** Fired when the loading stage changes (loading screen text). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnLoadingStageChangedSignature, ERRRunLoadingStage, Stage);

/** Fired when the new level is ready (layout used, grid built, snapshot applied, prewarm done): hide the loading screen, play. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLevelReadySignature, int32, LevelIndex, const FString&, LayoutName);

/**
 * Transition between the levels of a run (intermission, run-structure). Game instance subsystem, so it survives the OpenLevel.
 *
 * Flow: URRRunFlowSubsystem::EndRun(true) (shuttle boarded; ore secured, game paused with UI input) asks
 * HandleLevelExtracted. Run in progress and not its last level: CaptureLevelEndSnapshot, AdvanceToNextLevel (the next layout is
 * requested at once), phase Intermission, OnIntermissionStarted(Recap); the run flow then does NOT broadcast OnRunEnded, so the
 * success end screen stays hidden. Last level: the run is won (IsRunWon), the run flow shows its end screen as before.
 * Continue (intermission button, rr.Run.Continue): phase Loading, OnLoadingStarted; next layout still pending: stage
 * WaitingForLayout until URRRunSubsystem::OnLayoutReady; then OpenLevel (stage OpeningMap) on URRRunSettings::ArenaMapName or
 * the current map. After the map load (FCoreUObjectDelegates::PostLoadMapWithWorld: every BeginPlay done, so the grid is
 * built from the prepared layout and the snapshot is re-applied, before the first tick) the new world is held (stage
 * PreparingLevel): run flow disabled (no alert, no console), spawner timed spawning held, player gameplay input ignored
 * (ARRPlayerController). The hold ends on the spawner's OnPoolPrewarmFinished (or at once without a spawner or prewarm, or after
 * LoadingHoldTimeoutSeconds): run flow and spawner released, phase None, OnLevelReady.
 *
 * Widgets: ARRPlayerController creates IntermissionWidgetClass on OnIntermissionStarted and LoadingScreenWidgetClass on
 * OnLoadingStarted (old world) and again at its BeginPlay while IsLevelLoading (new world); a widget does not survive the travel.
 * Direct PIE or -game on a map: no transition, level 1 starts as before.
 */
UCLASS()
class ROGUERIGS_API URRRunTransitionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Listens to the map loads. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Removes the map-load listener and any ticker. */
	virtual void Deinitialize() override;

	/** The transition subsystem of WorldContext's game instance; null without an initialized game instance (bare test worlds). */
	static URRRunTransitionSubsystem* Get(const UObject* WorldContext);

	/**
	 * Called by URRRunFlowSubsystem::EndRun(true) after the ore is secured and the game paused. Run in progress, not the last
	 * level: starts the intermission (snapshot, advance, OnIntermissionStarted) and returns true (the caller skips its success
	 * end screen). Last level: marks the run won and returns false. No run: false.
	 */
	bool HandleLevelExtracted(ARRMiner* Miner);

	/**
	 * Intermission Continue button: starts the loading (OnLoadingStarted), waits for the next layout if it is still pending, then
	 * opens the arena map. False outside the intermission.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run|Transition")
	bool Continue();

	/** Current phase. */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	ERRRunTransitionPhase GetPhase() const { return Phase; }

	/** True during the intermission (between the extraction and Continue). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsInIntermission() const { return Phase == ERRRunTransitionPhase::Intermission; }

	/** True from Continue until OnLevelReady (the loading screen must be shown, gameplay is held). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsLevelLoading() const { return Phase == ERRRunTransitionPhase::Loading; }

	/** Current loading step (None outside the loading phase). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	ERRRunLoadingStage GetLoadingStage() const { return LoadingStage; }

	/** Recap of the last intermission (FinishedLevelIndex 0 before the first). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	const FRRIntermissionRecap& GetIntermissionRecap() const { return Recap; }

	/** Level being loaded (the run's current level), 0 outside the loading phase. */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	int32 GetLoadingLevelIndex() const { return IsLevelLoading() ? LoadingLevelIndex : 0; }

	/** True once the last level of the run was extracted, until the run is reset (end screen wording). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsRunWon() const { return bRunWon; }

	/** See FRROnIntermissionStartedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnIntermissionStartedSignature OnIntermissionStarted;

	/** See FRROnLoadingStartedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnLoadingStartedSignature OnLoadingStarted;

	/** See FRROnLoadingStageChangedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnLoadingStageChangedSignature OnLoadingStageChanged;

	/** See FRROnLevelReadySignature. Also the end of the loading (OnLoadingFinished). */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnLevelReadySignature OnLevelReady;

protected:
	/** Sets the loading stage and broadcasts OnLoadingStageChanged when it changed. */
	void SetLoadingStage(ERRRunLoadingStage NewStage);

	/** Run subsystem OnLayoutReady callback while waiting for the next layout: opens the map. */
	UFUNCTION()
	void HandleLayoutReady(int32 LevelIndex, const FString& LayoutName, bool bSuccess);

	/** Run subsystem OnRunReset callback: drops any transition. */
	UFUNCTION()
	void HandleRunReset();

	/** Requests the travel to the arena map (stage OpeningMap). */
	void OpenArenaMap();

	/** PostLoadMapWithWorld callback: holds the new world during the loading phase and waits for the prewarm. */
	void HandlePostLoadMap(UWorld* LoadedWorld);

	/** Spawner OnPoolPrewarmFinished callback: the level is ready. */
	UFUNCTION()
	void HandlePoolPrewarmFinished(ARREnemySpawner* Spawner);

	/** Ends the hold of the loaded world: run flow and spawner released, phase None, OnLevelReady. Reason is logged. */
	void FinishLoading(const TCHAR* Reason);

	/** Clears the phase, the hold and the listeners without any broadcast (run reset, deinitialize). */
	void ClearTransition();

	/** The run subsystem of this game instance. */
	URRRunSubsystem* GetRun() const;

	/** Current phase. */
	ERRRunTransitionPhase Phase = ERRRunTransitionPhase::None;

	/** Current loading step. */
	ERRRunLoadingStage LoadingStage = ERRRunLoadingStage::None;

	/** Recap of the last intermission. */
	UPROPERTY(Transient)
	FRRIntermissionRecap Recap;

	/** Level being loaded. */
	int32 LoadingLevelIndex = 0;

	/** Layout of the level being loaded. */
	FString LoadingLayoutName;

	/** Map that Continue opens (captured at the intermission start). */
	FString MapToOpen;

	/** World of the intermission (the travel is requested from it). */
	TWeakObjectPtr<UWorld> IntermissionWorld;

	/** World being held after the travel. */
	TWeakObjectPtr<UWorld> HeldWorld;

	/** Run flow of the held world and its enabled state before the hold. */
	TWeakObjectPtr<URRRunFlowSubsystem> HeldRunFlow;
	bool bHeldRunFlowWasEnabled = true;

	/** Spawner of the held world (its timed spawning is held). */
	TWeakObjectPtr<ARREnemySpawner> HeldSpawner;

	/** True once the last level was extracted (until the run reset). */
	bool bRunWon = false;

	/** FPlatformTime::Seconds at Continue and at the map load, for the logs. */
	double LoadingStartSeconds = 0.0;
	double MapLoadedSeconds = 0.0;

	/** PostLoadMapWithWorld registration. */
	FDelegateHandle PostLoadMapHandle;

	/** Core ticker of the hold timeout (valid while held). */
	FTSTicker::FDelegateHandle HoldTimeoutHandle;
};
