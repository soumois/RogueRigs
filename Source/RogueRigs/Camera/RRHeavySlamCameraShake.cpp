// Copyright Rogue Rigs. Heavy slam camera shake preset played at the heavy's feet on the slam frame (task-419-01).

#include "Camera/RRHeavySlamCameraShake.h"

URRHeavySlamCameraShake::URRHeavySlamCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Slam preset (provisional, owner 2026-11-02, task-419 Q8); BlendOutTime keeps the base 0.08 s.
	Duration = 0.3f;
	TranslationAmplitude = FVector(0.f, 8.f, 8.f);
	Frequency = 25.f;
}
