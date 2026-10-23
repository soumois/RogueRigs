// Rogue Rigs - robot separation world subsystem: a cheap push between overlapping robots, replacing RVO avoidance (task-034-01).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RRRobotSeparationSubsystem.generated.h"

class ARREnemy;

/**
 * Light separation of the robots (enemies-index fourth test: robots no longer block each other, a light separation keeps them
 * from stacking). Robots register at BeginPlay. The first query of a frame rebuilds a 2D spatial hash of the living robots
 * (bucket BucketSize); GetSeparation then reads the 3 x 3 buckets around a robot: each neighbour closer than the sum of both
 * separation radii (ARREnemy::GetSeparationRadius, body radius x SeparationRadiusScale) pushes it away in proportion to the overlap. No physics, no collision: the robot adds the push to its movement
 * input (ARREnemy::UpdateSeparation).
 */
UCLASS()
class ROGUERIGS_API URRRobotSeparationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Adds Robot to the separation set (from its BeginPlay). */
	void RegisterRobot(ARREnemy* Robot);

	/** Removes Robot (from its EndPlay). */
	void UnregisterRobot(ARREnemy* Robot);

	/**
	 * Push direction of Robot on XY: sum over overlapping neighbours of (away direction x overlap / contact distance), length
	 * clamped to 1. Zero without overlap. Rebuilds the hash once per frame.
	 */
	FVector GetSeparation(const ARREnemy* Robot);

	/** Number of registered robots (tests). */
	int32 GetRobotCount() const { return Robots.Num(); }

	/** Side of a hash bucket in uu: at least two separation radii of the biggest robot (heavy 48 x 1.3 = 62.4 uu, task-295-01). */
	static constexpr float BucketSize = 128.f;

protected:
	/** Rebuilds positions, separation radii and buckets of the living robots when the frame changed. */
	void RebuildIfNeeded();

	/** Registered robots. */
	TArray<TWeakObjectPtr<ARREnemy>> Robots;

	/** Per frame: XY of each living robot, same order as Entries. */
	TArray<FVector2D> Positions;

	/** Per frame: separation radius of each living robot. */
	TArray<float> Radii;

	/** Per frame: the robot of each entry. */
	TArray<const ARREnemy*> Entries;

	/** Per frame: bucket -> entry indices. */
	TMap<FIntPoint, TArray<int32>> Buckets;

	/** GFrameCounter of the last rebuild. */
	uint64 BuiltFrame = MAX_uint64;
};
