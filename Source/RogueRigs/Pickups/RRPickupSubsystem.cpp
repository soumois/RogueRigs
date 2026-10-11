// Rogue Rigs - pickup world subsystem: moves every live pickup in one loop per frame (no actor tick per pickup, no physics) and acquires grid and robot drops from the actor pool.

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

ARRPickup* URRPickupSubsystem::RequestSpawn(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, AActor* Owner, int32 OreValue, float HealAmount)
{
	return AcquirePickup(PickupClass, Location, Owner, [OreValue, HealAmount](ARRPickup* Pickup)
	{
		Pickup->SetOreValue(OreValue);
		if (HealAmount > 0.f)
		{
			Pickup->SetHealAmount(HealAmount);
		}
	});
}

ARRPickup* URRPickupSubsystem::RequestXPCube(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, float XPAmount, float PopYawDegrees)
{
	++TotalXPCubesRequested;
	return AcquirePickup(PickupClass, Location, nullptr, [XPAmount, PopYawDegrees](ARRPickup* Pickup)
	{
		Pickup->SetXPAmount(XPAmount);
		Pickup->SetPopYaw(PopYawDegrees);
	});
}

ARRPickup* URRPickupSubsystem::AcquirePickup(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, AActor* Owner, TFunction<void(ARRPickup*)> Init)
{
	UWorld* World = GetWorld();
	if (!PickupClass || !World)
	{
		return nullptr;
	}
	const FTransform SpawnTransform(Location);
	URRActorPoolSubsystem* Pool = World->GetSubsystem<URRActorPoolSubsystem>();
	if (!Pool)
	{
		ARRPickup* Pickup = World->SpawnActorDeferred<ARRPickup>(PickupClass, SpawnTransform, Owner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Pickup)
		{
			Init(Pickup);
			Pickup->FinishSpawning(SpawnTransform);
		}
		return Pickup;
	}

	RequestedClasses.AddUnique(PickupClass.Get());
	FRRPoolAcquireParams Params;
	Params.Owner = Owner;
	Params.CollisionHandling = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.MaxFrames = FMath::Max(SpawnMaxFrames, 1);
	// The drop outlives the robot (released at once) or grid that asked for it: the world-long subsystem is the requester.
	Params.Requester = this;
	Params.OnInit = [Init](AActor* Actor)
	{
		if (ARRPickup* Pickup = Cast<ARRPickup>(Actor))
		{
			Init(Pickup);
		}
	};
	if (SpawnMaxFrames <= 0)
	{
		return Pool->AcquireImmediate<ARRPickup>(PickupClass, SpawnTransform, Params);
	}
	// Delivered now when an idle pickup is reused (OnReady runs inside Acquire); a later delivery writes to the shared slot only.
	const TSharedRef<TWeakObjectPtr<ARRPickup>> Delivered = MakeShared<TWeakObjectPtr<ARRPickup>>();
	Params.OnReady = [Delivered](AActor* Actor) { *Delivered = Cast<ARRPickup>(Actor); };
	const FRRPoolRequestHandle Handle = Pool->Acquire(PickupClass, SpawnTransform, MoveTemp(Params));
	return Handle.IsValid() ? nullptr : Delivered->Get();
}

int32 URRPickupSubsystem::GetQueuedSpawnCount() const
{
	return GetPoolStats().Pending;
}

FRRActorPoolStats URRPickupSubsystem::GetPoolStats() const
{
	FRRActorPoolStats Sum;
	const URRActorPoolSubsystem* Pool = GetWorld() ? GetWorld()->GetSubsystem<URRActorPoolSubsystem>() : nullptr;
	if (!Pool)
	{
		return Sum;
	}
	for (const TWeakObjectPtr<UClass>& Class : RequestedClasses)
	{
		if (UClass* Resolved = Class.Get())
		{
			const FRRActorPoolStats Stats = Pool->GetStats(Resolved);
			Sum.PoolSize += Stats.PoolSize;
			Sum.Idle += Stats.Idle;
			Sum.Acquired += Stats.Acquired;
			Sum.Pending += Stats.Pending;
			Sum.CreatedThisFrame += Stats.CreatedThisFrame;
			Sum.LongestWaitFrames = FMath::Max(Sum.LongestWaitFrames, Stats.LongestWaitFrames);
			Sum.TotalCreated += Stats.TotalCreated;
			Sum.TotalReused += Stats.TotalReused;
		}
	}
	return Sum;
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
