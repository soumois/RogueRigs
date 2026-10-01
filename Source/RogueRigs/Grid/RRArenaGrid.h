// Rogue Rigs - arena grid actor: loads a layout JSON and builds floor and block instances.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRArenaGrid.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInterface;
class USceneComponent;
class UStaticMesh;

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

/**
 * Arena terrain built from a layout JSON (Content/RogueRigs/Data/Levels/<LayoutName>.json).
 * Rebuilds in OnConstruction so the arena shows in the editor viewport. One HISM component per
 * visual type (floor and each block type). Grid origin is the actor; the floor top is at the
 * actor's Z; cell (x, y) center is at ((x + 0.5) * CellSize, (y + 0.5) * CellSize) in actor space.
 * Derive a Blueprint to assign meshes and materials per type.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API ARRArenaGrid : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the root and one HISM component per visual type. */
	ARRArenaGrid();

	/** Reloads the layout and rebuilds every instance. Runs on placement, move and property edit. */
	virtual void OnConstruction(const FTransform& Transform) override;

	/** World-space center of cell (X, Y) at floor-top height. Cells outside the grid are not clamped. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FVector GetCellCenter(int32 X, int32 Y) const;

	/** Spawn cell read from the layout; (-1, -1) when no layout is loaded or it has no spawn. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FIntPoint GetSpawnCell() const;

	/** Type of cell (X, Y); Void when outside the grid or when no layout is loaded. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	ERRCellType GetCellType(int32 X, int32 Y) const;

protected:
	/** Reads and parses the layout file into Cells, GridWidth, GridHeight and SpawnCell. Returns false on any error (logged). */
	bool LoadLayout();

	/** Clears all HISM instances, applies meshes and materials, and adds floor and block instances from Cells. */
	void BuildInstances();

	/** Removes every instance from every HISM component. */
	void ClearAllInstances();

	/** Converts a layout type string ("rock", "heal_ore"...) to a cell type. Returns false for an unknown string. */
	static bool ParseCellType(const FString& TypeName, ERRCellType& OutType);

	/** HISM that renders the given block type; nullptr for Floor, Void and Spawn. */
	UHierarchicalInstancedStaticMeshComponent* GetBlockComponent(ERRCellType Type) const;

	/** Creates one HISM default subobject with block-all collision, attached to the root. */
	UHierarchicalInstancedStaticMeshComponent* CreateGridComponent(FName Name);

	/** Absolute path of the layout file for LayoutName. */
	FString GetLayoutFilePath() const;

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

	/** Edge length of the source meshes (the engine cube is 100 uu, pivot at its center). Used to scale instances. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = "1.0", Units = "cm"))
	float MeshBaseSize = 100.f;

	// --- Visuals per type (assigned in the derived Blueprint) ---

	/** Mesh of the floor slab under every non-void cell. */
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

	/** Floor slab instances, one per non-void cell. */
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

	// --- Loaded layout state (rebuilt from the file, not saved) ---

	/** Grid width in cells from the loaded layout; 0 when none is loaded. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	int32 GridWidth = 0;

	/** Grid height in cells from the loaded layout; 0 when none is loaded. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	int32 GridHeight = 0;

	/** Spawn cell from the loaded layout; (-1, -1) when none. */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Grid|State")
	FIntPoint SpawnCell = FIntPoint(-1, -1);

	/** Cell types, row-major: index = Y * GridWidth + X. The terrain source of truth. */
	UPROPERTY(Transient)
	TArray<ERRCellType> Cells;
};
