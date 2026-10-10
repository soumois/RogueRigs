// Rogue Rigs - arena grid actor: loads a layout JSON, builds floor and block instances, and owns terrain damage and destruction.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRArenaGrid.generated.h"

class ARRPickup;
class URRBarrelSource;
class UHierarchicalInstancedStaticMeshComponent;
class UInstancedStaticMeshComponent;
class UMaterialInterface;
class UPrimitiveComponent;
class USceneComponent;
class UStaticMesh;
struct FHitResult;
struct FRROreTypeRow;

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

/** What damaged or destroyed a block: the three terrain-breaking sources (destruction-rules), plus shots, which only break barrels. */
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
	Shot
};

/**
 * One visual type in one chunk of ChunkCells x ChunkCells cells: its rendering HISM, its hidden collision ISM (block types
 * only) and the instance index -> cell map shared by both. Components are created when the chunk first holds an instance.
 */
USTRUCT()
struct FRRGridChunkVisual
{
	GENERATED_BODY()

	/** Visual type: Floor (floor slabs, spawn included) or a block type. */
	UPROPERTY()
	ERRCellType MapKey = ERRCellType::Floor;

	/** Chunk coordinates (cell / ChunkCells). */
	UPROPERTY()
	FIntPoint Chunk = FIntPoint::ZeroValue;

	/** Rendering HISM of this type in this chunk; null until the chunk holds one instance of the type. */
	UPROPERTY()
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Render;

	/** Hidden collision ISM of this block type in this chunk, instance for instance with Render; null for Floor (the floor HISM collides). */
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> Collision;

	/** Cell rendered by each instance, in instance order. Kept in sync with remove-at-swap of both components. */
	UPROPERTY()
	TArray<FIntPoint> InstanceCells;
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

	/** Ore type row (OreTypes.csv row name) the deposit was placed from, or the first row of its block type for a layout vein; None when no row matches (heal veins today). */
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

	/** Ore colour of the row (written into the instance custom data when bHasColor). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	FLinearColor Color = FLinearColor::White;

	/** True when an ore type row gave the deposit its value and colour. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	bool bHasColor = false;

	/** True when the deposit came from the ore table placement (not from the layout file). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Deposit")
	bool bPlacedFromTable = false;
};

/** Fired when a block takes terrain damage without necessarily being destroyed: cell, block type, damage applied (dig-seconds), source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRROnCellDamagedSignature, FIntPoint, Cell, ERRCellType, Type, float, Amount, ERRTerrainDamageSource, Source);

/** Fired once when a block is destroyed (the cell is Floor afterwards): cell, block type it had, source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnCellDestroyedSignature, FIntPoint, Cell, ERRCellType, Type, ERRTerrainDamageSource, Source);

/**
 * Arena terrain built from a layout JSON (Content/RogueRigs/Data/Levels/<LayoutName>.json).
 * Rebuilds in OnConstruction so the arena shows in the editor viewport, and again in BeginPlay so runtime
 * state (health, instance maps) always exists; OnConstruction is ignored once play has begun, so runtime
 * destruction is never wiped. Instances are split in chunks of ChunkCells x ChunkCells cells: one HISM per visual type
 * (floor and each block type) per chunk, created at build (FRRGridChunkVisual), so a destruction updates one small component;
 * the default-subobject HISMs and ISMs are empty templates that hold the per-type settings. Block HISMs only render, and each
 * block chunk HISM has a hidden collision ISM holding the same instances in the same order (same indices), always at rest, so
 * the dig shake moves the rendering only (see SetCellShaking). Floor HISMs render and collide. No grid component affects
 * navigation (robots use the flow field, the map has no navmesh). Grid origin is
 * the actor; the floor top is at the actor's Z; cell (x, y) center is at ((x + 0.5) * CellSize, (y + 0.5) * CellSize)
 * in actor space. Floor instances exist only on cells without a block; destroying a block adds one.
 * Terrain damage (dig) and destruction (explosions) go through ApplyTerrainDamage / DestroyCell / DestroyCellsInRadius.
 * Drops: ore and heal ore blocks spawn one pickup (OrePickupClass / HealPickupClass) each time their accumulated damage
 * crosses a multiple of DropInterval, except on the hit that destroys them, and DestroyDropCount pickups when destroyed,
 * whatever the source, each worth its deposit's ChunkValue, spawned through URRPickupSubsystem (per-frame cap, queue).
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

	/** Advances the jitter of shaking blocks. */
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

	/** Ore type row of the deposit holding Cell (e.g. Gold); None when Cell is in no deposit or its deposit has no row (heal veins). */
	UFUNCTION(BlueprintPure, Category = "Grid|Deposits")
	FName GetDepositOreId(FIntPoint Cell) const;

	/** True when Cell is a standing block of a deposit placed from the ore table (consoles never mark those). */
	UFUNCTION(BlueprintPure, Category = "Grid|Deposits")
	bool IsTableDepositCell(FIntPoint Cell) const;

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
	 * Returns true when this call destroyed the block; false otherwise (no block, Amount <= 0, or still standing).
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	bool ApplyTerrainDamage(FIntPoint Cell, float Amount, ERRTerrainDamageSource Source);

	/**
	 * Destroys the block in Cell outright, whatever its health: removes its instance, adds a floor instance,
	 * sets the cell to Floor, spawns DestroyDropCount pickups for ore and heal ore, and broadcasts OnCellDestroyed.
	 * Returns false when the cell has no block.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	bool DestroyCell(FIntPoint Cell, ERRTerrainDamageSource Source);

	/**
	 * Destroys every block whose cell center lies within Radius (horizontal distance) of Center, whatever its type
	 * or health (explosions rule). Returns the number of blocks destroyed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	int32 DestroyCellsInRadius(FVector Center, float Radius, ERRTerrainDamageSource Source);

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
	 * explosion. Returns true when this call destroyed the barrel.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	bool ApplyShotDamage(FIntPoint Cell, float Damage);

	/**
	 * Explosion source object of the barrel in Cell: one per cell, created on first request and kept by the grid, so
	 * URRExplosionSubsystem lets each barrel explode only once. Returns nullptr when Cell is outside the grid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	URRBarrelSource* GetBarrelSource(FIntPoint Cell);

	/**
	 * Starts or stops the jitter of the block in Cell (dig feedback). Only the rendering instance moves: the collision
	 * instance stays at the cell, so a shaking block never pushes a character. Stopping restores the rest transform.
	 * Ignored on cells without a block.
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

	/** Pickup class this grid drops for a block type (OrePickupClass for Ore, HealPickupClass for HealOre, none otherwise); used by ARRBonusChest for its ore reward. */
	TSubclassOf<ARRPickup> GetDropPickupClass(ERRCellType Type) const { return GetPickupClass(Type); }

	/** Broadcast each time a block takes terrain damage (ApplyTerrainDamage), before a possible destruction. */
	UPROPERTY(BlueprintAssignable, Category = "Grid|Destruction")
	FRROnCellDamagedSignature OnCellDamaged;

	/** Broadcast once per destroyed block, after the cell has become Floor. */
	UPROPERTY(BlueprintAssignable, Category = "Grid|Destruction")
	FRROnCellDestroyedSignature OnCellDestroyed;

protected:
	/** Rebuilds the grid from the layout so play starts from a fresh, fully indexed state (placed actors do not rerun construction in PIE or cooked builds). */
	virtual void BeginPlay() override;

	/**
	 * Display event: the block in Cell was marked or unmarked (SetCellMarked, or a marked block destroyed). CellCenter is the
	 * world center of the cell on the floor. Implement in Blueprint for an outline that is not driven by the instance custom
	 * data (e.g. spawn and remove an outline mesh).
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Grid|Objectives")
	void OnCellMarkedChanged(FIntPoint Cell, bool bMarked, FVector CellCenter);

	/** Writes Value into the marked custom data float of the rendering instance of the block in Cell (no-op without one). */
	void WriteMarkedCustomData(FIntPoint Cell, float Value);

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

	/** Reads and parses the layout file into Cells, GridWidth, GridHeight and SpawnCell. Returns false on any error (logged). */
	bool LoadLayout();

	/** Destroys the chunk components, applies meshes and materials to the templates, creates the chunk components and adds floor and block instances from Cells, and resets health, maps and shaking. */
	void BuildInstances();

	/** Destroys every chunk component, empties the template components and resets the instance maps. */
	void ClearAllInstances();

	/** Converts a layout type string ("rock", "heal_ore"...) to a cell type. Returns false for an unknown string. */
	static bool ParseCellType(const FString& TypeName, ERRCellType& OutType);

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

	/** Creates and registers one chunk component of Class, copying the settings of Template, attached to the root (not navigation relevant). */
	UInstancedStaticMeshComponent* CreateChunkComponent(TSubclassOf<UInstancedStaticMeshComponent> Class, const UInstancedStaticMeshComponent* Template, FIntPoint Chunk);

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

	/** Actor-space rest transform of the block instance of a cell. */
	FTransform GetBlockInstanceTransform(FIntPoint Cell) const;

	/** Adds an instance for Cell to the rendering component of MapKey in Cell's chunk (and to its collision ISM for a block) and records it in the chunk map. Returns the instance index, or INDEX_NONE. */
	int32 AddCellInstance(ERRCellType MapKey, FIntPoint Cell, const FTransform& LocalTransform);

	/** Removes Cell's instance InstanceIndex from the rendering component of MapKey in Cell's chunk (and its collision ISM) and patches both maps for the instance moved into the freed slot. */
	void RemoveCellInstance(ERRCellType MapKey, FIntPoint Cell, int32 InstanceIndex);

	/** Stops shaking Cell, restoring its rest transform if it still has a block. */
	void StopShaking(FIntPoint Cell);

	/** Pickup class dropped by a block type: OrePickupClass for Ore, HealPickupClass for HealOre, none otherwise. */
	TSubclassOf<ARRPickup> GetPickupClass(ERRCellType Type) const;

	/**
	 * Spawns the dig drops owed by a damaged, still standing block: one per multiple of DropInterval crossed by its
	 * accumulated damage, capped so the multiple equal to its max health (the destroying hit) never drops.
	 */
	void SpawnDigDrops(FIntPoint Cell, ERRCellType Type);

	/** Queues the explosion of the barrel in Cell (row BarrelExplosionRow, at the cell center, source GetBarrelSource) in the world's URRExplosionSubsystem, during play only. */
	void QueueBarrelExplosion(FIntPoint Cell);

	/**
	 * Requests Count pickups of the class dropped by Type at the center of Cell (floor top), each worth OreValue, during play
	 * only, through URRPickupSubsystem::RequestSpawn (at most its MaxSpawnsPerFrame per frame, the rest queued).
	 */
	void SpawnPickups(FIntPoint Cell, ERRCellType Type, int32 Count, int32 OreValue = 1);

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
	 * When true, a block instance extends down through the floor slab (bottom at -FloorThickness) so the cliff has no
	 * notch under edge blocks, since there is no floor instance under a block. When false, the block stands on the
	 * floor top (bottom at the actor's Z) and its cell has a hole in the slab.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	bool bBlocksFillFloorSlab = true;

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

	/** Health of a rock block: drone dig time in seconds at mining speed 1 (drone gdd: 0.5 s per 67 uu block). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float RockHealth = 0.5f;

	/** Health of an ore block: drone dig time in seconds at mining speed 1 (ore-blocks gdd: 1.5 s per 67 uu block). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float OreHealth = 1.5f;

	/** Health of a heal ore block: drone dig time in seconds at mining speed 1 (drone gdd: 1.5 s per 67 uu block). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float HealOreHealth = 1.5f;

	/** Health of a special ore block: drone dig time in seconds at mining speed 1 (same as ore). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float SpecialOreHealth = 1.5f;

	/** Health of a barrel block: drone dig time in seconds at mining speed 1 (same as rock, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float BarrelHealth = 0.5f;

	/** Health of a barrel against shots (projectile damage, e.g. 10 per rifle bullet); separate from its dig health (provisional, barrel gdd). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01"))
	float BarrelShotHealth = 20.f;

	/** Row of ExplosionStats.csv queued when a barrel is destroyed, whatever the source. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction")
	FName BarrelExplosionRow = TEXT("Barrel");

	// --- Shake (dig feedback) ---

	/** Maximum horizontal offset of a shaking block from its rest position. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float ShakeAmplitude = 25.f;

	/** Maximum vertical offset (bob) of a shaking block from its rest position, in the same jitter. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "cm"))
	float ShakeVerticalAmplitude = 8.f;

	/** Oscillation frequency of a shaking block. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.0", Units = "Hz"))
	float ShakeFrequency = 8.f;

	// --- Drops (ore and heal ore) ---

	/** Pickup spawned by ore blocks (a Blueprint subclass of ARRPickup with type Ore). None = ore drops nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops")
	TSubclassOf<ARRPickup> OrePickupClass;

	/** Pickup spawned by heal ore blocks (a Blueprint subclass of ARRPickup with type Heal). None = heal ore drops nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops")
	TSubclassOf<ARRPickup> HealPickupClass;

	/** Accumulated damage, in dig-seconds, between two drops while a block is dug (provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops", meta = (ClampMin = "0.01", Units = "s"))
	float DropInterval = 1.5f;

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

	/** Mesh of rock blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> RockMesh;

	/** Material of rock blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> RockMaterial;

	/** Mesh of ore blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> OreMesh;

	/** Material of ore blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> OreMaterial;

	/** Mesh of healing ore blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> HealOreMesh;

	/** Material of healing ore blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> HealOreMaterial;

	/** Mesh of special ore blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> SpecialOreMesh;

	/** Material of special ore blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UMaterialInterface> SpecialOreMaterial;

	/** Mesh of barrel blocks. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Visuals")
	TObjectPtr<UStaticMesh> BarrelMesh;

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

	/** Template of the rock chunk HISMs (rendering only); empty. */
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

	/** Cell -> instance index in the chunk HISM of the cell's current visual (block, or floor), row-major; INDEX_NONE on void cells. A block's collision ISM uses the same index. */
	UPROPERTY(Transient)
	TArray<int32> CellInstanceIndex;

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

	/** Block cells marked as objective rocks (SetCellMarked). Reset on rebuild; a destroyed block leaves the set. */
	TSet<FIntPoint> MarkedCells;
};
