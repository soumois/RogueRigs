// Rogue Rigs - minimap data: a one-pixel-per-cell texture of the seen terrain, updated only on change, plus the marker positions.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Grid/RRArenaGrid.h"
#include "RRMinimap.generated.h"

class AActor;
class ARRMiner;
class URRRunFlowSubsystem;
class UTexture2D;

/** Tunables of the minimap (readability-rules Minimap line, colors from color-code). Edited on the HUD widget Blueprint. */
USTRUCT(BlueprintType)
struct FRRMinimapConfig
{
	GENERATED_BODY()

	/** False: no minimap object, no texture, no per-frame cost. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	bool bEnabled = true;

	/** Radius of the circle in cells (owner: about 40). The widget shows the texture from MinerPosition - radius to + radius. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap", meta = (ClampMin = "1"))
	float ViewRadiusCells = 40.f;

	/** Reveal radius in cells around the miner cell (readability-rules Minimap reveal, provisional 18): a cell whose center lies
	 *  within this distance of the miner cell center becomes seen. Same in every direction (not the camera footprint). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap", meta = (ClampMin = "0"))
	float RevealRadiusCells = 18.f;

	/** Rock, ore, special ore and barrel cells (breakable rock: light grey tinted violet). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	FColor RockColor = FColor(176, 166, 196, 255);

	/** Floor and spawn cells (provisional: color-code has no floor entry). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	FColor FloorColor = FColor(58, 56, 66, 255);

	/** Heal ore cells (heal veins: red). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	FColor HealVeinColor = FColor(214, 38, 38, 255);

	/** Void cells (provisional: color-code has no void entry). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	FColor VoidColor = FColor(8, 8, 12, 255);

	/** Cells never seen (transparent by default). Also the color outside the grid (texture address clamp: keep the border unseen). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	FColor UnseenColor = FColor(0, 0, 0, 0);
};

/**
 * Per-frame minimap state for the widget. Positions are in texture pixels (float, top-left origin, pixel (u, v) center at
 * u + 0.5, v + 0.5); divide by TextureSize for UV. Texture orientation: screen-aligned for camera yaw 0 (screen up = world +X,
 * screen right = world +Y): u = cell Y, v = GridWidth - 1 - cell X.
 */
USTRUCT(BlueprintType)
struct FRRMinimapState
{
	GENERATED_BODY()

	/** Texture size in pixels (= cells): X = grid height (cells along world Y), Y = grid width (cells along world X). */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FIntPoint TextureSize = FIntPoint::ZeroValue;

	/** Miner position in texture pixels: the circle center. */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FVector2D MinerPosition = FVector2D::ZeroVector;

	/** MinerPosition / TextureSize: the UV to sample at the circle center. */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FVector2D MinerUV = FVector2D::ZeroVector;

	/** Circle radius in cells (= texture pixels), from FRRMinimapConfig::ViewRadiusCells. */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	float ViewRadiusCells = 0.f;

	/** ViewRadiusCells / TextureSize per axis: the UV half-extent of the circle. */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FVector2D ViewRadiusUV = FVector2D::ZeroVector;

	/** True while the run flow has a console (whatever its state). */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	bool bHasConsole = false;

	/** Console position in texture pixels (valid when bHasConsole). Marker teal. */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FVector2D ConsolePosition = FVector2D::ZeroVector;

	/** True while the run flow has a bonus chest (whatever its state). */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	bool bHasBonusChest = false;

	/** Bonus chest position in texture pixels (valid when bHasBonusChest). Marker gold. */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FVector2D BonusChestPosition = FVector2D::ZeroVector;

	/** True once the shuttle has landed (until the level ends). */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	bool bHasShuttle = false;

	/** Shuttle position in texture pixels (valid when bHasShuttle). Marker white. */
	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	FVector2D ShuttlePosition = FVector2D::ZeroVector;
};

/**
 * Minimap data owned by URRHUDWidget: a transient UTexture2D (B8G8R8A8, sRGB, nearest filter), one pixel per grid cell,
 * UnseenColor until the cell has been within RevealRadiusCells of the miner once, then its terrain color. Updates are cheap:
 * the seen pass runs only when the miner enters another cell, a destroyed cell rewrites one pixel, and the
 * changed pixels of a frame go to the GPU as one dirty rectangle (UpdateTextureRegions), nothing on a frame without change.
 */
UCLASS()
class ROGUERIGS_API URRMinimap : public UObject
{
	GENERATED_BODY()

public:
	/** Binds to Grid (built, size > 0): creates the texture, all cells unseen, and listens to OnCellDestroyed. False otherwise. */
	bool Initialize(ARRArenaGrid* InGrid, const FRRMinimapConfig& InConfig);

	/** Unbinds from the grid. */
	void Shutdown();

	/** Per frame: marks the cells within RevealRadiusCells of RevealCenter's cell as seen (when that cell changed) and flushes
	 *  changed pixels. RevealCenter is the miner (null: no seen pass). */
	void Tick(const AActor* RevealCenter);

	/** The minimap texture (null before Initialize). */
	UTexture2D* GetTexture() const { return Texture; }

	/** Bound grid (null before Initialize or after the grid is destroyed). */
	ARRArenaGrid* GetGrid() const { return Grid.Get(); }

	/** Fills OutState from the miner and the run flow objectives (either may be null). False when not initialized. */
	bool GetState(const ARRMiner* Miner, const URRRunFlowSubsystem* RunFlow, FRRMinimapState& OutState) const;

	/** World location to texture pixels (float), see FRRMinimapState for the orientation. */
	FVector2D WorldToTexture(const FVector& WorldLocation) const;

	/** True once Cell has been within the reveal radius of the miner. */
	bool IsCellSeen(FIntPoint Cell) const;

	/** Marks Cell as seen and writes its color (no-op when outside the grid or already seen). Used by the seen pass. */
	void MarkCellSeen(FIntPoint Cell);

protected:
	/** Rewrites a seen destroyed cell's pixel (now Floor). */
	UFUNCTION()
	void HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source);

	/** Color of a seen cell of Type. */
	FColor GetCellColor(ERRCellType Type) const;

	/** Pixel index of Cell in Pixels (INDEX_NONE outside the grid). */
	int32 GetPixelIndex(FIntPoint Cell) const;

	/** Writes Color at Cell's pixel and grows the dirty rectangle. */
	void WritePixel(FIntPoint Cell, const FColor& Color);

	/** Marks the cells whose centers lie within RevealRadiusCells of RevealCenter's cell center, when that cell changed. */
	void UpdateSeenCells(const AActor* RevealCenter);

	/** Sends the dirty rectangle of Pixels to the texture, then clears it. */
	void FlushDirtyPixels();

	/** Grid the texture mirrors. */
	TWeakObjectPtr<ARRArenaGrid> Grid;

	/** The minimap texture. */
	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> Texture;

	/** Tunables copied at Initialize. */
	FRRMinimapConfig Config;

	/** Grid size in cells (X = width along world X, Y = height along world Y). */
	FIntPoint GridSize = FIntPoint::ZeroValue;

	/** CPU copy of the texture, row-major, TextureSize.X per row. */
	TArray<FColor> Pixels;

	/** Seen flag per cell, indexed like the grid (Y * width + X). */
	TBitArray<> SeenCells;

	/** Dirty rectangle in texture pixels, inclusive min, exclusive max; empty when Min >= Max. */
	FIntPoint DirtyMin = FIntPoint::ZeroValue;

	/** See DirtyMin. */
	FIntPoint DirtyMax = FIntPoint::ZeroValue;

	/** Center cell of the last seen pass; the pass is skipped while the miner stays on it. */
	FIntPoint LastSeenCell = FIntPoint(INDEX_NONE, INDEX_NONE);
};
