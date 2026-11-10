// Rogue Rigs - run layout list: the layout file of each level of a run, C++ defaults overridable in DefaultGame.ini.

#include "Run/RRRunSettings.h"

URRRunSettings::URRRunSettings()
{
	// Hand layouts (level 1 arena_m1_fine, owner 2026-10-17): since task-496 the fallback of a failed generation.
	LevelLayouts = { TEXT("arena_m1_fine"), TEXT("arena_l2"), TEXT("arena_l3"), TEXT("arena_l4") };
	// Generator data of each level (task-496 Q3). Level 1: the default params (copy of Tools/layout_gen/params/fine_default.json)
	// with the arena_m1_fine brief bounds, its heal_ore and barrel counts set to those of the default params (task-491).
	LevelGeneratorParams = { TEXT("arena_m1_fine.params.json"), TEXT("arena_l2.params.json"), TEXT("arena_l3.params.json"), TEXT("arena_l4.params.json") };
	LevelGeneratorBriefs = { TEXT("arena_m1_fine.gen.brief.json"), TEXT("arena_l2.brief.json"), TEXT("arena_l3.brief.json"), TEXT("arena_l4.brief.json") };
}
