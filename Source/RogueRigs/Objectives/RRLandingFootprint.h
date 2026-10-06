// Rogue Rigs - rectangular landing footprint of the console and the shuttle: the grid cells a mesh covers around its anchor cell.

#pragma once

#include "CoreMinimal.h"

class ARRArenaGrid;

/**
 * Footprint helpers shared by ARRConsole, ARRShuttle and URRRunFlowSubsystem. A footprint of Size cells (X along the grid X
 * axis, Y along the grid Y axis, actor rotation zero) covers X from Anchor.X - (Size.X - 1) / 2 to that + Size.X - 1, the same
 * on Y: an odd size is centred on the anchor cell, an even size extends one more cell towards +X / +Y. The actor stands at the
 * centre of that rectangle (a cell corner for an even size).
 */
namespace RRLandingFootprint
{
	/** Size clamped to at least 1 x 1 cell. */
	ROGUERIGS_API FIntPoint ClampSize(FIntPoint Size);

	/** Lowest (min X, min Y) cell of the footprint of Size around Anchor. */
	ROGUERIGS_API FIntPoint GetMinCell(FIntPoint Anchor, FIntPoint Size);

	/** Every cell of the footprint of Size around Anchor (appended to OutCells, which is reset first). */
	ROGUERIGS_API void GetCells(FIntPoint Anchor, FIntPoint Size, TArray<FIntPoint>& OutCells);

	/** World position of the footprint centre on Grid (midpoint of its first and last cell centres, floor top). */
	ROGUERIGS_API FVector GetCenter(const ARRArenaGrid* Grid, FIntPoint Anchor, FIntPoint Size);

	/** True when IsOpen(Cell) holds for every cell of the footprint of Size around Anchor. */
	ROGUERIGS_API bool Fits(FIntPoint Anchor, FIntPoint Size, TFunctionRef<bool(FIntPoint Cell)> IsOpen);
}
