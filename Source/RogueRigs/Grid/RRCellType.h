// Rogue Rigs - cell type of the arena grid and of a layout file, shared by the grid and the layout data (task-206-02).

#pragma once

#include "CoreMinimal.h"
#include "RRCellType.generated.h"

/** Content of one grid cell, as read from the layout file. Unlisted cells are Floor. */
UENUM(BlueprintType)
enum class ERRCellType : uint8
{
	/** Open floor: floor instance, no block. */
	Floor,
	/** Outside the asteroid: no floor, no block. */
	Void,
	/** Rock block standing on floor. */
	Rock,
	/** Ore block standing on floor. */
	Ore,
	/** Healing ore block standing on floor. */
	HealOre,
	/** Special ore block standing on floor. */
	SpecialOre,
	/** Barrel block standing on floor. */
	Barrel,
	/** The miner's start cell: open floor. */
	Spawn
};
