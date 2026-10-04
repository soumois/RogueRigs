// Rogue Rigs - world lookups shared by every system: the arena grid and the miner of a world.

#pragma once

#include "CoreMinimal.h"

class ARRArenaGrid;
class ARRMiner;
class UWorld;

/**
 * World lookups (sys-world-queries). Every system that needs "the" arena grid or "the" miner goes through these, so a run
 * that swaps grids or miners changes one place. They iterate the world's actors (no cache): callers that query every frame
 * keep their own weak cache and call these only when it is empty or stale.
 */
namespace RRWorld
{
	/** First ARRArenaGrid of World (actor iteration order); null without a world or a grid. */
	ROGUERIGS_API ARRArenaGrid* FindArenaGrid(const UWorld* World);

	/**
	 * First ARRMiner of World that is valid and not being destroyed; when bLivingOnly, it must also not be dead (State.Dead).
	 * Null without a world or a matching miner.
	 */
	ROGUERIGS_API ARRMiner* FindMiner(const UWorld* World, bool bLivingOnly);

	/** First living ARRMiner of World (FindMiner with bLivingOnly): the target of robots, the flow field goal, the run's miner. */
	inline ARRMiner* FindLivingMiner(const UWorld* World) { return FindMiner(World, /*bLivingOnly*/ true); }
}
