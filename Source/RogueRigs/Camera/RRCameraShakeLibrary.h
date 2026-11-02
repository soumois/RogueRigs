// Copyright Rogue Rigs. Static calls to play a camera shake on the local player's camera (task-310-01, located call task-416-01).

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "RRCameraShakeLibrary.generated.h"

class APlayerCameraManager;
class UCameraShakeBase;
class URRCameraShakeModifier;

/**
 * Static entry points any actor (C++ or Blueprint) uses to shake the local player's camera.
 * Every shake plays through URRCameraShakeModifier: different shake classes add up, the same class restarts
 * at the stronger scale, the summed translation is capped.
 */
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

	/**
	 * Starts a camera shake caused at a world point, scaled by the 2D distance between Epicenter and the camera
	 * view target (the miner): Scale at the epicenter, falling linearly to Scale * EdgeFalloffScale at MaxDistance,
	 * nothing played beyond MaxDistance.
	 * @param WorldContextObject Any object of the world (usually the calling actor).
	 * @param Epicenter World point where the shake comes from (explosion, slam).
	 * @param MaxDistance 2D distance in uu beyond which nothing plays; <= 0 plays nothing.
	 * @param Scale Multiplier on the shake amplitude at the epicenter; <= 0 plays nothing.
	 * @param ShakeClass Shake to play; null = URRCameraShake (chest preset).
	 * @return The started shake instance, or null when out of range or there is no local player camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "RogueRigs|CameraShake", meta = (WorldContext = "WorldContextObject"))
	static UCameraShakeBase* PlayCameraShakeAtLocation(const UObject* WorldContextObject, FVector Epicenter, float MaxDistance, float Scale = 1.f, TSubclassOf<UCameraShakeBase> ShakeClass = nullptr);

	/**
	 * Distance factor of a located shake: 1 at Distance2D 0, linear down to the modifier's EdgeFalloffScale
	 * at MaxDistance, 0 beyond MaxDistance or when MaxDistance <= 0.
	 */
	UFUNCTION(BlueprintPure, Category = "RogueRigs|CameraShake")
	static float GetDistanceFalloffScale(float Distance2D, float MaxDistance);

protected:
	/** First local player's camera manager of the world, or null. */
	static APlayerCameraManager* GetLocalCameraManager(const UObject* WorldContextObject);

	/** The project shake modifier of CameraManager, added on first use. */
	static URRCameraShakeModifier* GetOrAddShakeModifier(APlayerCameraManager* CameraManager);

	/** Starts ShakeClass (null = URRCameraShake) at Scale through the project shake modifier. */
	static UCameraShakeBase* StartShake(APlayerCameraManager* CameraManager, float Scale, TSubclassOf<UCameraShakeBase> ShakeClass);
};
