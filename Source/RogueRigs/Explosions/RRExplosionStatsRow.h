// Rogue Rigs - DataTable row holding one explosion source's stats (ExplosionStats.csv).

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Grid/RRArenaGrid.h"
#include "RRExplosionStatsRow.generated.h"

class ARRExplosionFX;

/**
 * One row of Content/RogueRigs/Data/Tables/ExplosionStats.csv (row name = explosion source, e.g. Barrel, Kamikaze).
 * Column names match the property names. Read by URRExplosionSubsystem when an explosion is queued.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRExplosionStatsRow : public FTableRowBase
{
	GENERATED_BODY()

	/** 2D radius in uu: every block whose cell center is inside is destroyed, every character whose actor location is inside is hit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	float Radius = 0.f;

	/** Character damage (SetByCaller Data.Damage of URRGE_DamageExplosion), before the miner no-one-shot cap. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	float Damage = 0.f;

	/** Terrain source reported to the grid for the destroyed blocks (Barrel or Kamikaze). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	ERRTerrainDamageSource TerrainSource = ERRTerrainDamageSource::Barrel;

	/** Visual spawned at the center (a Blueprint child of ARRExplosionFX); empty or missing asset = no visual. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	TSoftClassPtr<ARRExplosionFX> FXClass;
};
