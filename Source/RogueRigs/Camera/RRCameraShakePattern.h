// Copyright Rogue Rigs. Perlin-noise translation pattern used by URRCameraShake (task-310-01).

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "RRCameraShakePattern.generated.h"

/**
 * Translation-only Perlin-noise shake pattern. It reads duration, blend-out, amplitude and frequency
 * from its owning URRCameraShake, so the tunables live on the shake class. No rotation, no FOV change.
 * Written in the project because the engine Perlin pattern lives in the EngineCameras plugin.
 */
UCLASS()
class ROGUERIGS_API URRCameraShakePattern : public UCameraShakePattern
{
	GENERATED_BODY()

protected:
	/** Reports the fixed duration and blend times of the owning shake. */
	virtual void GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const override;

	/** Starts the timing state and picks random noise offsets per axis. */
	virtual void StartShakePatternImpl(const FCameraShakePatternStartParams& Params) override;

	/** Advances time and writes the camera-local translation offset. */
	virtual void UpdateShakePatternImpl(const FCameraShakePatternUpdateParams& Params, FCameraShakePatternUpdateResult& OutResult) override;

	/** True once the duration has elapsed. */
	virtual bool IsFinishedImpl() const override;

	/** Stops the shake, immediately or through its blend-out. */
	virtual void StopShakePatternImpl(const FCameraShakePatternStopParams& Params) override;

	/** Resets the timing state. */
	virtual void TeardownShakePatternImpl() override;

	/** Timing state (elapsed time, blend weights, finished flag). */
	FCameraShakeState State;

	/** Random start offsets into the noise, one per axis, so each play looks different. */
	FVector NoiseOffset = FVector::ZeroVector;
};
