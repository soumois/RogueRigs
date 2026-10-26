// Rogue Rigs - level-to-level transition of a run: stats screen after an extraction, then the cards shop screen (task-317-01),
// Continue, map travel and the loading hold until the next level is ready (task-206-03).

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RRRunTransitionSubsystem.generated.h"

class ARREnemySpawner;
class ARRMiner;
class URRIntermissionShopSubsystem;
class URRRunFlowSubsystem;
class URRRunSubsystem;

/** Phase of the transition between two run levels. */
UENUM(BlueprintType)
enum class ERRRunTransitionPhase : uint8
{
	/** No transition: a level is being played (or no run). */
	None,
	/**
	 * Level extracted: game paused, stats screen (recap) shown. Levels 1-3: next layout prepared in the background, NextLevel opens
	 * the shop phase. Last level: run won, NextLevel shows the run won end screen (task-317-01).
	 */
	Stats,
	/** NextLevel pressed on the stats screen (levels 1-3): cards shop screen open; Continue starts the loading. */
	Shop,
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

/** Robots of one type destroyed during the finished level (stats screen line, task-317-01). */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRecapRobotKills
{
	GENERATED_BODY()

	/** Run-log robot type key (enemy stats row name, or the class name without a row). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	FName RobotType;

	/** Name shown on the stats screen (the robot type as text for now; no icon). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	FText DisplayName;

	/** Robots of this type destroyed during the level. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 Count = 0;
};

/** Damage dealt by one weapon during the finished level (stats screen line, task-317-01). */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRecapWeaponDamage
{
	GENERATED_BODY()

	/** Run-log weapon key (weapon class name). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	FName WeaponClassName;

	/** Weapon name shown on the stats screen (WeaponStats.csv DisplayName through ARRWeapon::GetClassDisplayName; the key when not found). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	FText DisplayName;

	/** Damage dealt by this weapon during the level (turret hits credited to their launcher). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	float Damage = 0.f;
};

/** Recap of a finished level shown by the stats screen (intermission gdd: level recap, run total). */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRIntermissionRecap
{
	GENERATED_BODY()

	/** True on the last level's stats screen: the run is won, NextLevel shows the run won end screen (no shop). */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	bool bRunWon = false;

	/** Level just extracted, 1-based. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 FinishedLevelIndex = 0;

	/** Level that Continue starts (FinishedLevelIndex + 1); 0 when the run is won. */
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

	/** Layout of the next level (already requested); empty when the run is won. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	FString NextLayoutName;

	/** Robots destroyed during the level, per robot type (run log KillsByRobotType), most first. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	TArray<FRRRecapRobotKills> KillsByRobotType;

	/** Robots destroyed during the level, all types. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	int32 TotalKills = 0;

	/** Damage dealt during the level, per weapon (run log DamageDealtByWeapon), most first. */
	UPROPERTY(BlueprintReadOnly, Category = "Run|Transition")
	TArray<FRRRecapWeaponDamage> DamageByWeapon;
};

/** Fired when the stats screen starts (level extracted, game paused; levels 1-3: next layout requested): the recap to show. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnIntermissionStartedSignature, const FRRIntermissionRecap&, Recap);

/** Fired when NextLevel opens the cards shop screen (levels 1-3; shop offer already drawn). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnShopScreenStartedSignature);

/** Fired when NextLevel closes the last level's stats screen: the run won end screen follows (OnRunEnded(true)). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRROnRunWonScreenStartedSignature);

/** Fired when Continue is accepted: the loading screen must show now; level and layout to load. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLoadingStartedSignature, int32, LevelIndex, const FString&, LayoutName);

/** Fired when the loading stage changes (loading screen text). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnLoadingStageChangedSignature, ERRRunLoadingStage, Stage);

/** Fired when the new level is ready (layout used, grid built, snapshot applied, prewarm done): hide the loading screen, play. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLevelReadySignature, int32, LevelIndex, const FString&, LayoutName);

/**
 * Fired when the next level has been fully prepared in the background in the current world (task-318-01): layout, terrain and
 * flow field rebuilt, actors and objectives reset, miner placed, pool warm. Continue then starts it without a map load.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnNextLevelReadySignature, int32, LevelIndex, const FString&, LayoutName);

/** Screen fade of a level transition (task-318-02): black over the whole viewport, transition widgets included. */
UENUM(BlueprintType)
enum class ERRTransitionFade : uint8
{
	/** No fade: nothing drawn. */
	None,
	/** Continue with the next level ready: the cards screen fades to black (URRRunSettings::TransitionFadeOutSeconds). */
	FadingOut,
	/** Level started: black fades away over the new level (URRRunSettings::TransitionFadeInSeconds). */
	FadingIn
};

/** Fired when a screen fade starts (task-318-02): which fade and its duration in seconds (real time, runs while paused). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnScreenFadeStartedSignature, ERRTransitionFade, Fade, float, Duration);

/** Fired when a screen fade ends (FadingOut: the level switch follows in the same frame; FadingIn: screen clear). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnScreenFadeFinishedSignature, ERRTransitionFade, Fade);

/** Fired when the next level preparation advances (task-318-02): progress 0-1 (GetNextLevelProgress), for the loading bar. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnNextLevelProgressChangedSignature, float, Progress);

/** Step of the in-place preparation of the next level (task-318-01), in order; Ready is the last. */
UENUM(BlueprintType)
enum class ERRNextLevelPrepStep : uint8
{
	/** No preparation running or done. */
	None,
	/** Waiting for the next layout (built off the game thread since the extraction). */
	WaitingForLayout,
	/** Releasing robots, pickups, projectiles, turrets and FX; objectives, run flow, spawner and explosions reset. */
	ClearingLevel,
	/** Grid rebuilt from the prepared layout. */
	RebuildingTerrain,
	/** Miner and drone placed at the player start, flow field recomputed on the new terrain. */
	PlacingMiner,
	/** Everything prepared: Continue starts the level at once. */
	Ready
};

/**
 * Transition between the levels of a run (intermission, run-structure). Game instance subsystem, so it survives the OpenLevel.
 *
 * Flow: URRRunFlowSubsystem::EndRun(true) (shuttle boarded; ore secured, game paused with UI input) asks
 * HandleLevelExtracted. Run in progress: the recap (ore, kills per robot type, damage per weapon from URRRunLogSubsystem) is
 * built, phase Stats, OnIntermissionStarted(Recap); the run flow then does NOT broadcast OnRunEnded. Not the last level:
 * CaptureLevelEndSnapshot and AdvanceToNextLevel (the next layout is requested at once) first; NextLevel (stats button,
 * rr.Run.Stats.Next) opens the shop (URRIntermissionShopSubsystem::OpenShop), phase Shop, OnShopScreenStarted. Last level: the
 * run is won (IsRunWon, Recap.bRunWon); NextLevel broadcasts OnRunWonScreenStarted, phase None, and asks the run flow for its run
 * won end screen (URRRunFlowSubsystem::ShowRunWonEndScreen: OnRunEnded(true)); no shop (task-317-01).
 * Continue (shop screen button, rr.Run.Continue; refused outside the Shop phase): phase Loading, OnLoadingStarted; next layout still pending: stage
 * WaitingForLayout until URRRunSubsystem::OnLayoutReady; then OpenLevel (stage OpeningMap) on URRRunSettings::ArenaMapName or
 * the current map. After the map load (FCoreUObjectDelegates::PostLoadMapWithWorld: every BeginPlay done, so the grid is
 * built from the prepared layout and the snapshot is re-applied, before the first tick) the new world is held (stage
 * PreparingLevel): run flow disabled (no alert, no console), spawner timed spawning held, player gameplay input ignored
 * (ARRPlayerController). The hold ends on the spawner's OnPoolPrewarmFinished (or at once without a spawner or prewarm, or after
 * LoadingHoldTimeoutSeconds): run flow and spawner released, phase None, OnLevelReady.
 *
 * Widgets: ARRPlayerController creates IntermissionWidgetClass (stats screen) on OnIntermissionStarted, ShopScreenWidgetClass on
 * OnShopScreenStarted and LoadingScreenWidgetClass on
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
	 * Called by URRRunFlowSubsystem::EndRun(true) after the run log level entry, the ore secured and the game paused. Run in
	 * progress: starts the stats phase (levels 1-3: snapshot and advance first; last level: run won) with OnIntermissionStarted and
	 * returns true (the caller skips its success end screen). No run (direct PIE, test worlds) or a failed snapshot: false.
	 */
	bool HandleLevelExtracted(ARRMiner* Miner);

	/**
	 * Stats screen Next level button. Levels 1-3: opens the shop (offer drawn), phase Shop, OnShopScreenStarted. Last level (run
	 * won): OnRunWonScreenStarted, phase None, then the run flow's run won end screen (OnRunEnded(true)). False outside the Stats phase.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run|Transition")
	bool NextLevel();

	/**
	 * Shop screen Continue button: closes the shop, phase Loading. Next level prepared in this world (task-318-02): ready = fade
	 * out (no loading screen, no OnLoadingStarted), switch, fade in; still preparing = OnLoadingStarted (loading screen with
	 * GetLoadingProgress), then switch and fade in when ready. No preparation: OnLoadingStarted, waits for the next layout if it
	 * is still pending, then opens the arena map (fade in at OnLevelReady). False outside the Shop phase.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run|Transition")
	bool Continue();

	/** Current phase. */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	ERRRunTransitionPhase GetPhase() const { return Phase; }

	/** True between the extraction and Continue (stats screen or shop screen). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsInIntermission() const { return Phase == ERRRunTransitionPhase::Stats || Phase == ERRRunTransitionPhase::Shop; }

	/** True while the stats screen is up (phase Stats). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsInStatsScreen() const { return Phase == ERRRunTransitionPhase::Stats; }

	/** True while the cards shop screen is up (phase Shop). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsInShopScreen() const { return Phase == ERRRunTransitionPhase::Shop; }

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

	/**
	 * Called by URRLoadingScreenWidget on its first paint. While the travel waits (stage WaitingForLayout or OpeningMap, map not
	 * requested yet), records the frame (GFrameCounter) of the first loading screen paint; ignored otherwise (new-world widget).
	 */
	void NotifyLoadingScreenPainted(const UObject* Painter);

	/** True once the last level of the run was extracted, until the run is reset (end screen wording). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsRunWon() const { return bRunWon; }

	/**
	 * True once the next level is fully prepared in the current world (task-318-01, from the cards screen opening): Continue starts
	 * it at once, without a map load. False otherwise (not prepared yet, no transition, or the map-travel fallback).
	 */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	bool IsNextLevelReady() const { return PrepStep == ERRNextLevelPrepStep::Ready; }

	/**
	 * Progress 0-1 of the next level preparation: completed steps / steps (layout wait, clearing, terrain, placement); 1 when ready.
	 * 0 when no preparation runs.
	 */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	float GetNextLevelProgress() const;

	/** Current step of the next level preparation (None when none runs). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	ERRNextLevelPrepStep GetNextLevelPrepStep() const { return PrepStep; }

	/**
	 * Progress 0-1 for the loading screen bar (task-318-02): GetNextLevelProgress while the next level is prepared in this world;
	 * 0 on the map-travel fallback (no measurable progress) and outside a loading.
	 */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	float GetLoadingProgress() const;

	/** Current screen fade (None when no fade runs). */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	ERRTransitionFade GetScreenFade() const { return ScreenFade; }

	/** Black opacity 0-1 of the screen fade now (FadingOut 0 to 1, FadingIn 1 to 0, linear in real time); 0 without a fade. */
	UFUNCTION(BlueprintPure, Category = "Run|Transition")
	float GetScreenFadeAlpha() const;

	/** See FRROnScreenFadeStartedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnScreenFadeStartedSignature OnScreenFadeStarted;

	/** See FRROnScreenFadeFinishedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnScreenFadeFinishedSignature OnScreenFadeFinished;

	/** See FRROnNextLevelProgressChangedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnNextLevelProgressChangedSignature OnNextLevelProgressChanged;

	/** See FRROnNextLevelReadySignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnNextLevelReadySignature OnNextLevelReady;

	/** See FRROnIntermissionStartedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnIntermissionStartedSignature OnIntermissionStarted;

	/** See FRROnShopScreenStartedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnShopScreenStartedSignature OnShopScreenStarted;

	/** See FRROnRunWonScreenStartedSignature. */
	UPROPERTY(BlueprintAssignable, Category = "Run|Transition")
	FRROnRunWonScreenStartedSignature OnRunWonScreenStarted;

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

	/** Fills Recap's kills per robot type and damage per weapon from the run log's current level counters (before the next level resets them). */
	void FillCombatRecap(const UObject* WorldContext);

	/** Run subsystem OnLayoutReady callback while waiting for the next layout: opens the map. */
	UFUNCTION()
	void HandleLayoutReady(int32 LevelIndex, const FString& LayoutName, bool bSuccess);

	/** Run subsystem OnRunReset callback: drops any transition. */
	UFUNCTION()
	void HandleRunReset();

	/**
	 * Stage OpeningMap, then requests the travel once the old-world loading screen has been painted in an earlier frame
	 * (task-208-01), or after URRRunSettings::LoadingPaintWaitMaxFrames frames without a paint (no widget class, -nullrhi).
	 * Waits on the core ticker, which also runs while the game is paused.
	 */
	void OpenArenaMap();

	/** Core ticker callback of the paint wait: travels when ready; returns false once the travel is requested. */
	bool TickTravelWait(float DeltaTime);

	/** True when the wait is over: painted in a frame before the current one, or the frame limit reached. */
	bool IsTravelWaitOver() const;

	/** Requests the travel (UGameplayStatics::OpenLevel) and logs the loading start, first paint and travel frames. */
	void RequestArenaMapTravel();

	/** PostLoadMapWithWorld callback: holds the new world during the loading phase and waits for the prewarm. */
	void HandlePostLoadMap(UWorld* LoadedWorld);

	/** Spawner OnPoolPrewarmFinished callback: the level is ready. */
	UFUNCTION()
	void HandlePoolPrewarmFinished(ARREnemySpawner* Spawner);

	/** Ends the hold of the loaded world: run flow and spawner released, phase None, OnLevelReady. Reason is logged. */
	void FinishLoading(const TCHAR* Reason);

	/** Clears the phase, the hold and the listeners without any broadcast (run reset, deinitialize). */
	void ClearTransition();

	/**
	 * Starts the in-place preparation of the next level in IntermissionWorld (cards screen opening): one step per core ticker
	 * frame (the world is paused). False when it cannot run (no world, grid or miner): Continue then uses the map travel.
	 */
	bool StartNextLevelPreparation();

	/** Core ticker callback: runs the next preparation step; false once ready or dropped. */
	bool TickNextLevelPreparation(float DeltaTime);

	/** Step ClearingLevel: releases or destroys the level actors and resets the run flow, spawner and explosions. Returns actors removed. */
	int32 ClearLevelActors(UWorld& World);

	/** Step PlacingMiner: miner (and drone) moved to the player start, velocity cleared; flow field recomputed. */
	void PlaceMinerAtStart(UWorld& World);

	/** Stops the preparation ticker and forgets the preparation (no broadcast). */
	void StopNextLevelPreparation();

	/**
	 * Starts the prepared level in the same world (Continue when ready, or preparation finished after Continue): snapshot kept in
	 * place, run log level start, level-up choices allowed again, game unpaused with game input, phase None, OnLevelReady.
	 */
	void StartPreparedLevel();

	/** Sets the preparation step and broadcasts OnNextLevelProgressChanged (logged while the loading screen waits for it). */
	void SetPrepStep(ERRNextLevelPrepStep NewStep);

	/**
	 * Starts a screen fade of Duration seconds (real time, core ticker: runs while paused) and broadcasts OnScreenFadeStarted; a
	 * duration of 0 or less ends it at once. FadingOut ends in StartPreparedLevel, FadingIn in the screen cleared.
	 */
	void StartScreenFade(ERRTransitionFade Fade, float Duration);

	/** End of the current fade: OnScreenFadeFinished, then the switch (FadingOut) or nothing more (FadingIn). */
	void FinishScreenFade();

	/** Stops the fade ticker and forgets the fade (no broadcast). */
	void StopScreenFade();

	/** Current screen fade. */
	ERRTransitionFade ScreenFade = ERRTransitionFade::None;

	/** FPlatformTime::Seconds at the fade start and its duration. */
	double ScreenFadeStartSeconds = 0.0;
	float ScreenFadeDuration = 0.f;

	/** Core ticker ending the fade (valid while it runs). */
	FTSTicker::FDelegateHandle ScreenFadeHandle;

	/** Current preparation step. */
	ERRNextLevelPrepStep PrepStep = ERRNextLevelPrepStep::None;

	/** True when Continue was pressed before the preparation finished: the level starts as soon as it is ready. */
	bool bStartWhenPrepared = false;

	/** FPlatformTime::Seconds when the preparation started and when it was ready (logs). */
	double PrepStartSeconds = 0.0;
	double PrepReadySeconds = 0.0;

	/** Core ticker of the preparation (valid while it runs). */
	FTSTicker::FDelegateHandle PrepTickHandle;

	/** The run subsystem of this game instance. */
	URRRunSubsystem* GetRun() const;

	/** The intermission shop of this game instance (opened with the intermission, closed on Continue or a reset). */
	URRIntermissionShopSubsystem* GetShop() const;

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

	/** Miner extracted at the stats phase start (the shop draws its offer for it on NextLevel). */
	TWeakObjectPtr<ARRMiner> IntermissionMiner;

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

	/** GFrameCounter at Continue, at the first old-world loading screen paint (0 = none yet) and when the paint wait began. */
	uint64 LoadingStartFrame = 0;
	uint64 LoadingFirstPaintFrame = 0;
	uint64 TravelWaitStartFrame = 0;

	/** True once OpenLevel was requested for the current loading (later paints belong to the new world). */
	bool bTravelRequested = false;

	/** Core ticker of the paint wait before the travel (valid while waiting). */
	FTSTicker::FDelegateHandle TravelWaitHandle;

	/** Core ticker of the hold timeout (valid while held). */
	FTSTicker::FDelegateHandle HoldTimeoutHandle;
};
