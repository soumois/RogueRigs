// Rogue Rigs - debug floating damage number: a world-space text that rises, fades and faces the camera, then destroys itself.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRFloatingDamageText.generated.h"

class UMaterialInterface;
class UTextRenderComponent;

/**
 * M1 debug damage number (damage-numbers): one short-lived actor per hit, spawned by ARRTargetDummy.
 * Shows the rounded damage with a UTextRenderComponent, rises at RiseSpeed, fades its alpha to 0 over Lifetime,
 * and turns every tick to face the active player camera. Not for horde scale: the M2 version uses Niagara.
 */
UCLASS()
class ROGUERIGS_API ARRFloatingDamageText : public AActor
{
	GENERATED_BODY()

public:
	/** Creates the text render (centered, translucent text material so the alpha fade shows). */
	ARRFloatingDamageText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Sets the shown value (rounded to an integer), color and world size. Call right after spawning. */
	void InitDamageText(float Damage, const FLinearColor& Color, float WorldSize);

	/** Rises, fades and faces the camera; destroys the actor once Lifetime has elapsed. */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Applies TextMaterial and the lifetime, then faces the camera once so the first frame is readable. */
	virtual void BeginPlay() override;

	/** Turns the text toward the active player camera (parallel to the view plane). */
	void FaceCamera();

	/** World-space text showing the damage value. Root component, no collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage Text")
	TObjectPtr<UTextRenderComponent> TextRender;

	/** Material of the text. Must be translucent and use vertex color alpha for the fade (engine DefaultTextMaterialTranslucent by default). */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Text")
	TObjectPtr<UMaterialInterface> TextMaterial;

	/** Seconds from spawn to full fade and destruction. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Text", meta = (ClampMin = "0.05", Units = "s"))
	float Lifetime = 0.8f;

	/** Upward speed of the text (uu/s, world Z). */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Text", meta = (Units = "cm/s"))
	float RiseSpeed = 150.f;

	/** Color set by InitDamageText; its alpha is scaled by the fade each tick. */
	FLinearColor BaseColor = FLinearColor::White;

	/** Seconds elapsed since BeginPlay. */
	float Age = 0.f;
};
