// Rogue Rigs - generic actor pool world subsystem (sys-actor-pool): reuses released actors and spreads the creation of missing ones over a capped number of frames.

#include "Pool/RRActorPoolSubsystem.h"

#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Pool/RRPoolable.h"
#include "RogueRigs.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRActorPoolSubsystem)

DECLARE_CYCLE_STAT(TEXT("RR Actor Pool Tick"), STAT_RRActorPoolTick, STATGROUP_Game);

bool URRActorPoolSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URRActorPoolSubsystem::Deinitialize()
{
	ClearPool(false);
	Super::Deinitialize();
}

TStatId URRActorPoolSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRActorPoolSubsystem, STATGROUP_Tickables);
}

URRActorPoolSubsystem::FClassPool& URRActorPoolSubsystem::GetClassPool(UClass* Class)
{
	return ClassPools.FindOrAdd(TObjectKey<UClass>(Class));
}

FRRPoolRequestHandle URRActorPoolSubsystem::Acquire(TSubclassOf<AActor> Class, const FTransform& Transform, FRRPoolAcquireParams Params)
{
	FRRPoolRequestHandle Handle;
	UClass* ActorClass = Class.Get();
	if (!ActorClass || ActorClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRActorPoolSubsystem: Acquire refused (class %s null or abstract)."), *GetNameSafe(ActorClass));
		return Handle;
	}
	if (AActor* Reused = PopIdle(ActorClass))
	{
		ReactivateActor(Reused, Transform, Params);
		++GetClassPool(ActorClass).TotalReused;
		DeliverActor(Reused, Params, 0);
		return Handle;
	}
	FPendingRequest& Request = Pending.AddDefaulted_GetRef();
	Request.Id = ++LastRequestId;
	Request.Class = ActorClass;
	Request.Transform = Transform;
	Request.bHasRequester = Params.Requester.IsValid();
	Request.FramesLeft = FMath::Max(Params.MaxFrames, 1);
	Request.QueuedTick = TickCount;
	Request.Params = MoveTemp(Params);
	Handle.Id = Request.Id;
	return Handle;
}

AActor* URRActorPoolSubsystem::AcquireImmediate(TSubclassOf<AActor> Class, const FTransform& Transform, const FRRPoolAcquireParams& Params)
{
	UClass* ActorClass = Class.Get();
	if (!ActorClass || ActorClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRActorPoolSubsystem: AcquireImmediate refused (class %s null or abstract)."), *GetNameSafe(ActorClass));
		return nullptr;
	}
	AActor* Actor = PopIdle(ActorClass);
	if (Actor)
	{
		ReactivateActor(Actor, Transform, Params);
		++GetClassPool(ActorClass).TotalReused;
	}
	else
	{
		Actor = CreateActor(ActorClass, Transform, Params);
	}
	if (Actor)
	{
		FRRPoolAcquireParams NoReady = Params;
		NoReady.OnReady = nullptr;
		DeliverActor(Actor, NoReady, 0);
	}
	return Actor;
}

void URRActorPoolSubsystem::Prewarm(TSubclassOf<AActor> Class, int32 Count, int32 MaxFrames)
{
	UClass* ActorClass = Class.Get();
	if (!ActorClass || ActorClass->HasAnyClassFlags(CLASS_Abstract) || Count <= 0)
	{
		return;
	}
	for (int32 Index = 0; Index < Count; ++Index)
	{
		FPendingRequest& Request = Pending.AddDefaulted_GetRef();
		Request.Id = ++LastRequestId;
		Request.Class = ActorClass;
		Request.bPrewarm = true;
		Request.FramesLeft = FMath::Max(MaxFrames, 1);
		Request.QueuedTick = TickCount;
	}
}

int32 URRActorPoolSubsystem::FindPending(uint32 Id) const
{
	return Pending.IndexOfByPredicate([Id](const FPendingRequest& Request) { return Request.Id == Id; });
}

bool URRActorPoolSubsystem::CancelRequest(FRRPoolRequestHandle& Handle)
{
	const int32 Index = Handle.IsValid() ? FindPending(Handle.Id) : INDEX_NONE;
	Handle.Reset();
	if (Index == INDEX_NONE)
	{
		return false;
	}
	Pending.RemoveAt(Index);
	return true;
}

bool URRActorPoolSubsystem::IsRequestPending(const FRRPoolRequestHandle& Handle) const
{
	return Handle.IsValid() && FindPending(Handle.Id) != INDEX_NONE;
}

bool URRActorPoolSubsystem::Release(AActor* Actor)
{
	if (!IsValid(Actor) || Actor->IsActorBeingDestroyed())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRActorPoolSubsystem: Release refused (%s null or being destroyed)."), *GetNameSafe(Actor));
		return false;
	}
	if (!IsPoolMember(Actor))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRActorPoolSubsystem: Release refused (%s was not created by the pool)."), *Actor->GetName());
		return false;
	}
	if (IsIdle(Actor))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRActorPoolSubsystem: Release refused (%s is already idle)."), *Actor->GetName());
		return false;
	}
	ParkActor(Actor);
	return true;
}

bool URRActorPoolSubsystem::IsIdle(const AActor* Actor) const
{
	return Actor && IdleActors.Contains(TObjectKey<AActor>(Actor));
}

bool URRActorPoolSubsystem::IsPoolMember(const AActor* Actor) const
{
	return Actor && MemberClasses.Contains(TObjectKey<AActor>(Actor));
}

bool URRActorPoolSubsystem::IsPooledIdle(const AActor* Actor)
{
	const UWorld* World = Actor ? Actor->GetWorld() : nullptr;
	const URRActorPoolSubsystem* Pool = World ? World->GetSubsystem<URRActorPoolSubsystem>() : nullptr;
	return Pool && Pool->IsIdle(Actor);
}

FRRActorPoolStats URRActorPoolSubsystem::GetStats(TSubclassOf<AActor> Class) const
{
	FRRActorPoolStats Stats;
	const UClass* Filter = Class.Get();
	for (const TPair<TObjectKey<UClass>, FClassPool>& Pair : ClassPools)
	{
		if (Filter && Pair.Key != TObjectKey<UClass>(Filter))
		{
			continue;
		}
		const FClassPool& ClassPool = Pair.Value;
		Stats.PoolSize += ClassPool.MemberCount;
		Stats.Idle += ClassPool.Idle.Num();
		Stats.Acquired += ClassPool.MemberCount - ClassPool.Idle.Num();
		Stats.CreatedThisFrame += ClassPool.CreatedFrame == GFrameCounter ? ClassPool.CreatedThisFrame : 0;
		Stats.LongestWaitFrames = FMath::Max(Stats.LongestWaitFrames, ClassPool.LongestWaitFrames);
		Stats.TotalCreated += ClassPool.TotalCreated;
		Stats.TotalReused += ClassPool.TotalReused;
	}
	for (const FPendingRequest& Request : Pending)
	{
		Stats.Pending += !Filter || Request.Class.Get() == Filter ? 1 : 0;
	}
	return Stats;
}

void URRActorPoolSubsystem::ResetStats()
{
	for (TPair<TObjectKey<UClass>, FClassPool>& Pair : ClassPools)
	{
		Pair.Value.LongestWaitFrames = 0;
		Pair.Value.TotalCreated = 0;
		Pair.Value.TotalReused = 0;
	}
}

void URRActorPoolSubsystem::ClearPool(bool bDestroyIdle)
{
	Pending.Reset();
	TArray<AActor*> ToDestroy;
	for (const TPair<TObjectKey<AActor>, TObjectKey<UClass>>& Pair : MemberClasses)
	{
		if (AActor* Actor = Pair.Key.ResolveObjectPtr())
		{
			Actor->OnDestroyed.RemoveDynamic(this, &URRActorPoolSubsystem::HandleMemberDestroyed);
			if (bDestroyIdle && IdleActors.Contains(Pair.Key))
			{
				ToDestroy.Add(Actor);
			}
		}
	}
	ClassPools.Reset();
	IdleActors.Reset();
	MemberClasses.Reset();
	for (AActor* Actor : ToDestroy)
	{
		Actor->Destroy();
	}
}

void URRActorPoolSubsystem::HandleMemberDestroyed(AActor* DestroyedActor)
{
	const TObjectKey<AActor> Key(DestroyedActor);
	TObjectKey<UClass> ClassKey;
	if (!MemberClasses.RemoveAndCopyValue(Key, ClassKey))
	{
		return;
	}
	if (FClassPool* ClassPool = ClassPools.Find(ClassKey))
	{
		--ClassPool->MemberCount;
		if (IdleActors.Remove(Key) > 0)
		{
			ClassPool->Idle.RemoveSingleSwap(DestroyedActor);
		}
	}
}

AActor* URRActorPoolSubsystem::PopIdle(UClass* Class)
{
	FClassPool* ClassPool = ClassPools.Find(TObjectKey<UClass>(Class));
	if (!ClassPool)
	{
		return nullptr;
	}
	while (ClassPool->Idle.Num() > 0)
	{
		AActor* Actor = ClassPool->Idle.Pop(EAllowShrinking::No).Get();
		if (IsValid(Actor) && !Actor->IsActorBeingDestroyed())
		{
			IdleActors.Remove(TObjectKey<AActor>(Actor));
			return Actor;
		}
	}
	return nullptr;
}

AActor* URRActorPoolSubsystem::CreateActor(UClass* Class, const FTransform& Transform, const FRRPoolAcquireParams& Params)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	AActor* Actor = World->SpawnActorDeferred<AActor>(Class, Transform, Params.Owner.Get(), Params.Instigator.Get(), Params.CollisionHandling);
	if (!Actor)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRActorPoolSubsystem: %s could not be created."), *Class->GetName());
		return nullptr;
	}
	if (Params.OnInit)
	{
		Params.OnInit(Actor);
	}
	Actor->FinishSpawning(Transform);
	if (!IsValid(Actor) || Actor->IsActorBeingDestroyed())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRActorPoolSubsystem: %s was destroyed during its spawn."), *Class->GetName());
		return nullptr;
	}

	FClassPool& ClassPool = GetClassPool(Class);
	if (ClassPool.CreatedFrame != GFrameCounter)
	{
		ClassPool.CreatedFrame = GFrameCounter;
		ClassPool.CreatedThisFrame = 0;
	}
	++ClassPool.CreatedThisFrame;
	++ClassPool.TotalCreated;
	++ClassPool.MemberCount;
	MemberClasses.Add(TObjectKey<AActor>(Actor), TObjectKey<UClass>(Class));
	Actor->OnDestroyed.AddDynamic(this, &URRActorPoolSubsystem::HandleMemberDestroyed);
	return Actor;
}

void URRActorPoolSubsystem::ReactivateActor(AActor* Actor, const FTransform& Transform, const FRRPoolAcquireParams& Params)
{
	Actor->SetOwner(Params.Owner.Get());
	Actor->SetInstigator(Params.Instigator.Get());
	Actor->SetActorEnableCollision(true);

	FVector Location = Transform.GetLocation();
	const ESpawnActorCollisionHandlingMethod Handling = Params.CollisionHandling;
	if (Handling == ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		|| Handling == ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding)
	{
		if (UWorld* World = GetWorld())
		{
			World->FindTeleportSpot(Actor, Location, Transform.Rotator());
		}
	}
	Actor->SetActorTransform(FTransform(Transform.GetRotation(), Location, Transform.GetScale3D()), false, nullptr, ETeleportType::ResetPhysics);
	Actor->SetActorHiddenInGame(false);
	if (Actor->PrimaryActorTick.bCanEverTick && Actor->PrimaryActorTick.bStartWithTickEnabled)
	{
		Actor->SetActorTickEnabled(true);
	}
	TInlineComponentArray<UActorComponent*> Components(Actor);
	for (UActorComponent* Component : Components)
	{
		if (Component->PrimaryComponentTick.bCanEverTick && Component->PrimaryComponentTick.bStartWithTickEnabled)
		{
			Component->SetComponentTickEnabled(true);
		}
	}
	if (Params.OnInit)
	{
		Params.OnInit(Actor);
	}
}

void URRActorPoolSubsystem::DeliverActor(AActor* Actor, const FRRPoolAcquireParams& Params, int32 FramesWaited)
{
	FClassPool& ClassPool = GetClassPool(Actor->GetClass());
	ClassPool.LongestWaitFrames = FMath::Max(ClassPool.LongestWaitFrames, FramesWaited);
	if (Actor->Implements<URRPoolable>())
	{
		IRRPoolable::Execute_OnPoolAcquired(Actor);
	}
	if (Params.OnReady && IsValid(Actor))
	{
		Params.OnReady(Actor);
	}
}

void URRActorPoolSubsystem::ParkActor(AActor* Actor)
{
	if (Actor->Implements<URRPoolable>())
	{
		IRRPoolable::Execute_OnPoolReleased(Actor);
	}
	if (!IsValid(Actor) || Actor->IsActorBeingDestroyed())
	{
		return;
	}
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
	TInlineComponentArray<UActorComponent*> Components(Actor);
	for (UActorComponent* Component : Components)
	{
		Component->SetComponentTickEnabled(false);
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(Actor);
	}
	Actor->SetOwner(nullptr);

	const TObjectKey<AActor> Key(Actor);
	if (!IdleActors.Contains(Key))
	{
		IdleActors.Add(Key);
		GetClassPool(Actor->GetClass()).Idle.Add(Actor);
	}
}

void URRActorPoolSubsystem::ServeRequest(FPendingRequest& Request, bool bCreate)
{
	UClass* Class = Request.Class.Get();
	if (!Class)
	{
		return;
	}
	if (Request.bPrewarm)
	{
		if (AActor* Actor = bCreate ? CreateActor(Class, FTransform::Identity, Request.Params) : nullptr)
		{
			ParkActor(Actor);
		}
		return;
	}
	AActor* Actor = PopIdle(Class);
	if (Actor)
	{
		ReactivateActor(Actor, Request.Transform, Request.Params);
		++GetClassPool(Class).TotalReused;
	}
	else if (bCreate)
	{
		Actor = CreateActor(Class, Request.Transform, Request.Params);
	}
	if (Actor)
	{
		DeliverActor(Actor, Request.Params, Request.FramesWaited);
	}
}

bool URRActorPoolSubsystem::IsRequestDead(const FPendingRequest& Request)
{
	return !Request.Class.IsValid() || (Request.bHasRequester && !Request.Params.Requester.IsValid());
}

void URRActorPoolSubsystem::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_RRActorPoolTick);
	++TickCount;
	if (Pending.IsEmpty())
	{
		return;
	}
	Pending.RemoveAll([](const FPendingRequest& Request) { return IsRequestDead(Request); });
	for (FPendingRequest& Request : Pending)
	{
		if (Request.QueuedTick < TickCount)
		{
			++Request.FramesWaited;
		}
	}

	// Reuse first: actors released since the request cost nothing and do not use the creation budget. Callbacks may add or
	// cancel requests, so each request is removed from Pending before it is served.
	for (int32 Index = 0; Index < Pending.Num();)
	{
		const FPendingRequest& Candidate = Pending[Index];
		const FClassPool* ClassPool = Candidate.bPrewarm ? nullptr : ClassPools.Find(TObjectKey<UClass>(Candidate.Class.Get()));
		if (ClassPool && ClassPool->Idle.Num() > 0)
		{
			FPendingRequest Request = MoveTemp(Pending[Index]);
			Pending.RemoveAt(Index);
			ServeRequest(Request, false);
			continue;
		}
		++Index;
	}

	// Earliest deadline first; the budget is the smallest rate that meets every deadline: max over k of ceil(N_k / k), N_k =
	// requests due within k ticks. One batch of N with MaxFrames M gives ceil(N / M), and a request at its last tick is forced.
	// Requests queued by a callback during this tick go last and wait for the next tick.
	const uint32 ServedTick = TickCount;
	Pending.StableSort([ServedTick](const FPendingRequest& A, const FPendingRequest& B)
	{
		const bool bNewA = A.QueuedTick >= ServedTick;
		const bool bNewB = B.QueuedTick >= ServedTick;
		return bNewA != bNewB ? bNewB : A.FramesLeft < B.FramesLeft;
	});
	int32 Budget = 0;
	for (int32 Index = 0; Index < Pending.Num() && Pending[Index].QueuedTick < ServedTick; ++Index)
	{
		const int32 Due = Index + 1;
		const int32 Window = FMath::Max(Pending[Index].FramesLeft, 1);
		Budget = FMath::Max(Budget, FMath::DivideAndRoundUp(Due, Window));
	}
	for (int32 Served = 0; Served < Budget && Pending.Num() > 0; ++Served)
	{
		if (Pending[0].QueuedTick >= ServedTick)
		{
			break; // queued by a callback of this tick: next tick
		}
		FPendingRequest Request = MoveTemp(Pending[0]);
		Pending.RemoveAt(0);
		ServeRequest(Request, true);
	}

	// Age the requests that went through this tick.
	for (FPendingRequest& Request : Pending)
	{
		if (Request.QueuedTick < ServedTick)
		{
			Request.FramesLeft = FMath::Max(Request.FramesLeft - 1, 1);
		}
	}
}
