// Rogue Rigs - polls the URRCheatSettings default object every frame (paused too) and applies the cheats it requests to its world.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRCheatSubsystem.generated.h"

class ARRMiner;
class UCameraComponent;
class ULightComponentBase;
class UPointLightComponent;

/**
 * Property route of the cheats (sys-cheats): each tick, paused or not, reads the URRCheatSettings class default object,
 * applies the requested cheats to this world through the shared RRCheats actions and resets the one-shot fields.
 * bInvulnerable is applied on change (and on the first tick when true): a true value with no miner yet is retried until one
 * is tagged. bLightHalo (and any LightHalo value while on) is applied on change: a halo point light on the player miner, the
 * directional and sky lights scaled, an exposure bias on the miner camera; turning it off (or the subsystem ending) restores them.
 * Game and PIE worlds; never in shipping builds.
 */
UCLASS()
class ROGUERIGS_API URRCheatSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Game and PIE worlds of non-shipping builds. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** Applies and resets the requested cheats. */
	virtual void Tick(float DeltaTime) override;

	/** Ticks while the game is paused (the level-up screen pauses it). */
	virtual bool IsTickableWhenPaused() const override { return true; }

	/** Stat id of the tick. */
	virtual TStatId GetStatId() const override;

public:
	/** Restores the lights and camera the halo changed. */
	virtual void Deinitialize() override;

	/** Halo light added to the player miner by the light halo cheat; null while the halo is off. */
	UPointLightComponent* GetLightHalo() const { return HaloLight.Get(); }

protected:
	/** Halo values last applied (bOn false = halo off or not applied yet). */
	struct FAppliedHalo
	{
		/** bLightHalo. */
		bool bOn = false;
		/** LightHaloIntensity (candelas). */
		float Intensity = 0.f;
		/** LightHaloRadius (uu). */
		float Radius = 0.f;
		/** LightHaloHeight (uu above the miner). */
		float Height = 0.f;
		/** LightHaloColor. */
		FLinearColor Color = FLinearColor::Black;
		/** bLightHaloShadows. */
		bool bShadows = false;
		/** LightHaloAmbientScale. */
		float AmbientScale = 1.f;
		/** LightHaloExposureBias (EV). */
		float ExposureBias = 0.f;
	};

	/** Halo values requested by the URRCheatSettings default object. */
	static FAppliedHalo ReadHaloSettings();

	/** True when two halo states differ in any value. */
	static bool HaloDiffers(const FAppliedHalo& A, const FAppliedHalo& B);

	/** Removes the halo (light, light scaling, exposure), then applies Wanted when on. False when on but no miner exists yet (retried). */
	bool ApplyLightHalo(const FAppliedHalo& Wanted);

	/** Destroys the halo light and restores the scaled lights and the camera exposure. */
	void RemoveLightHalo();

	/** bInvulnerable value last applied by this subsystem (false at start). */
	bool bAppliedInvulnerable = false;

	/** Light halo values last applied. */
	FAppliedHalo AppliedHalo;

	/** Halo point light attached to the miner. */
	TWeakObjectPtr<UPointLightComponent> HaloLight;

	/** Directional and sky lights scaled by the halo, with their intensity before it. */
	TArray<TPair<TWeakObjectPtr<ULightComponentBase>, float>> ScaledLights;

	/** Miner camera given the halo exposure bias. */
	TWeakObjectPtr<UCameraComponent> HaloCamera;

	/** Camera exposure bias override flag before the halo. */
	bool bCameraHadBiasOverride = false;

	/** Camera exposure bias before the halo. */
	float CameraOriginalBias = 0.f;
};
