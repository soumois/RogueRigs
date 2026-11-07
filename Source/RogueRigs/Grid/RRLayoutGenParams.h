// Rogue Rigs - data parameters of the 240 x 240 layout generator (layout-generator-recipe "Direct 240 generation") and the
// optional layout brief, both read from JSON files (Tools/layout_gen/params/fine_default.json, <name>.brief.json).

#pragma once

#include "CoreMinimal.h"

/** Outline of the asteroid: polar radius with harmonics plus value-noise jitter, clamped from the grid edge. */
struct FRRLayoutGenOutlineParams
{
	/** Base radius in cells. */
	double Base = 0.0;
	/** Harmonics (X amplitude in cells, Y frequency, Z phase in rad) added to the base radius. */
	TArray<FVector> Harmonics;
	/** Cells kept void along each grid edge. */
	int32 EdgeMargin = 0;
	/** Amplitude in cells of the value-noise jitter on the radius. */
	double NoiseAmp = 0.0;
	/** Scale in cells of that value noise. */
	double NoiseScale = 1.0;
};

/** Notching of straight outline runs. */
struct FRRLayoutGenNotchParams
{
	/** Longest straight outline run allowed (cells). */
	int32 MaxRun = 0;
	/** Notch length range along the run (cells). */
	int32 LenMin = 0;
	int32 LenMax = 0;
	/** Notch depth into the asteroid (cells). */
	int32 Depth = 0;
	/** Safety bound on notching iterations. */
	int32 MaxIter = 0;
};

/** Spawn clearing: wobbly disc plus an open square zone around the spawn cell. */
struct FRRLayoutGenSpawnParams
{
	/** Half side of the open square zone (cells): the zone is 2 ZoneHalf + 1 wide. */
	int32 ZoneHalf = 0;
	/** Radius of the cleared disc (cells). */
	double DiscRadius = 0.0;
	/** Amplitude of the disc radius wobble (cells). */
	double DiscWobble = 0.0;
	/** Spatial frequencies of the wobble along X and Y. */
	double DiscFreqX = 0.0;
	double DiscFreqY = 0.0;
};

/** Wander walk shared by the main network, the pockets and the filler. */
struct FRRLayoutGenWalkParams
{
	/** Step length (cells). */
	double Step = 1.0;
	/** Gaussian heading noise per step (rad). */
	double Turn = 0.0;
	/** Chance per step of a sharp turn, and its angle range (rad). */
	double SharpChance = 0.0;
	double SharpMin = 0.0;
	double SharpMax = 0.0;
	/** Brush radius range (cells) and its Gaussian drift per step. */
	double BrushMin = 0.0;
	double BrushMax = 0.0;
	double BrushDrift = 0.0;
	/** Value-noise jitter on the brush edge: amplitude and scale (cells). */
	double BrushNoiseAmp = 0.0;
	double BrushNoiseScale = 1.0;
	/** A walk never carves within this many cells (Chebyshev) of void. */
	int32 VoidMargin = 0;
};

/** Main network of carved paths. */
struct FRRLayoutGenMainParams
{
	/** Walks run until floor reaches this share of the asteroid cells. */
	double FloorShare = 0.0;
	/** Walk length range (steps). */
	int32 StepsMin = 0;
	int32 StepsMax = 0;
	/** Starting brush radius range (cells). */
	double Rho0Min = 0.0;
	double Rho0Max = 0.0;
	/** Safety bound on the number of walks. */
	int32 MaxWalks = 0;
};

/** Hidden pockets carved inside rock, close to but not touching the main network. */
struct FRRLayoutGenPocketParams
{
	/** Pockets wanted. */
	int32 Count = 0;
	/** Walk length: StepsBase + random [0, StepsRand). */
	int32 StepsBase = 0;
	int32 StepsRand = 0;
	/** Extra heading noise over the walk turn (rad). */
	double TurnAdd = 0.0;
	/** Starting brush radius range (cells). */
	double Rho0Min = 0.0;
	double Rho0Max = 0.0;
	/** Smallest pocket kept (cells). */
	int32 MinCells = 0;
	/** Distance range (BFS) from the main network to the pocket. */
	int32 DistMin = 0;
	int32 DistMax = 0;
	/** Later walks never carve within this many cells of a pocket. */
	int32 Guard = 0;
	/** Attempts to place pockets. */
	int32 Tries = 0;
	/** Pocket starts are drawn this many cells inside the grid edge. */
	int32 Margin = 0;
};

/** Far-cell filler: carves toward cells too far from floor. */
struct FRRLayoutGenFillerParams
{
	/** Largest allowed BFS distance from any asteroid cell to floor. */
	int32 MaxD = 0;
	/** Extra walk length: ExtraMin + random [0, ExtraRand). */
	int32 ExtraMin = 0;
	int32 ExtraRand = 0;
	/** Multiplier on the walk turn. */
	double TurnScale = 1.0;
	/** Starting brush radius range (cells). */
	double Rho0Min = 0.0;
	double Rho0Max = 0.0;
	/** Gaussian noise on the aim angle (rad). */
	double AimNoise = 0.0;
	/** Safety bound on filler walks per pass. */
	int32 MaxIter = 0;
};

/** Cleanup and the passage rule. */
struct FRRLayoutGenCleanupParams
{
	/** Unreachable regions under this size (cells) are refilled. */
	int32 MinRegion = 0;
	/** Unreachable regions farther than this many rock cells from the main network are refilled. */
	int32 MaxGap = 0;
	/** Rock islands of this size or less (not touching void) become floor. */
	int32 MaxRockIsland = 0;
	/** Passage width: every open cell lies in an all-open Passage x Passage window. */
	int32 Passage = 0;
	/** Bound on repair rounds per repair pass. */
	int32 RepairRounds = 0;
	/** Bound on repair / refill / filler loops. */
	int32 Loops = 0;
};

/** Heal veins (Vein x Vein blocks of heal ore). */
struct FRRLayoutGenHealParams
{
	/** Side of a vein block (cells). */
	int32 Vein = 0;
	/** Veins placed in hidden-pocket floor. */
	int32 PocketVeins = 0;
	/** Veins placed in walls touching the main network. */
	int32 WallVeins = 0;
	/** Half side of the square around the spawn kept free of veins. */
	int32 ExcludeHalf = 0;
	/** Rock cells needed around a wall vein / a pocket vein (4-neighbours of the block). */
	int32 WallMinRock = 0;
	int32 PocketMinRock = 0;
	/** Random tries per vein (best spread kept). */
	int32 Tries = 0;
};

/** Barrels on reachable floor against a wall. */
struct FRRLayoutGenBarrelParams
{
	/** Barrel count range drawn per seed. */
	int32 Min = 0;
	int32 Max = 0;
	/** Pair count range (chains). */
	int32 PairsMin = 0;
	int32 PairsMax = 0;
	/** Distance range between the two barrels of a pair (cells). */
	double PairDistMin = 0.0;
	double PairDistMax = 0.0;
	/** Half side of the square around the spawn kept free of barrels. */
	int32 SpawnExclude = 0;
	/** Cells kept between a barrel and a heal vein. */
	int32 HealClear = 0;
	/** Random tries per barrel (best spread kept). */
	int32 Tries = 0;
};

/** Recipe seed acceptance. */
struct FRRLayoutGenSearchParams
{
	/** Seeds tried upward from the requested one. */
	int32 MaxSeeds = 1;
	/** Asteroid pieces required. */
	int32 Pieces = 1;
	/** Longest straight outline run allowed. */
	int32 MaxOutlineRun = 0;
	/** Largest BFS distance from an asteroid cell to floor. */
	int32 MaxFar = 0;
	/** Asteroid size range (non-void cells). */
	int32 AsteroidMin = 0;
	int32 AsteroidMax = 0;
	/** Reachable floor share range. */
	double ReachMin = 0.0;
	double ReachMax = 1.0;
	/** Valid hidden pockets required. */
	int32 MinPockets = 0;
};

/** Layout review rules (vault: layout-review-rules) enforced by repair passes and checked by the seed acceptance. */
struct FRRLayoutGenReviewParams
{
	/** Side of the scanned windows (cells) and scan stride. */
	int32 Window = 24;
	int32 Stride = 2;
	/** Accepted open share (floor, spawn, barrel) of every void-free window. */
	double OpenMin = 0.0;
	double OpenMax = 1.0;
	/** The repair pass aims inside this tighter range, so decoration cannot push a window out of the accepted one. */
	double RepairOpenMin = 0.0;
	double RepairOpenMax = 1.0;
	/** Straight lane: open span LaneWidthMin..LaneWidthMax wide whose walls stay within LaneTolerance cells; longest allowed run. */
	int32 LaneMax = 0;
	int32 LaneWidthMin = 0;
	int32 LaneWidthMax = 0;
	int32 LaneTolerance = 0;
	/** Largest distance (cells) from a hidden pocket to the main network. */
	int32 PocketMaxDist = 0;
	/** Radius range of a rock outcrop added into a too-open window, and of a wall bump or niche breaking a straight lane. */
	double OutcropRhoMin = 0.0;
	double OutcropRhoMax = 0.0;
	double LaneRhoMin = 0.0;
	double LaneRhoMax = 0.0;
	/** Edge noise amplitude of outcrops, bumps and niches (cells; scale walk.brush_noise_scale). */
	double ShapeNoise = 0.0;
	/** Repair actions per pass, failed candidates tried per action, repair passes (each followed by the cleanup loop). */
	int32 MaxActions = 0;
	int32 MaxTries = 0;
	int32 Passes = 0;
};

/** Every tunable value of the layout generator, read from a params JSON file. */
struct ROGUERIGS_API FRRLayoutGenParams
{
	/** Grid size in cells. */
	int32 Width = 0;
	int32 Height = 0;
	/** Seed mixed into the position hash of the value noise. */
	int32 NoiseSeed = 0;

	FRRLayoutGenOutlineParams Outline;
	FRRLayoutGenNotchParams Notch;
	FRRLayoutGenSpawnParams Spawn;
	FRRLayoutGenWalkParams Walk;
	FRRLayoutGenMainParams Main;
	FRRLayoutGenPocketParams Pockets;
	FRRLayoutGenFillerParams Filler;
	FRRLayoutGenCleanupParams Cleanup;
	FRRLayoutGenHealParams Heal;
	FRRLayoutGenBarrelParams Barrels;
	FRRLayoutGenSearchParams Search;
	FRRLayoutGenReviewParams Review;
};

/** Optional brief bounds (level-layout-schema): checked on top of the recipe checks. */
struct ROGUERIGS_API FRRLayoutBrief
{
	/** Expected grid size (0 = not set). */
	int32 Width = 0;
	int32 Height = 0;
	/** Block coverage range of the non-void cells, when set. */
	bool bHasRockCoverage = false;
	double RockCoverageMin = 0.0;
	double RockCoverageMax = 1.0;
	/** Minimum reachable floor share, when set. */
	bool bHasReachableFloorMin = false;
	double ReachableFloorMin = 0.0;
	/** Minimum passage width (0 = not set). */
	int32 MinPassageWidth = 0;
	/** Count range per layout type name ("heal_ore", "barrel"...), in file order. */
	TArray<TPair<FString, FIntPoint>> Counts;
	/** Raw file text, copied to <name>.brief.json next to the layout. */
	FString SourceText;
};

/** Readers of the generator data files. Thread-safe: no UObject, no game-thread state. */
namespace RRLayoutGen
{
	/** Default params file: <project>/Tools/layout_gen/params/fine_default.json. */
	ROGUERIGS_API FString GetDefaultParamsPath();

	/** Reads every generator parameter from FilePath. Returns false with OutError naming the file and the missing keys. */
	ROGUERIGS_API bool LoadParams(const FString& FilePath, FRRLayoutGenParams& OutParams, FString& OutError);

	/** Reads a brief file (level-layout-schema). Returns false with OutError on a read or parse error. */
	ROGUERIGS_API bool LoadBrief(const FString& FilePath, FRRLayoutBrief& OutBrief, FString& OutError);
}
