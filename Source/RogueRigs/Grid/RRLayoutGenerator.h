// Rogue Rigs - deterministic 240 x 240 layout generator, C++ port of Tools/layout_gen/arena_gen_fine.js
// (vault: layout-generator-recipe "Direct 240 generation", sys-layout-generator). Every tunable value comes from
// FRRLayoutGenParams; randomness comes from one FRandomStream seeded per layout, so the same params and seed give the same grid.

#pragma once

#include "CoreMinimal.h"
#include "Grid/RRCellType.h"
#include "Grid/RRLayoutGenParams.h"
#include "Math/RandomStream.h"

/** Measures of one generated layout, read by the recipe seed acceptance. */
struct ROGUERIGS_API FRRLayoutGenStats
{
	/** Asteroid (non-void) cells. */
	int32 Asteroid = 0;
	/** Block share (rock, heal ore, barrel) of the asteroid cells. */
	double Coverage = 0.0;
	/** Share of the floor cells (floor, spawn) 4-connected to the spawn. */
	double Reach = 0.0;
	/** 4-connected asteroid pieces. */
	int32 Pieces = 0;
	/** Largest BFS distance from an asteroid cell to floor, measured before heal veins and barrels. */
	int32 MaxFar = 0;
	/** Open cells outside every all-open Passage x Passage window. */
	int32 Narrow = 0;
	/** Valid hidden pockets (unreachable regions of MinRegion+ cells, gap 1 to MaxGap). */
	int32 Pockets = 0;
	/** Longest straight outline run. */
	int32 OutlineRun = 0;
	/** Heal ore and barrel cells. */
	int32 HealOre = 0;
	int32 Barrels = 0;
	/** Smallest and largest open share (floor, spawn, barrel) over the void-free review windows (layout-review-rules). */
	double WindowMin = 1.0;
	double WindowMax = 0.0;
	/** Longest straight lane (cells) and largest distance from a hidden pocket to the main network. */
	int32 LongestLane = 0;
	int32 FarthestPocket = 0;
	/** Review repairs applied while carving: dead windows carved, outcrops added to too-open windows, lanes broken. */
	int32 RepairCarves = 0;
	int32 RepairOutcrops = 0;
	int32 RepairLanes = 0;
	/** Count per layout type name (void, rock, heal_ore, barrel, spawn), for the brief. */
	TMap<FString, int32> Counts;
};

/** One accepted (or last tried) layout of a seed search. */
struct ROGUERIGS_API FRRGeneratedLayout
{
	/** Seed that produced Cells. */
	int32 Seed = 0;
	/** Grid size. */
	int32 Width = 0;
	int32 Height = 0;
	/** Cell types, row-major. */
	TArray<ERRCellType> Cells;
	/** Measures of Cells. */
	FRRLayoutGenStats Stats;
	/** Failed acceptance checks; empty when the layout passed. */
	TArray<FString> Fails;
	/** Seeds tried by the search, and generation time of the accepted seed alone and of the whole search (s). */
	int32 SeedsTried = 0;
	double Seconds = 0.0;
	double SearchSeconds = 0.0;
};

/**
 * Generates one layout grid from params and a seed. Plain C++ (no UObject, no game-thread state): one instance per thread.
 * Carve() runs the carving steps of the recipe: outline and notching, spawn clearing, main network, hidden pockets,
 * far-cell filler, cleanup, the review repair passes (24 x 24 windows, straight lanes) and the 3-cell passage repair. Decorate() places heal veins and barrels; Generate() runs the
 * whole recipe with its seed acceptance and returns the first passing seed.
 */
class ROGUERIGS_API FRRLayoutGenerator
{
public:
	/** Keeps a copy of the params; the grid is empty until Carve() runs. */
	explicit FRRLayoutGenerator(const FRRLayoutGenParams& InParams);

	/** Runs the carving steps for Seed and leaves the carved grid in GetCells() (spawn cell set). */
	void Carve(int32 Seed);

	/** Places the heal veins and barrels on the carved grid. Returns false when the quotas cannot be met. */
	bool Decorate();

	/** Measures the current grid. */
	FRRLayoutGenStats Measure() const;

	/** Recipe checks (and brief bounds when Brief is not null) of Stats; returns the failed checks. */
	TArray<FString> Check(const FRRLayoutGenStats& Stats, const FRRLayoutBrief* Brief) const;

	/** Carve, measure the far distance, decorate, measure and check one seed. */
	FRRGeneratedLayout RunSeed(int32 Seed, const FRRLayoutBrief* Brief);

	/** Tries seeds upward from FirstSeed (search.max_seeds) and returns the first that passes, or the last tried. */
	FRRGeneratedLayout Generate(int32 FirstSeed, const FRRLayoutBrief* Brief);

	/** The layout as level-layout-schema JSON (compact, every non-floor cell, row-major): same cells give the same bytes. */
	static FString ToJson(const FString& Name, const FRRGeneratedLayout& Layout);

	/** Layout type name of a cell type ("rock", "heal_ore"...), empty for floor. */
	static const TCHAR* TypeName(ERRCellType Type);

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

	/** FNV-1a hash of a cell array (same value as HashGrid for the same cells): compares a run layout with a commandlet output. */
	static uint32 HashCells(const TArray<ERRCellType>& Cells);

	/** Smallest and largest open share over the void-free review windows (stride Review.Stride). */
	void MeasureWindows(double& OutMin, double& OutMax) const;

	/** Longest straight lane of the grid (both axes), in cells. */
	int32 MeasureLongestLane() const;

	/** Time spent in the far-cell filler during the last Carve, in seconds. */
	double GetFillerSeconds() const { return FillerSeconds; }

	/** Filler iterations of the last Carve, and those whose walk carved no cell. */
	int32 GetFillerIterations() const { return FillerIterations; }
	int32 GetFillerIdleIterations() const { return FillerIdleIterations; }

	/** Idle filler iterations of the last Carve whose walk lost at least one rock cell to a pocket guard. */
	int32 GetFillerIdleGuardBlocked() const { return FillerIdleGuardBlocked; }

	/** Hidden pockets of the last Carve: placed, wholly removed, partly removed (some cells back to rock). */
	void CountPockets(int32& OutPlaced, int32& OutRemoved, int32& OutPartly) const;

	/** Guard cells released during the last Carve because the pocket cells they guarded went back to rock. */
	int32 GetGuardReleased() const { return GuardReleased; }

	/** Guard cells not within Pockets.Guard of a hidden cell (stale), and cells within it but not guarded (missing). */
	void CountGuardMismatch(int32& OutStale, int32& OutMissing) const;

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

	/** Guard square (Pockets.Guard, Chebyshev) of every cell still hidden, into Out. */
	void BuildGuard(TArray<uint8>& Out) const;

	/** Rebuilds HiddenGuard from the cells still hidden, after pocket cells went back to rock: the guard of a removed pocket
	 *  (or of the removed part of one) no longer blocks carving; the guard of every cell still hidden stays. */
	void ReleaseStaleGuard();

	/** Walkable cells 4-connected to the spawn; OutCount receives their number. */
	TArray<uint8> Reach(int32* OutCount = nullptr) const;

	/** Reach into caller buffers (reused without reallocation): Mask receives the reachable mask, Stack is scratch. */
	void ReachInto(TArray<uint8>& Mask, TArray<int32>& Stack, int32* OutCount) const;

	/** BFS distance (4-connected through non-void cells) from the cells where Source is non-zero. */
	TArray<int32> Bfs(const TArray<uint8>& Source) const;

	/** Bfs into caller buffers (reused without reallocation): Dist receives the distances, Queue is scratch. */
	void BfsInto(const TArray<uint8>& Source, TArray<int32>& Dist, TArray<int32>& Queue) const;

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

	/** Largest BFS distance from an asteroid cell to floor. */
	int32 MeasureMaxFar() const;

	/** One review window: origin and open share. */
	struct FReviewWindow
	{
		int32 X = 0;
		int32 Y = 0;
		int32 Open = 0;
		double Share = 0.0;
	};

	/** One straight lane run: axis (true = along X), anchor line B, first cell A along the axis, length. */
	struct FLaneRun
	{
		bool bAlongX = true;
		int32 B = 0;
		int32 Start = 0;
		int32 Len = 0;
	};

	/** One changed cell and its previous type, to undo a repair. */
	using FCellUndo = TPair<int32, ERRCellType>;

	/** Grid index of the cell at position A along the lane axis and B across it. */
	int32 LaneIndex(bool bAlongX, int32 A, int32 B) const { return bAlongX ? B * W + A : A * W + B; }

	/** Open cells in the review window at (X, Y); false when the window holds void. */
	bool CountWindow(int32 X, int32 Y, int32& OutOpen) const;

	/** Scans the review windows: min and max share, and the windows outside [Low, High] sorted worst first. */
	void ScanWindows(double Low, double High, double& OutMin, double& OutMax, TArray<FReviewWindow>* OutOffending) const;

	/** For each open cell, the first and last cell (across the lane axis) of the open span through it. */
	void BuildSpans(bool bAlongX, TArray<int32>& OutLo, TArray<int32>& OutHi) const;

	/** Straight lane runs along one axis: returns the longest; runs longer than MinLen go to OutRuns. */
	int32 ScanLanes(bool bAlongX, int32 MinLen, TArray<FLaneRun>* OutRuns) const;

	/** Length of the lane run of anchor line B that holds position A (0 when none). */
	int32 LaneRunAt(bool bAlongX, int32 B, int32 A) const;

	/** True when cell (X, Y) lies in an all-open Passage x Passage window. */
	bool IsCovered(int32 X, int32 Y) const;

	/** True when every open cell of the box lies in an all-open Passage x Passage window. */
	bool BoxCovered(int32 X0, int32 Y0, int32 X1, int32 Y1) const;

	/** Restores the cells of Undo (newest last). */
	void Undo(const TArray<FCellUndo>& Changes);

	/** Rock cells of a noisy disc become floor (safe, outside pocket guards); unreachable or narrow carved cells are put back. */
	void CarveDisc(double CX, double CY, double Rho, TArray<FCellUndo>& Changes);

	/** Keeps the carved cells of Changes that reach the main network and lie in a passage window; puts the others back. */
	void SettleCarve(TArray<FCellUndo>& Changes);

	/** Reachable floor cells of a noisy disc become rock (outside the spawn zone and pockets). */
	void StampRock(double CX, double CY, double Rho, const TArray<uint8>& ReachMask, TArray<FCellUndo>& Changes);

	/** True when the rock of Changes (all reachable floor before) cut nothing off, kept the passage rule around it and left
	 *  no asteroid cell farther from floor than max(FarBefore, search.max_far). */
	bool RockKeepsRules(const TArray<FCellUndo>& Changes, int32 ReachBefore, int32 FarBefore) const;

	/** Repair of a window under RepairOpenMin: a filler walk from the main network into its deepest rock. */
	bool RepairDeadWindow(const FReviewWindow& Window);

	/** Repair of a window over RepairOpenMax: an organic rock outcrop on an open cell, else a bump on one of its walls. */
	bool RepairOpenWindow(const FReviewWindow& Window);

	/** Repair of a straight lane: a wall bump (wide lane) or a wall niche at its middle, or at a third. */
	bool RepairLane(const FLaneRun& Lane);

	/** Review repair pass: repairs the worst window, else the longest lane, until none is left or none can be fixed. */
	int32 RepairReviewRules();

	/** Picks N cells of Candidates, each the farthest from Picked among Tries random valid ones; places and records them. */
	bool PickSpread(const TArray<FIntPoint>& Candidates, int32 Count, int32 Tries, TFunctionRef<bool(int32, int32)> IsValid,
		TArray<FIntPoint>& Picked, TFunctionRef<void(int32, int32)> Place);

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

	/** 1 on hidden pocket cells, and within the pocket guard of one still hidden (released when its cells go back to rock). */
	TArray<uint8> Hidden;
	TArray<uint8> HiddenGuard;

	/** Per-cell walk stamp, to add each cell once per walk. */
	TArray<uint32> Stamp;
	uint32 WalkId = 0;

	/** Asteroid cells (non-void), floor cells counted by the carving steps, pockets kept. */
	int32 Total = 0;
	int32 FloorCount = 0;
	int32 PocketCount = 0;

	/** Review repairs of the current layout. */
	int32 RepairCarves = 0;
	int32 RepairOutcrops = 0;
	int32 RepairLanes = 0;

	/** Filler cost of the current layout (instrumentation only, never read by the generation). */
	double FillerSeconds = 0.0;
	int32 FillerIterations = 0;
	int32 FillerIdleIterations = 0;
	int32 FillerIdleGuardBlocked = 0;

	/** Rock cells the last WanderWalk dropped because of a pocket guard (instrumentation only). */
	int32 LastWalkGuardDropped = 0;

	/** Cells of each pocket placed, and guard cells released (instrumentation only, never read by the generation). */
	TArray<TArray<int32>> PocketCells;
	int32 GuardReleased = 0;
};
