// Copyright Rogue Rigs. Perlin-noise translation pattern used by URRCameraShake (task-310-01).

#include "Camera/RRCameraShakePattern.h"
#include "Camera/RRCameraShake.h"

void URRCameraShakePattern::GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const
{
	const URRCameraShake* Shake = GetShakeInstance<URRCameraShake>();
	const URRCameraShake* Source = Shake ? Shake : GetDefault<URRCameraShake>();
	OutInfo.Duration = FCameraShakeDuration(Source->GetDuration());
	OutInfo.BlendIn = 0.f;
	OutInfo.BlendOut = FMath::Min(Source->GetBlendOutTime(), Source->GetDuration());
}

void URRCameraShakePattern::StartShakePatternImpl(const FCameraShakePatternStartParams& Params)
{
	State.Start(this, Params);
	NoiseOffset = FVector(FMath::FRandRange(0.f, 100.f), FMath::FRandRange(0.f, 100.f), FMath::FRandRange(0.f, 100.f));
}

void URRCameraShakePattern::UpdateShakePatternImpl(const FCameraShakePatternUpdateParams& Params, FCameraShakePatternUpdateResult& OutResult)
{
	const float BlendWeight = State.Update(Params.DeltaTime);
	const URRCameraShake* Shake = GetShakeInstance<URRCameraShake>();
	if (BlendWeight <= 0.f || !Shake)
	{
		return;
	}

	const float Phase = State.GetElapsedTime() * Shake->GetFrequency();
	const FVector& Amplitude = Shake->GetTranslationAmplitude();
	OutResult.Location = FVector(
		Amplitude.X * FMath::PerlinNoise1D(NoiseOffset.X + Phase),
		Amplitude.Y * FMath::PerlinNoise1D(NoiseOffset.Y + Phase),
		Amplitude.Z * FMath::PerlinNoise1D(NoiseOffset.Z + Phase));

	// The base class applies ShakeScale * DynamicScale (default flags); the blend weight is applied here.
	OutResult.ApplyScale(BlendWeight);
}

bool URRCameraShakePattern::IsFinishedImpl() const
{
	return !State.IsPlaying();
}

void URRCameraShakePattern::StopShakePatternImpl(const FCameraShakePatternStopParams& Params)
{
	State.Stop(Params.bImmediately);
}

void URRCameraShakePattern::TeardownShakePatternImpl()
{
	State = FCameraShakeState();
}
