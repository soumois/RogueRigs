// Copyright Rogue Rigs. One static call to play a camera shake on the local player's camera (task-310-01).

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "RRCameraShakeLibrary.generated.h"

class UCameraShakeBase;

/** Static entry point any actor (C++ or Blueprint) uses to shake the local player's camera. */
UCLASS()
class ROGUERIGS_API URRCameraShakeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Starts a camera shake on the first local player's camera manager. No distance falloff, no time dilation.
	 * @param WorldContextObject Any object of the world (usually the calling actor).
	 * @param Scale Multiplier on the shake amplitude (1 = preset strength); <= 0 plays nothing.
	 * @param ShakeClass Shake to play; null = URRCameraShake (chest preset).
	 * @return The started shake instance, or null when there is no local player camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "RogueRigs|CameraShake", meta = (WorldContext = "WorldContextObject"))
	static UCameraShakeBase* PlayCameraShake(const UObject* WorldContextObject, float Scale = 1.f, TSubclassOf<UCameraShakeBase> ShakeClass = nullptr);
};
