// Copyright Rogue Rigs. Heavy slam camera shake preset played at the heavy's feet on the slam frame (task-419-01).

#pragma once

#include "CoreMinimal.h"
#include "Camera/RRCameraShake.h"
#include "RRHeavySlamCameraShake.generated.h"

/**
 * Heavy slam preset of the shared camera shake: translation-only Perlin shake played by ARRHeavy::PerformSlam at the heavy's
 * feet on each slam frame (never during the telegraph, miner hit or not), through URRCameraShakeLibrary::PlayCameraShakeAtLocation.
 * Defaults (provisional, owner 2026-11-02, task-419 Q8): 8 uu on the two screen-plane axes at the heavy, 0.3 s, 25 Hz.
 * Being its own class, it adds to a hit shake already playing (URRCameraShakeModifier mixing); a second slam restarts it.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API URRHeavySlamCameraShake : public URRCameraShake
{
	GENERATED_BODY()

public:
	/** Sets the slam preset defaults over the chest preset ones. */
	URRHeavySlamCameraShake(const FObjectInitializer& ObjectInitializer);
};
