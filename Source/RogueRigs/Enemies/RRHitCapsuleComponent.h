// Rogue Rigs - robot hit capsule: the body-size capsule shots overlap; passive on robot moves (no overlap query of its own).

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "RRHitCapsuleComponent.generated.h"

/**
 * Hit capsule of a robot (ARREnemy::HitCapsule, task-093-01). Keeps overlap events on (the projectile's begin overlap needs
 * them on both components), but while bPassiveOverlaps is on and rr.Enemy.PassiveHitCapsule is 1 it skips its own overlap
 * query when the robot moves: projectiles move every frame and find it with their own sweep and end-of-move overlap, so the
 * robot saves one overlap query per move. Disabling query collision (death) still clears its overlaps through the engine path.
 */
UCLASS(ClassGroup = "Collision", meta = (BlueprintSpawnableComponent))
class ROGUERIGS_API URRHitCapsuleComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	/** True when rr.Enemy.PassiveHitCapsule is 1 (default). Read when the overlap state is re-evaluated: set it before robots spawn. */
	static bool IsPassiveEnabled();

	/** Overlap queries made by this component since creation (engine path only; a passive capsule never adds to it). Test hook. */
	int32 GetOverlapQueryCount() const { return OverlapQueryCount; }

	/** Skips the overlap query of a plain move (no pending or end overlaps given) while passive; otherwise the engine update. */
	virtual bool UpdateOverlapsImpl(const TOverlapArrayView* NewPendingOverlaps = nullptr, bool bDoNotifies = true,
		const TOverlapArrayView* OverlapsAtEndLocation = nullptr) override;

protected:
	/** When true (and the console variable is 1), robot moves do not query overlaps for this capsule (see the class comment). */
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Collision")
	bool bPassiveOverlaps = true;

	/** Engine overlap updates that ran a query (UpdateOverlapsImpl reached with overlap events and query collision on). */
	int32 OverlapQueryCount = 0;
};
