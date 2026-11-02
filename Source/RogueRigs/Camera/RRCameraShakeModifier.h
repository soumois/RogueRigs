// Copyright Rogue Rigs. Camera shake modifier that mixes the project shakes and caps their summed offset (task-416-01).

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "RRCameraShakeModifier.generated.h"

/**
 * Camera shake modifier every project shake plays through (URRCameraShakeLibrary adds it on the local
 * player camera manager the first time a shake is played). Mixing rules:
 * - shakes of different classes play together and their translation offsets add up (engine behaviour);
 * - a new shake of a single-instance class already playing restarts it at the stronger of the old and new scales;
 * - the summed translation of all its shakes is clamped to MaxSummedTranslation each frame.
 * Shakes started with APlayerCameraManager::StartCameraShake go through the engine modifier and are not capped.
 */
UCLASS()
class ROGUERIGS_API URRCameraShakeModifier : public UCameraModifier_CameraShake
{
	GENERATED_BODY()

public:
	/** Default constructor. */
	URRCameraShakeModifier(const FObjectInitializer& ObjectInitializer);

	/** Adds a shake; a single-instance class already playing restarts at max(old scale, new scale). */
	virtual UCameraShakeBase* AddCameraShake(TSubclassOf<UCameraShakeBase> NewShake, const FAddCameraShakeParams& Params) override;

	/** Applies every active shake, then clamps the summed translation offset to MaxSummedTranslation. */
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

	/** Cap on the length of the summed translation offset in uu. */
	float GetMaxSummedTranslation() const { return MaxSummedTranslation; }

	/** Scale factor a located shake keeps at its max distance (1 at the epicenter, linear in between). */
	float GetEdgeFalloffScale() const { return EdgeFalloffScale; }

protected:
	/** Cap on the length of the summed translation offset of all shakes in uu, so a pile-up stays readable (12 uu, provisional, owner 2026-11-02). */
	UPROPERTY(EditDefaultsOnly, Category = "RogueRigs|CameraShake", meta = (ClampMin = "0.0", Units = "cm"))
	float MaxSummedTranslation = 12.f;

	/** Scale factor of a located shake at exactly its max distance; nothing plays beyond it (0.5, provisional, owner 2026-11-02). */
	UPROPERTY(EditDefaultsOnly, Category = "RogueRigs|CameraShake", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EdgeFalloffScale = 0.5f;
};
