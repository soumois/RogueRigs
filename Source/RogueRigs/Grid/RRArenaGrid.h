// Rogue Rigs - arena grid actor: loads a layout JSON, builds floor and block instances, and owns terrain damage and destruction.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRArenaGrid.generated.h"

class ARRPickup;
class URRBarrelSource;
class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInterface;
class USceneComponent;
class UStaticMesh;
struct FHitResult;

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

/** Instance index -> cell map of one HISM component. Index i is the cell rendered by instance i. */
USTRUCT()
struct FRRInstanceCellMap
{
	GENERATED_BODY()

	/** Cell rendered by each instance, in instance order. Kept in sync with HISM remove-at-swap. */
	UPROPERTY()
	TArray<FIntPoint> InstanceCells;
};

/** Fired when a block takes terrain damage without necessarily being destroyed: cell, block type, damage applied (dig-seconds), source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRROnCellDamagedSignature, FIntPoint, Cell, ERRCellType, Type, float, Amount, ERRTerrainDamageSource, Source);

/** Fired once when a block is destroyed (the cell is Floor afterwards): cell, block type it had, source. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRROnCellDestroyedSignature, FIntPoint, Cell, ERRCellType, Type, ERRTerrainDamageSource, Source);

/**
 * Arena terrain built from a layout JSON (Content/RogueRigs/Data/Levels/<LayoutName>.json).
 * Rebuilds in OnConstruction so the arena shows in the editor viewport, and again in BeginPlay so runtime
 * state (health, instance maps) always exists; OnConstruction is ignored once play has begun, so runtime
 * destruction is never wiped. One HISM component per visual type (floor and each block type). Grid origin is
 * the actor; the floor top is at the actor's Z; cell (x, y) center is at ((x + 0.5) * CellSize, (y + 0.5) * CellSize)
 * in actor space. Floor instances exist only on cells without a block; destroying a block adds one.
 * Terrain damage (dig) and destruction (explosions) go through ApplyTerrainDamage / DestroyCell / DestroyCellsInRadius.
 * Drops: ore and heal ore blocks spawn one pickup (OrePickupClass / HealPickupClass) each time their accumulated damage
 * crosses a multiple of DropInterval, except on the hit that destroys them, and DestroyDropCount pickups when destroyed,
 * whatever the source. Derive a Blueprint to assign meshes, materials and pickup classes.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRArenaGrid : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root and one HISM component per visual type. Tick is enabled only while a block shakes. */
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
	 * Grid cell of a blocking hit on this grid: the cell of the hit instance when the hit component is a block HISM
	 * with a valid Item, else the cell just behind the impact point (impact point minus 1 uu along the impact normal).
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

	/** Starts or stops the jitter of the block in Cell (dig feedback). Stopping restores its rest transform. Ignored on cells without a block. */
	UFUNCTION(BlueprintCallable, Category = "Grid|Destruction")
	void SetCellShaking(FIntPoint Cell, bool bShaking);

	/** Broadcast each time a block takes terrain damage (ApplyTerrainDamage), before a possible destruction. */
	UPROPERTY(BlueprintAssignable, Category = "Grid|Destruction")
	FRROnCellDamagedSignature OnCellDamaged;

	/** Broadcast once per destroyed block, after the cell has become Floor. */
	UPROPERTY(BlueprintAssignable, Category = "Grid|Destruction")
	FRROnCellDestroyedSignature OnCellDestroyed;

protected:
	/** Rebuilds the grid from the layout so play starts from a fresh, fully indexed state (placed actors do not rerun construction in PIE or cooked builds). */
	virtual void BeginPlay() override;

	/** Loads the layout and builds instances, or clears everything when loading fails. */
	void RebuildFromLayout();

	/** Reads and parses the layout file into Cells, GridWidth, GridHeight and SpawnCell. Returns false on any error (logged). */
	bool LoadLayout();

	/** Clears all HISM instances, applies meshes and materials, adds floor and block instances from Cells, and resets health, maps and shaking. */
	void BuildInstances();

	/** Removes every instance from every HISM component and resets the instance maps. */
	void ClearAllInstances();

	/** Converts a layout type string ("rock", "heal_ore"...) to a cell type. Returns false for an unknown string. */
	static bool ParseCellType(const FString& TypeName, ERRCellType& OutType);

	/** True for the block types (Rock, Ore, HealOre, SpecialOre, Barrel). */
	static bool IsBlockType(ERRCellType Type);

	/** HISM that renders the given block type; nullptr for Floor, Void and Spawn. */
	UHierarchicalInstancedStaticMeshComponent* GetBlockComponent(ERRCellType Type) const;

	/** Starting health of a block type in dig-seconds; 0 for non-block types. */
	float GetBlockMaxHealth(ERRCellType Type) const;

	/** Creates one HISM default subobject with block-all collision, attached to the root. */
	UHierarchicalInstancedStaticMeshComponent* CreateGridComponent(FName Name);

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

	/** Adds an instance for Cell to Component and records it in the matching map. Returns the instance index, or INDEX_NONE. */
	int32 AddCellInstance(UHierarchicalInstancedStaticMeshComponent* Component, ERRCellType MapKey, FIntPoint Cell, const FTransform& LocalTransform);

	/** Removes the instance at InstanceIndex from Component and patches both maps for the instance moved into the freed slot. */
	void RemoveCellInstance(UHierarchicalInstancedStaticMeshComponent* Component, ERRCellType MapKey, int32 InstanceIndex);

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

	/** Spawns Count pickups of the class dropped by Type at the center of Cell (floor top), during play only. */
	void SpawnPickups(FIntPoint Cell, ERRCellType Type, int32 Count);

	// --- Layout and dimensions ---

	/** Layout file name without extension, read from Content/RogueRigs/Data/Levels/<LayoutName>.json. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	FString LayoutName = TEXT("arena_m1");

	/** Edge length of one cell in Unreal units. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = "1.0", Units = "cm"))
	float CellSize = 200.f;

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

	// --- Block health, in dig-seconds at mining speed 1 (provisional) ---

	/** Health of a rock block: drone dig time in seconds at mining speed 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float RockHealth = 1.5f;

	/** Health of an ore block: drone dig time in seconds at mining speed 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float OreHealth = 4.5f;

	/** Health of a heal ore block: drone dig time in seconds at mining speed 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float HealOreHealth = 4.5f;

	/** Health of a special ore block: drone dig time in seconds at mining speed 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float SpecialOreHealth = 4.5f;

	/** Health of a barrel block: drone dig time in seconds at mining speed 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Destruction", meta = (ClampMin = "0.01", Units = "s"))
	float BarrelHealth = 1.5f;

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

	/** Pickups spawned when an ore or heal ore block is destroyed, whatever the source (provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Drops", meta = (ClampMin = "0"))
	int32 DestroyDropCount = 3;

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

	/** Floor slab instances, one per open (non-void, non-block) cell. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FloorInstances;

	/** Rock block instances. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> RockInstances;

	/** Ore block instances. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> OreInstances;

	/** Healing ore block instances. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> HealOreInstances;

	/** Special ore block instances. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> SpecialOreInstances;

	/** Barrel block instances. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Components")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> BarrelInstances;

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

	/** Dig drops already spawned per cell, row-major; compared with the DropInterval multiples crossed. */
	UPROPERTY(Transient)
	TArray<int32> CellDigDrops;

	/** Cell -> instance index in the HISM of the cell's current visual (block component, or floor), row-major; INDEX_NONE on void cells. */
	UPROPERTY(Transient)
	TArray<int32> CellInstanceIndex;

	/** Instance index -> cell map per visual, keyed by cell type (Floor for the floor HISM, each block type for its HISM). */
	UPROPERTY(Transient)
	TMap<ERRCellType, FRRInstanceCellMap> InstanceCellMaps;

	/** Cells whose block is currently shaking, with a per-cell phase so neighbors do not move in lockstep. */
	TMap<FIntPoint, float> ShakingCells;
};
