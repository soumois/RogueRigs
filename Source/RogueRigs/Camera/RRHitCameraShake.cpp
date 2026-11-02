// Copyright Rogue Rigs. Hit camera shake preset played when the miner loses health (task-417-01).

#include "Camera/RRHitCameraShake.h"

URRHitCameraShake::URRHitCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Hit preset (provisional, owner 2026-11-02, task-417 Q4); BlendOutTime keeps the base 0.08 s.
	Duration = 0.2f;
	TranslationAmplitude = FVector(0.f, 5.f, 5.f);
	Frequency = 25.f;
}
