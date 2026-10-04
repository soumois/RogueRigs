// Rogue Rigs - floating damage number (robots and target dummy): a world-space text that rises, fades and faces the camera, then destroys itself.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RRFloatingDamageText.generated.h"

class UMaterialInterface;
class UTextRenderComponent;

/**
 * M1 damage number (damage-numbers): one short-lived actor per hit, spawned through SpawnForHit by ARREnemy and ARRTargetDummy.
 * Shows the rounded damage with a UTextRenderComponent, rises at RiseSpeed, fades its alpha to 0 over Lifetime,
 * and turns every tick to face the active player camera. A second, near-black text render (OutlineRender) sits just
 * behind it, slightly larger and offset, as a dark outline/drop shadow for readability; it shows the same value and fades with it. Not for horde scale: the M2 version uses Niagara.
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

	/**
	 * Spawns TextClass at BaseLocation plus a random horizontal jitter in [-JitterRadius, JitterRadius] on X and Y
	 * (always spawns, ignoring collision), then calls InitDamageText. Shared by every actor that shows damage numbers;
	 * the caller picks color and size (normal or critical). Returns nullptr when World or TextClass is null.
	 */
	static ARRFloatingDamageText* SpawnForHit(UWorld* World, TSubclassOf<ARRFloatingDamageText> TextClass, const FVector& BaseLocation,
		float JitterRadius, float Damage, const FLinearColor& Color, float WorldSize, AActor* Owner = nullptr);

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

	/** Dark copy of the text drawn just behind TextRender (outline/drop shadow). Attached to TextRender, no collision. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage Text")
	TObjectPtr<UTextRenderComponent> OutlineRender;

	/** Color of the outline text (near-black); its alpha is scaled by the same fade as the main text. */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Text|Outline")
	FLinearColor OutlineColor = FLinearColor(0.02f, 0.02f, 0.02f, 1.f);

	/** Outline world size as a multiple of the main text size (above 1 so it shows around the glyphs). */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Text|Outline", meta = (ClampMin = "1.0"))
	float OutlineSizeScale = 1.12f;

	/** Offset of the outline from the main text, in the text frame (X toward the camera: keep it negative so the outline stays behind; Y right, Z up for a drop shadow). */
	UPROPERTY(EditDefaultsOnly, Category = "Damage Text|Outline", meta = (Units = "cm"))
	FVector OutlineOffset = FVector(-2.f, 2.f, -3.f);

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
