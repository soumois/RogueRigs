// Copyright Rogue Rigs. Explosion camera shake preset played at each resolved explosion (task-418-01).

#pragma once

#include "CoreMinimal.h"
#include "Camera/RRCameraShake.h"
#include "RRExplosionCameraShake.generated.h"

/**
 * Explosion preset of the shared camera shake: translation-only Perlin shake played by URRExplosionSubsystem at the centre
 * of each resolved explosion (never during a barrel or kamikaze fuse), through URRCameraShakeLibrary::PlayCameraShakeAtLocation.
 * Defaults (provisional, owner 2026-11-02, task-418 Q6): 10 uu on the two screen-plane axes (the barrel strength at scale 1),
 * 0.35 s, 25 Hz. The per-source strength is the ShakeScale column of ExplosionStats.csv (kamikaze 0.7 = 7 uu).
 * Being its own class, it adds to a hit shake already playing (URRCameraShakeModifier mixing); a chain link restarts it.
 */
UCLASS(Blueprintable)
class ROGUERIGS_API URRExplosionCameraShake : public URRCameraShake
{
	GENERATED_BODY()

public:
	/** Sets the explosion preset defaults over the chest preset ones. */
	URRExplosionCameraShake(const FObjectInitializer& ObjectInitializer);
};
