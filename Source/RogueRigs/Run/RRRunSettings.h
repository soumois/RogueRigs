// Rogue Rigs - run layout list: the layout file of each level of a run, C++ defaults overridable in DefaultGame.ini.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RRRunSettings.generated.h"

/**
 * Data of a run (run-structure): the layout played on each level, read with GetDefault<URRRunSettings>() by URRRunSubsystem.
 * Not a UDeveloperSettings (the module does not link DeveloperSettings, same choice as URRIconSettings). Override without code
 * in Config/DefaultGame.ini, section [/Script/RogueRigs.RRRunSettings] (an array value replaces the whole default list).
 */
UCLASS(Config = Game, DefaultConfig)
class ROGUERIGS_API URRRunSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Fills the default layout list (level 1 arena_m1_fine, then arena_l2, arena_l3, arena_l4). */
	URRRunSettings();

	/**
	 * Layout file names without extension (Content/RogueRigs/Data/Levels/<Name>.json), index 0 = level 1. The number of
	 * entries is the number of levels of a run (4). A run started on a map uses that map's grid LayoutName for level 1.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run")
	TArray<FString> LevelLayouts;

	/**
	 * Map opened by the intermission Continue for levels 2+ (package name or short name, e.g. L_ArenaM1); empty = the map the
	 * intermission was shown on (the arena grid then builds from the prepared layout).
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Transition")
	FString ArenaMapName;

	/**
	 * Safety limit of the loading hold after the map travel (task-206-03): when the pool prewarm has not reported its end
	 * within this many seconds of real time, the level is released anyway, with a warning. 0 = no limit. Provisional value.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Transition")
	float LoadingHoldTimeoutSeconds = 10.f;
};
