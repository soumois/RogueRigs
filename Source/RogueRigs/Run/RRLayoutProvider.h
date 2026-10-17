// Rogue Rigs - layout provider slot (task-206-02): builds the layout of a run level off the game thread; today from the
// layout JSON written by the Level-Design agent, later from the deterministic C++ generator (arena-grid).

#pragma once

#include "CoreMinimal.h"

struct FRRLayoutData;

/** What a layout provider is asked for: one run level. The generator adds its inputs (seed...) here when it exists. */
struct ROGUERIGS_API FRRLayoutRequest
{
	/** Run level the layout is for, 1-based. */
	int32 LevelIndex = 0;

	/** Layout name of the level (URRRunSettings::LevelLayouts entry, or the name given to RequestLayoutByName). */
	FString LayoutName;
};

/**
 * Source of level layouts. URRRunSubsystem calls BuildLayout on a worker thread (UE::Tasks), so an implementation must be
 * thread-safe: no UObject access, no game-thread state, only the request and data captured at construction. The returned
 * layout is invalid (FRRLayoutData::Error set) on failure. The future deterministic C++ generator implements this interface
 * and is installed with URRRunSubsystem::SetLayoutProvider.
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

/** Today's provider: reads Content/RogueRigs/Data/Levels/<LayoutName>.json (level-layout-schema) with the grid's parser. */
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
