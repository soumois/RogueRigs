// Rogue Rigs - flow field data and algorithms (BFS, incremental relaxation) without any UObject or grid access, so the same code
// runs on the game thread (time-sliced fallback) and on a worker thread over a private copy (task-066-01).

#pragma once

#include "CoreMinimal.h"

/**
 * One flow field: walkability, distances and directions of every cell, row-major, plus the relaxation heap of its incremental
 * updates. Reads and writes only its own arrays: a copy can be computed on a worker thread while the game thread reads another.
 * Rules (see URRFlowFieldSubsystem): 8-neighbour, a diagonal only when both orthogonal cells are walkable, every step costs 1;
 * next cell = lowest distance, then closest to the goal in a straight line, then neighbour order.
 */
struct ROGUERIGS_API FRRFlowFieldData
{
	/** Value of Directions for a cell without a next cell. */
	static constexpr uint8 NoDirection = 0xFF;

	/** Steps between two clock reads while working under a deadline. */
	static constexpr int32 BudgetCheckInterval = 32;

	/** Neighbour offsets: 4 orthogonal, then 4 diagonal. */
	static const FIntPoint Offsets[8];

	/** Index of the opposite offset of each entry of Offsets (the step back). */
	static const uint8 Opposite[8];

	/** Dimensions of the field in cells. */
	FIntPoint Size = FIntPoint::ZeroValue;

	/** Goal cell the distances lead to; (INDEX_NONE, INDEX_NONE) before the first computation. */
	FIntPoint GoalCell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/** Walkable flag per cell: set from the grid by full rebuilds, then only turned on by openings. */
	TArray<bool> Walkable;

	/** BFS steps to the goal per cell; INDEX_NONE when unreachable. */
	TArray<int32> Distances;

	/** Neighbour offset index (into Offsets) to move to per cell; NoDirection when none. */
	TArray<uint8> Directions;

	/** Relaxation min-heap: (distance << 32) | cell index; stale entries (distance changed) are skipped. */
	TArray<uint64> RelaxHeap;

	/** True when Cell is inside the field. */
	bool IsInField(FIntPoint Cell) const { return Cell.X >= 0 && Cell.Y >= 0 && Cell.X < Size.X && Cell.Y < Size.Y; }

	/** Row-major index of a cell of the field. */
	int32 GetIndex(FIntPoint Cell) const { return Cell.Y * Size.X + Cell.X; }

	/** Cell of a row-major index of the field. */
	FIntPoint GetCell(int32 Index) const { return FIntPoint(Index % Size.X, Index / Size.X); }

	/** True when the distance and direction arrays match Size (a computed field). */
	bool IsComputed() const { return Size.X > 0 && Distances.Num() == Size.X * Size.Y && Directions.Num() == Distances.Num(); }

	/** True when the step from Cell along neighbour Index stays on walkable cells without cutting a corner. */
	bool CanStep(FIntPoint Cell, int32 Index) const;

	/** Next-cell neighbour index of Cell in InDistances toward Goal; NoDirection when none. */
	uint8 ChooseDirection(FIntPoint Cell, const TArray<int32>& InDistances, FIntPoint Goal) const;

	/** True when neighbour Candidate of Cell beats neighbour Current as next cell toward Goal, both at the same distance. */
	static bool IsBetterTie(FIntPoint Cell, int32 Candidate, int32 Current, FIntPoint Goal);

	/**
	 * Advances a BFS from Goal over Walkable into OutDistances / OutDirections: Queue / Head hold the breadth-first front,
	 * DirectionHead the direction pass over the visited cells. Stops at DeadlineSeconds (FPlatformTime::Seconds; <= 0: none).
	 * True when done.
	 */
	bool AdvanceBfs(FIntPoint Goal, TArray<int32>& OutDistances, TArray<uint8>& OutDirections, TArray<int32>& Queue, int32& Head,
		int32& DirectionHead, double DeadlineSeconds) const;

	/** Complete BFS from InGoalCell over the current Walkable into Distances / Directions; drops the relaxation. */
	void ComputeFull(FIntPoint InGoalCell);

	/** Pushes Cell (an opened cell, already walkable) and its 8 neighbours on the relaxation, giving Cell a distance from its neighbours. */
	void SeedAround(FIntPoint Cell);

	/** Pushes a relaxation entry for the cell of index Index at its current distance. */
	void PushRelax(int32 Index);

	/** Pops relaxation entries (lowest distance first) until empty or DeadlineSeconds (<= 0: none). True when the relaxation is empty. */
	bool RunRelaxation(double DeadlineSeconds);
};

/**
 * One worker-thread update of the flow field (task-066-01). The game thread fills it from the live field (copies) and the
 * validated queued openings, launches Run on a worker, and swaps Field into the live field once the task is complete. While it
 * runs only the worker touches it; it never reads the grid, actors or the live field.
 */
struct ROGUERIGS_API FRRFlowFieldJob
{
	/** Private field: copy of the live walkability (openings already applied), and of the distances for an incremental update. */
	FRRFlowFieldData Field;

	/** Cell indices opened by this job (already walkable in Field.Walkable); seeded on the copied field by an incremental update. */
	TArray<int32> OpenedIndices;

	/** Goal cell of a goal rebuild. */
	FIntPoint RebuildGoalCell = FIntPoint(INDEX_NONE, INDEX_NONE);

	/** True: full BFS from RebuildGoalCell; false: incremental relaxation of OpenedIndices on the copied field. */
	bool bGoalRebuild = false;

	/** Subsystem generation at launch; a result whose generation is outdated (a synchronous rebuild happened) is dropped. */
	uint32 Generation = 0;

	/** Worker time of Run, in ms. */
	double WorkerMilliseconds = 0.0;

	/** Computes the job on the calling thread (the worker). */
	void Run();
};
