// Rogue Rigs - world lookups shared by every system: the arena grid and the miner of a world.

#include "World/RRWorld.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"

ARRArenaGrid* RRWorld::FindArenaGrid(const UWorld* World)
{
	if (!World)
	{
		return nullptr;
	}
	TActorIterator<ARRArenaGrid> It(World);
	return It ? *It : nullptr;
}

ARRMiner* RRWorld::FindMiner(const UWorld* World, bool bLivingOnly)
{
	if (!World)
	{
		return nullptr;
	}
	for (TActorIterator<ARRMiner> It(World); It; ++It)
	{
		if (IsValid(*It) && !It->IsActorBeingDestroyed() && (!bLivingOnly || !It->IsDead()))
		{
			return *It;
		}
	}
	return nullptr;
}
