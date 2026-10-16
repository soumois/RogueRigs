// Rogue Rigs - decal material warm-up (sys-ground-telegraph): draws each ground decal material once at level start, out of sight, so its first gameplay frame is not the engine default decal.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRDecalWarmUp.generated.h"

class UDecalComponent;
class UMaterialInterface;
class UWorld;

/**
 * Makes ground decal materials (M_GroundTelegraph, MI_ExplosionShockwave) ready before their first gameplay frame
 * (task-201-01). In an uncooked run (editor, -game) a material's shader map starts incomplete: the first time the decal pass
 * asks for its pixel shader, FMaterial::TryGetShaders submits an on-demand compile and fails, and the decal renderer
 * (DecalRenderingShared.cpp TryGetDeferredDecalMaterial) draws the material's fallback, the engine default deferred decal
 * material (pale concrete), over the whole decal box for that frame. A hidden decal has no scene proxy, so only a drawn decal
 * warms its material up. This actor draws one tiny decal per material, floating CameraDistance in front of the player camera
 * (its box touches no surface, so neither the material nor the fallback shades a pixel), for WarmUpSeconds of camera time,
 * then destroys those decals and stops ticking: no cost in steady state. It keeps a reference to every material for the
 * world's life, so a warmed material is not unloaded and reloaded cold. Setting the material also precaches its PSOs
 * (cooked builds). One actor per world, spawned on the first WarmUp; only in game worlds that render (never under -nullrhi).
 */
UCLASS(NotBlueprintable, NotPlaceable, Transient)
class ROGUERIGS_API ARRDecalWarmUp : public AActor
{
	GENERATED_BODY()

public:
	/** Scene root, tick enabled when a warm-up is pending (also while the game is paused). */
	ARRDecalWarmUp();

	/**
	 * Warms Material up in World: spawns the world's warm-up actor if needed and adds the material (once per material).
	 * No-op outside a game world, without rendering, for a null material or for a material that is not a deferred decal.
	 */
	static void WarmUp(UWorld* World, UMaterialInterface* Material);

	/** Adds Material to the warmed set and (re)starts the warm-up draw; ignored when it is already in the set. */
	void AddMaterial(UMaterialInterface* Material);

	/** True while warm-up decals exist (until WarmUpSeconds of camera time have passed since the last added material). */
	bool IsWarmingUp() const { return Decals.Num() > 0; }

	/** Materials warmed in this world (kept referenced for the world's life). */
	const TArray<TObjectPtr<UMaterialInterface>>& GetWarmedMaterials() const { return Materials; }

	/** Keeps the warm-up decals in front of the player camera and ends the warm-up after WarmUpSeconds. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Moves the actor CameraDistance in front of the first player camera; false (nothing moved) while there is no camera. */
	bool PlaceInFrontOfCamera();

	/** Destroys the warm-up decals and stops the tick; the materials stay referenced. */
	void FinishWarmUp();

	/** Seconds the decals are drawn once a player camera exists: covers an on-demand shader compile served by the cache. */
	UPROPERTY(EditDefaultsOnly, Category = "Decal Warm-Up", meta = (ClampMin = "0.1"))
	float WarmUpSeconds = 2.f;

	/** Distance in uu from the player camera to the decal box (beyond the near plane, far above any surface). */
	UPROPERTY(EditDefaultsOnly, Category = "Decal Warm-Up", meta = (ClampMin = "20"))
	float CameraDistance = 100.f;

	/** Half size in uu of each warm-up decal box: small enough to touch no surface at CameraDistance. */
	UPROPERTY(EditDefaultsOnly, Category = "Decal Warm-Up", meta = (ClampMin = "1"))
	float BoxHalfSize = 4.f;

	/** Every material added in this world (strong references for the world's life). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInterface>> Materials;

	/** Warm-up decals still drawn (one per material added since the last FinishWarmUp). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDecalComponent>> Decals;

	/** Camera seconds left before FinishWarmUp. */
	float RemainingSeconds = 0.f;
};
