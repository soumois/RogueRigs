// Rogue Rigs - grey-box explosion visual: a mesh growing from 0 to its radius while fading (or growing, holding, then fading), then destroyed.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRExplosionFX.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

/**
 * Explosion visual spawned by URRExplosionSubsystem at the explosion center (sys-explosions). Abstract: a Blueprint child
 * sets the mesh (a sphere) and a translucent material with the scalar parameter FadeParameterName. Over Duration the mesh
 * scale goes linearly from 0 to Radius / MeshRadius. With HoldSeconds = 0 (default, e.g. BP_HealRing) the parameter goes from
 * 1 to 0 over the same Duration. With HoldSeconds > 0 (explosions, set by the spawner) the mesh grows at full opacity, holds
 * its final state (full size, opacity 1) for HoldSeconds, then fades from 1 to 0 over FadeSeconds at full size. The actor
 * destroys itself at GetLifetime(). No collision, no gameplay effect.
 */
UCLASS(Abstract, Blueprintable)
class ROGUERIGS_API ARRExplosionFX : public AActor
{
	GENERATED_BODY()

public:
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

	/** Seconds from BeginPlay to the self-destruction: Duration, plus HoldSeconds + FadeSeconds when HoldSeconds > 0. */
	UFUNCTION(BlueprintPure, Category = "Explosion")
	float GetLifetime() const;

	/** Creates the dynamic material on slot 0 and shows the start state (scale 0, opacity 1). */
	virtual void BeginPlay() override;

	/** Advances the growth and the fade; destroys the actor at Duration. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Sets the mesh scale (GrowAlpha 0 = nothing, 1 = full radius) and the fade parameter to Opacity. */
	void ApplyState(float GrowAlpha, float Opacity);

	/** Applies the state of the timeline at Age (growth, hold, fade; see the class comment). */
	void ApplyAge();

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

	/** Dynamic instance of the mesh's slot 0 material; null when the mesh has no material. */
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> FadeMaterial;

	/** Radius the mesh grows to, set by the spawner. */
	float Radius = 0.f;

	/** Seconds since BeginPlay. */
	float Age = 0.f;
};
