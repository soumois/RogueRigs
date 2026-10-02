// Rogue Rigs - path provider interface: gives ground robots a move direction toward a goal, so the pathfinding backend can be swapped.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RRPathProvider.generated.h"

/** UObject side of IRRPathProvider (C++ only). */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URRPathProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * Something that knows how to reach a goal (the miner) from any world position. URREnemyMovementComponent asks its active
 * provider every tick; URRFlowFieldSubsystem is the default implementation. Functions are non-const so a provider may
 * refresh lazily when queried.
 */
class ROGUERIGS_API IRRPathProvider
{
	GENERATED_BODY()

public:
	/** Unit horizontal direction (Z 0) to move from From toward the goal; zero at the goal or when the goal is unreachable. */
	virtual FVector GetMoveDirection(const FVector& From) = 0;

	/** True when the goal can be reached by walking from From. */
	virtual bool IsReachable(const FVector& From) = 0;

	/** Walking distance from From to the goal in uu; negative when unreachable. */
	virtual float GetPathDistance(const FVector& From) = 0;

	/** World location of the goal; false when there is no goal. */
	virtual bool GetGoalLocation(FVector& OutLocation) = 0;

	/** True when a straight walk from From to To crosses no obstacle (used to steer straight at the goal when close). */
	virtual bool HasDirectPath(const FVector& From, const FVector& To) = 0;
};
