// Rogue Rigs - test-only XP pickup: a native, non-abstract ARRPickup of type XP for the enemy tests (no Blueprint needed).

#include "Tests/RRTestPickupXP.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTestPickupXP)

ARRTestPickupXP::ARRTestPickupXP()
{
	PickupType = ERRPickupType::XP;
}

ARRTestPickupXPBig::ARRTestPickupXPBig()
{
	PickupType = ERRPickupType::XP;
}

ARRTestPickupXPMerging::ARRTestPickupXPMerging()
{
	XPBigPickupClass = ARRTestPickupXPBig::StaticClass();
}
