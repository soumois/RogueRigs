// Rogue Rigs - grey-box explosion visual: a mesh growing from 0 to the explosion radius while fading, then destroyed.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRExplosionFX.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

/**
 * Explosion visual spawned by URRExplosionSubsystem at the explosion center (sys-explosions). Abstract: a Blueprint child
 * sets the mesh (a sphere) and a translucent material with the scalar parameter FadeParameterName. Over Duration the mesh
 * scale goes linearly from 0 to Radius / MeshRadius and the parameter from 1 to 0; the actor then destroys itself.
 * No collision, no gameplay effect.
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

	/** Creates the dynamic material on slot 0 and shows the start state (scale 0, opacity 1). */
	virtual void BeginPlay() override;

	/** Advances the growth and the fade; destroys the actor at Duration. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Sets the mesh scale and the fade parameter for Alpha in [0, 1] (0 = start, 1 = end). */
	void ApplyProgress(float Alpha);

	/** The growing mesh (set the sphere and the material in the Blueprint). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosion")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Seconds to grow to the radius and fade out (explosions gdd: 0.25 s). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosion", meta = (ClampMin = "0.01"))
	float Duration = 0.25f;

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
