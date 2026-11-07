// Rogue Rigs - asynchronous layout loader (task-493-01): reads a layout JSON on a worker thread, loads the grid Blueprint and its
// assets asynchronously, spawns the grid and builds it over several frames, with a 0-100 load percentage and a completion event.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tasks/Task.h"
#include "RRLayoutLoaderSubsystem.generated.h"

class ARRArenaGrid;
struct FRRLayoutData;

/** Phase of a layout load. */
UENUM(BlueprintType)
enum class ERRLayoutLoadPhase : uint8
{
	/** No load started yet. */
	Idle,
	/** Layout file read on a worker thread and assets loading asynchronously, in parallel. */
	Loading,
	/** Grid spawned, its instances built over frames. */
	Building,
	/** Last load finished: grid complete, percentage 100. */
	Done,
	/** Last load failed (file, layout or grid class); see the log. */
	Failed
};

/** Fired once per load: at 100 % with the built grid, or on failure with bSuccess false and a null grid. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRROnLayoutLoadCompleted, ARRArenaGrid*, Grid, bool, bSuccess);

/** Measures of the last load, for logs and tests. */
struct ROGUERIGS_API FRRLayoutLoadStats
{
	/** Frames ticked from the start to the end of the load. */
	int32 Frames = 0;
	/** Longest frame (real time between two loader ticks) during the load, ms. */
	double MaxFrameMs = 0.0;
	/** Longest game-thread time the loader itself took in one frame, ms (start call included). */
	double MaxStepMs = 0.0;
	/** Real time from StartLoad to completion, s. */
	double TotalSeconds = 0.0;
	/** Assets requested from the streamable manager, and how many were already in memory at the request. */
	int32 AssetsRequested = 0;
	int32 AssetsAlreadyLoaded = 0;
	/** Build units of the grid (2 per chunk + 6). */
	int32 BuildUnits = 0;
	/** Cells per type in the file (index = ERRCellType) and read back from the built grid (a cell turned into ore by an ore table deposit counts as its file type, rock). */
	TArray<int32> FileCounts;
	TArray<int32> GridCounts;
	/** True when GridCounts equals FileCounts. */
	bool bCountsMatch = false;
};

/**
 * Loads a level from a layout JSON without a blocking load (task-493-01, Q4: only the test map uses it; the game maps keep the
 * grid's own synchronous path). StartLoad launches, in parallel, the file read and parse on a worker thread (UE::Tasks) and an
 * asynchronous load of the grid Blueprint (URRLayoutLoaderSettings::GridClass) and every asset it references (meshes,
 * materials, textures, pickup classes; hard package dependencies from the asset registry, else the class alone). When both are
 * done it spawns the grid with SetExternalBuild and builds it with ARRArenaGrid::StepIncrementalBuild under BuildBudgetMs per
 * frame. LoadPercent (0 to 100, never decreasing during a load) weighs the three phases with the settings weights;
 * OnLoadCompleted fires at 100. One load at a time.
 */
UCLASS()
class ROGUERIGS_API URRLayoutLoaderSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** The loader of the world of WorldContext, or null. */
	static URRLayoutLoaderSubsystem* Get(const UObject* WorldContext);

	/**
	 * Starts loading LayoutPath and building its grid at GridTransform. LayoutPath: absolute path, path relative to the project
	 * folder (e.g. Saved/GeneratedLevels/arena_gen.json), or a bare layout name (Content/RogueRigs/Data/Levels/<name>.json).
	 * Returns false (nothing started, logged) while another load runs or when the grid class is not set.
	 */
	UFUNCTION(BlueprintCallable, Category = "Layout Loader")
	bool StartLoad(const FString& LayoutPath, FTransform GridTransform);

	/** Load percentage, 0 to 100 (100 once the grid is complete; stays at its value after a failure). */
	UFUNCTION(BlueprintPure, Category = "Layout Loader")
	float GetLoadPercent() const { return LoadPercent; }

	/** True from StartLoad to completion or failure. */
	UFUNCTION(BlueprintPure, Category = "Layout Loader")
	bool IsLoading() const { return Phase == ERRLayoutLoadPhase::Loading || Phase == ERRLayoutLoadPhase::Building; }

	/** Current phase. */
	UFUNCTION(BlueprintPure, Category = "Layout Loader")
	ERRLayoutLoadPhase GetPhase() const { return Phase; }

	/** Grid of the current or last load (null before the spawn and after a failure). */
	UFUNCTION(BlueprintPure, Category = "Layout Loader")
	ARRArenaGrid* GetLoadedGrid() const { return LoadedGrid; }

	/** Measures of the current or last load. */
	const FRRLayoutLoadStats& GetLastLoadStats() const { return Stats; }

	/** Absolute path of the current or last layout file. */
	const FString& GetLayoutFilePath() const { return LayoutFilePath; }

	/** Advances the load by one frame of DeltaSeconds (called by Tick; tests call it directly). */
	void TickLoad(float DeltaSeconds);

	/** Load percentage 0 to 100, readable from Blueprint (same value as GetLoadPercent). */
	UPROPERTY(BlueprintReadOnly, Category = "Layout Loader")
	float LoadPercent = 0.f;

	/** Fired once per load, at 100 % (bSuccess true) or on failure. */
	UPROPERTY(BlueprintAssignable, Category = "Layout Loader")
	FRROnLayoutLoadCompleted OnLoadCompleted;

	// UTickableWorldSubsystem
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual void Deinitialize() override;

	/** Resolves LayoutPath as StartLoad does (absolute, project-relative, or bare layout name) to an absolute file path. */
	static FString ResolveLayoutPath(const FString& LayoutPath);

private:
	/** Gathers the soft paths of the grid class and its hard package dependencies (asset registry), for the async request. */
	void CollectGridAssets(const FSoftObjectPath& GridClassPath, TArray<FSoftObjectPath>& OutPaths) const;

	/** Spawns the grid and begins its incremental build; false on failure (logged). */
	bool SpawnAndBeginBuild();

	/** Recomputes LoadPercent from the phase fractions (never lower than its previous value during a load). */
	void UpdatePercent(float ParseFraction, float AssetFraction, float BuildFraction);

	/** Ends the load: counts read back, summary log, release of the asset handle, OnLoadCompleted. */
	void Finish(bool bSuccess, const FString& Reason);

	/** Fills Stats.FileCounts, GridCounts and bCountsMatch. */
	void ReadBackCounts();

	/** Phase of the current or last load. */
	ERRLayoutLoadPhase Phase = ERRLayoutLoadPhase::Idle;

	/** Worker-thread read and parse of the layout file. */
	UE::Tasks::TTask<TSharedPtr<FRRLayoutData>> ParseTask;

	/** Parsed layout, once ParseTask is done. */
	TSharedPtr<FRRLayoutData> Layout;

	/** Streamable manager of the loader, and the handle of the current request (kept until the grid is built, so nothing is collected). */
	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> AssetHandle;

	/** Grid class requested by the current load. */
	TSoftClassPtr<ARRArenaGrid> RequestedGridClass;

	/** Grid of the current or last load. */
	UPROPERTY(Transient)
	TObjectPtr<ARRArenaGrid> LoadedGrid;

	/** Where the grid is spawned. */
	FTransform PendingTransform;

	/** Absolute path of the layout file. */
	FString LayoutFilePath;

	/** Measures of the current or last load. */
	FRRLayoutLoadStats Stats;

	/** Real time of StartLoad and of the previous loader tick. */
	double StartSeconds = 0.0;
	double LastTickSeconds = 0.0;

	/** Integer percentage last logged when per-frame logging is off. */
	int32 LastLoggedDecile = -1;
};
