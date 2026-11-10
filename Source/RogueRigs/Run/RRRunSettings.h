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
	 * Run terrains from the C++ layout generator (task-496): true = FRRGeneratorLayoutProvider (the LevelLayouts files are the
	 * fallback of a failed generation), false = the LevelLayouts hand files only. Read when the run subsystem starts.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Generator")
	bool bGenerateRunLayouts = true;

	/**
	 * Generator params file of each level (index 0 = level 1), in Content/RogueRigs/Data/Levels (staged with the game, never
	 * Tools/). Owner decision task-496 Q3: levels 2-4 keep the params stored beside their layouts, level 1 the default params.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Generator")
	TArray<FString> LevelGeneratorParams;

	/** Generator brief file of each level (index 0 = level 1), same folder; an empty entry = recipe checks only. */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Generator")
	TArray<FString> LevelGeneratorBriefs;

	/**
	 * Map opened by the intermission Continue for levels 2+ (package name or short name, e.g. L_ArenaM1); empty = the map the
	 * intermission was shown on (the arena grid then builds from the prepared layout).
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Transition")
	FString ArenaMapName;

	/**
	 * Map opened by the main menu New Run (task-338-01, main-menu gdd: L_ArenaM1 at level 1), package name or short name. Read by
	 * URRMainMenuWidget::StartNewRunInWorld after URRRunSubsystem::ResetRun.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Menu")
	FString NewRunMapName = TEXT("/Game/RogueRigs/Maps/L_ArenaM1");

	/**
	 * Map opened by the run won end screen "Main Menu" button (task-390-01, main-menu gdd: run won returns to the menu), package
	 * name or short name. Read by URRRunFlowSubsystem::ReturnToMainMenu after URRRunSubsystem::ResetRun.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Menu")
	FString MainMenuMapName = TEXT("/Game/RogueRigs/Maps/L_MainMenu");

	/**
	 * Safety limit of the loading hold after the map travel (task-206-03): when the pool prewarm has not reported its end
	 * within this many seconds of real time, the level is released anyway, with a warning. 0 = no limit. Provisional value.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Transition")
	float LoadingHoldTimeoutSeconds = 10.f;

	/**
	 * Most frames the travel waits for the first paint of the old-world loading screen (task-208-01) before opening the map
	 * anyway (no LoadingScreenWidgetClass, -nullrhi, widget never painted). Provisional value.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Transition", meta = (ClampMin = "0"))
	int32 LoadingPaintWaitMaxFrames = 5;

	/**
	 * Continue with the next level already prepared (task-318-02, intermission gdd): seconds of the fade to black over the cards
	 * screen before the switch. Real time. 0 = cut. Provisional value (owner 2026-10-26).
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Transition", meta = (ClampMin = "0"))
	float TransitionFadeOutSeconds = 0.3f;

	/**
	 * Seconds of the fade from black to the new level once it started (after the fade out or the loading screen). Real time.
	 * 0 = cut. Provisional value (owner 2026-10-26).
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Transition", meta = (ClampMin = "0"))
	float TransitionFadeInSeconds = 0.5f;

	/**
	 * Death sequence (death-screen gdd Q1, task-484): global time dilation while the miner's death plays before the run-lost
	 * screen (0.3 = the game runs at 30 % speed). Provisional value (owner 2026-11-07).
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Death", meta = (ClampMin = "0.01", ClampMax = "1"))
	float DeathTimeDilation = 0.3f;

	/**
	 * Death sequence: seconds of real time between the lethal hit and the run-lost screen (the game paused then). 0 = the screen
	 * comes at the next tick. Provisional value (owner 2026-11-07).
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Run|Death", meta = (ClampMin = "0"))
	float DeathSequenceRealSeconds = 1.f;
};
