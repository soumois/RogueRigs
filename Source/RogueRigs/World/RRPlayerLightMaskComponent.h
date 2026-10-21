// Rogue Rigs - player light mask writer (task-272-07): pushes the miner position and the mask tunables into MPC_PlayerLight each tick.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RRPlayerLightMaskComponent.generated.h"

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

/**
 * Feeds the radial post-process light mask around the miner (M_PP_PlayerLightMask, mpp-player-light-mask). Each tick
 * (TG_PostUpdateWork, after movement, so the position matches the rendered frame) it writes the owner location into the
 * PlayerPosition vector of the world's instance of ParameterCollection (MPC_PlayerLight, loaded in the constructor) and
 * every tunable below into the matching scalar or vector parameter. The engine skips unchanged values, so a still miner
 * with fixed tunables costs only the name lookups. The world instance is cached and fetched again when the world changes.
 * Defaults keep the picture unchanged (bMaskEnabled false: the material outputs the scene colour). No tick on a dedicated
 * server; a missing collection logs one warning and disables the tick. The tunables are EditAnywhere so a console
 * `set` / `setnopec` on the component changes them at runtime (look sweeps).
 */
UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRPlayerLightMaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Loads MPC_PlayerLight; sets up a tick in TG_PostUpdateWork. */
	URRPlayerLightMaskComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Writes the owner position and the tunables into the collection instance of the world. */
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

	/** MaskEnabled: false = the material outputs the scene colour unchanged (default, look untouched). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask")
	bool bMaskEnabled = false;

	/** MaskRadius (uu): XY distance at which the mask falls to one half (220, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "1", Units = "cm"))
	float MaskRadius = 220.f;

	/** MaskExponent: falloff steepness, mask = 1 / (1 + (r / MaskRadius)^MaskExponent) (2, provisional). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float MaskExponent = 2.f;

	/** NearGain: extra scene-colour gain at the mask centre, scaled by the mask (0 = none). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float NearGain = 0.f;

	/** FarGain: scene-colour gain everywhere (1 = unchanged far from the miner). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float FarGain = 1.f;

	/** AddIntensity: additive NearColour light scaled by the mask (0 = none). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask", meta = (ClampMin = "0"))
	float AddIntensity = 0.f;

	/** NearColour: tint at the mask centre (warm, 1 / 0.76 / 0.62). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask")
	FLinearColor NearColour = FLinearColor(1.f, 0.76f, 0.62f, 1.f);

	/** FarColour: tint far from the miner (white = unchanged). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RogueRigs|LightMask")
	FLinearColor FarColour = FLinearColor(1.f, 1.f, 1.f, 1.f);

	/** Cached collection instance of CachedWorld. */
	TWeakObjectPtr<UMaterialParameterCollectionInstance> CachedInstance;

	/** World the cached instance belongs to. */
	TWeakObjectPtr<UWorld> CachedWorld;

	/** Writes everything once (BeginPlay and every tick). */
	void PushParameters();
};
