// Rogue Rigs - arena grid actor: loads a layout JSON, builds floor and block instances, and owns terrain damage and destruction.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid/RRCellType.h"
#include "RRArenaGrid.generated.h"

class ARRPickup;
class URRBarrelSource;
class URRGroundTelegraphComponent;
class UHierarchicalInstancedStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UMaterialInterface;
class UPrimitiveComponent;
class USceneComponent;
class UStaticMesh;
struct FHitResult;
struct FRROreTypeRow;
struct FRRLayoutData;

/** What damaged or destroyed a block: the four terrain-breaking sources (destruction-rules), plus shots, which only break barrels. */
UENUM(BlueprintType)
enum class ERRTerrainDamageSource : uint8
{
	/** The drone's cutting laser (dig). */
	Drone,
	/** An exploding barrel. */
	Barrel,
	/** An exploding kamikaze robot. */
	Kamikaze,
	/** Projectiles: only barrels take shot damage (BarrelShotHealth); the barrel then explodes. */
	Shot,
	/**
	 * The extractor set on the ground (arrival and extraction landing, task-372-01): ARRArenaGrid::ClearExtractorFootprint.
	 * Breaks a touched deposit whole, its pickups appear at rest on their cells (no pop), and a barrel is removed without exploding.
	 */
	Extractor
};

/** Counts of one ARRArenaGrid::ClearExtractorFootprint call (task-372-01), by block type; logged once per extractor placement. */
USTRUCT(BlueprintType)
struct FRRFootprintClearResult
{
	GENERATED_BODY()

	/** Rock blocks destroyed (inside the footprint, or in a deposit broken whole). */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 Rock = 0;

	/** Ore blocks destroyed. */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 Ore = 0;

	/** Heal ore blocks destroyed. */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 HealOre = 0;

	/** Special ore blocks destroyed. */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 SpecialOre = 0;

	/** Barrels removed (never exploded). */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 Barrel = 0;

	/** Blocks of the counts above that lie outside the footprint: the other blocks of the deposits it broke whole. */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 OutsideFootprint = 0;

	/** Deposits (ore or heal vein) broken whole. */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 Deposits = 0;

	/** Pickups requested at rest on the floor (ore chunks and heal pickups). */
	UPROPERTY(BlueprintReadOnly, Category = "Grid|Destruction")
	int32 Pickups = 0;

	/** Every block destroyed or removed (sum of the type counts). */
	int32 GetTotal() const { return Rock + Ore + HealOre + SpecialOre + Barrel; }
};

/**
 * One visual type in one chunk of ChunkCells x ChunkCells cells: its rendering HISM (floor slabs, barrels, and the crystal
 * clusters of ore and heal ore since task-230-08), its hidden collision ISM (block types only) and the instance index -> cell
 * map shared by both. Rock and special ore cells have no per-cell rendering instance since task-230-07 (dual-grid rock pieces
 * draw them, FRRGridPieceVisual): their visual holds the collision ISM alone. Components are created when the chunk first holds an instance.
 */
USTRUCT()
struct FRRGridChunkVisual
{
	GENERATED_BODY()

	/** Visual type: Floor (floor slabs, every non-void cell) or a block type. */
	UPROPERTY()
	ERRCellType MapKey = ERRCellType::Floor;

	/** Chunk coordinates (cell / ChunkCells). */
	UPROPERTY()
	FIntPoint Chunk = FIntPoint::ZeroValue;

	/** Rendering HISM of this type in this chunk; null until the chunk holds one instance, and always null for rock and special ore (ARRArenaGrid::HasCellRenderInstance). */
	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Render;

	/** Hidden collision ISM of this block type in this chunk, instance for instance with Render when both exist; null for Floor (the floor HISM collides). */
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> Collision;

	/** Cell of each instance, in instance order (Render and Collision share it). Kept in sync with remove-at-swap of both components. */
	UPROPERTY()
	TArray<FIntPoint> InstanceCells;
};

/**
 * Dual-grid rock piece shapes (rock-render-piece-set): one piece per cell corner, chosen by which of the 4 cells around the
 * corner are solid (rock, ore, heal ore, special ore). Rotations by 90 degree steps cover the 15 non-empty cases.
 */
UENUM(BlueprintType)
enum class ERRRockPieceShape : uint8
{
	/** 4 solid cells: flat top, no outline. */
	Full,
	/** 2 solid cells on one side. */
	Edge,
	/** 1 solid cell (convex corner). */
	Outer,
	/** 3 solid cells (concave corner). */
	Inner,
	/** 2 opposite solid cells (joined saddle). */
	Diagonal
};

/** Piece sets of the dual grid: intact pieces, and broken pieces (own ISMs) around damaged cells (task-230-08). */
UENUM(BlueprintType)
enum class ERRRockPieceSet : uint8
{
	/** Intact pieces (RockPieceMeshes). */
	Intact,
	/** Broken pieces (RockPieceBrokenMeshes, fractured prisms): a corner switches to it as soon as one of its 4 cells has display damage above 0, and never switches back. */
	Broken
};

/**
 * Meshes of the dual-grid rock pieces, one list per shape; each entry is one variant (several supported, the variant of a
 * corner is chosen by a hash of the corner). Canonical orientation of the source meshes (SM_RockSlab.report.json): pivot at
 * the piece centre (a cell corner) on the floor top, RockPieceMeshSize wide, top at RockPieceMeshHeight. Null or empty
 * entries are tolerated: the instances are still placed and tracked, nothing is drawn.
 */
USTRUCT(BlueprintType)
struct FRRRockPieceMeshes
{
	GENERATED_BODY()

	/** Variants of the full piece (4 solid cells; Blender canonical rock corners A B C D). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock Pieces")
	TArray<TObjectPtr<UStaticMesh>> Full;

	/** Variants of the edge piece (canonical rock corners B C, rock on +X). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock Pieces")
	TArray<TObjectPtr<UStaticMesh>> Edge;

	/** Variants of the outer corner piece (canonical rock corner C). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock Pieces")
	TArray<TObjectPtr<UStaticMesh>> Outer;

	/** Variants of the inner corner piece (canonical rock corners B C D, notch at A). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock Pieces")
	TArray<TObjectPtr<UStaticMesh>> Inner;

	/** Variants of the diagonal piece (canonical rock corners A C). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock Pieces")
	TArray<TObjectPtr<UStaticMesh>> Diagonal;

	/** Variant list of Shape. */
	const TArray<TObjectPtr<UStaticMesh>>& GetMeshes(ERRRockPieceShape Shape) const
	{
		switch (Shape)
		{
		case ERRRockPieceShape::Edge:     return Edge;
		case ERRRockPieceShape::Outer:    return Outer;
		case ERRRockPieceShape::Inner:    return Inner;
		case ERRRockPieceShape::Diagonal: return Diagonal;
		default:                          return Full;
		}
	}
};

/**
 * One rock piece ISM: one piece set, one shape, one variant, one chunk (rendering only, no collision). Holds the corner index
 * of each instance in instance order, kept in sync with remove-at-swap.
 */
USTRUCT()
struct FRRGridPieceVisual
{
	GENERATED_BODY()

	/** Piece set of every instance (intact or broken). */
	UPROPERTY()
	ERRRockPieceSet Set = ERRRockPieceSet::Intact;

	/** Shape of every instance. */
	UPROPERTY()
	ERRRockPieceShape Shape = ERRRockPieceShape::Full;

	/** Variant index of every instance (index in the shape's mesh list). */
	UPROPERTY()
	int32 Variant = 0;

	/** Chunk coordinates (corner / ChunkCells, clamped to the last chunk: a corner on a chunk line belongs to the higher chunk). */
	UPROPERTY()
	FIntPoint Chunk = FIntPoint::ZeroValue;

	/**
	 * Rendering ISM (remove at swap); null until the chunk holds one instance of this shape and variant. A plain ISM, not a HISM
	 * (task-230-14): a HISM removal leaves its render slot unmapped until the async tree rebuild, and that slot holds uninitialized
	 * instance data that the distance field and Lumen card scenes read (precision ensure, LumenMeshCards assert).
	 */
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> Render;

	/** Corner index (ARRArenaGrid::GetCornerIndex) of each instance, in instance order. */
	UPROPERTY()
	TArray<int32> InstanceCorners;
};

/**
 * One deposit crystal ISM (task-234-03): one block type (ore or heal ore), one mesh variant of its deposit mesh list, over the
 * whole grid (rendering only, no collision). One instance per standing deposit of that type and variant, at the deposit centre
 * on the block top. A plain ISM with remove-at-swap, as the rock pieces (task-230-14: a HISM removal leaves an uninitialized
 * render slot read by the distance field and Lumen card scenes).
 */
USTRUCT()
struct FRRGridDepositVisual
{
	GENERATED_BODY()

	/** Block type of every deposit drawn here (Ore or HealOre). */
	UPROPERTY()
	ERRCellType CellType = ERRCellType::Ore;

	/** Index of the mesh in the type's deposit mesh list (non-null entries only). */
	UPROPERTY()
	int32 Variant = 0;

	/** Rendering ISM (remove at swap); null when the build placed no deposit of this type and variant. */
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> Render;

	/** Deposit id (index in ARRArenaGrid::Deposits) of each instance, in instance order. */
	UPROPERTY()
	TArray<int32> InstanceDeposits;

	/** True when the instances carry the pulse phase and rate (task-257-02); false for a mesh whose rock slot would read them as shake (task-258-01). */
	UPROPERTY()
	bool bPulseData = false;
};

/**
 * One deposit: blocks of one type mined as one (ore-blocks, heal-ore). Ore table deposits are GroupSize x GroupSize squares
 * placed at build (OreTypes.csv); layout veins (ore or heal ore cells written in the layout) are their 4-connected groups.
 * A deposit is identified by its id (index in ARRArenaGrid::Deposits), never by neighbouring blocks: deposits may touch.
 */
USTRUCT(BlueprintType)
struct FRRGridDeposit
{
	GENERATED_BODY()

	/** Ore type row (OreTypes.csv row name) the deposit was placed from, or the first row of its block type for a layout vein; None when no row matches. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	FName OreId;

	/** Block type of every cell of the deposit. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	ERRCellType CellType = ERRCellType::Ore;

	/** Every cell of the deposit as built (destroyed cells stay listed; GetDepositCells filters them). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	TArray<FIntPoint> Cells;

	/** Ore value of each chunk dropped by a block of this deposit (row ChunkValue; 1 without a row). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	int32 ChunkValue = 1;

	/** Health restored by each heal pickup dropped by a block of this deposit (row HealValue; 0 without a row). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	float HealValue = 0.f;

	/** Ore colour of the row (written into the instance custom data when bHasColor). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	FLinearColor Color = FLinearColor::White;

	/** True when an ore type row with a colour (alpha above 0) gave the deposit its colour. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	bool bHasColor = false;

	/** True when the deposit came from the ore table placement (not from the layout file). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	bool bPlacedFromTable = false;

	/**
	 * Pickups already popped from the deposit (task-235-01, loot over the dig): out of ARRArenaGrid::GetDepositPickupCount;
	 * kept when the dig stops, so digging again resumes. Runtime state, 0 at build.
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	int32 PoppedPickups = 0;

	/** Blocks of the deposit destroyed so far (runtime state, 0 at build): a block broken outright owes its own pickups. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	int32 BrokenBlocks = 0;
};

/** One block an explosion broke or queued for breaking (QueueDestroyCellsInRadius): a barrel whose fuse it only lit is not one. */
struct FRRExplosionBrokenBlock
{
	/** Cell of the block. */
	FIntPoint Cell = FIntPoint::ZeroValue;
	/** What the cell held before the explosion. */
	ERRCellType Type = ERRCellType::Rock;
};

/** Fired when a block takes terrain damage without necessarily being destroyed: cell, block type, damage applied (dig-seconds), source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRROnCellDamagedSignature, FIntPoint, Cell, ERRCellType, Type, float, Amount, ERRTerrainDamageSource, Source);

/** Fired once when a block is destroyed (the cell is Floor afterwards): cell, block type it had, source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnCellDestroyedSignature, FIntPoint, Cell, ERRCellType, Type, ERRTerrainDamageSource, Source);

/**
 * Arena terrain built from a layout JSON (Content/RogueRigs/Data/Levels/<LayoutName>.json), or, on levels 2+ of a run, from
 * the layout the run subsystem prepared off the game thread (URRRunSubsystem::GetPreparedLayout, see LoadLayout).
 * Rebuilds in OnConstruction so the arena shows in the editor viewport, and again in BeginPlay so runtime
 * state (health, instance maps) always exists; OnConstruction is ignored once play has begun, so runtime
 * destruction is never wiped. Instances are split in chunks of ChunkCells x ChunkCells cells: one HISM per visual type
 * (floor and each block type) per chunk, created at build (FRRGridChunkVisual), so a destruction updates one small component;
 * the default-subobject HISMs and ISMs are empty templates that hold the per-type settings. Block HISMs only render, and each
 * block chunk HISM has a hidden collision ISM holding the same instances in the same order (same indices), always at rest, so
 * the dig shake moves the rendering only (see SetCellShaking). Floor HISMs render and collide. No grid component affects
 * navigation (robots use the flow field, the map has no navmesh). Grid origin is
 * the actor; the floor top is at the actor's Z; cell (x, y) center is at ((x + 0.5) * CellSize, (y + 0.5) * CellSize)
 * in actor space. A floor slab lies under every non-void cell (task-230-07). Rock display (dual grid, task-230-07): rock,
 * ore, heal ore and special ore cells have no per-cell rendering instance; one rock piece per cell corner (corner (x, y) at
 * (x * CellSize, y * CellSize)), chosen by its 4 surrounding cells, is drawn by per-chunk piece ISMs (one per piece set,
 * shape and variant, FRRGridPieceVisual); a destroyed cell rebuilds its 4 corner pieces. Collision stays one cube per cell.
 * Damage display (task-230-08): each piece carries per-quadrant custom data (damage, shake, marked, solid mask; layout at
 * PieceDamageCustomDataIndex) read by the piece material; a corner switches to the broken piece set at the first damage of
 * one of its cells; ore and heal ore cells also draw one crystal cluster each (OreCrystalMesh) in their own chunk HISMs, or,
 * when their type has deposit meshes (task-234-03), one crystal formation per deposit in the deposit crystal ISMs.
 * Terrain damage (dig) and destruction (explosions) go through ApplyTerrainDamage / DestroyCell / DestroyCellsInRadius.
 * Drops (task-235-01, loot over the dig): a deposit holds N = blocks x DestroyDropCount pickups (4 for a 2 x 2); while dug,
 * pickup k pops when the dig progress of the damaged block (all deposit blocks take the same drone damage) reaches k / N,
 * from the block it belongs to (Cells[k / DestroyDropCount]); a block destroyed outright owes its own share, and an explosion
 * pops all the remaining ones at once. Ore and heal ore blocks outside any deposit keep the former rule: one pickup per
 * multiple of DropInterval crossed while standing, DestroyDropCount when destroyed. Each pickup is worth its deposit's
 * ChunkValue / HealValue, spawned through URRPickupSubsystem (per-frame cap, queue).
 * Deposits (ore-blocks, owner 2026-10-10): the layout's ore and heal veins plus the OreTypes.csv deposits placed at build on
 * rock (BuildDeposits); every deposit has an id and is mined as one (GetDepositCells). Derive a Blueprint to assign meshes,
 * materials and pickup classes.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRArenaGrid : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root and the template components: one HISM per visual type and one hidden collision ISM per block type, none navigation relevant. Tick is enabled only while a block shakes. */
	ARRArenaGrid();

	/** Reloads the layout and rebuilds every instance. Runs on placement, move and property edit; ignored once play has begun. */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** Advances barrel fuses, destroys queued explosion blocks under the frame budget and advances the jitter of shaking blocks. */
	virtual void Tick(float DeltaSeconds) override;

	/** World-space center of cell (X, Y) at floor-top height. Cells outside the grid are not clamped. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FVector GetCellCenter(int32 X, int32 Y) const;

	/** Cell containing a world position (projected on the grid plane). May be outside the grid: check with GetCellType or IsBlockCell. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FIntPoint GetCellAtWorld(FVector WorldLocation) const;

	/**
	 * World-space center of the vertical face of the block in Cell that looks toward the neighbour Cell + Side,
	 * at half the block height above the floor top. Side is a unit grid step: (1, 0), (-1, 0), (0, 1) or (0, -1)
	 * (other values are reduced to their dominant axis). Does not check that Cell holds a block.
	 */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FVector GetCellFaceCenter(FIntPoint Cell, FIntPoint Side) const;

	/** Edge length of one cell in uu. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	float GetCellSize() const { return CellSize; }

	/** Layout file name without extension (Content/RogueRigs/Data/Levels/<LayoutName>.json); the run uses it for level 1 of a run started on this map. */
	const FString& GetLayoutName() const { return LayoutName; }

	/** Layout dimensions in cells (width, height); (0, 0) when no layout is loaded. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FIntPoint GetGridSize() const { return FIntPoint(GridWidth, GridHeight); }

	/** Height of a block above the floor top in uu. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	float GetBlockHeight() const { return BlockHeight; }

	/** Spawn cell read from the layout; (-1, -1) when no layout is loaded or it has no spawn. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FIntPoint GetSpawnCell() const;

	/** Current type of a cell (a destroyed block reads Floor); Void when outside the grid or when no layout is loaded. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	ERRCellType GetCellType(FIntPoint Cell) const;

	/** True when the cell currently holds a block (rock, ore, heal ore, special ore, barrel). */
	UFUNCTION(BlueprintPure, Category = "Grid")
	bool IsBlockCell(FIntPoint Cell) const;

	/**
	 * Deposit of Cell (ore-blocks, heal-ore: a deposit is mined as one): when Cell belongs to a deposit, every block of that
	 * deposit still standing, Cell included (by deposit id, never by neighbouring blocks); otherwise Cell alone when it is a
	 * block, else nothing. Returns the count. The drone expands each dug cell with it.
	 */
	int32 GetDepositCells(FIntPoint Cell, TArray<FIntPoint>& OutCells) const;

	/** Id of the deposit holding the standing block in Cell (index into the deposit list); INDEX_NONE for any other cell. */
	UFUNCTION(BlueprintPure, Category = "Grid|Deposits")
	int32 GetDepositId(FIntPoint Cell) const;

	/** Ore type row of the deposit holding Cell (e.g. Duskite); None when Cell is in no deposit or its deposit has no row. */
	UFUNCTION(BlueprintPure, Category = "Grid|Deposits")
	FName GetDepositOreId(FIntPoint Cell) const;

	/** True when Cell is a standing block of a deposit placed from the ore table (consoles never mark those). */
	UFUNCTION(BlueprintPure, Category = "Grid|Deposits")
	bool IsTableDepositCell(FIntPoint Cell) const;

	/** Pickups a deposit holds in all (task-235-01): its block count x DestroyDropCount; 0 for an invalid id. */
	int32 GetDepositPickupCount(int32 DepositId) const;

	/** Deposit DepositId as built, or nullptr for an invalid id. */
	const FRRGridDeposit* GetDeposit(int32 DepositId) const { return Deposits.IsValidIndex(DepositId) ? &Deposits[DepositId] : nullptr; }

	/** Number of deposits built (layout veins and table deposits), destroyed ones included. */
	int32 GetDepositCount() const { return Deposits.Num(); }

	/** Number of deposits placed from the ore table row OreId at the last build. */
	UFUNCTION(BlueprintPure, Category = "Grid|Deposits")
	int32 GetPlacedDepositCount(FName OreId) const;

	/** Index of the first of the 3 per-instance custom data floats (R, G, B) holding the ore colour on deposit block HISMs. */
	UFUNCTION(BlueprintPure, Category = "Grid|Deposits")
	int32 GetOreColorCustomDataIndex() const { return OreColorCustomDataIndex; }

	/** Path of the ore type table, relative to FPaths::ProjectContentDir(). */
	static const TCHAR* OreTypesPath;

	/** Copies row OreId of OreTypes.csv into OutRow. Returns false (logged) when the file or the row is missing. */
	static bool GetOreType(FName OreId, FRROreTypeRow& OutRow);

	/**
	 * Grid line of sight on the XY plane (Z ignored): distance from From along Direction to the first block cell crossed,
	 * capped at MaxDistance. 0 when From is inside a block cell. A ray passing exactly through a cell corner is blocked if
	 * either side cell is a block. Void and floor never block. Assumes an unscaled grid actor. Use it for sight checks and
	 * to clip effects at rock (shotgun wave).
	 */
	UFUNCTION(BlueprintPure, Category = "Grid|Sight")
	float GetClearDistance(FVector From, FVector Direction, float MaxDistance) const;

	/** True when no block cell lies between From and To on the XY plane (To's own cell included). See GetClearDistance. */
	UFUNCTION(BlueprintPure, Category = "Grid|Sight")
	bool HasLineOfSight(FVector From, FVector To) const;

	/** Remaining health of a block in dig-seconds at mining speed 1; 0 when the cell has no block. */
	UFUNCTION(BlueprintPure, Category = "Grid|Destruction")
	float GetCellHealth(FIntPoint Cell) const;

	/**
	 * Removes Amount dig-seconds of health from the block in Cell (the drone passes DeltaSeconds * MiningSpeed).
	 * Spawns the dig drops of ore and heal ore when the hit does not destroy the block, broadcasts OnCellDamaged,
	 * then destroys the block (DestroyCell) when its health reaches 0.
	 * Returns true when this call destroyed the block (or lit a barrel's fuse, see DestroyCell); false otherwise (no block,
	 * Amount <= 0, still standing, or a barrel already fusing: damage during the fuse is ignored).
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	bool ApplyTerrainDamage(FIntPoint Cell, float Amount, ERRTerrainDamageSource Source);

	/**
	 * Destroys the block in Cell outright, whatever its health: removes its collision (and barrel) instance, keeps the floor
	 * slab already under it, sets the cell to Floor, rebuilds its 4 corner rock pieces (rock piece types), spawns DestroyDropCount pickups for ore and heal ore, and broadcasts OnCellDestroyed.
	 * Returns false when the cell has no block.
	 * Barrels (task-114-01, barrel gdd): during play a barrel brought to 0 is not destroyed at once: it lights its fuse
	 * (BarrelFuseSeconds) and returns true; the cell, mesh and collision stay, a Danger ground telegraph is drawn at the
	 * explosion radius, further calls return false (ignored) until the fuse ends; then the explosion is queued and the cell
	 * is destroyed when that explosion resolves (it is inside its own radius). BarrelFuseSeconds 0, or outside a game world:
	 * destroyed at once and queued as before.
	 * Whole deposits (task-187-01, ore-blocks): when an explosion source (Barrel, Kamikaze) destroys a block of a deposit,
	 * the deposit's other standing blocks are destroyed in the same call, each with its own drops and OnCellDestroyed;
	 * Drone and Shot destroy the single block (the drone mines whole deposits itself).
	 * Deposit pickups (task-235-01): a destroyed deposit block pops what the deposit still owes for its broken blocks; an
	 * explosion source pops every remaining pickup of the deposit at once.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	bool DestroyCell(FIntPoint Cell, ERRTerrainDamageSource Source);

	/**
	 * Destroys every block whose cell center lies within Radius (horizontal distance) of Center, whatever its type
	 * or health (explosions rule). Returns the number of blocks destroyed (barrels whose fuse it lit included, and the
	 * blocks outside the radius of the deposits it breaks whole, task-187-01).
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	int32 DestroyCellsInRadius(FVector Center, float Radius, ERRTerrainDamageSource Source);

	/**
	 * Explosion terrain path (task-121-01): same cells as DestroyCellsInRadius, but the destruction is spread over frames.
	 * Barrels in the radius are handled at once (fuse lit, or the exploding barrel's own cell broken), so chains keep their
	 * timing; the other blocks are queued nearest to Center first and destroyed under CellDestructionBudgetMs of game-thread
	 * time per frame (at least one block per frame), starting in this call. A queued block stays a block for every reader
	 * until its frame. Budget 0 or outside a game world: destroyed at once (DestroyCellsInRadius). Returns the number of
	 * blocks handled at once or queued. OutBrokenBlocks, when given, receives the blocks of the radius that break by this call
	 * or by the queue, nearest first, barrels last; a barrel whose fuse this only lights stays out (it still stands), and so do
	 * the blocks outside the radius of a deposit broken whole.
	 */
	int32 QueueDestroyCellsInRadius(FVector Center, float Radius, ERRTerrainDamageSource Source, TArray<FRRExplosionBrokenBlock>* OutBrokenBlocks = nullptr);

	/**
	 * Extractor ground placement (task-372-01, destruction-rules item 4): destroys at once every block of the Size footprint
	 * whose lowest cell is MinCell (cells outside the grid ignored), with source Extractor. A deposit (ore or heal vein) touching
	 * the footprint breaks whole, every pickup it still owes appearing at rest on the floor of its own block (no pop arc); an ore
	 * or heal block outside any deposit leaves its DestroyDropCount pickups at rest the same way; a barrel is removed without
	 * exploding (a lit fuse is ended). OnCellDestroyed fires per block (run log source "extractor"). Logs one line on LogRogueRigs
	 * with Context and the counts by type, even when nothing was in the way. Call before the extractor is shown.
	 */
	FRRFootprintClearResult ClearExtractorFootprint(FIntPoint MinCell, FIntPoint Size, const FString& Context);

	/** Blocks queued by QueueDestroyCellsInRadius and not destroyed yet. */
	int32 GetPendingCellDestructionCount() const { return PendingCellDestructions.Num() - PendingCellDestructionHead; }

	/** Game-thread time per frame spent destroying queued explosion blocks, in ms (0 = no spreading). */
	float GetCellDestructionBudgetMs() const { return CellDestructionBudgetMs; }

	/** Sets CellDestructionBudgetMs (clamped at 0); for tests and tuning. */
	void SetCellDestructionBudgetMs(float BudgetMs) { CellDestructionBudgetMs = FMath::Max(0.f, BudgetMs); }

	/** Remaining shot health of a barrel cell (separate from its dig health); 0 when the cell holds no barrel. */
	UFUNCTION(BlueprintPure, Category = "Grid|Destruction")
	float GetCellShotHealth(FIntPoint Cell) const;

	/**
	 * Grid cell of a blocking hit on this grid: the cell of the hit instance when the hit component is a block collision
	 * ISM (or block HISM) with a valid Item, else the cell just behind the impact point (impact point minus 1 uu along the impact normal).
	 */
	FIntPoint GetCellFromHit(const FHitResult& Hit) const;

	/**
	 * Applies Damage from a projectile to the block in Cell. Only barrels take shot damage (shots never break other
	 * blocks): it lowers the barrel's shot health and destroys the cell (DestroyCell, source Shot) at 0, which queues its
	 * explosion (after the barrel fuse). Returns true when this call brought the barrel to 0 (fuse lit or destroyed); a
	 * fusing barrel ignores shots.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	bool ApplyShotDamage(FIntPoint Cell, float Damage);

	/**
	 * Explosion source object of the barrel in Cell: one per cell, created on first request and kept by the grid, so
	 * URRExplosionSubsystem lets each barrel explode only once. Returns nullptr when Cell is outside the grid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	URRBarrelSource* GetBarrelSource(FIntPoint Cell);

	/** Fuse of a barrel brought to 0 before it explodes, in seconds (BarrelFuseSeconds). */
	UFUNCTION(BlueprintPure, Category = "Grid|Destruction")
	float GetBarrelFuseSeconds() const { return BarrelFuseSeconds; }

	/** True while the barrel in Cell is fusing: at 0 health, still standing, its explosion not yet resolved. */
	UFUNCTION(BlueprintPure, Category = "Grid|Destruction")
	bool IsBarrelFusing(FIntPoint Cell) const { return BarrelFuses.Contains(Cell); }

	/** Elapsed fuse fraction of the barrel in Cell, 0..1 (1 once its explosion is queued); 0 when it is not fusing. */
	UFUNCTION(BlueprintPure, Category = "Grid|Destruction")
	float GetBarrelFuseProgress(FIntPoint Cell) const;

	/** Number of barrels currently fusing. */
	int32 GetFusingBarrelCount() const { return BarrelFuses.Num(); }

	/**
	 * Index of the per-instance custom data float of the barrel HISMs that holds the fuse progress (0..1, 0 when not fusing),
	 * written each grid tick so the barrel body material can blink (task-197-01). After the marked flag and the ore colour floats.
	 */
	UFUNCTION(BlueprintPure, Category = "Grid|Destruction")
	int32 GetBarrelFuseCustomDataIndex() const { return BarrelFuseCustomDataIndex; }

	/** Ground telegraph drawn for the fusing barrel in Cell; nullptr when it is not fusing. Pooled: do not keep it. */
	URRGroundTelegraphComponent* GetBarrelTelegraph(FIntPoint Cell) const;

	/** Telegraph components created so far for barrel fuses (in use and free); for tests and stress checks. */
	int32 GetBarrelTelegraphPoolSize() const { return BarrelTelegraphs.Num(); }

	/**
	 * Starts or stops the jitter of the block in Cell (dig feedback). Only the rendering instance moves (barrels, ore crystals);
	 * rock piece cells write the shake float of their quadrant in the 4 corner pieces instead (the material shakes, task-230-08).
	 * The collision instance stays at the cell, so a shaking block never pushes a character. Stopping restores the rest transform.
	 * Ignored on cells without a block. A fusing barrel keeps its fuse shake until it is destroyed: stopping it is ignored.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	void SetCellShaking(FIntPoint Cell, bool bShaking);

	/**
	 * Marks or unmarks the block in Cell as an objective rock (console marked rocks, pulsing white outline). Writes
	 * MarkedCustomDataValue / 0 into per-instance custom data float MarkedCustomDataIndex of the block's rendering HISM (a
	 * material reads it with PerInstanceCustomData), then calls OnCellMarkedChanged. Marking is refused on cells without a
	 * block; a destroyed marked block is unmarked by DestroyCell (event called with bMarked false). Returns true when the
	 * mark changed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Objectives")
	bool SetCellMarked(FIntPoint Cell, bool bMarked);

	/** True while the block in Cell is marked (SetCellMarked). */
	UFUNCTION(BlueprintPure, Category = "Grid|Objectives")
	bool IsCellMarked(FIntPoint Cell) const { return MarkedCells.Contains(Cell); }

	/** Every marked cell, in no particular order. */
	UFUNCTION(BlueprintPure, Category = "Grid|Objectives")
	TArray<FIntPoint> GetMarkedCells() const { return MarkedCells.Array(); }

	/** Index of the per-instance custom data float that holds the marked flag on every block HISM. */
	UFUNCTION(BlueprintPure, Category = "Grid|Objectives")
	int32 GetMarkedCustomDataIndex() const { return MarkedCustomDataIndex; }

	// --- Rock piece custom data (task-230-08, read by the rock piece material, task-230-09) ---
	// Every piece instance carries PieceCustomDataCount floats. Quadrant q of a piece at corner (x, y) is the cell
	// (x - 1 + (q & 1), y - 1 + (q >> 1)): q = (+X side) + 2 * (+Y side) in grid actor axes (not the piece's rotated local axes).

	/** First of 4 floats: display damage d per quadrant (GetCellDisplayDamage). */
	static constexpr int32 PieceDamageCustomDataIndex = 0;

	/** First of 4 floats: shake per quadrant, 0 when still, 1 + phase / (2 pi) in [1, 2) while the cell shakes. */
	static constexpr int32 PieceShakeCustomDataIndex = 4;

	/** First of 4 floats: marked outline per quadrant, MarkedCustomDataValue when the cell is marked, else 0. */
	static constexpr int32 PieceMarkedCustomDataIndex = 8;

	/** One float: solid mask of the 4 quadrants (bit q set when quadrant q is a rock piece cell), 0..15 as a float. */
	static constexpr int32 PieceSolidMaskCustomDataIndex = 12;

	/** Number of custom data floats per rock piece instance. */
	static constexpr int32 PieceCustomDataCount = 13;

	/**
	 * Display damage d of Cell for the rock pieces, 0..1. A rock piece cell (rock, ore, heal ore, special ore): 1 - health / max
	 * health (above 0 as soon as the block is below 100 %). Any other cell (open floor, barrel, void, outside the grid): the
	 * largest d of its rock piece 4-neighbours, 0 without one (provisional choice, as in the task-230-06 previews: the slumping
	 * wall next to a dug cell keeps the damage of its side).
	 */
	UFUNCTION(BlueprintPure, Category = "Grid|Destruction")
	float GetCellDisplayDamage(FIntPoint Cell) const;

	/** Pickup class this grid drops for a block type (OrePickupClass for Ore, HealPickupClass for HealOre, none otherwise); used by ARRBonusChest for its ore reward. */
	TSubclassOf<ARRPickup> GetDropPickupClass(ERRCellType Type) const { return GetPickupClass(Type); }

	/** Broadcast each time a block takes terrain damage (ApplyTerrainDamage), before a possible destruction. */
	UPROPERTY(BlueprintAssignable, Category = "Grid|Destruction")
	FRROnCellDamagedSignature OnCellDamaged;

	/** Broadcast once per destroyed block, after the cell has become Floor. */
	UPROPERTY(BlueprintAssignable, Category = "Grid|Destruction")
	FRROnCellDestroyedSignature OnCellDestroyed;

	/**
	 * Next run level prepared in the same world (task-318-01): rebuilds cells, deposits and instances from the layout of the run's
	 * current level (the prepared layout, as in BeginPlay), dropping fuses, queued destructions, marks and shaking. Returns false
	 * when the layout could not be loaded (the grid is then empty).
	 */
	bool RebuildForRunLevel();

protected:
	/** Rebuilds the grid from the layout so play starts from a fresh, fully indexed state (placed actors do not rerun construction in PIE or cooked builds); in a rendering game world, creates one hidden barrel telegraph so its material is warmed up before the first fuse. */
	virtual void BeginPlay() override;

	/** Drops the queued explosion blocks (level end, grid destroyed): nothing is destroyed after the grid leaves play. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Every block cell whose center lies within Radius (2D) of Center, in row-major order. */
	void CollectBlocksInRadius(const FVector& Center, float Radius, TArray<FIntPoint>& OutCells) const;

	/**
	 * Destroys queued explosion blocks in queue order until this frame's CellDestructionBudgetMs is spent (shared by every
	 * call in the same frame, GFrameCounter); at least one block per frame. Called by QueueDestroyCellsInRadius and Tick.
	 */
	void ProcessPendingCellDestructions();

	/** Empties the queue of explosion blocks (rebuild, end of play) and updates the tick. */
	void ClearPendingCellDestructions();

	/**
	 * Display event: the block in Cell was marked or unmarked (SetCellMarked, or a marked block destroyed). CellCenter is the
	 * world center of the cell on the floor. Implement in Blueprint for an outline that is not driven by the instance custom
	 * data (e.g. spawn and remove an outline mesh).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Grid|Objectives")
	void OnCellMarkedChanged(FIntPoint Cell, bool bMarked, FVector CellCenter);

	/** Writes Value into the marked custom data float of the rendering instance of the block in Cell (ore and heal crystals, barrels; no-op without one), and refreshes the custom data of its 4 corner pieces (rock piece cells). */
	void WriteMarkedCustomData(FIntPoint Cell, float Value);

	/** Writes Value into custom data float DataIndex of the rendering instance of the block in Cell (no-op without one or without that float). */
	void WriteCellCustomData(FIntPoint Cell, int32 DataIndex, float Value);

	/** Loads the layout and builds instances, or clears everything when loading fails. */
	void RebuildFromLayout();

	/**
	 * Builds Deposits and CellDepositId from the freshly loaded Cells: first the layout veins (4-connected Ore and HealOre
	 * groups, as the former vein flood fill), then, when bPlaceOreDeposits, every OreTypes.csv row with a RockShare: its
	 * deposits of GroupSize x GroupSize rock blocks (turned into the row CellType), chosen by a hash of the anchor cell, OreSeed
	 * and the ore id, so the result depends only on the layout, the table and the seed. Logs the count per ore type.
	 */
	void BuildDeposits();

	/** Places the deposits of one ore type row on rock blocks (see BuildDeposits); OriginalRockCount sets the target. Returns the count placed. */
	int32 PlaceOreDeposits(FName OreId, const FRROreTypeRow& Row, int32 OriginalRockCount);

	/** Writes the ore colour of every deposit with a row into the custom data of its rendering instances (OreColorCustomDataIndex). */
	void WriteDepositColors();

	/**
	 * Fills Cells, GridWidth, GridHeight and SpawnCell. In a game world with a run past level 1, from the layout the run
	 * subsystem prepared for the current level (FindRunPreparedLayout; LayoutName then takes its name); otherwise, or when
	 * none is prepared, by reading LayoutName's file synchronously. Returns false on any error (logged).
	 */
	bool LoadLayout();

	/**
	 * Prepared layout of the current run level from URRRunSubsystem (task-206-02): null outside a game world, without a run,
	 * on level 1 or when nothing was requested for the level; a request still in flight is waited for (logged warning).
	 */
	TSharedPtr<const FRRLayoutData> FindRunPreparedLayout() const;

	/** Copies a valid Layout into Cells, GridWidth, GridHeight and SpawnCell. */
	void ApplyLayoutData(const FRRLayoutData& Layout);

	/** Destroys the chunk components, applies meshes and materials to the templates, creates the chunk components and adds floor and block instances from Cells, and resets health, maps and shaking. */
	void BuildInstances();

	/** Destroys every chunk component, empties the template components and resets the instance maps. */
	void ClearAllInstances();

	/** True for the block types (Rock, Ore, HealOre, SpecialOre, Barrel). */
	static bool IsBlockType(ERRCellType Type);

	/** Template HISM of a visual type (FloorInstances for Floor, the block HISM for a block type); nullptr for Void and Spawn. Holds no instances. */
	UHierarchicalInstancedStaticMeshComponent* GetTemplateComponent(ERRCellType MapKey) const;

	/** Template collision ISM of a block type; nullptr for non-block types. Holds no instances. */
	UInstancedStaticMeshComponent* GetTemplateCollisionComponent(ERRCellType MapKey) const;

	/** Slot of a visual type in ChunkVisuals (Floor 0, Rock 1, Ore 2, HealOre 3, SpecialOre 4, Barrel 5); INDEX_NONE for Void and Spawn. */
	static int32 GetVisualSlot(ERRCellType MapKey);

	/** Index in ChunkVisuals of visual type MapKey in the chunk holding Cell; INDEX_NONE when either is invalid. */
	int32 GetVisualIndex(ERRCellType MapKey, FIntPoint Cell) const;

	/** Visual type currently rendering Cell: its block type, Floor for an open cell, Void (no visual) for a void cell. */
	ERRCellType GetCellMapKey(FIntPoint Cell) const;

	/** Rendering HISM of the chunk visual that holds Cell's current instance; nullptr when none. */
	UHierarchicalInstancedStaticMeshComponent* GetCellRenderComponent(FIntPoint Cell) const;

	/** Chunk visual of type MapKey for the chunk holding Cell, its components created on first use; nullptr for an invalid type or cell. */
	FRRGridChunkVisual* GetOrCreateVisual(ERRCellType MapKey, FIntPoint Cell);

	/** Creates one chunk component of Class, copying the settings of Template, attached to the root (not navigation relevant); floor and terrain block render components (floor, rock, ore, heal ore, special ore templates) go on lighting channels 0 and 1 for the miner's floor-and-walls light, barrels and collision ISMs keep the template's channels; registers it when bRegister (callers that change settings first register it themselves). */
	UInstancedStaticMeshComponent* CreateChunkComponent(TSubclassOf<UInstancedStaticMeshComponent> Class, const UInstancedStaticMeshComponent* Template, FIntPoint Chunk, bool bRegister = true);

	/** Starting health of a block type in dig-seconds; 0 for non-block types. */
	float GetBlockMaxHealth(ERRCellType Type) const;

	/** Creates one HISM default subobject attached to the root: block-all collision when bWithCollision, else render only. */
	UHierarchicalInstancedStaticMeshComponent* CreateGridComponent(FName Name, bool bWithCollision);

	/** Creates one hidden, shadowless ISM default subobject with block-all collision and remove-at-swap, attached to the root. */
	UInstancedStaticMeshComponent* CreateCollisionComponent(FName Name);

	/** Absolute path of the layout file for LayoutName. */
	FString GetLayoutFilePath() const;

	/** True when Cell is inside the loaded grid. */
	bool IsValidCell(FIntPoint Cell) const;

	/** Row-major index of a valid cell. */
	int32 GetCellIndex(FIntPoint Cell) const { return Cell.Y * GridWidth + Cell.X; }

	/** Actor-space rest transform of the floor slab instance of a cell. */
	FTransform GetFloorInstanceTransform(FIntPoint Cell) const;

	/** Actor-space rest transform of the block instance of a cell: the MeshBaseSize cube filling the block volume. Collision instances always use it. */
	FTransform GetBlockInstanceTransform(FIntPoint Cell) const;

	/**
	 * Actor-space rest transform of the rendering instance of a block cell. Barrels: BarrelMesh uniformly scaled from its bounds to fit
	 * CellSize in width and BlockHeight in height (smaller factor), bounds bottom centre on the cell centre at the floor top (z = 0:
	 * a barrel cell keeps its floor slab and the barrel stands on it). Other types, or a null / boundless BarrelMesh: GetBlockInstanceTransform.
	 */
	FTransform GetBlockRenderTransform(FIntPoint Cell) const;

	/** Adds an instance for Cell to the rendering component of MapKey in Cell's chunk (and to its collision ISM for a block) and records it in the chunk map; marks the rendering component's render state dirty so the new instance is drawn on this frame (task-202-01). Returns the instance index, or INDEX_NONE. */
	int32 AddCellInstance(ERRCellType MapKey, FIntPoint Cell, const FTransform& LocalTransform);

	/** Removes Cell's instance InstanceIndex from the rendering component of MapKey in Cell's chunk (and its collision ISM) and patches both maps for the instance moved into the freed slot. */
	void RemoveCellInstance(ERRCellType MapKey, FIntPoint Cell, int32 InstanceIndex);

	/** Cell -> instance index map of a visual type: CellFloorInstanceIndex for Floor, CellInstanceIndex for a block type. */
	TArray<int32>& GetInstanceIndexMap(ERRCellType MapKey);

	/** True when a cell of this type has a floor slab instance: every non-void cell since task-230-07 (rock pieces stand on the floor top, rock-render-piece-set). */
	static bool HasFloorSlab(ERRCellType Type);

	// --- Dual-grid rock pieces (task-230-07, rock-render-piece-set) ---

	/** Number of ERRRockPieceSet values (intact, broken). */
	static constexpr int32 RockPieceSetCount = 2;

	/** Number of ERRRockPieceShape values (full, edge, outer, inner, diagonal). */
	static constexpr int32 RockPieceShapeCount = 5;

	/** Builds the piece table, sizes the corner maps and PieceVisuals, and adds one intact piece per non-empty corner in one batch per ISM. Called by BuildInstances after the cell instances. */
	void BuildCornerPieces();

	/** True for the cell types drawn by the dual-grid rock pieces (solid for the corner masks): Rock, Ore, HealOre, SpecialOre. Barrels are not: they keep their own mesh on a floor slab. */
	static bool IsRockPieceType(ERRCellType Type);

	/** True when Cell is inside the grid and currently holds a rock piece type. */
	bool IsRockPieceCell(FIntPoint Cell) const;

	/** Corner index of Corner (row-major over (GridWidth + 1) x (GridHeight + 1) corners); INDEX_NONE outside. */
	int32 GetCornerIndex(FIntPoint Corner) const;

	/** Corner of a valid corner index. */
	FIntPoint GetCornerFromIndex(int32 CornerIndex) const { return FIntPoint(CornerIndex % (GridWidth + 1), CornerIndex / (GridWidth + 1)); }

	/** 4-bit solid mask of the cells around Corner: bit 0 cell (x-1, y-1), bit 1 (x, y-1), bit 2 (x-1, y), bit 3 (x, y), i.e. bit = (+X side) | (+Y side) << 1. */
	uint8 GetCornerMask(FIntPoint Corner) const;

	/** Fills PieceShapeByMask and PieceTurnsByMask from the canonical rock corners of each shape (Y mirrored when bRockPieceImportFlipsY), by trying the 4 yaw quarter turns. */
	void BuildPieceTable();

	/** Variant of the piece of Set and Shape at Corner: a hash of the corner and RockPieceSeed modulo the variant count of that set and shape (at least 1). */
	int32 GetPieceVariant(FIntPoint Corner, ERRRockPieceShape Shape, ERRRockPieceSet Set = ERRRockPieceSet::Intact) const;

	/** Mesh lists of a piece set: RockPieceMeshes or RockPieceBrokenMeshes. */
	const FRRRockPieceMeshes& GetPieceMeshSet(ERRRockPieceSet Set) const { return Set == ERRRockPieceSet::Broken ? RockPieceBrokenMeshes : RockPieceMeshes; }

	/** Yaw quarter turns of the piece at Corner for Mask: the table value, or a hash-chosen turn for the full piece (random rotation only on full). */
	int32 GetPieceTurns(FIntPoint Corner, uint8 Mask) const;

	/** Mesh of a piece set, shape and variant; nullptr when unset (tolerated: instances are kept, nothing is drawn). */
	UStaticMesh* GetPieceMesh(ERRRockPieceSet Set, ERRRockPieceShape Shape, int32 Variant) const;

	/** Number of variants of a piece set and shape (mesh list length, at least 1). */
	int32 GetPieceVariantCount(ERRRockPieceSet Set, ERRRockPieceShape Shape) const;

	/** Actor-space transform of a piece at Corner: corner position on the floor top, Turns x 90 degrees of yaw, scaled from RockPieceMeshSize and RockPieceMeshHeight to CellSize and BlockHeight. */
	FTransform GetCornerPieceTransform(FIntPoint Corner, int32 Turns) const;

	/** Index in PieceVisuals of a piece set, shape and variant in the chunk of Corner; INDEX_NONE when invalid. */
	int32 GetPieceVisualIndex(ERRRockPieceSet Set, ERRRockPieceShape Shape, int32 Variant, FIntPoint Corner) const;

	/** Piece visual at VisualIndex with its ISM created on first use (settings of the RockInstances template, piece mesh, RockPieceMaterial or RockPieceBrokenMaterial, PieceCustomDataCount custom data floats, no collision, Nanite allowed when bRockPiecesUseNanite, and for the broken set only when bBrokenRockPiecesUseNanite). */
	FRRGridPieceVisual* GetOrCreatePieceVisual(int32 VisualIndex);

	/** Sets the piece of Corner from the current cells and its broken flag: removes its old instance and adds the new set and shape (nothing when the mask is empty); a same-visual change only updates the transform; then writes its custom data. */
	void RefreshCornerPiece(FIntPoint Corner);

	/** Removes the piece instance of a corner index and patches the corner maps for the instance moved into the freed slot (remove at swap). */
	void RemoveCornerPiece(int32 CornerIndex);

	/** Refreshes the 4 corner pieces of Cell (called when the cell stops being solid). */
	void UpdateCellCornerPieces(FIntPoint Cell);

	// --- Rock piece damage display (task-230-08, rock-render-damage) ---

	/** Display damage of a rock piece cell, 1 - health / max health clamped to 0..1; 0 for any other cell. */
	float GetSolidCellDamage(FIntPoint Cell) const;

	/** Fills the PieceCustomDataCount floats of the piece at Corner from the current cells (damage, shake, marked, solid mask). */
	void ComputeCornerPieceData(FIntPoint Corner, float (&OutData)[PieceCustomDataCount]) const;

	/** Writes the custom data of the piece of a corner index (no-op for an empty corner or an ISM without the floats); only writes when a value changed, and then marks the ISM render state dirty. */
	void WriteCornerPieceData(int32 CornerIndex);

	/** Rewrites the custom data of the 4 corner pieces of Cell (shake or mark change of that cell). */
	void RefreshCellCornerPieceData(FIntPoint Cell);

	/** Rewrites the custom data of every piece whose quadrants include Cell or one of its 4-neighbours (corners x - 1 .. x + 2, y - 1 .. y + 2): the damage or solidity of Cell changed, which also changes the derived damage of its empty neighbours. */
	void RefreshPieceDataAround(FIntPoint Cell);

	/** Switches the 4 corner pieces of Cell to the broken set once Cell has display damage above 0 (sticky, CornerBroken). */
	void BreakCellCornerPieces(FIntPoint Cell);

	/** True for the block types with a per-cell rendering instance: barrels (their mesh), ore and heal ore (crystal clusters, task-230-08) unless their deposits draw one crystal each (UsesDepositCrystals, task-234-03). Rock and special ore have only their collision instance. */
	bool HasCellRenderInstance(ERRCellType Type) const;

	// --- Deposit crystals (task-234-03, ore-blocks: a 2 x 2 deposit is one entity, also in its look) ---

	/** True when the deposits of block type Type draw one crystal formation each: Ore or HealOre with at least one non-null mesh in OreDepositMeshes / HealDepositMeshes. Otherwise the type keeps one crystal cluster per cell. */
	bool UsesDepositCrystals(ERRCellType Type) const;

	/** Non-null meshes of the deposit mesh list of Type (OreDepositMeshes for Ore, HealDepositMeshes for HealOre), in list order; empty for other types. */
	void GetDepositMeshes(ERRCellType Type, TArray<UStaticMesh*>& OutMeshes) const;

	/** Override material of the deposit crystals of Type (OreDepositMaterial or HealDepositMaterial), applied to the DepositCrystalSlotName slot only; null keeps the mesh materials. */
	UMaterialInterface* GetDepositMaterial(ERRCellType Type) const;

	/** Hash of a deposit id and DepositCrystalSeed: the mesh variant is Hash % mesh count, the yaw quarter turns a second mix of it. */
	uint32 GetDepositCrystalHash(int32 DepositId) const;

	/** Actor-space transform of the crystal of a deposit: centre of its cells' bounding box on the block top (the mesh pivot is its footprint centre, base at z 0), hashed yaw quarter turn, uniform DepositCrystalScale. */
	FTransform GetDepositCrystalTransform(int32 DepositId) const;

	/** Adds one crystal instance per deposit of a type that UsesDepositCrystals, batched per type and variant, with its marked and ore colour data. Called by BuildInstances. */
	void BuildDepositCrystals();

	/** Removes the crystal of DepositId (no-op without one) and patches the maps for the instance moved into the freed slot (remove at swap). */
	void RemoveDepositCrystal(int32 DepositId);

	/** Deposit crystal ISM of DepositId and its instance in OutInstance; nullptr when the deposit has no crystal. */
	UInstancedStaticMeshComponent* GetDepositCrystalComponent(int32 DepositId, int32& OutInstance) const;

	/** Writes the marked float (MarkedCustomDataValue when one standing block of the deposit is marked, else 0) and the ore colour (when the deposit has one) into the crystal of DepositId; marks the ISM render state dirty when bMarkDirty. */
	void WriteDepositCrystalData(int32 DepositId, bool bMarkDirty);

	/** Custom data floats a per-cell crystal cluster HISM needs for the pulse data: max(PulsePhaseCustomDataIndex, PulseRateCustomDataIndex) + 1 (6 by default). */
	int32 GetCrystalPulseFloatCount() const { return FMath::Max(PulsePhaseCustomDataIndex, PulseRateCustomDataIndex) + 1; }

	/** task-267-01: custom data floats a deposit formation ISM needs for the pulse data: max(DepositPulsePhaseCustomDataIndex, DepositPulseRateCustomDataIndex) + 1 (15 by default). */
	int32 GetDepositPulseFloatCount() const { return FMath::Max(DepositPulsePhaseCustomDataIndex, DepositPulseRateCustomDataIndex) + 1; }

	/**
	 * task-258-01: true when Mesh has a slot other than DepositCrystalSlotName (the `rock` slot of the heal deposit meshes,
	 * MI_RockSlab). Every slot of an ISM reads the same per-instance custom data, and M_RockSlab reads it with the rock piece
	 * layout (0-3 damage, 4-7 shake: a value of 1 or more shakes its quadrant, 8-11 marked, 12 solid mask).
	 */
	bool DepositMeshHasRockSlot(const UStaticMesh* Mesh) const;

	/**
	 * task-258-01: whether the formation ISM of Mesh carries the pulse data: always for a crystal-only mesh; for a mesh with a
	 * rock slot only when both deposit pulse indices are at or above PieceCustomDataCount (indices the rock never reads). Since
	 * task-267-01 the deposit indices are 13 and 14, so the heal deposits carry it again (with 0 at the rock floats 4-12).
	 */
	bool CanWriteDepositPulseData(const UStaticMesh* Mesh) const;

	/**
	 * task-257-02: writes FRRPulseDesync phase and rate of (PulseSeed, Domain, Id) to Instance of Component at PhaseIndex /
	 * RateIndex (no render state dirty; skipped when the component has too few floats). Per-cell clusters pass
	 * PulsePhaseCustomDataIndex / PulseRateCustomDataIndex, deposit formations the Deposit* indices (task-267-01).
	 */
	void WriteCrystalPulseData(UInstancedStaticMeshComponent* Component, int32 Instance, uint32 Domain, uint32 Id, int32 PhaseIndex, int32 RateIndex);

	/** task-257-02: pulse data of every per-cell ore and heal ore crystal cluster (types without deposit meshes), id = cell index. Called by BuildInstances. */
	void WriteCellCrystalPulseData();

	/** True when a standing block of DepositId is in ShakingCells. */
	bool IsDepositShaking(int32 DepositId) const;

	/** Actor-space transform of the crystal cluster of an ore or heal ore cell: cell centre, bottom of the mesh bounds on the block top, uniform OreCrystalScale, yaw hashed from the cell and RockPieceSeed. */
	FTransform GetCrystalTransform(FIntPoint Cell) const;

	/** Stops shaking Cell, restoring its rest transform if it still has a block. */
	void StopShaking(FIntPoint Cell);

	/** Deterministic shake phase of Cell (0..2 pi), so adjacent shaking blocks do not move in lockstep. */
	static float GetShakePhase(FIntPoint Cell);

	/** Pickup class dropped by a block type: OrePickupClass for Ore, HealPickupClass for HealOre, none otherwise. */
	TSubclassOf<ARRPickup> GetPickupClass(ERRCellType Type) const;

	/**
	 * Spawns the dig drops owed by a damaged, still standing block: one per multiple of DropInterval crossed by its
	 * accumulated damage, capped so the multiple equal to its max health (the destroying hit) never drops.
	 */
	void SpawnDigDrops(FIntPoint Cell, ERRCellType Type);

	/**
	 * Pops the pickups of deposit DepositId until PoppedPickups reaches Target (clamped to GetDepositPickupCount), pickup k
	 * from the centre of its block Cells[k / DestroyDropCount]; Progress and Cause are only logged (Log for a dig, Verbose
	 * otherwise, with the world time). bAtRest: the pickups appear at rest on their block's floor instead of popping (source Extractor).
	 */
	void PopDepositPickups(int32 DepositId, int32 Target, float Progress, const TCHAR* Cause, bool bAtRest = false);

	/** Queues the explosion of the barrel in Cell (row BarrelExplosionRow, at the cell center, source GetBarrelSource) in the world's URRExplosionSubsystem, during play only. Returns true when queued. */
	bool QueueBarrelExplosion(FIntPoint Cell);

	/** Fuse state of one barrel brought to 0 (BarrelFuses). The telegraph is kept alive by BarrelTelegraphs. */
	struct FBarrelFuse
	{
		/** World time when the fuse was lit. */
		double StartTime = 0.0;
		/** True once the fuse ended and the explosion was queued: the next DestroyCell on the cell destroys it for real. */
		bool bExplosionQueued = false;
		/** Danger ground telegraph at the cell centre, from the pool. */
		URRGroundTelegraphComponent* Telegraph = nullptr;
	};

	/** Lights the fuse of the barrel in Cell: starts its fuse shake, zeroes its healths, records the start time, writes fuse progress 0 and shows a pooled Danger telegraph at the explosion radius. */
	void StartBarrelFuse(FIntPoint Cell);

	/** Ends the fuse of Cell (if any): stops its shake, writes fuse progress 0, hides its telegraph and returns it to the pool. */
	void EndBarrelFuse(FIntPoint Cell);

	/** Creates a hidden barrel telegraph registered on this actor and adds it to BarrelTelegraphs (the caller puts it in use or in FreeBarrelTelegraphs). */
	URRGroundTelegraphComponent* CreateBarrelTelegraph();

	/** Ends every fuse (rebuild). */
	void ClearBarrelFuses();

	/** Advances every fuse: telegraph progress and fuse custom data = elapsed / BarrelFuseSeconds; at the end queues the explosion (destroys the cell at once if it cannot be queued). */
	void TickBarrelFuses();

	/** Enables the actor tick while a block shakes, a barrel fuses or explosion blocks are queued; disables it otherwise. */
	void UpdateTickEnabled();

	/** Radius of the barrel explosion row (BarrelExplosionRow), 0 when it cannot be read. */
	float GetBarrelExplosionRadius() const;

	/**
	 * Requests Count pickups of the class dropped by Type at the center of Cell (floor top), each worth OreValue (ore) and
	 * HealValue (heal, 0 = pickup default), during play only, through URRPickupSubsystem::RequestSpawn (at most its
	 * MaxSpawnsPerFrame per frame, the rest queued). bAtRest (source Extractor, task-372-01): each pickup starts its life at rest
	 * on that floor (ARRPickup::SetSpawnAtRest) instead of popping. Adds Count to PickupRequestCount when it requests.
	 */
	void SpawnPickups(FIntPoint Cell, ERRCellType Type, int32 Count, int32 OreValue = 1, float HealValue = 0.f, bool bAtRest = false);

	/** True for the explosion terrain sources (Barrel, Kamikaze): they break a touched deposit whole (DestroyCell). */
	static bool IsExplosionSource(ERRTerrainDamageSource Source);

	/** True for the sources that break a touched deposit whole and pop all its remaining pickups: the explosion sources and Extractor (task-372-01). */
	static bool BreaksDepositWhole(ERRTerrainDamageSource Source);

	/** Pickups requested by SpawnPickups since the grid was spawned; ClearExtractorFootprint counts its drops with it. */
	int32 PickupRequestCount = 0;

	/**
	 * Destroys the single block in Cell (DestroyCell without the whole-deposit rule): barrel fuse, instances, floor, drops,
	 * unmark, OnCellDestroyed. Returns false when the cell has no block or a fusing barrel ignores it.
	 */
	bool DestroySingleCell(FIntPoint Cell, ERRTerrainDamageSource Source);

	/** Blocks destroyed (or barrel fuses lit) by DestroySingleCell since the grid was spawned; DestroyCellsInRadius counts with it. */
	int32 DestroyedBlockCount = 0;

	// --- Layout and dimensions ---

	/** Layout file name without extension, read from Content/RogueRigs/Data/Levels/<LayoutName>.json. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	FString LayoutName = TEXT("arena_m1_fine");

	/** Edge length of one cell in Unreal units (67 since task-029-04: 240 x 240 cells cover the former 80 x 80 cells of 200 uu). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = "1.0", Units = "cm"))
	float CellSize = 67.f;

	/** Height of a block above the floor top (provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = "1.0", Units = "cm"))
	float BlockHeight = 100.f;

	/** Thickness of the floor slab under the floor top; thick so the asteroid edge reads as a cliff. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = "1.0", Units = "cm"))
	float FloorThickness = 200.f;

	/**
	 * When true, a block collision instance (and a barrel without BarrelMesh) extends down through the floor slab (bottom at
	 * -FloorThickness). False (default since task-230-07): blocks stand on the floor top (bottom at the actor's Z), on the
	 * floor slab every non-void cell has, so the rounded rock outline never shows the void (rock-render-piece-set).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	bool bBlocksFillFloorSlab = false;

	/** Edge length of the source meshes (the engine cube is 100 uu, pivot at its center). Used to scale instances. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = "1.0", Units = "cm"))
	float MeshBaseSize = 100.f;

	/** Side of a render chunk in cells: each visual type gets one HISM (and blocks one collision ISM) per ChunkCells x ChunkCells cells, so a destruction rebuilds one small component. Read at build. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rendering", meta = (ClampMin = "1"))
	int32 ChunkCells = 16;

	/** Whether the floor and block chunk HISMs feed the mesh distance field scene (Lumen, distance field shadows and AO). On by default: with chunks, turning it off measured no gain (task-029-01). Read at build. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rendering")
	bool bGridAffectsDistanceFields = true;

	// --- Block health, in dig-seconds at mining speed 1 (provisional) ---

	/** Health of a rock block: drone dig time in seconds at mining speed 1 (drone gdd: 0.65 s per 67 uu block, owner 2026-10-20, provisional; was 0.5). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float RockHealth = 0.65f;

	/** Health of an ore block: drone dig time in seconds at mining speed 1 (ore-blocks gdd: 2 s per 67 uu block, owner 2026-10-20, provisional; was 1.5). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float OreHealth = 2.f;

	/** Health of a heal ore block: drone dig time in seconds at mining speed 1 (heal-ore gdd: 2 s per 67 uu block, owner 2026-10-20, provisional; was 1.5). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float HealOreHealth = 2.f;

	/** Health of a special ore block: drone dig time in seconds at mining speed 1 (same as ore: 2 s, was 1.5). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float SpecialOreHealth = 2.f;

	/** Health of a barrel block: drone dig time in seconds at mining speed 1 (0.5 s, provisional; was equal to rock before rock went to 0.65 s in task-260-01). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float BarrelHealth = 0.5f;

	/** Health of a barrel against shots (projectile damage, e.g. 10 per rifle bullet); separate from its dig health (provisional, barrel gdd). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01"))
	float BarrelShotHealth = 20.f;

	/** Row of ExplosionStats.csv queued when a barrel is destroyed, whatever the source. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction")
	FName BarrelExplosionRow = TEXT("Barrel");

	/** Delay between a barrel reaching 0 health (any source) and its explosion, in seconds (owner 2026-10-12, barrel gdd). 0 = explodes at once. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "s"))
	float BarrelFuseSeconds = 1.5f;

	/**
	 * Game-thread time per frame for destroying the blocks of explosions, in ms (task-121-01: a 700 uu barrel explosion cost
	 * about 8 ms in one frame). The blocks of an explosion are destroyed nearest first over as many frames as needed, at
	 * least one per frame. 0 = every block destroyed in the explosion frame (former behaviour).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "ms"))
	float CellDestructionBudgetMs = 2.f;

	// --- Shake (dig feedback) ---

	/** Maximum horizontal offset of a shaking block from its rest position (8, owner 2026-10-19, task-231; was 25). Mirrored by MI_RockSlab ShakeParams r for rock pieces; ore and heal crystals move their instance by it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float ShakeAmplitude = 8.f;

	/** Maximum vertical offset (bob) of a shaking block from its rest position, in the same jitter (3, owner 2026-10-19, task-231; was 8). Mirrored by MI_RockSlab ShakeParams g. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float ShakeVerticalAmplitude = 3.f;

	/** Oscillation frequency of a shaking block. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "Hz"))
	float ShakeFrequency = 8.f;

	// --- Barrel fuse feedback (render only, task-197-01; provisional, owner 2026-10-16) ---

	/** Horizontal shake amplitude of a fusing barrel when its fuse is lit (progress 0). Replaces the dig shake during the fuse. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float BarrelFuseShakeStartAmplitude = 2.f;

	/** Horizontal shake amplitude of a fusing barrel at the explosion (progress 1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float BarrelFuseShakeEndAmplitude = 12.f;

	/** Vertical shake amplitude (bob) of a fusing barrel when its fuse is lit (progress 0). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float BarrelFuseShakeVerticalStartAmplitude = 1.f;

	/** Vertical shake amplitude (bob) of a fusing barrel at the explosion (progress 1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float BarrelFuseShakeVerticalEndAmplitude = 5.f;

	/** Growth curve of the fuse shake: amplitude = lerp(start, end, progress ^ exponent); 1 = linear, above 1 = grows faster near the end. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.1"))
	float BarrelFuseShakeExponent = 2.f;

	/** Per-instance custom data float of the barrel HISMs that receives the fuse progress (after marked flag 0 and ore colour 1..3); the barrel HISM gets index + 1 floats. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0", ClampMax = "7"))
	int32 BarrelFuseCustomDataIndex = 4;

	// --- Drops (ore and heal ore) ---

	/** Pickup spawned by ore blocks (a Blueprint subclass of ARRPickup with type Ore). None = ore drops nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops")
	TSubclassOf<ARRPickup> OrePickupClass;

	/** Pickup spawned by heal ore blocks (a Blueprint subclass of ARRPickup with type Heal). None = heal ore drops nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops")
	TSubclassOf<ARRPickup> HealPickupClass;

	/** Accumulated damage, in dig-seconds, between two drops while a block outside any deposit is dug (provisional). Kept equal to the ore health (2, was 1.5; task-259-01) so such a block drops nothing during the dig and DestroyDropCount at destruction. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops", meta = (ClampMin = "0.01", Units = "s"))
	float DropInterval = 2.f;

	/** Pickups spawned when an ore or heal ore block is destroyed, whatever the source (ore-blocks gdd: one chunk per broken 67 uu block, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops", meta = (ClampMin = "0"))
	int32 DestroyDropCount = 1;

	// --- Ore deposits (ore-blocks, owner 2026-10-10) ---

	/** When true, the build places the ore table deposits (OreTypes.csv) on the layout rock. Layout veins are deposits either way. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Deposits")
	bool bPlaceOreDeposits = true;

	/** Seed of the deterministic deposit placement: same layout, table and seed give the same deposits. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Deposits")
	int32 OreSeed = 1;

	/** First of the 3 per-instance custom data floats (R, G, B) receiving the ore colour on the HISMs of deposit block types with a row (after the marked flag). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Deposits", meta = (ClampMin = "0", ClampMax = "5"))
	int32 OreColorCustomDataIndex = 1;

	/** Per-instance custom data float of the block HISMs that holds the marked flag (each block HISM gets index + 1 floats). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Objectives", meta = (ClampMin = "0", ClampMax = "7"))
	int32 MarkedCustomDataIndex = 0;

	/** Value written into the marked custom data float of a marked block (0 when unmarked). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Objectives")
	float MarkedCustomDataValue = 1.f;

	// --- Visuals per type (assigned in the derived Blueprint) ---

	/** Mesh of the floor slab under every open (non-void, non-block) cell. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> FloorMesh;

	/** Material of the floor slab. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> FloorMaterial;

	/** Collision shape of rock blocks (the hidden collision ISM, scaled like a MeshBaseSize cube). Since task-230-07 rock is drawn by the rock pieces, not by this mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> RockMesh;

	/** Material of the RockInstances template (unused for drawing since task-230-07: the rock pieces clear it and use RockPieceMaterial). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> RockMaterial;

	/** Collision shape of ore blocks. Since task-230-07 ore cells are drawn as rock pieces plus a crystal cluster (OreCrystalMesh, task-230-08). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> OreMesh;

	/** Unused since task-230-08: the OreInstances template draws the crystal clusters with OreCrystalMaterial. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> OreMaterial;

	/** Collision shape of healing ore blocks. Since task-230-07 heal ore cells are drawn as rock pieces. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> HealOreMesh;

	/** Unused since task-230-08: the HealOreInstances template draws the crystal clusters with HealOreCrystalMaterial. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> HealOreMaterial;

	/** Collision shape of special ore blocks. Since task-230-07 special ore cells are drawn as rock pieces. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> SpecialOreMesh;

	/** Material of the SpecialOreInstances template (no per-cell special ore drawing since task-230-07). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> SpecialOreMaterial;

	// --- Dual-grid rock pieces (task-230-07; meshes assigned in the Blueprint, task-230-11) ---

	/** Intact rock piece meshes per shape, one entry per variant (variant chosen per corner by hash). Unset meshes are tolerated (pieces tracked, not drawn). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	FRRRockPieceMeshes RockPieceMeshes;

	/** Material of every rock piece ISM (M_RockSlab instance, task-230-09); null keeps the material of each piece mesh. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	TObjectPtr<UMaterialInterface> RockPieceMaterial;

	/** Horizontal size of the source piece meshes (SM_RockSlab.report.json: 67 uu); pieces are scaled by CellSize / this. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rock Pieces", meta = (ClampMin = "1.0", Units = "cm"))
	float RockPieceMeshSize = 67.f;

	/** Top height of the source piece meshes above their pivot (SM_RockSlab.report.json: 100 uu); pieces are scaled by BlockHeight / this. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rock Pieces", meta = (ClampMin = "1.0", Units = "cm"))
	float RockPieceMeshHeight = 100.f;

	/**
	 * True when the imported piece meshes are the Blender canonical pieces mirrored on Y (the FBX import flips Y; provisional:
	 * SM_RockSlab.report.json asks to verify the first placement, task-230-12). Read at build to orient the pieces.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	bool bRockPieceImportFlipsY = true;

	/** Broken rock piece meshes per shape (SM_RockSlabBroken_<Shape>_01 and _02, task-230-11), same canonical orientation as the intact pieces; swapped in around damaged cells. Unset meshes are tolerated (pieces tracked, not drawn). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	FRRRockPieceMeshes RockPieceBrokenMeshes;

	/** Material of the broken piece ISMs (sag by world position offset, task-230-09); null falls back to RockPieceMaterial, then to the mesh material. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	TObjectPtr<UMaterialInterface> RockPieceBrokenMaterial;

	/** Crystal cluster drawn on each ore and heal ore cell (rock-render-damage; Duskite: SM_OreChunk_01), one instance per cell in the ore and heal ore chunk HISMs. Null tolerated (instances tracked, not drawn). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Crystals")
	TObjectPtr<UStaticMesh> OreCrystalMesh;

	/** Material of the ore crystal clusters, applied to every slot; null keeps the mesh materials. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Crystals")
	TObjectPtr<UMaterialInterface> OreCrystalMaterial;

	/** Material of the heal ore crystal clusters (same mesh as ore), applied to every slot; null keeps the mesh materials. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Crystals")
	TObjectPtr<UMaterialInterface> HealOreCrystalMaterial;

	/** Uniform scale of the crystal clusters (provisional: SM_OreChunk_01 is about 30 uu wide, 1.5 gives about 45 uu on a 67 uu cell). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Crystals", meta = (ClampMin = "0.01"))
	float OreCrystalScale = 1.5f;

	/**
	 * Crystal formations of the Duskite (Ore) deposits (task-234-03; SM_DuskiteDeposit_01..03 from task-234-02): one instance per
	 * deposit, mesh = entry hash(deposit id, DepositCrystalSeed) % count. Source meshes: pivot at the footprint centre, base at
	 * z 0, footprint 134 x 134 uu (2 x 2 cells of 67). Null entries are skipped; empty (or only null): ore keeps one OreCrystalMesh cluster per cell.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	TArray<TObjectPtr<UStaticMesh>> OreDepositMeshes;

	/** Crystal formations of the heal ore deposits (heal veins), same rules as OreDepositMeshes; empty for now: heal ore keeps one cluster per cell. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	TArray<TObjectPtr<UStaticMesh>> HealDepositMeshes;

	/** Optional override of the Duskite deposit crystals: applied only to the mesh slot named DepositCrystalSlotName. Null (default) keeps every slot material of the mesh (set at import). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	TObjectPtr<UMaterialInterface> OreDepositMaterial;

	/** Optional override of the heal deposit crystals (meshes with `rock` and `crystal` slots): applied only to the slot named DepositCrystalSlotName; null (default) keeps the mesh materials. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	TObjectPtr<UMaterialInterface> HealDepositMaterial;

	/** Material slot name the per-type deposit material overrides (FName compare, case-insensitive); a mesh without such a slot keeps all its materials. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	FName DepositCrystalSlotName = TEXT("crystal");

	/** Uniform scale of the deposit crystals (1: a 134 uu footprint covers a 2 x 2 deposit of 67 uu cells; provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits", meta = (ClampMin = "0.01"))
	float DepositCrystalScale = 1.f;

	/** Seed of the per-deposit mesh variant and quarter-turn hash: same layout, deposits and seed give the same formations. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	int32 DepositCrystalSeed = 1;

	/**
	 * task-257-02 (owner 2026-10-20, provisional): seed of the crystal pulse desync. Every ore and heal ore crystal instance
	 * (deposit formation: id = deposit id; per-cell cluster: id = cell index) gets FRRPulseDesync phase and rate of (PulseSeed,
	 * id) in its per-instance custom data (deposits: DepositPulsePhaseCustomDataIndex / DepositPulseRateCustomDataIndex; clusters:
	 * PulsePhaseCustomDataIndex / PulseRateCustomDataIndex): same layout and seed, same pulses.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	int32 PulseSeed = 1;

	/** Lowest crystal pulse rate (scale of the material PulseFrequency; owner 2026-10-20: -20 percent, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Ore Deposits", meta = (ClampMin = "0.01"))
	float PulseRateMin = 0.8f;

	/** Highest crystal pulse rate (owner 2026-10-20: +20 percent, provisional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Ore Deposits", meta = (ClampMin = "0.01"))
	float PulseRateMax = 1.2f;

	/** Per-instance custom data float of the per-cell crystal cluster HISMs (fallback without deposit meshes, M_XPNutGlow children) receiving the pulse phase in turns (task-257-01: 4, after marked 0 and ore colour 1-3). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Ore Deposits", meta = (ClampMin = "0", ClampMax = "15"))
	int32 PulsePhaseCustomDataIndex = 4;

	/** Per-instance custom data float of the per-cell crystal cluster HISMs receiving the pulse rate (M_XPNutGlow, task-257-01: 5). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Ore Deposits", meta = (ClampMin = "0", ClampMax = "15"))
	int32 PulseRateCustomDataIndex = 5;

	/**
	 * task-267-01: per-instance custom data float of every deposit formation ISM (Duskite and heal) receiving the pulse phase in
	 * turns (M_DepositCrystalGlow). 13, beyond the 13 rock piece floats (0-12) M_RockSlab reads on the `rock` slot of the heal
	 * deposit meshes, so the heal deposits carry the pulse without touching the rock shake inputs 4-7.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Ore Deposits", meta = (ClampMin = "0", ClampMax = "31"))
	int32 DepositPulsePhaseCustomDataIndex = 13;

	/** task-267-01: per-instance custom data float of every deposit formation ISM receiving the pulse rate (M_DepositCrystalGlow): 14. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Ore Deposits", meta = (ClampMin = "0", ClampMax = "31"))
	int32 DepositPulseRateCustomDataIndex = 14;

	/** When false (default, owner 2026-10-19, task-237-01) the ore and heal ore crystals cast no shadow at all (CastShadow, dynamic, static and contact shadow off): deposit formation ISMs and the per-cell cluster fallback chunks. When true they keep the Ore / HealOre template shadow settings. Read when the components are created (BuildInstances). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Ore Deposits")
	bool bDepositCrystalsCastShadow = false;

	/**
	 * When true (default, owner 2026-10-20, task-263-01) every render component built from a block template casts shadows
	 * (CastShadow and dynamic shadow on): rock piece ISMs, barrel HISMs, special ore, and the ore / heal ore crystals unless
	 * bDepositCrystalsCastShadow keeps those off. With the player's light as the only shadow-casting light, rocks cast, nothing else.
	 * The floor chunks never cast (they only receive) and the collision ISMs stay hidden without shadow, whatever this flag.
	 * Read when the components are created (BuildInstances); it overrides the block template shadow flags.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rendering")
	bool bBlocksCastShadows = true;

	/** Seed of the per-corner variant and full-piece rotation hash: same layout and seed give the same pieces. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	int32 RockPieceSeed = 1;

	/** Allow Nanite on the rock piece ISMs (owner 2026-10-17: Nanite assumed; the meshes need Nanite enabled at import). False forces the fallback mesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	bool bRockPiecesUseNanite = true;

	/** Allow Nanite on the broken piece ISMs too (task-230-17: false). Under Nanite the M_RockSlab broken block sag did not show in game: with r.Nanite 0 the same damaged wall sags. The shake offset does draw on intact Nanite pieces (task-231-01, rr.Debug.ShakeCell captures). Broken pieces exist only around damaged cells, so their non-Nanite fallback costs little. Ignored when bRockPiecesUseNanite is false. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid|Rock Pieces")
	bool bBrokenRockPiecesUseNanite = false;

	/** Mesh of barrel blocks (render only). Fitted to one cell from its bounds (GetBlockRenderTransform); its collision is never used. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> BarrelMesh;

	/** Shape of the hidden barrel collision ISM, scaled like a MeshBaseSize cube to the block volume. Defaults to the engine cylinder (the barrel shape before task-107); null falls back to BarrelMesh. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> BarrelCollisionMesh;

	/** Material of barrel blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> BarrelMaterial;

	// --- Components ---

	/** Root: the grid origin (cell (0, 0) corner, floor top). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<USceneComponent> GridRoot;

	/** Template of the floor chunk HISMs (mesh, material, collision, shadow settings); empty. Floor slabs: one per open (non-void, non-block) cell. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FloorInstances;

	/** Template of the rock piece ISMs (rendering only: shadow, distance field and visibility settings; mesh and material are replaced per piece); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> RockInstances;

	/** Template of the ore chunk HISMs (rendering only); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> OreInstances;

	/** Template of the healing ore chunk HISMs (rendering only); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> HealOreInstances;

	/** Template of the special ore chunk HISMs (rendering only); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> SpecialOreInstances;

	/** Template of the barrel chunk HISMs (rendering only); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> BarrelInstances;

	/** Template of the rock collision chunk ISMs (hidden, remove-at-swap); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UInstancedStaticMeshComponent> RockCollision;

	/** Template of the ore collision chunk ISMs (hidden, remove-at-swap); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UInstancedStaticMeshComponent> OreCollision;

	/** Template of the healing ore collision chunk ISMs (hidden, remove-at-swap); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UInstancedStaticMeshComponent> HealOreCollision;

	/** Template of the special ore collision chunk ISMs (hidden, remove-at-swap); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UInstancedStaticMeshComponent> SpecialOreCollision;

	/** Template of the barrel collision chunk ISMs (hidden, remove-at-swap); empty. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UInstancedStaticMeshComponent> BarrelCollision;

	// --- Loaded layout and runtime state (rebuilt from the file, not saved) ---

	/** Grid width in cells from the loaded layout; 0 when none is loaded. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	int32 GridWidth = 0;

	/** Grid height in cells from the loaded layout; 0 when none is loaded. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	int32 GridHeight = 0;

	/** Spawn cell from the loaded layout; (-1, -1) when none. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	FIntPoint SpawnCell = FIntPoint(-1, -1);

	/** Current cell types, row-major: index = Y * GridWidth + X. The terrain source of truth; destroyed blocks become Floor. */
	UPROPERTY(Transient)
	TArray<ERRCellType> Cells;

	/** Remaining block health per cell in dig-seconds, row-major; 0 on cells without a block. */
	UPROPERTY(Transient)
	TArray<float> CellHealth;

	/** Remaining shot health per cell, row-major: BarrelShotHealth on barrel cells, 0 elsewhere and once destroyed. */
	UPROPERTY(Transient)
	TArray<float> CellShotHealth;

	/** Explosion source object per barrel cell, created on first request (GetBarrelSource); kept so each stays unique and alive. */
	UPROPERTY(Transient)
	TMap<FIntPoint, TObjectPtr<URRBarrelSource>> BarrelSources;

	/** Deposit id per cell, row-major; INDEX_NONE outside deposits and once the block is destroyed. */
	UPROPERTY(Transient)
	TArray<int32> CellDepositId;

	/** Every deposit of the last build, id = index (layout veins first, then table deposits). */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	TArray<FRRGridDeposit> Deposits;

	/** Deposits placed per ore type row at the last build. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	TMap<FName, int32> PlacedDepositCounts;

	/** Dig drops already spawned per cell, row-major; compared with the DropInterval multiples crossed. */
	UPROPERTY(Transient)
	TArray<int32> CellDigDrops;

	/** Cell -> instance index of the cell's block in its chunk block collision ISM (and barrel HISM, same index), row-major; INDEX_NONE on non-block cells. Rock piece types have no per-cell rendering instance. */
	UPROPERTY(Transient)
	TArray<int32> CellInstanceIndex;

	/** Cell -> instance index of the cell's floor slab in its chunk floor HISM, row-major; INDEX_NONE on void cells. */
	UPROPERTY(Transient)
	TArray<int32> CellFloorInstanceIndex;

	/** Every rock piece visual, index = ((set * piece shape count + shape) * PieceVariantSlots + variant) * chunk count + chunk Y * ChunkCount.X + chunk X (GetPieceVisualIndex); owns the piece ISMs. */
	UPROPERTY(Transient)
	TArray<FRRGridPieceVisual> PieceVisuals;

	/** Deposit crystal ISMs (task-234-03), one per block type and mesh variant, created at build for the types that UsesDepositCrystals. */
	UPROPERTY(Transient)
	TArray<FRRGridDepositVisual> DepositVisuals;

	/** Deposit id -> index in DepositVisuals of its crystal; INDEX_NONE without one (type on per-cell clusters, or crystal removed at the first broken block). */
	TArray<int32> DepositCrystalVisual;

	/** Deposit id -> instance index of its crystal in its DepositVisuals ISM; INDEX_NONE without one. */
	TArray<int32> DepositCrystalInstance;

	/** Variant slots per piece set and shape in PieceVisuals: the longest mesh list at the last build (at least 1). */
	int32 PieceVariantSlots = 1;

	/** Corner -> index in PieceVisuals of its piece, row-major over (GridWidth + 1) x (GridHeight + 1); INDEX_NONE for an empty corner. */
	TArray<int32> CornerPieceVisual;

	/** Corner -> instance index of its piece in CornerPieceVisual's ISM; INDEX_NONE for an empty corner. */
	TArray<int32> CornerPieceInstance;

	/** Corner -> broken flag: set when one of its 4 cells first has display damage above 0, never cleared until the next build (damage only rises); the corner's piece uses the broken set. */
	TBitArray<> CornerBroken;

	/** Piece shape per corner mask (0..15), from BuildPieceTable; meaningless for mask 0 (empty corner). */
	ERRRockPieceShape PieceShapeByMask[16] = {};

	/** Yaw quarter turns per corner mask (0..15), from BuildPieceTable. */
	uint8 PieceTurnsByMask[16] = {};

	/** Chunk counts along X and Y at the last build (grid size / ChunkCells, rounded up). */
	UPROPERTY(Transient)
	FIntPoint ChunkCount = FIntPoint::ZeroValue;

	/** Every chunk visual, index = GetVisualSlot(type) * chunk count + chunk Y * ChunkCount.X + chunk X (see GetVisualIndex); owns the chunk components. */
	UPROPERTY(Transient)
	TArray<FRRGridChunkVisual> ChunkVisuals;

	/** Chunk component (render or collision) -> index in ChunkVisuals, for GetCellFromHit. Components are kept alive by ChunkVisuals. */
	TMap<const UPrimitiveComponent*, int32> VisualByComponent;

	/** Cells whose block is currently shaking, with a per-cell phase so neighbors do not move in lockstep. */
	TMap<FIntPoint, float> ShakingCells;

	/** Fusing barrels by cell (task-114-01): lit at 0 health, removed when the cell is destroyed or on rebuild. */
	TMap<FIntPoint, FBarrelFuse> BarrelFuses;

	/** Every ground telegraph created for barrel fuses (one at BeginPlay in a rendering game world, then on demand; registered on this actor, reused). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URRGroundTelegraphComponent>> BarrelTelegraphs;

	/** Hidden telegraphs of BarrelTelegraphs ready for the next fuse. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URRGroundTelegraphComponent>> FreeBarrelTelegraphs;

	/** Block cells marked as objective rocks (SetCellMarked). Reset on rebuild; a destroyed block leaves the set. */
	TSet<FIntPoint> MarkedCells;

	/** One queued explosion block (QueueDestroyCellsInRadius). */
	struct FPendingCellDestruction
	{
		/** Cell to destroy; skipped if it is no longer a block when its turn comes. */
		FIntPoint Cell = FIntPoint::ZeroValue;
		/** Terrain source passed to DestroyCell. */
		ERRTerrainDamageSource Source = ERRTerrainDamageSource::Barrel;
	};

	/** Queued explosion blocks, FIFO per explosion, each explosion's blocks nearest to its center first; consumed from PendingCellDestructionHead. */
	TArray<FPendingCellDestruction> PendingCellDestructions;

	/** Index of the next block to destroy in PendingCellDestructions (the array is reset when it is consumed). */
	int32 PendingCellDestructionHead = 0;

	/** GFrameCounter of the frame whose budget DestructionSpentSeconds and DestructionCellsThisFrame count. */
	uint64 DestructionBudgetFrame = MAX_uint64;

	/** Game-thread seconds already spent destroying queued blocks in DestructionBudgetFrame. */
	double DestructionSpentSeconds = 0.0;

	/** Queued blocks destroyed in DestructionBudgetFrame (the first one of a frame ignores the budget). */
	int32 DestructionCellsThisFrame = 0;
};
