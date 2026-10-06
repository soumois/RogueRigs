// Rogue Rigs - pickup world subsystem: moves every live pickup in one loop per frame (no actor tick per pickup, no physics).

#include "Pickups/RRPickupSubsystem.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRPickupSubsystem)

DECLARE_CYCLE_STAT(TEXT("RR Pickups Tick"), STAT_RRPickupsTick, STATGROUP_Game);

bool URRPickupSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URRPickupSubsystem::Deinitialize()
{
	Pickups.Reset();
	Super::Deinitialize();
}

void URRPickupSubsystem::RegisterPickup(ARRPickup* Pickup)
{
	if (Pickup)
	{
		// BeginPlay runs once per actor: no duplicate check (O(1) per spawn burst).
		Pickups.Add(Pickup);
	}
}

void URRPickupSubsystem::UnregisterPickup(ARRPickup* Pickup)
{
	// The pickup being updated (collected during its own update) is found in O(1).
	const int32 Index = bUpdating && Pickups.IsValidIndex(UpdatingIndex) && Pickups[UpdatingIndex] == Pickup ? UpdatingIndex : Pickups.Find(Pickup);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (bUpdating)
	{
		Pickups[Index] = nullptr;
		bNeedsCompaction = true;
	}
	else
	{
		Pickups.RemoveAtSwap(Index, 1, EAllowShrinking::No);
	}
}

int32 URRPickupSubsystem::GetPickupCount() const
{
	int32 Count = 0;
	for (const ARRPickup* Pickup : Pickups)
	{
		Count += IsValid(Pickup) ? 1 : 0;
	}
	return Count;
}

int32 URRPickupSubsystem::GetMagnetizedCount() const
{
	int32 Count = 0;
	for (const ARRPickup* Pickup : Pickups)
	{
		Count += IsValid(Pickup) && Pickup->IsMagnetized() ? 1 : 0;
	}
	return Count;
}

void URRPickupSubsystem::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_RRPickupsTick);
	Super::Tick(DeltaTime);

	if (Pickups.Num() == 0)
	{
		return;
	}

	ARRMiner* Miner = Cast<ARRMiner>(UGameplayStatics::GetPlayerPawn(this, 0));
	const float Radius = Miner ? Miner->GetPickupRadius() : 0.f;
	const float RadiusSquared = FMath::Square(Radius);

	bUpdating = true;
	// Pickups registered during the loop (none expected) are updated from the next frame.
	const int32 Count = Pickups.Num();
	for (int32 Index = 0; Index < Count; ++Index)
	{
		UpdatingIndex = Index;
		ARRPickup* Pickup = Pickups[Index];
		if (IsValid(Pickup) && !Pickup->IsActorBeingDestroyed())
		{
			Pickup->UpdateMotion(DeltaTime, Miner, RadiusSquared);
		}
	}
	bUpdating = false;
	UpdatingIndex = INDEX_NONE;

	if (bNeedsCompaction)
	{
		bNeedsCompaction = false;
		Pickups.RemoveAllSwap([](const TObjectPtr<ARRPickup>& Pickup) { return Pickup == nullptr; }, EAllowShrinking::No);
	}
}

TStatId URRPickupSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRPickupSubsystem, STATGROUP_Tickables);
}
