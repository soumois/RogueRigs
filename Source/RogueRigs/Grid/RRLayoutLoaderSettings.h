// Rogue Rigs - settings of the asynchronous layout loader (task-493-01): grid class, build budget per frame, phase weights of the percentage.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "RRLayoutLoaderSettings.generated.h"

class ARRArenaGrid;

/**
 * Data of URRLayoutLoaderSubsystem, read with GetDefault<URRLayoutLoaderSettings>(). Not a UDeveloperSettings (same choice as
 * URRRunSettings). Override without code in Config/DefaultGame.ini, section [/Script/RogueRigs.RRLayoutLoaderSettings].
 * Values provisional (task-493-01).
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRLayoutLoaderSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Grid Blueprint the loader loads asynchronously (with every asset it references) and spawns. */
	UPROPERTY(Config, EditAnywhere, Category = "Layout Loader")
	TSoftClassPtr<ARRArenaGrid> GridClass = TSoftClassPtr<ARRArenaGrid>(FSoftObjectPath(TEXT("/Game/RogueRigs/Blueprints/Terrain/BP_ArenaGrid.BP_ArenaGrid_C")));

	/** Game-thread time the grid build may take per frame, in milliseconds (at least one build unit runs per frame). */
	UPROPERTY(Config, EditAnywhere, Category = "Layout Loader", meta = (ClampMin = "0.1", Units = "ms"))
	float BuildBudgetMs = 5.f;

	/** Share of the percentage given to reading and parsing the layout file (worker thread). */
	UPROPERTY(Config, EditAnywhere, Category = "Layout Loader|Percentage", meta = (ClampMin = "0.0"))
	float ParseWeight = 0.05f;

	/** Share of the percentage given to the asynchronous asset loading. */
	UPROPERTY(Config, EditAnywhere, Category = "Layout Loader|Percentage", meta = (ClampMin = "0.0"))
	float AssetWeight = 0.35f;

	/** Share of the percentage given to the grid build over frames. */
	UPROPERTY(Config, EditAnywhere, Category = "Layout Loader|Percentage", meta = (ClampMin = "0.0"))
	float BuildWeight = 0.6f;

	/** Logs one line per frame during a load (percentage, phase, frame time, loader time). */
	UPROPERTY(Config, EditAnywhere, Category = "Layout Loader")
	bool bLogEachFrame = true;
};
