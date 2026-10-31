// Rogue Rigs - anim instance of the miner: ground speed, move direction relative to the aim, blend speed and play rate for ABP_Miner.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RRMinerAnimInstance.generated.h"

class ARRWeapon;
class UAnimSequenceBase;

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
 * task-401-01: left hand IK. LeftHandIKTarget is the location of the socket LeftHandGrip of the held weapon mesh, expressed
 * in the space of the bone that carries WeaponSocket_R (the right hand), i.e. the space the ABP Two Bone IK uses
 * (Joint Target / Effector Location Space = Bone Space of the right hand bone). LeftHandIKAlpha is 1 only when the held
 * weapon has a mesh with LeftHandGrip, else 0. Event driven: recomputed when the miner broadcasts OnWeaponAdded, never
 * searched per frame (the weapon is rigid on its socket, so the value does not depend on the pose).
 * task-406-01: UpperBodySequence is the one swappable source of the upper body layer of ABP_Miner (asset set in the ABP
 * class defaults); no C++ logic reads it.
 * task-407-01: right arm correction on the upper body pose. RightElbowOffset and RightHandOffset (cm, component axes:
 * +Y forward, +Z up, +X left) are the tuned values; AppliedRightElbowOffset and AppliedRightHandOffset are what ABP_Miner
 * reads: the tuned values, or (0, 0, elbowUp) and (0, handForward, 0) while the cheat cvar rr.Debug.MinerArmFix
 * "<elbowUp> <handForward>" is set. The cheat cvar rr.Debug.MinerAnimFreeze 1 sets the mesh GlobalAnimRateScale to 0 (anim
 * time stops, the graph keeps evaluating, so a new offset shows on the same frame); 0 restores the previous scale.
 */
UCLASS()
class ROGUERIGS_API URRMinerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/** Reads the pawn owner velocity and yaw and refreshes every exposed value. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** Binds the miner OnWeaponAdded and computes the left hand IK target for the weapons already held. */
	virtual void NativeBeginPlay() override;

	/** Unbinds the miner OnWeaponAdded. */
	virtual void NativeUninitializeAnimation() override;

	/** Name of the socket of the held weapon mesh the left hand grips (the handguard). */
	static const FName LeftHandGripSocketName;

protected:
	/** OnWeaponAdded callback: recomputes the left hand IK target when the weapon is the hand-held one. */
	UFUNCTION()
	void HandleWeaponAdded(ARRWeapon* Weapon);

	/** Recomputes LeftHandIKTarget and LeftHandIKAlpha from Weapon (alpha 0 when it is not hand-held or has no LeftHandGrip). */
	void RefreshLeftHandIK(const ARRWeapon* Weapon);

	/** Run clip speed for a direction (degrees, -180 to 180): linear interpolation between the four run speeds. */
	float ComputeRunSpeedForDirection(float InDirection) const;

	/** Prints MoveX, MoveY and GroundSpeed to LogRogueRigs at about 2 Hz while the console variable rr.Debug.MinerAnim is 1. */
	void LogDebugValues();

	/** Refreshes AppliedRightElbowOffset and AppliedRightHandOffset from the tuned offsets or the rr.Debug.MinerArmFix override. */
	void RefreshRightArmOffsets();

	/** Applies rr.Debug.MinerAnimFreeze to the owning mesh: 1 sets GlobalAnimRateScale to 0, 0 restores the scale saved when freezing. */
	void ApplyDebugAnimFreeze();

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

	/** Location of the held weapon socket LeftHandGrip in the bone space of the right hand (the bone of WeaponSocket_R), cm. Zero when LeftHandIKAlpha is 0. Feeds the left arm Two Bone IK of ABP_Miner. */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	FVector LeftHandIKTarget = FVector::ZeroVector;

	/** 1 when the hand-held weapon mesh has the socket LeftHandGrip (IK on), else 0. */
	UPROPERTY(BlueprintReadOnly, Category = "RogueRigs|Anim")
	float LeftHandIKAlpha = 0.f;

	/** Miner whose OnWeaponAdded is bound. */
	TWeakObjectPtr<class ARRMiner> BoundMiner;

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

	/**
	 * Source of the upper body layer in ABP_Miner (task-406): the pose blended over the locomotion legs above the spine.
	 * Swap it for another weapon pose. No default in C++: set in the ABP_Miner class defaults. Read only by the ABP.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|UpperBody")
	TObjectPtr<UAnimSequenceBase> UpperBodySequence = nullptr;

	/**
	 * Right elbow correction on the upper body pose (task-407), cm in component axes (+Y forward, +Z up, +X left; outward =
	 * negative X): added to the idle pose elbow to give the right arm IK pole. Default up 5 cm (provisional, owner
	 * 2026-10-31). Tune in the ABP_Miner class defaults; the ABP reads AppliedRightElbowOffset.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|RightArm", meta = (Units = "Centimeters"))
	FVector RightElbowOffset = FVector(0.0, 0.0, 5.0);

	/**
	 * Right hand correction on the upper body pose (task-407), cm in component axes (+Y forward, +Z up, +X left): added to
	 * the idle pose hand to give the right arm IK target; the rifle and the left hand follow. Default forward 3 cm
	 * (provisional, owner 2026-10-31). Tune in the ABP_Miner class defaults; the ABP reads AppliedRightHandOffset.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|RightArm", meta = (Units = "Centimeters"))
	FVector RightHandOffset = FVector(0.0, 3.0, 0.0);

	/** Elbow offset the ABP applies, cm: RightElbowOffset, or (0, 0, elbowUp) while rr.Debug.MinerArmFix is set. Refreshed each update. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation|RightArm")
	FVector AppliedRightElbowOffset = FVector(0.0, 0.0, 5.0);

	/** Hand offset the ABP applies, cm: RightHandOffset, or (0, handForward, 0) while rr.Debug.MinerArmFix is set. Refreshed each update. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation|RightArm")
	FVector AppliedRightHandOffset = FVector(0.0, 3.0, 0.0);

	/** True while rr.Debug.MinerAnimFreeze holds the owning mesh GlobalAnimRateScale at 0. */
	bool bDebugAnimFrozen = false;

	/** Mesh GlobalAnimRateScale saved when the debug freeze started; restored when it ends. */
	float SavedGlobalAnimRateScale = 1.f;
};
