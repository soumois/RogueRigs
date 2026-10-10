// Rogue Rigs - DataTable row holding one ore type (OreTypes.csv): share of the rock, deposit size, chunk value, heal value, colour.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Grid/RRArenaGrid.h"
#include "RROreTypeRow.generated.h"

/**
 * One row of Content/RogueRigs/Data/Tables/OreTypes.csv (row name = ore id, e.g. Gold). Column names match the property
 * names. Read by ARRArenaGrid at build: RockShare of the layout's rock blocks become deposits of GroupSize x GroupSize blocks
 * of CellType, placed deterministically from the cell and the grid's OreSeed (ore-blocks, owner 2026-10-10).
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRROreTypeRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Block type the deposits are made of (Ore for gold): its health, mesh, material and pickup class come from the grid. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ore Type")
	ERRCellType CellType = ERRCellType::Ore;

	/** Share of the layout's rock blocks turned into this ore (0.08 = 8 %); the deposit count is RockShare x rock blocks / GroupSize^2, rounded. 0 = never placed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ore Type", meta = (ClampMin = "0", ClampMax = "1"))
	float RockShare = 0.f;

	/** Side of a deposit in blocks (2 = 2 x 2). Every block of a deposit is mined as one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ore Type", meta = (ClampMin = "1"))
	int32 GroupSize = 2;

	/** Ore value of each chunk a broken block of this ore drops (ARRPickup::SetOreValue). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ore Type", meta = (ClampMin = "0"))
	int32 ChunkValue = 1;

	/**
	 * Health restored by each heal pickup a broken block of this type drops (ARRPickup::SetHealAmount); 0 for non-heal ores.
	 * Row Heal (heal-ore, owner 2026-10-10): 15 per pickup, so a 2 x 2 heal vein restores 60.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ore Type", meta = (ClampMin = "0"))
	float HealValue = 0.f;

	/** Colour parameter of the ore crystals (linear RGB), written into the per-instance custom data of its blocks for the material; alpha 0 = no colour written (the material keeps its own). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ore Type")
	FLinearColor Color = FLinearColor::White;
};
