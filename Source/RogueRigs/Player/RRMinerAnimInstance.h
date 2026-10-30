// Rogue Rigs - anim instance of the miner: ground speed, move direction relative to the aim, blend speed and play rate for ABP_Miner.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RRMinerAnimInstance.generated.h"

/**
 * Native parent of ABP_Miner. Each update it reads the owning pawn velocity and actor yaw (the miner yaw follows the
 * aim only) and exposes the values the locomotion blend space needs. No gameplay logic, no input.
 * Units: speeds in cm/s (uu/s), angles in degrees. Direction convention: 0 forward (along the aim), 90 right,
 * -90 left, +/-180 backward.
 * Q4 rule (provisional, owner 2026-10-30): the clips run slower than the miner, so the clips are sped up to match the
 * step rhythm: LocomotionPlayRate = clamp(GroundSpeed / RunSpeedForDirection, 1, MaxPlayRate) and the blend space is
 * fed BlendSpeed = GroundSpeed / LocomotionPlayRate. The lower bound 1 is part of the rule: a clip never plays slower
 * than authored; under the run speed the blend space picks the walk clips instead.
 * task-386-16: LocomotionPlayRate is fixed at 1 for now (Q4 rule off) and MoveX/MoveY expose the move vector in the aim
 * frame over MaxWalkSpeed, length clamped to 1, for the simple 2D blend space.
 * task-386-21: the cheat cvar rr.Debug.MinerAnimForce "x y" overrides MoveX, MoveY (clamped to length 1) and
 * GroundSpeed (length * MaxWalkSpeed) for captures; empty = off.
 */
UCLASS()
class ROGUERIGS_API URRMinerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/** Reads the pawn owner velocity and yaw and refreshes every exposed value. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/** Run clip speed for a direction (degrees, -180 to 180): linear interpolation between the four run speeds. */
	float ComputeRunSpeedForDirection(float InDirection) const;

	/** Prints MoveX, MoveY and GroundSpeed to LogRogueRigs at about 2 Hz while the console variable rr.Debug.MinerAnim is 1. */
	void LogDebugValues();

	/** Horizontal (XY) speed of the pawn owner in cm/s; 0 without a pawn. */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float GroundSpeed = 0.f;

	/** Move direction relative to the actor yaw (the aim), degrees -180 to 180: 0 forward, 90 right, -90 left, 180 back. 0 when still. */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float Direction = 0.f;

	/** Run clip speed in cm/s for the current Direction (interpolated from the four RunSpeed tunables). */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float RunSpeedForDirection = 0.f;

	/** Play rate of the locomotion clips. Fixed at 1 since task-386-16 (simple blend space); the Q4 clamp rule (GroundSpeed / RunSpeedForDirection clamped to 1..MaxPlayRate) is off for now. */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float LocomotionPlayRate = 1.f;

	/** Speed axis fed to the blend space, cm/s: GroundSpeed / LocomotionPlayRate (Q4). */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float BlendSpeed = 0.f;

	/**
	 * Forward/back component of the move vector (task-386-16): ground velocity dotted with the actor forward (the aim)
	 * divided by the CharacterMovement MaxWalkSpeed; 1 = full speed forward, -1 = full speed backward. The vector
	 * (MoveX, MoveY) is clamped to length 1. X axis of the simple locomotion blend space.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float MoveX = 0.f;

	/**
	 * Left/right component of the move vector (task-386-16): ground velocity dotted with the actor right divided by the
	 * CharacterMovement MaxWalkSpeed; 1 = full speed right, -1 = full speed left. Y axis of the simple locomotion blend space.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float MoveY = 0.f;

	/** World time (seconds) of the last debug log line; throttles the rr.Debug.MinerAnim log to about 2 Hz. */
	double LastDebugLogTime = -1.0;

	/** Root speed of the forward run clip (A_Miner_RunFwd), cm/s, measured by task-386-01. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RogueRigs|Anim", meta = (ClampMin = "1.0", Units = "CentimetersPerSecond"))
	float RunSpeedForward = 278.8f;

	/** Root speed of the backward run clip (A_Miner_RunBwd), cm/s, measured by task-386-01. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RogueRigs|Anim", meta = (ClampMin = "1.0", Units = "CentimetersPerSecond"))
	float RunSpeedBackward = 243.7f;

	/** Root speed of the left run clip (A_Miner_RunLeft), cm/s, measured by task-386-01. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RogueRigs|Anim", meta = (ClampMin = "1.0", Units = "CentimetersPerSecond"))
	float RunSpeedLeft = 427.1f;

	/** Root speed of the right run clip (A_Miner_RunRight), cm/s, measured by task-386-01. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RogueRigs|Anim", meta = (ClampMin = "1.0", Units = "CentimetersPerSecond"))
	float RunSpeedRight = 427.1f;

	/** Upper bound of LocomotionPlayRate (Q4, provisional, owner 2026-10-30). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RogueRigs|Anim", meta = (ClampMin = "1.0"))
	float MaxPlayRate = 2.5f;
};
