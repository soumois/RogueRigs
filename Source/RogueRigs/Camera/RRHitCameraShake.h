// Copyright Rogue Rigs. Hit camera shake preset played when the miner loses health (task-417-01).

#pragma once

#include "CoreMinimal.h"
#include "Camera/RRCameraShake.h"
#include "RRHitCameraShake.generated.h"

/**
 * Hit preset of the shared camera shake: translation-only Perlin shake played by ARRMiner on each damage taken
 * (lethal hit included, never on a hit refused by the i-frames). Same strength whatever the damage.
 * Defaults (provisional, owner 2026-11-02, task-417 Q4): 5 uu on the two screen-plane axes, 0.2 s, 25 Hz.
 * Being its own class, it adds to an explosion or slam shake already playing (URRCameraShakeModifier mixing).
 */
UCLASS(Blueprintable)
class ROGUERIGS_API URRHitCameraShake : public URRCameraShake
{
	GENERATED_BODY()

public:
	/** Sets the hit preset defaults over the chest preset ones. */
	URRHitCameraShake(const FObjectInitializer& ObjectInitializer);
};
