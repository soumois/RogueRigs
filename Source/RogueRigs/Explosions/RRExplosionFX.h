// Rogue Rigs - grey-box explosion visual: a mesh growing from 0 to its radius while fading (or growing, holding, then fading), with an optional Niagara burst and ground shockwave decal, then destroyed.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRExplosionFX.generated.h"

class UDecalComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UNiagaraComponent;
class UNiagaraSystem;
class UStaticMeshComponent;

/**
 * Explosion visual spawned by URRExplosionSubsystem at the explosion center (sys-explosions). Abstract: a Blueprint child
 * sets the mesh (a sphere) and a translucent material with the scalar parameter FadeParameterName. Over Duration the mesh
 * scale goes linearly from 0 to Radius / MeshRadius. With HoldSeconds = 0 (default, e.g. BP_HealRing) the parameter goes from
 * 1 to 0 over the same Duration. With HoldSeconds > 0 (explosions, set by the spawner) the mesh grows at full opacity, holds
 * its final state (full size, opacity 1) for HoldSeconds, then fades from 1 to 0 over FadeSeconds at full size.
 * Optional, both off when unset (BP_ExplosionFX; set by BP_ExplosionFX_Barrel, barrel and kamikaze): BurstSystem, a one-shot Niagara system played once at BeginPlay at
 * the center with the float user parameter BurstRadiusParameterName = Radius; ShockwaveMaterial, a deferred decal projected
 * straight down (floor and rock tops) whose box covers Radius, with the scalars Radius, Progress and Opacity on the ring's own
 * timeline: Progress 0 to 1 over ShockwaveGrowSeconds at Opacity 1, then Opacity 1 to 0 over ShockwaveFadeSeconds, so the ring
 * outlives the sphere. The mesh is hidden at GetVisualLifetime, the decal at GetShockwaveLifetime; the actor destroys itself at
 * GetLifetime(), which covers the ring when a shockwave is set and BurstSeconds when a burst is set. No collision, no gameplay
 * effect.
 */
UCLASS(Abstract, Blueprintable)
class ROGUERIGS_API ARRExplosionFX : public AActor
{
	GENERATED_BODY()

public:
	/** Shockwave material parameter names (same names as the ground telegraph material, sys-ground-telegraph). */
	static const FName ShockwaveRadiusParameterName;
	static const FName ShockwaveProgressParameterName;
	static const FName ShockwaveOpacityParameterName;

	/** Creates the mesh (root, no collision, no shadow) and enables the tick. */
	ARRExplosionFX();

	/** Sets the radius the mesh grows to, in uu. Call before FinishSpawning (the subsystem spawns deferred). */
	UFUNCTION(BlueprintCallable, Category = "Explosion")
	void SetRadius(float InRadius) { Radius = FMath::Max(InRadius, 0.f); }

	/** Radius the mesh grows to, in uu. */
	UFUNCTION(BlueprintPure, Category = "Explosion")
	float GetRadius() const { return Radius; }

	/**
	 * Sets the seconds the mesh stays at its final state (full size, opacity 1) between the growth and the fade; 0 = grow and
	 * fade together (legacy). Call before FinishSpawning (the explosion subsystem passes its VisualHoldSeconds).
	 */
	UFUNCTION(BlueprintCallable, Category = "Explosion")
	void SetHoldSeconds(float InHoldSeconds) { HoldSeconds = FMath::Max(InHoldSeconds, 0.f); }

	/** Seconds the mesh holds its final state before fading (0 = grow and fade together). */
	UFUNCTION(BlueprintPure, Category = "Explosion")
	float GetHoldSeconds() const { return HoldSeconds; }

	/** Seconds of the sphere visual: Duration, plus HoldSeconds + FadeSeconds when HoldSeconds > 0. */
	UFUNCTION(BlueprintPure, Category = "Explosion")
	float GetVisualLifetime() const;

	/** Seconds of the shockwave ring: ShockwaveGrowSeconds + ShockwaveFadeSeconds (counts only when ShockwaveMaterial is set). */
	UFUNCTION(BlueprintPure, Category = "Explosion|Shockwave")
	float GetShockwaveLifetime() const;

	/**
	 * Seconds from BeginPlay to the self-destruction: the longest of GetVisualLifetime(), GetShockwaveLifetime() when
	 * ShockwaveMaterial is set, and BurstSeconds when BurstSystem is set.
	 */
	UFUNCTION(BlueprintPure, Category = "Explosion")
	float GetLifetime() const;

	/** The playing burst component (null when BurstSystem is unset). */
	UNiagaraComponent* GetBurstComponent() const { return BurstComponent; }

	/** The shockwave ring material of this class (null = no ring); read on the class default to warm it up at level start. */
	UMaterialInterface* GetShockwaveMaterial() const { return ShockwaveMaterial; }

	/** The shockwave decal (null when ShockwaveMaterial is unset). */
	UDecalComponent* GetShockwaveDecal() const { return ShockwaveDecal; }

	/** Creates the dynamic material on slot 0, the optional burst and shockwave, and shows the start state (scale 0, opacity 1). */
	virtual void BeginPlay() override;

	/** Advances the growth and the fade; hides the visual at GetVisualLifetime and destroys the actor at GetLifetime. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Sets the mesh scale (GrowAlpha 0 = nothing, 1 = full radius) and the fade parameter to Opacity. */
	void ApplySphereState(float GrowAlpha, float Opacity);

	/** Sets the shockwave Progress and Opacity scalars; no-op without a shockwave. */
	void ApplyShockwaveState(float Progress, float Opacity);

	/** Applies the sphere and the shockwave timelines at Age, each until its part is hidden (see the class comment). */
	void ApplyAge();

	/** Plays BurstSystem once at the actor location with the radius user parameter; no-op when unset. */
	void StartBurst();

	/** Creates the shockwave decal with a dynamic instance of ShockwaveMaterial; no-op when unset. */
	void CreateShockwave();

	/** The growing mesh (set the sphere and the material in the Blueprint). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Seconds to grow to the radius (explosions gdd: 0.25 s); with HoldSeconds = 0 the fade happens over the same time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = "0.01"))
	float Duration = 0.25f;

	/** Seconds of the fade after the hold, at full size; used only when HoldSeconds > 0 (explosions gdd: fades in 0.25 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = "0.01"))
	float FadeSeconds = 0.25f;

	/** Seconds the final state (full size, opacity 1) is held before the fade; 0 = legacy grow-and-fade. Set by the spawner. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = "0"))
	float HoldSeconds = 0.f;

	/** Radius of the mesh at scale 1, in uu (engine sphere: 50). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = "0.01"))
	float MeshRadius = 50.f;

	/** Scalar material parameter driven from 1 to 0 (the material's opacity). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion")
	FName FadeParameterName = TEXT("Opacity");

	/** Optional one-shot (non-looping) Niagara system played once at the center at BeginPlay; unset = no burst. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Burst")
	TObjectPtr<UNiagaraSystem> BurstSystem;

	/** Float user parameter of BurstSystem receiving the visual radius in uu (Radius); ignored if the system does not declare it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Burst")
	FName BurstRadiusParameterName = TEXT("User.Radius");

	/** Seconds the actor stays alive for the burst to finish when BurstSystem is set (provisional, owner 2026-10-16). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Burst", meta = (ClampMin = "0"))
	float BurstSeconds = 1.5f;

	/** Optional deferred decal material of the ground shockwave ring (scalars Radius, Progress, Opacity); unset = no ring. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Shockwave")
	TObjectPtr<UMaterialInterface> ShockwaveMaterial;

	/** Half height of the shockwave projection box in uu (above and below the center): reaches the floor and rock tops. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Shockwave", meta = (ClampMin = "1"))
	float ShockwaveHalfDepth = 256.f;

	/** Seconds for the ring to grow from the center to Radius (Progress 0 to 1) at opacity 1 (provisional, owner 2026-10-16). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Shockwave", meta = (ClampMin = "0.01"))
	float ShockwaveGrowSeconds = 0.6f;

	/** Seconds for the ring to fade (Opacity 1 to 0) at full size after its growth (provisional, owner 2026-10-16). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Shockwave", meta = (ClampMin = "0.01"))
	float ShockwaveFadeSeconds = 0.2f;

	/** Decal sort order of the shockwave: drawn above the ground telegraphs (sort order 0) it overlaps. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion|Shockwave")
	int32 ShockwaveSortOrder = 1;

	/** Dynamic instance of the mesh's slot 0 material; null when the mesh has no material. */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> FadeMaterial;

	/** Burst component created at BeginPlay when BurstSystem is set (destroyed with the actor). */
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> BurstComponent;

	/** Shockwave decal created at BeginPlay when ShockwaveMaterial is set (destroyed with the actor). */
	UPROPERTY(Transient)
	TObjectPtr<UDecalComponent> ShockwaveDecal;

	/** Dynamic instance of ShockwaveMaterial on the decal. */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ShockwaveMaterialInstance;

	/** Radius the mesh grows to, set by the spawner. */
	float Radius = 0.f;

	/** Seconds since BeginPlay. */
	float Age = 0.f;

	/** True once the faded mesh is hidden (sphere over; the ring or the burst may still play). */
	bool bSphereHidden = false;

	/** True once the faded shockwave decal is hidden (ring over; the sphere or the burst may still play). */
	bool bShockwaveHidden = false;
};
