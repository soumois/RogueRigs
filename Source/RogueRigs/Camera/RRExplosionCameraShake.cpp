// Copyright Rogue Rigs. Explosion camera shake preset played at each resolved explosion (task-418-01).

#include "Camera/RRExplosionCameraShake.h"

URRExplosionCameraShake::URRExplosionCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Explosion preset (provisional, owner 2026-11-02, task-418 Q6); BlendOutTime keeps the base 0.08 s.
	Duration = 0.35f;
	TranslationAmplitude = FVector(0.f, 10.f, 10.f);
	Frequency = 25.f;
}
