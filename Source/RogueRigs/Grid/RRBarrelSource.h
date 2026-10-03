// Rogue Rigs - explosion source key of one barrel cell: a tiny object owned by the arena grid, one per barrel cell.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RRBarrelSource.generated.h"

/**
 * Stable, unique explosion source of one barrel cell (sys-grid, sys-explosions). URRExplosionSubsystem lets a
 * source explode only once and keys sources by object, so each barrel cell gets its own instance, created by
 * ARRArenaGrid::GetBarrelSource on first use and kept by the grid for its life. Holds no logic.
 */
UCLASS()
class ROGUERIGS_API URRBarrelSource : public UObject
{
	GENERATED_BODY()

public:
	/** Grid cell of the barrel this object stands for. Set once by the grid at creation. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Grid")
	FIntPoint Cell = FIntPoint(-1, -1);
};
