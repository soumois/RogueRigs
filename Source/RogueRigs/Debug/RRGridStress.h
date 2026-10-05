// Rogue Rigs - test-only grid granularity tools: subdivide the arena grid at runtime and benchmark its per-cell systems (task-023-01).

#pragma once

#include "CoreMinimal.h"

class ARRArenaGrid;
class UWorld;

/**
 * Stress tooling for the cell size question (vault: sys-stress, stress-grid-cells). Nothing here runs unless a cheat command
 * calls it; the shipped grid, layout and defaults never change.
 * - rr.Stress.Subdivide <Factor> [NoNav]: splits every cell of the arena grid into Factor x Factor cells of CellSize / Factor (same
 *   world size, same block area), rebuilds the instances and the flow field. 1 = rebuild at the original size (baseline). NoNav is kept for
 *   old command lines and has no effect since task-029-01 (grid components are never navigation relevant).
 * - rr.Stress.GridBench [Iterations] [Quit]: logs "GridBench:" lines (instance counts, memory, flow-field BFS ms, spawn
 *   candidate scan ms, line of sight us per ray, explosion terrain destruction ms), then exits when Quit is 1.
 */
namespace RRGridStress
{
	/**
	 * Splits every cell of Grid into Factor x Factor cells (CellSize / Factor). Each sub cell keeps the type of its cell, except a
	 * spawn cell (only the middle sub cell is Spawn, the others Floor) and a barrel (only the middle sub cell is Barrel, the others
	 * Rock, so one barrel stays one explosion). Rebuilds the instances (health, maps, marks reset) and forces a flow-field
	 * computation. bDisableNavigation: no effect since task-029-01 (the grid
	 * never affects navigation); kept for old command lines. Returns false when Grid is null, empty or Factor < 1.
	 */
	ROGUERIGS_API bool SubdivideGrid(ARRArenaGrid* Grid, int32 Factor, bool bDisableNavigation = false);

	/** Forces the flow field of World to recompute now (bypasses its once-per-frame guard). Returns the BFS duration in ms, -1 without one. */
	ROGUERIGS_API float ForceFlowFieldCompute(UWorld* World);

	/** Seed of every random draw of the grid stress tools, so all cell sizes test the same world points. */
	constexpr int32 BenchSeed = 23;

	/**
	 * Draws up to Count world points in the ring [MinRange, MaxRange] around Center whose cell holds a block. The same seed gives
	 * the same points at every cell size (subdivision keeps the block area).
	 */
	ROGUERIGS_API TArray<FVector> DrawBlockPoints(const ARRArenaGrid* Grid, const FVector& Center, int32 Count, float MinRange, float MaxRange, int32 Seed);

	/** Runs the grid micro-benchmarks on World and logs them (see the namespace comment). Destroys terrain: run it in its own launch. */
	ROGUERIGS_API void RunBench(UWorld* World, int32 Iterations);
}
