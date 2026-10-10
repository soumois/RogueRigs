// Rogue Rigs - robot CharacterMovement: light kinematic walk on the flat arena (rr.Enemy.LightMovement), cached floor on the engine path.

#include "Enemies/RRRobotMovementComponent.h"

#include "Engine/ScopedMovementUpdate.h"
#include "GameFramework/Character.h"
#include "HAL/IConsoleManager.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRRobotMovementComponent)

namespace RRRobotMovement
{
	/** rr.Enemy.LightMovement: 1 = robots walk with the light step (task-071-01), 0 = engine CharacterMovement path. */
	static TAutoConsoleVariable<int32> CVarLightMovement(
		TEXT("rr.Enemy.LightMovement"),
		1,
		TEXT("1: robots walk with the light kinematic step and skip root capsule overlap updates (default). 0: engine CharacterMovement path, as before task-071-01. Overlap setting read when a robot begins play."),
		ECVF_Default);
}

bool URRRobotMovementComponent::IsLightMovementEnabled()
{
	return RRRobotMovement::CVarLightMovement.GetValueOnGameThread() != 0;
}

void URRRobotMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(URRRobotMovementComponent_Tick);

	if (!CanUseLightMovement())
	{
		++EngineMoveCount;
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		RecordLightFloor();
		return;
	}

	// Same entry as the engine tick: input consumed first, then the skip checks and the base movement component tick.
	const FVector InputVector = ConsumeInputVector();
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	UPawnMovementComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!HasValidData())
	{
		return;
	}
	++LightMoveCount;
	PerformLightMovement(DeltaTime, InputVector);
}

bool URRRobotMovementComponent::CanUseLightMovement() const
{
	return bAllowLightMovement && bLightFloorValid && IsLightMovementEnabled() && HasValidData()
		&& MovementMode == MOVE_Walking && !UpdatedComponent->IsSimulatingPhysics()
		&& PendingImpulseToApply.IsZero() && PendingForceToApply.IsZero()
		&& FMath::Abs(UpdatedComponent->GetComponentLocation().Z - LightFloorZ) <= LightHeightTolerance;
}

void URRRobotMovementComponent::RecordLightFloor()
{
	bLightFloorValid = HasValidData() && MovementMode == MOVE_Walking && CurrentFloor.IsWalkableFloor();
	if (bLightFloorValid)
	{
		LightFloorZ = UpdatedComponent->GetComponentLocation().Z;
		LightFloorGap = FMath::Max(0.f, CurrentFloor.FloorDist);
	}
	LaunchFlightTimeLeft = 0.f;
}

void URRRobotMovementComponent::PerformLightMovement(float DeltaTime, const FVector& InputVector)
{
	bJustTeleported = false;

	// Same acceleration and analog scale as ControlledCharacterMove.
	Acceleration = ScaleInputAcceleration(ConstrainInputAcceleration(InputVector));
	Acceleration.Z = 0.f;
	AnalogInputModifier = ComputeAnalogInputModifier();

	// Knockback: the engine turns a launch into a fall from the floor gap at the launch velocity (no ground friction in the air)
	// and brakes on landing. Here the launch replaces the horizontal velocity, which stays unbraked for the same fall time
	// (sqrt(2 x gap / gravity), about 0.066 s); a launch during that time keeps the remaining fall time, as the engine fall does.
	if (!PendingLaunchVelocity.IsZero())
	{
		Velocity = FVector(PendingLaunchVelocity.X, PendingLaunchVelocity.Y, 0.f);
		PendingLaunchVelocity = FVector::ZeroVector;
		if (LaunchFlightTimeLeft <= 0.f)
		{
			const float Gravity = FMath::Abs(GetGravityZ());
			LaunchFlightTimeLeft = Gravity > UE_SMALL_NUMBER ? FMath::Sqrt(2.f * LightFloorGap / Gravity) : 0.f;
		}
	}
	else if (LaunchFlightTimeLeft > 0.f)
	{
		LaunchFlightTimeLeft -= DeltaTime;
	}
	else
	{
		Velocity.Z = 0.f;
		CalcVelocity(DeltaTime, GroundFriction, /*bFluid*/ false, GetMaxBrakingDeceleration());
	}
	Velocity.Z = 0.f;

	// Orient to movement (yaw only, robots stay upright), applied by the same move as the translation.
	const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
	FRotator NewRotation(0.f, CurrentRotation.Yaw, 0.f);
	if (bOrientRotationToMovement)
	{
		FRotator DeltaRotation = GetDeltaRotation(DeltaTime);
		const FRotator Desired = ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRotation);
		const float DesiredYaw = FRotator::NormalizeAxis(Desired.Yaw);
		if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredYaw, 1e-3f))
		{
			NewRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredYaw, DeltaRotation.Yaw);
		}
	}

	const FVector Delta = Velocity * DeltaTime;
	const bool bRotates = !NewRotation.Equals(CurrentRotation, 1e-3f);
	if (Delta.IsNearlyZero() && !bRotates)
	{
		Velocity = FVector::ZeroVector;
		UpdateComponentVelocity();
		return;
	}

	{
		// One child transform and overlap update for the move and the slide.
		FScopedMovementUpdate ScopedMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, NewRotation.Quaternion(), /*bSweep*/ true, Hit);
		if (Hit.IsValidBlockingHit())
		{
			// Rock or the miner: slide along the horizontal part of the surface (the capsule never climbs).
			const FVector Normal = FVector(Hit.Normal.X, Hit.Normal.Y, 0.f).GetSafeNormal();
			if (!Normal.IsZero())
			{
				SlideAlongSurface(Delta, 1.f - Hit.Time, Normal, Hit, /*bHandleImpact*/ false);
			}
			// As PhysWalking: the velocity reflects the actual move after a blocked step.
			if (DeltaTime > UE_SMALL_NUMBER)
			{
				const FVector Moved = UpdatedComponent->GetComponentLocation() - OldLocation;
				Velocity = FVector(Moved.X, Moved.Y, 0.f) / DeltaTime;
			}
		}
	}
	UpdateComponentVelocity();
}

void URRRobotMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult) const
{
	// Same capsule height (no step, no launch) and a short horizontal move on a walkable floor: the flat floor is unchanged.
	constexpr float HeightTolerance = 0.1f;
	if (bFloorCacheValid && FloorCacheDistance > 0.f && IsMovingOnGround() && CurrentFloor.IsWalkableFloor()
		&& !bForceNextFloorCheck && !bJustTeleported && !DownwardSweepResult
		&& FMath::Abs(CapsuleLocation.Z - FloorCacheLocation.Z) <= HeightTolerance
		&& FVector::DistSquared2D(CapsuleLocation, FloorCacheLocation) <= FMath::Square(FloorCacheDistance))
	{
		if (&OutFloorResult != &CurrentFloor)
		{
			OutFloorResult = CurrentFloor;
		}
		++CachedFloorHits;
		return;
	}

	Super::FindFloor(CapsuleLocation, OutFloorResult, bCanUseCachedLocation, DownwardSweepResult);
	bFloorCacheValid = OutFloorResult.IsWalkableFloor();
	FloorCacheLocation = CapsuleLocation;
}
