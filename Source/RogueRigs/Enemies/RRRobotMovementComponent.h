// Rogue Rigs - robot CharacterMovement: light kinematic walk on the flat arena (rr.Enemy.LightMovement), cached floor on the engine path.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RRRobotMovementComponent.generated.h"

/**
 * CharacterMovement of every robot (sys-enemies-movement), set by ARREnemy as its movement subobject class.
 *
 * Light movement (task-071-01, console variable rr.Enemy.LightMovement, default 1; 0 = engine path for every robot): while the
 * robot walks on the floor height recorded by the last engine-path frame, a tick is one kinematic step instead of the engine's
 * walking physics: input acceleration and the engine's own CalcVelocity (same speed, acceleration, friction and braking),
 * a pending launch (shotgun knockback) replaces the horizontal velocity and stays unbraked for the time the engine's fall from
 * the floor gap lasts (LaunchFlightTimeLeft),
 * then one swept capsule move that also applies the orient-to-movement yaw, a horizontal slide along a blocking surface (rock,
 * the miner), both inside one deferred movement update (one child transform and overlap update per frame). No floor search,
 * no step up, no separate rotation move. The capsule never changes height on this path.
 * The engine path runs instead (and records the floor height again) while the robot is not walking (spawn fall, death), its
 * height moved away from the recorded floor (teleport, relocation at another height), forces or impulses are pending, or the
 * switch is off. On the engine path, FindFloor reuses the last walking floor until the capsule moved FloorCacheDistance (2D),
 * its height changed, or the engine forces a check (task-018-03).
 */
UCLASS(ClassGroup = (RogueRigs), meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRRobotMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	//~ UActorComponent
	/** Light step when CanUseLightMovement, else the engine CharacterMovement tick (then records the floor height). */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Forgets the cached floor, the recorded light-step floor and any launch flight time (robot reused from the actor pool). */
	void ResetLightMovementState()
	{
		bFloorCacheValid = false;
		bLightFloorValid = false;
		LaunchFlightTimeLeft = 0.f;
	}

	//~ UCharacterMovementComponent
	/** Returns the cached walking floor when still valid (see class comment), else the engine floor check (and caches it). */
	virtual void FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult = nullptr) const override;

	/** True when rr.Enemy.LightMovement is on (robots then use the light step and skip root capsule overlap updates). */
	static bool IsLightMovementEnabled();

	/** Number of FindFloor calls answered from the cache since BeginPlay (tests, profiling). */
	int32 GetCachedFloorHits() const { return CachedFloorHits; }

	/** Ticks run by the light step since BeginPlay (tests, profiling). */
	int32 GetLightMoveCount() const { return LightMoveCount; }

	/** Ticks run by the engine CharacterMovement path since BeginPlay (tests, profiling). */
	int32 GetEngineMoveCount() const { return EngineMoveCount; }

protected:
	/** True when this tick may use the light step: switch on, walking, floor height recorded and unchanged, no physics, no pending force. */
	bool CanUseLightMovement() const;

	/** One kinematic step: velocity from InputVector (or a pending launch), one swept move with the yaw, slide on a blocking hit. */
	void PerformLightMovement(float DeltaTime, const FVector& InputVector);

	/** After an engine-path tick: records the capsule height as the light floor when the robot walks on a walkable floor. */
	void RecordLightFloor();

	/** Horizontal distance (uu) a walking robot may move on its cached floor before a real floor check. 0 = always check. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Walking", meta = (ClampMin = "0", Units = "cm"))
	float FloorCacheDistance = 25.f;

	/** This robot class may use the light step when rr.Enemy.LightMovement is on. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Walking")
	bool bAllowLightMovement = true;

	/** Height difference (uu) from the recorded floor height beyond which a tick goes back to the engine path (teleport, push up). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Walking", meta = (ClampMin = "0", Units = "cm"))
	float LightHeightTolerance = 0.5f;

	/** Capsule location of the last real floor check. */
	mutable FVector FloorCacheLocation = FVector::ZeroVector;

	/** True when FloorCacheLocation holds a walkable floor check. */
	mutable bool bFloorCacheValid = false;

	/** FindFloor calls answered from the cache. */
	mutable int32 CachedFloorHits = 0;

	/** Capsule height (world Z) recorded on the walkable floor by the last engine-path tick; the light step keeps it. */
	double LightFloorZ = 0.0;

	/** True when LightFloorZ holds a walking height. */
	bool bLightFloorValid = false;

	/** Gap (uu) between the capsule and the floor recorded with LightFloorZ (engine floor distance, about 1.9 to 2.4). */
	float LightFloorGap = 0.f;

	/** Seconds a launched robot keeps its launch velocity unbraked, as the engine's fall from LightFloorGap (0 = on the ground). */
	float LaunchFlightTimeLeft = 0.f;

	/** Ticks run by the light step. */
	int32 LightMoveCount = 0;

	/** Ticks run by the engine path. */
	int32 EngineMoveCount = 0;
};
