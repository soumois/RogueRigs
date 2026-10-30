// Rogue Rigs - anim instance of the miner: ground speed, move direction relative to the aim, blend speed and play rate for ABP_Miner.

#include "Player/RRMinerAnimInstance.h"

#include "GameFramework/Pawn.h"

void URRMinerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const APawn* PawnOwner = TryGetPawnOwner();
	const FVector Velocity = PawnOwner ? PawnOwner->GetVelocity() : FVector::ZeroVector;
	const FVector GroundVelocity(Velocity.X, Velocity.Y, 0.0);

	GroundSpeed = static_cast<float>(GroundVelocity.Size());

	if (PawnOwner && !FMath::IsNearlyZero(GroundSpeed))
	{
		// Velocity in the actor yaw frame: X forward (the aim), Y right. Same math as UKismetAnimationLibrary::CalculateDirection.
		const FRotator YawRotation(0.0, PawnOwner->GetActorRotation().Yaw, 0.0);
		const FVector LocalVelocity = YawRotation.UnrotateVector(GroundVelocity);
		Direction = static_cast<float>(FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X)));
	}
	else
	{
		Direction = 0.f;
	}

	RunSpeedForDirection = ComputeRunSpeedForDirection(Direction);
	LocomotionPlayRate = FMath::Clamp(GroundSpeed / RunSpeedForDirection, 1.f, FMath::Max(MaxPlayRate, 1.f));
	BlendSpeed = GroundSpeed / LocomotionPlayRate;
}

float URRMinerAnimInstance::ComputeRunSpeedForDirection(float InDirection) const
{
	const float AbsDirection = FMath::Abs(InDirection);
	const float SideSpeed = InDirection >= 0.f ? RunSpeedRight : RunSpeedLeft;

	float Speed;
	if (AbsDirection <= 90.f)
	{
		// Forward to side.
		Speed = FMath::Lerp(RunSpeedForward, SideSpeed, AbsDirection / 90.f);
	}
	else
	{
		// Side to backward.
		Speed = FMath::Lerp(SideSpeed, RunSpeedBackward, FMath::Min(AbsDirection - 90.f, 90.f) / 90.f);
	}

	// Guard the division in NativeUpdateAnimation against a zero speed set by hand.
	return FMath::Max(Speed, 1.f);
}
