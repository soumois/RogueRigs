// Rogue Rigs - pickup world subsystem: moves every live pickup in one loop per frame (no actor tick per pickup; landed pickups simulate Chaos physics, task-229-02) and acquires grid and robot drops from the actor pool.

#include "Pickups/RRPickupSubsystem.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
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
	PendingXPCubes.Reset();
	PhysicalMaterials.Reset();
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

ARRPickup* URRPickupSubsystem::RequestXPCube(TSubclassOf<ARRPickup> PickupClass, const FVector& Location, float XPAmount, float PopYawDegrees,
	const FRRPickupScalePop& ScalePop)
{
	++TotalXPCubesRequested;
	// task-110-01: cubes of a class with a big block are collected and merged per bucket at the end of the frame.
	if (PickupClass && GetDefault<ARRPickup>(PickupClass)->GetXPBigPickupClass() && GetWorld())
	{
		FRRPendingXPCube& Pending = PendingXPCubes.AddDefaulted_GetRef();
		Pending.PickupClass = PickupClass;
		Pending.Location = Location;
		Pending.XPAmount = XPAmount;
		Pending.PopYawDegrees = PopYawDegrees;
		Pending.ScalePop = ScalePop;
		return nullptr;
	}
	return AcquirePickup(PickupClass, Location, nullptr, [XPAmount, PopYawDegrees, ScalePop](ARRPickup* Pickup)
	{
		Pickup->SetXPAmount(XPAmount);
		Pickup->SetPopYaw(PopYawDegrees);
		if (ScalePop.IsEnabled())
		{
			Pickup->SetSpawnScalePop(ScalePop);
		}
	});
}

void URRPickupSubsystem::FlushXPCubes()
{
	if (PendingXPCubes.Num() == 0)
	{
		return;
	}
	// Taken first: acquiring may run Blueprint code that requests more cubes (they wait for the next flush).
	TArray<FRRPendingXPCube> Requests = MoveTemp(PendingXPCubes);
	PendingXPCubes.Reset();

	// Buckets keyed by small class and cell, kept in first-request order (deterministic).
	using FBucketKey = TTuple<UClass*, int32, int32>;
	TMap<FBucketKey, int32> BucketIndexByKey;
	TArray<TArray<int32>> Buckets;
	const float CellSize = FMath::Max(XPMergeBucketSize, 1.f);
	for (int32 Index = 0; Index < Requests.Num(); ++Index)
	{
		const FRRPendingXPCube& Request = Requests[Index];
		const FBucketKey Key(Request.PickupClass.Get(), FMath::FloorToInt32(Request.Location.X / CellSize), FMath::FloorToInt32(Request.Location.Y / CellSize));
		int32* Found = BucketIndexByKey.Find(Key);
		const int32 BucketIndex = Found ? *Found : BucketIndexByKey.Add(Key, Buckets.AddDefaulted());
		Buckets[BucketIndex].Add(Index);
	}

	auto RequestSmall = [this](const FRRPendingXPCube& Request)
	{
		const float XPAmount = Request.XPAmount;
		const float PopYawDegrees = Request.PopYawDegrees;
		const FRRPickupScalePop ScalePop = Request.ScalePop;
		AcquirePickup(Request.PickupClass, Request.Location, nullptr, [XPAmount, PopYawDegrees, ScalePop](ARRPickup* Pickup)
		{
			Pickup->SetXPAmount(XPAmount);
			Pickup->SetPopYaw(PopYawDegrees);
			if (ScalePop.IsEnabled())
			{
				Pickup->SetSpawnScalePop(ScalePop);
			}
		});
	};

	for (const TArray<int32>& Bucket : Buckets)
	{
		const TSubclassOf<ARRPickup> SmallClass = Requests[Bucket[0]].PickupClass;
		const TSubclassOf<ARRPickup> BigClass = SmallClass ? GetDefault<ARRPickup>(SmallClass)->GetXPBigPickupClass() : nullptr;
		const int32 Groups = BigClass && XPMergeGroupSize >= 2 && Bucket.Num() > XPMergeThreshold ? Bucket.Num() / XPMergeGroupSize : 0;
		for (int32 Group = 0; Group < Groups; ++Group)
		{
			float SumXP = 0.f;
			FVector SumLocation = FVector::ZeroVector;
			for (int32 Member = 0; Member < XPMergeGroupSize; ++Member)
			{
				const FRRPendingXPCube& Request = Requests[Bucket[Group * XPMergeGroupSize + Member]];
				SumXP += Request.XPAmount;
				SumLocation += Request.Location;
			}
			const float PopYawDegrees = Requests[Bucket[Group * XPMergeGroupSize]].PopYawDegrees;
			++TotalXPBigBlocksRequested;
			AcquirePickup(BigClass, SumLocation / XPMergeGroupSize, nullptr, [SumXP, PopYawDegrees](ARRPickup* Pickup)
			{
				Pickup->SetXPAmount(SumXP);
				Pickup->SetPopYaw(PopYawDegrees);
			});
		}
		for (int32 Member = Groups * XPMergeGroupSize; Member < Bucket.Num(); ++Member)
		{
			RequestSmall(Requests[Bucket[Member]]);
		}
	}
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

UPhysicalMaterial* URRPickupSubsystem::FindOrAddPhysicalMaterial(TSubclassOf<ARRPickup> PickupClass)
{
	if (!PickupClass)
	{
		return nullptr;
	}
	if (const TObjectPtr<UObject>* Found = PhysicalMaterials.Find(PickupClass.Get()))
	{
		return static_cast<UPhysicalMaterial*>(Found->Get());
	}
	UPhysicalMaterial* Material = GetDefault<ARRPickup>(PickupClass)->MakePhysicalMaterial(this);
	PhysicalMaterials.Add(PickupClass.Get(), Material);
	return Material;
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

	// task-110-01: the XP cubes requested this frame (robot deaths tick before the tickable subsystems) merge per bucket.
	FlushXPCubes();

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
