// Rogue Rigs - robot CharacterMovement: reuses the walking floor while the robot moves less than a short distance on the flat arena.

#include "Enemies/RRRobotMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRRobotMovementComponent)

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
