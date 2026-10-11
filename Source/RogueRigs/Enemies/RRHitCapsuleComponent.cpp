// Rogue Rigs - robot hit capsule: the body-size capsule shots overlap; passive on robot moves (no overlap query of its own).

#include "Enemies/RRHitCapsuleComponent.h"

#include "HAL/IConsoleManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRHitCapsuleComponent)

namespace RRHitCapsule
{
	static TAutoConsoleVariable<int32> CVarPassiveHitCapsule(
		TEXT("rr.Enemy.PassiveHitCapsule"),
		1,
		TEXT("1: a robot's hit capsule runs no overlap query when the robot moves (projectiles find it). 0: engine overlap update. Set before robots spawn."),
		ECVF_Default);
}

bool URRHitCapsuleComponent::IsPassiveEnabled()
{
	return RRHitCapsule::CVarPassiveHitCapsule.GetValueOnGameThread() != 0;
}

bool URRHitCapsuleComponent::UpdateOverlapsImpl(const TOverlapArrayView* NewPendingOverlaps, bool bDoNotifies, const TOverlapArrayView* OverlapsAtEndLocation)
{
	const bool bActive = GetGenerateOverlapEvents() && IsQueryCollisionEnabled();
	if (bActive && bPassiveOverlaps && IsPassiveEnabled() && NewPendingOverlaps == nullptr && OverlapsAtEndLocation == nullptr
		&& GetAttachChildren().Num() == 0)
	{
		// Nothing to do on a move: the cached skip holds until a collision or attachment change clears it (engine
		// ClearSkipUpdateOverlaps), which brings this function back (death: query collision off, engine path clears overlaps).
		return true;
	}
	if (bActive)
	{
		++OverlapQueryCount;
	}
	return Super::UpdateOverlapsImpl(NewPendingOverlaps, bDoNotifies, OverlapsAtEndLocation);
}
