// Rogue Rigs - run layout list: the layout file of each level of a run, C++ defaults overridable in DefaultGame.ini.

#include "Run/RRRunSettings.h"

URRRunSettings::URRRunSettings()
{
	// Level 1 stays arena_m1_fine (owner 2026-10-17); levels 2 to 4 are pre-written layouts until the C++ generator exists.
	LevelLayouts = { TEXT("arena_m1_fine"), TEXT("arena_l2"), TEXT("arena_l3"), TEXT("arena_l4") };
}
