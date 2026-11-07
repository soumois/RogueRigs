// Rogue Rigs - deterministic 240 x 240 layout generator, C++ port of Tools/layout_gen/arena_gen_fine.js
// (vault: layout-generator-recipe "Direct 240 generation", sys-layout-generator). Every tunable value comes from
// FRRLayoutGenParams; randomness comes from one FRandomStream seeded per layout, so the same params and seed give the same grid.

#pragma once

#include "CoreMinimal.h"
#include "Grid/RRCellType.h"
#include "Grid/RRLayoutGenParams.h"
#include "Math/RandomStream.h"

/**
 * Generates one layout grid from params and a seed. Plain C++ (no UObject, no game-thread state): one instance per thread.
 * Carve() runs the carving steps of the recipe: outline and notching, spawn clearing, main network, hidden pockets,
 * far-cell filler, cleanup and the 3-cell passage repair.
 */
class ROGUERIGS_API FRRLayoutGenerator
{
public:
	/** Keeps a copy of the params; the grid is empty until Carve() runs. */
	explicit FRRLayoutGenerator(const FRRLayoutGenParams& InParams);

	/** Runs the carving steps for Seed and leaves the carved grid in GetCells() (spawn cell set). */
	void Carve(int32 Seed);

	/** The grid, row-major (index = Y * Width + X). */
	const TArray<ERRCellType>& GetCells() const { return Grid; }

	/** Grid width in cells. */
	int32 GetWidth() const { return W; }

	/** Grid height in cells. */
	int32 GetHeight() const { return H; }

	/** Open cells (floor, spawn, barrel) that lie in no all-open Passage x Passage window; 0 when the passage rule holds. */
	int32 CountNarrowCells() const;

	/** FNV-1a hash of the grid, to compare two runs. */
	uint32 HashGrid() const;

protected:
	/** Uniform random value in [0, 1) from the stream. */
	double Rnd();

	/** Approximate Gaussian (sum of three uniforms), as the Node generator. */
	double Gauss();

	/** Position hash in [0, 1) mixed with the layout noise seed. */
	double Hash(int32 X, int32 Y, int32 K) const;

	/** Smooth value noise in [0, 1]. */
	double ValueNoise(double X, double Y, double Scale, int32 K) const;

	/** True when (X, Y) is inside the grid. */
	bool InBounds(int32 X, int32 Y) const { return X >= 0 && Y >= 0 && X < W && Y < H; }

	/** True when (X, Y) is inside the grid and not void (part of the asteroid). */
	bool IsAsteroid(int32 X, int32 Y) const { return InBounds(X, Y) && Grid[Y * W + X] != ERRCellType::Void; }

	/** Cell type, void outside the grid. */
	ERRCellType TypeAt(int32 X, int32 Y) const { return InBounds(X, Y) ? Grid[Y * W + X] : ERRCellType::Void; }

	/** Passage rule open cell: floor, spawn or barrel. */
	static bool IsOpen(ERRCellType Type) { return Type == ERRCellType::Floor || Type == ERRCellType::Spawn || Type == ERRCellType::Barrel; }

	/** Walkable cell: floor or spawn. */
	static bool IsWalkable(ERRCellType Type) { return Type == ERRCellType::Floor || Type == ERRCellType::Spawn; }

	/** True when (X, Y) is within Half cells (Chebyshev) of the spawn cell. */
	bool InSpawnZone(int32 X, int32 Y, int32 Half) const { return FMath::Abs(X - SX) <= Half && FMath::Abs(Y - SY) <= Half; }

	/** One straight outline run: its cells and the outward direction. */
	struct FOutlineRun
	{
		TArray<FIntPoint> Cells;
		FIntPoint Dir;
	};

	/** Straight outline runs (cells with void on one side) longer than MaxRun. */
	TArray<FOutlineRun> FindOutlineRuns(int32 MaxRun) const;

	/** Wander walk from (X, Y): returns carved cell indices, filtered (safe from void, outside pocket guards). */
	TArray<int32> WanderWalk(double X, double Y, double Angle, int32 Len, double Rho0, double Turn);

	/** Rock cells of Cells become floor. */
	void ApplyCarve(const TArray<int32>& Cells);

	/** Marks pocket cells hidden and their guard square. */
	void MarkHidden(const TArray<int32>& Cells);

	/** Walkable cells 4-connected to the spawn; OutCount receives their number. */
	TArray<uint8> Reach(int32* OutCount = nullptr) const;

	/** BFS distance (4-connected through non-void cells) from the cells where Source is non-zero. */
	TArray<int32> Bfs(const TArray<uint8>& Source) const;

	/** BFS distance from walkable cells. */
	TArray<int32> BfsFromWalkable() const;

	/** One unreachable walkable region and its gap (rock cells) to the main network. */
	struct FRegion
	{
		TArray<int32> Cells;
		int32 Gap = 0;
	};

	/** Unreachable walkable regions; OutReach receives the reachable mask when not null. */
	TArray<FRegion> FindRegions(TArray<uint8>* OutReach = nullptr, int32* OutReachCount = nullptr) const;

	/** Step 5: carves toward cells farther than maxd from floor. Returns the number of walks carved. */
	int32 RunFiller();

	/** Refills small or distant unreachable regions with rock. */
	void RefillRegions();

	/** Small rock islands that touch no void become floor. */
	void FillRockIslands();

	/** Cells covered by an all-open Passage x Passage window. */
	TArray<uint8> CoveredMap() const;

	/** Narrow floor cells become rock until every open cell is covered. Returns the cells changed. */
	int32 PassageRepair();

	/** Thin outline peninsulas farther than maxd from floor (inside the void margin) become void. */
	void TrimFar();

	/** Params copy. */
	FRRLayoutGenParams P;

	/** Random stream of the current layout. */
	FRandomStream Stream;

	/** Grid size and cell count. */
	int32 W = 0;
	int32 H = 0;
	int32 N = 0;

	/** Spawn cell. */
	int32 SX = 0;
	int32 SY = 0;

	/** Noise seed of the current layout (seed mixed with params.noise_seed). */
	int32 NoiseSeedMixed = 0;

	/** The grid being built. */
	TArray<ERRCellType> Grid;

	/** 1 where no void lies within the walk void margin. */
	TArray<uint8> Safe;

	/** 1 on hidden pocket cells, and within the pocket guard of one. */
	TArray<uint8> Hidden;
	TArray<uint8> HiddenGuard;

	/** Per-cell walk stamp, to add each cell once per walk. */
	TArray<uint32> Stamp;
	uint32 WalkId = 0;

	/** Asteroid cells (non-void), floor cells counted by the carving steps, pockets kept. */
	int32 Total = 0;
	int32 FloorCount = 0;
	int32 PocketCount = 0;
};
