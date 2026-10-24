// Copyright Rogue Rigs. Shared light camera shake played on the local player's camera (task-310-01).

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "RRCameraShake.generated.h"

/**
 * Light Perlin-noise camera shake built entirely in C++ (no Blueprint asset).
 * Its root pattern is a URRCameraShakePattern that reads the tunables below.
 * Default values are the bonus chest lid slam preset. Play it through URRCameraShakeLibrary::PlayCameraShake.
 * A Blueprint or C++ subclass can override the defaults for another preset.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API URRCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	/** Creates the Perlin pattern subobject and sets it as the root pattern. */
	URRCameraShake(const FObjectInitializer& ObjectInitializer);

	/** Total shake duration in seconds. */
	float GetDuration() const { return Duration; }

	/** Blend-out time in seconds at the end of the shake. */
	float GetBlendOutTime() const { return BlendOutTime; }

	/** Peak translation per camera-local axis in uu (X forward, Y right, Z up). */
	const FVector& GetTranslationAmplitude() const { return TranslationAmplitude; }

	/** Perlin noise frequency in cycles per second. */
	float GetFrequency() const { return Frequency; }

protected:
	/** Total shake duration in seconds (chest preset 0.2 s, provisional, owner 2026-10-23). */
	UPROPERTY(EditDefaultsOnly, Category = "RogueRigs|CameraShake", meta = (ClampMin = "0.01", Units = "s"))
	float Duration = 0.2f;

	/** Blend-out time in seconds at the end of the shake, so it does not stop on a jump (provisional, owner 2026-10-23). */
	UPROPERTY(EditDefaultsOnly, Category = "RogueRigs|CameraShake", meta = (ClampMin = "0.0", Units = "s"))
	float BlendOutTime = 0.08f;

	/**
	 * Peak translation per camera-local axis in uu (X forward/zoom, Y right, Z up).
	 * Chest preset about 4 uu on the two screen-plane axes, none along the view (provisional, owner 2026-10-23). No rotation.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "RogueRigs|CameraShake")
	FVector TranslationAmplitude = FVector(0.f, 4.f, 4.f);

	/** Perlin noise frequency in cycles per second (provisional, owner 2026-10-23). */
	UPROPERTY(EditDefaultsOnly, Category = "RogueRigs|CameraShake", meta = (ClampMin = "0.1"))
	float Frequency = 25.f;
};
