// Rogue Rigs - layout provider slot (task-206-02): builds the layout of a run level off the game thread. Since task-496 the run
// uses the deterministic C++ generator (FRRGeneratorLayoutProvider, sys-layout-generator), the hand layout JSON as fallback.

#pragma once

#include "CoreMinimal.h"

struct FRRLayoutData;

/** What a layout provider is asked for: one run level. */
struct ROGUERIGS_API FRRLayoutRequest
{
	/** Run level the layout is for, 1-based. */
	int32 LevelIndex = 0;

	/** Layout name of the level (URRRunSettings::LevelLayouts entry, or the name given to RequestLayoutByName): the hand layout. */
	FString LayoutName;

	/** Seed of the run (URRRunSubsystem::GetRunSeed), for the logs. */
	int32 RunSeed = 0;

	/** Seed of this level, derived from the run seed and the level index: the first seed the generator tries. */
	int32 LevelSeed = 0;

	/** Debug (rr.Layout.ForceGenFail): the generator is skipped as if no seed passed, so the fallback runs. */
	bool bForceGeneratorFailure = false;
};

/**
 * Source of level layouts. URRRunSubsystem calls BuildLayout on a worker thread (UE::Tasks), so an implementation must be
 * thread-safe: no UObject access, no game-thread state, only the request and data captured at construction. The returned
 * layout is invalid (FRRLayoutData::Error set) on failure. Installed with URRRunSubsystem::SetLayoutProvider.
 */
class ROGUERIGS_API IRRLayoutProvider
{
public:
	virtual ~IRRLayoutProvider() = default;

	/** Builds the layout of Request. Called off the game thread; never null. */
	virtual TSharedRef<FRRLayoutData> BuildLayout(const FRRLayoutRequest& Request) const = 0;

	/** Short provider name for the logs ("json", "generator"). */
	virtual FString GetProviderName() const = 0;
};

/** Hand layouts: reads Content/RogueRigs/Data/Levels/<LayoutName>.json (level-layout-schema) with the grid's parser. */
class ROGUERIGS_API FRRJsonLayoutProvider : public IRRLayoutProvider
{
public:
	/** Captures the layout folder on the calling (game) thread, so BuildLayout never touches path state off it. */
	FRRJsonLayoutProvider();

	/** Reads and parses LevelsDirectory/<Request.LayoutName>.json. */
	virtual TSharedRef<FRRLayoutData> BuildLayout(const FRRLayoutRequest& Request) const override;

	/** "json". */
	virtual FString GetProviderName() const override { return TEXT("json"); }

protected:
	/** Absolute folder of the layout files, resolved at construction. */
	FString LevelsDirectory;
};

/**
 * Run layouts from the C++ generator (task-496): FRRLayoutGenerator::Generate from Request.LevelSeed with the params and brief
 * files of the level (URRRunSettings::LevelGeneratorParams / LevelGeneratorBriefs, files in Content/RogueRigs/Data/Levels, staged
 * with the game). The layout is built in memory, no file is written; it keeps the level's LayoutName so the grid and the
 * transition see the same name as before. When the files are missing or invalid, or no seed passes, it logs an error and returns
 * the hand layout <LayoutName>.json (FRRJsonLayoutProvider).
 */
class ROGUERIGS_API FRRGeneratorLayoutProvider : public IRRLayoutProvider
{
public:
	/** Captures the layout folder and the per-level params and brief file names from URRRunSettings (game thread). */
	FRRGeneratorLayoutProvider();

	/** Generates the level, or the hand layout on failure (error logged). */
	virtual TSharedRef<FRRLayoutData> BuildLayout(const FRRLayoutRequest& Request) const override;

	/** "generator". */
	virtual FString GetProviderName() const override { return TEXT("generator"); }

protected:
	/** Generates Request into OutLayout; false with OutError on any failure (OutLayout untouched then). */
	bool Generate(const FRRLayoutRequest& Request, FRRLayoutData& OutLayout, FString& OutError) const;

	/** Absolute folder of the layout, params and brief files, resolved at construction. */
	FString LevelsDirectory;

	/** Params file name of each level (index 0 = level 1). */
	TArray<FString> ParamsFiles;

	/** Brief file name of each level (index 0 = level 1); empty = no brief bounds. */
	TArray<FString> BriefFiles;

	/** The hand layout fallback. */
	FRRJsonLayoutProvider Fallback;
};
