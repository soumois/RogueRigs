// Rogue Rigs - robot CharacterMovement: reuses the walking floor while the robot moves less than a short distance on the flat arena.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RRRobotMovementComponent.generated.h"

/**
 * CharacterMovement of every robot (sys-enemies), set by ARREnemy as its movement subobject class. The arena floor is flat
 * (terrain is whole grid cells), so while walking on a valid floor the robot keeps its last floor result instead of sweeping a
 * new one until it has moved FloorCacheDistance (2D) from the last real check, its capsule height changed, or the engine forces
 * a check (teleport, base change, landing). Falling, knockback and stepping use the engine path unchanged.
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRRobotMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	//~ UCharacterMovementComponent
	/** Returns the cached walking floor when still valid (see class comment), else the engine floor check (and caches it). */
	virtual void FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult = nullptr) const override;

	/** Number of FindFloor calls answered from the cache since BeginPlay (tests, profiling). */
	int32 GetCachedFloorHits() const { return CachedFloorHits; }

protected:
	/** Horizontal distance (uu) a walking robot may move on its cached floor before a real floor check. 0 = always check. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Walking", meta = (ClampMin = "0", Units = "cm"))
	float FloorCacheDistance = 25.f;

	/** Capsule location of the last real floor check. */
	mutable FVector FloorCacheLocation = FVector::ZeroVector;

	/** True when FloorCacheLocation holds a walkable floor check. */
	mutable bool bFloorCacheValid = false;

	/** FindFloor calls answered from the cache. */
	mutable int32 CachedFloorHits = 0;
};
