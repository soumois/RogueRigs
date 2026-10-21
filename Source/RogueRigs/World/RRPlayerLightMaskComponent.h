// Rogue Rigs - player light mask writer (task-272-07, task-272-13): pushes the miner position into MPC_PlayerLight each tick; the tunables only for console sweeps.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RRPlayerLightMaskComponent.generated.h"

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

/**
 * Feeds the radial post-process light mask around the miner (M_PP_PlayerLightMask, mpp-player-light-mask). Each tick
 * (TG_PostUpdateWork, after movement, so the position matches the rendered frame) it writes the owner location into the
 * PlayerPosition vector of the world's instance of ParameterCollection (MPC_PlayerLight, loaded in the constructor).
 * MPC_PlayerLight is the owner's single tuning place: editing its defaults in the editor updates the look live, and this
 * component never touches those parameters by default. bDriveTunables (console sweeps only, `setnopec ... bDriveTunables 1`)
 * makes it also push the tunables below each tick, overriding the collection defaults in the world instance.
 * The engine skips unchanged values and ignores names missing from the collection (no log). The world instance is cached
 * and fetched again when the world changes. No tick on a dedicated server; a missing collection logs one warning and
 * disables the tick.
 */
UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRPlayerLightMaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Loads MPC_PlayerLight; sets up a tick in TG_PostUpdateWork. */
	URRPlayerLightMaskComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Writes the owner position (and the tunables when bDriveTunables) into the collection instance of the world. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** The collection this component writes (MPC_PlayerLight by default). */
	UMaterialParameterCollection* GetParameterCollection() const { return ParameterCollection; }

protected:
	/** Disables the tick on a dedicated server or without a collection (one warning); otherwise writes once immediately. */
	virtual void BeginPlay() override;

	/** Returns the collection instance of the current world, fetched once and again only when the world changes. Null if none. */
	UMaterialParameterCollectionInstance* ResolveCollectionInstance();

	/** Material Parameter Collection read by M_PP_PlayerLightMask (/Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask")
	TObjectPtr<UMaterialParameterCollection> ParameterCollection;

	/**
	 * Console sweeps only: when true, the tunables below are pushed each tick and override the MPC_PlayerLight defaults.
	 * False (default): only PlayerPosition is written; the owner tunes the look in MPC_PlayerLight.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask")
	bool bDriveTunables = false;

	/** Enable (0 or 1): 0 = the material outputs the scene colour unchanged. Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0", ClampMax = "1"))
	float Enable = 1.f;

	/** CenterBrightness: scene-colour gain at the miner. Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float CenterBrightness = 3.f;

	/** FarBrightness: scene-colour gain far from the miner. Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float FarBrightness = 0.4f;

	/** Radius (uu): XY distance of the falloff. Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "1", Units = "cm"))
	float Radius = 225.f;

	/** Exponent: falloff steepness. Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float Exponent = 2.f;

	/** TintStrength: how much WarmTint / FarTint colour the picture (0 = none). Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float TintStrength = 1.f;

	/** WarmTint: tint at the miner. Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask")
	FLinearColor WarmTint = FLinearColor(1.f, 0.76f, 0.62f, 1.f);

	/** FarTint: tint far from the miner. Pushed only when bDriveTunables. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask")
	FLinearColor FarTint = FLinearColor(0.85f, 0.55f, 0.9f, 1.f);

	/** Cached collection instance of CachedWorld. */
	TWeakObjectPtr<UMaterialParameterCollectionInstance> CachedInstance;

	/** World the cached instance belongs to. */
	TWeakObjectPtr<UWorld> CachedWorld;

	/** Writes the position, and the tunables when bDriveTunables (BeginPlay and every tick). */
	void PushParameters();
};
