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

	/** Damage to every character that is not the miner (robots): SetByCaller Data.Damage of URRGE_DamageExplosion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	float Damage = 0.f;

	/** Damage to an ARRMiner target, before the miner no-one-shot cap (task-294-01; Kamikaze 30 while robots take 40). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	float MinerDamage = 0.f;

	/** Terrain source reported to the grid for the destroyed blocks (Barrel or Kamikaze). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	ERRTerrainDamageSource TerrainSource = ERRTerrainDamageSource::Barrel;

	/** Visual spawned at the center (a Blueprint child of ARRExplosionFX); empty or missing asset = no visual. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	TSoftClassPtr<ARRExplosionFX> FXClass;

	/**
	 * Camera shake strength at the centre, as a multiplier of the subsystem's ExplosionShakeClass preset (URRExplosionCameraShake,
	 * 10 uu): Barrel 1 (10 uu), Kamikaze 0.7 (7 uu), provisional owner 2026-11-02 (task-418 Q6). 0 = no shake.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	float ShakeScale = 0.f;

	/**
	 * Camera shake max distance as a multiple of Radius: the shake falls linearly to the edge scale (50 %) at Radius x this
	 * factor and nothing plays beyond (2 for both rows: Barrel 1400 uu, Kamikaze 624 uu, provisional owner 2026-11-02, task-418 Q7).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion Stats")
	float ShakeMaxDistanceFactor = 0.f;
};
