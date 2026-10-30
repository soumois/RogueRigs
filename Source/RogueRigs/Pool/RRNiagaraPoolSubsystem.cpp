// Rogue Rigs - Niagara component pool world subsystem (sys-actor-pool): capped per-system pools of reusable one-shot Niagara components (robot death puff, debris).

#include "Pool/RRNiagaraPoolSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "RogueRigs.h"
#include "UObject/UObjectHash.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRNiagaraPoolSubsystem)

bool URRNiagaraPoolSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URRNiagaraPoolSubsystem::Deinitialize()
{
	auto DestroyAll = [this](TArray<TObjectPtr<UNiagaraComponent>>& List)
	{
		for (UNiagaraComponent* Component : List)
		{
			if (IsValid(Component))
			{
				Component->OnSystemFinished.RemoveDynamic(this, &URRNiagaraPoolSubsystem::HandleSystemFinished);
				Component->DestroyComponent();
			}
		}
		List.Reset();
	};
	for (TPair<TObjectPtr<UNiagaraSystem>, FRRNiagaraSystemPool>& Pair : Pools)
	{
		DestroyAll(Pair.Value.InUse);
		DestroyAll(Pair.Value.Free);
	}
	Pools.Reset();
	TrackedWorldEffects.Reset();
	Super::Deinitialize();
}

UNiagaraComponent* URRNiagaraPoolSubsystem::Spawn(UNiagaraSystem* System, const FVector& Location, const FRotator& Rotation,
	int32 Cap, TFunctionRef<void(UNiagaraComponent&)> BeforeActivate)
{
	if (!System || Cap <= 0)
	{
		return nullptr;
	}
	FRRNiagaraSystemPool& Pool = Pools.FindOrAdd(System);
	Pool.Cap = Cap;

	UNiagaraComponent* Component = nullptr;
	while (!Component && Pool.Free.Num() > 0)
	{
		// Members destroyed by someone else leave the pool.
		UNiagaraComponent* Candidate = Pool.Free.Pop(EAllowShrinking::No);
		Component = IsValid(Candidate) ? Candidate : nullptr;
	}
	if (!Component && Pool.Free.Num() + Pool.InUse.Num() >= Pool.Cap)
	{
		ReclaimInactive(Pool);
		while (!Component && Pool.Free.Num() > 0)
		{
			UNiagaraComponent* Candidate = Pool.Free.Pop(EAllowShrinking::No);
			Component = IsValid(Candidate) ? Candidate : nullptr;
		}
	}
	if (!Component && Pool.Free.Num() + Pool.InUse.Num() < Pool.Cap)
	{
		Component = CreateComponent(System);
		if (Component)
		{
			++Pool.CreatedCount;
		}
	}
	if (!Component)
	{
		++Pool.SkippedCount;
		UE_LOG(LogRogueRigs, Verbose, TEXT("Niagara pool %s full (%d in use, cap %d): effect skipped."), *System->GetName(),
			Pool.InUse.Num(), Pool.Cap);
		return nullptr;
	}

	Pool.InUse.Add(Component);
	Component->SetWorldLocationAndRotation(Location, Rotation);
	BeforeActivate(*Component);
	// A system that cannot run completes inside Activate: the finish callback has already put the component back, and it
	// does not count as a play. A system not ready yet (compiling, PSOs) stays in use, waiting to start (task-194-01).
	Component->Activate(/*bReset*/ true);
	const bool bPlaying = Pool.InUse.Contains(Component);
	if (bPlaying)
	{
		++Pool.PlayedCount;
	}
	UE_LOG(LogRogueRigs, Verbose, TEXT("Niagara pool %s: %s %s (active %d, waiting %d), %d in use, %d free."), *System->GetName(),
		*Component->GetName(), bPlaying ? TEXT("played") : TEXT("completed at once"), Component->IsActive(),
		IsWaitingToStart(*Component), Pool.InUse.Num(), Pool.Free.Num());
	return bPlaying ? Component : nullptr;
}

void URRNiagaraPoolSubsystem::Prewarm(UNiagaraSystem* System, int32 Count, int32 Cap)
{
	if (!System || Cap <= 0)
	{
		return;
	}
	FRRNiagaraSystemPool& Pool = Pools.FindOrAdd(System);
	Pool.Cap = Cap;
#if WITH_EDITORONLY_DATA
	// Editor builds compile a loaded system on its first activation, and a component activated before that compile ends
	// waits (inactive) instead of playing: in an S7 burst every debris of the first deaths waited (task-194-01). Start the
	// deferred compile now, in the background, so the first death finds the system ready. Cooked builds have nothing to do.
	if (System->HasOutstandingCompilationRequests())
	{
		System->PollForCompilationComplete(/*bFlushRequestCompile*/ true);
	}
#endif
	const int32 Target = FMath::Min(Count, Cap);
	while (Pool.Free.Num() + Pool.InUse.Num() < Target)
	{
		UNiagaraComponent* Component = CreateComponent(System);
		if (!Component)
		{
			return;
		}
		++Pool.CreatedCount;
		Pool.Free.Add(Component);
	}
}

int32 URRNiagaraPoolSubsystem::GetInUseCount(const UNiagaraSystem* System) const
{
	const FRRNiagaraSystemPool* Pool = Pools.Find(System);
	return Pool ? Pool->InUse.Num() : 0;
}

int32 URRNiagaraPoolSubsystem::GetComponentCount(const UNiagaraSystem* System) const
{
	const FRRNiagaraSystemPool* Pool = Pools.Find(System);
	return Pool ? Pool->Free.Num() + Pool->InUse.Num() : 0;
}

int32 URRNiagaraPoolSubsystem::GetCreatedCount(const UNiagaraSystem* System) const
{
	const FRRNiagaraSystemPool* Pool = Pools.Find(System);
	return Pool ? Pool->CreatedCount : 0;
}

int32 URRNiagaraPoolSubsystem::GetSkippedCount(const UNiagaraSystem* System) const
{
	const FRRNiagaraSystemPool* Pool = Pools.Find(System);
	return Pool ? Pool->SkippedCount : 0;
}

int32 URRNiagaraPoolSubsystem::GetPlayedCount(const UNiagaraSystem* System) const
{
	const FRRNiagaraSystemPool* Pool = Pools.Find(System);
	return Pool ? Pool->PlayedCount : 0;
}

void URRNiagaraPoolSubsystem::GetPooledSystems(TArray<UNiagaraSystem*>& OutSystems) const
{
	for (const TPair<TObjectPtr<UNiagaraSystem>, FRRNiagaraSystemPool>& Pair : Pools)
	{
		if (Pair.Key)
		{
			OutSystems.Add(Pair.Key);
		}
	}
}

int32 URRNiagaraPoolSubsystem::ClearForNextLevel()
{
	int32 Cleared = 0;
	TArray<FString> Parts;
	for (TPair<TObjectPtr<UNiagaraSystem>, FRRNiagaraSystemPool>& Pair : Pools)
	{
		FRRNiagaraSystemPool& Pool = Pair.Value;
		int32 SystemCleared = 0;
		// Copy: DeactivateImmediate completes the system and the finish callback moves the component to Free meanwhile.
		const TArray<TObjectPtr<UNiagaraComponent>> InUse = Pool.InUse;
		for (UNiagaraComponent* Component : InUse)
		{
			if (!IsValid(Component))
			{
				continue;
			}
			if (Component->IsActive())
			{
				++SystemCleared;
				Component->DeactivateImmediate();
			}
			else if (IsWaitingToStart(*Component))
			{
				// Deactivate clears the pending activation (DeactivateInternal); the tick that would start it later goes off.
				++SystemCleared;
				Component->Deactivate();
				Component->SetComponentTickEnabled(false);
			}
		}
		for (UNiagaraComponent* Component : Pool.InUse)
		{
			if (IsValid(Component))
			{
				Pool.Free.Add(Component);
			}
		}
		Pool.InUse.Reset();
		if (SystemCleared > 0)
		{
			Cleared += SystemCleared;
			Parts.Add(FString::Printf(TEXT("%s %d"), *GetNameSafe(Pair.Key), SystemCleared));
		}
	}

	int32 WorldCleared = 0;
	TArray<FString> WorldNames;
	for (const TWeakObjectPtr<UFXSystemComponent>& WeakEffect : TrackedWorldEffects)
	{
		UFXSystemComponent* Effect = WeakEffect.Get();
		if (!IsValid(Effect) || Effect->IsBeingDestroyed() || !IsEffectAlive(*Effect))
		{
			continue;
		}
		++WorldCleared;
		WorldNames.Add(GetNameSafe(Effect->GetFXSystemAsset()));
		const bool bWaiting = !Effect->IsActive();
		// Completion destroys an auto-destroy one-shot and returns an auto-release one to the engine pool.
		Effect->DeactivateImmediate();
		if (!IsValid(Effect) || Effect->IsBeingDestroyed())
		{
			continue;
		}
		if (bWaiting)
		{
			// Still waiting for its system (compile, PSOs): drop the pending activation and the tick that would start it.
			Effect->Deactivate();
			Effect->SetComponentTickEnabled(false);
		}
		// Same end as the bonus chest effects (task-388-01): manual-release components go back to the engine pool, unpooled ones are destroyed.
		const UNiagaraComponent* Niagara = Cast<UNiagaraComponent>(Effect);
		const ENCPoolMethod PoolMethod = Niagara ? Niagara->PoolingMethod : ENCPoolMethod::None;
		if (PoolMethod == ENCPoolMethod::ManualRelease)
		{
			Effect->ReleaseToPool();
		}
		else if (PoolMethod == ENCPoolMethod::None)
		{
			Effect->DestroyComponent();
		}
	}
	TrackedWorldEffects.Reset();
	if (WorldCleared > 0)
	{
		Cleared += WorldCleared;
		Parts.Add(FString::Printf(TEXT("world-owned %d (%s)"), WorldCleared, *FString::Join(WorldNames, TEXT(", "))));
	}

	UE_LOG(LogRogueRigs, Log, TEXT("URRNiagaraPoolSubsystem: robot death effects still alive removed for the next level: %d [%s]."), Cleared,
		*FString::Join(Parts, TEXT(", ")));
	return Cleared;
}

int32 URRNiagaraPoolSubsystem::CountLiveEffects() const
{
	int32 Count = 0;
	for (const TPair<TObjectPtr<UNiagaraSystem>, FRRNiagaraSystemPool>& Pair : Pools)
	{
		for (const UNiagaraComponent* Component : Pair.Value.InUse)
		{
			if (IsValid(Component) && (Component->IsActive() || IsWaitingToStart(*Component)))
			{
				++Count;
			}
		}
	}
	for (const TWeakObjectPtr<UFXSystemComponent>& WeakEffect : TrackedWorldEffects)
	{
		const UFXSystemComponent* Effect = WeakEffect.Get();
		if (IsValid(Effect) && IsEffectAlive(*Effect))
		{
			++Count;
		}
	}
	return Count;
}

void URRNiagaraPoolSubsystem::TrackWorldEffect(UFXSystemComponent* Effect)
{
	TrackedWorldEffects.RemoveAllSwap([](const TWeakObjectPtr<UFXSystemComponent>& WeakEffect)
	{
		const UFXSystemComponent* Tracked = WeakEffect.Get();
		return !IsValid(Tracked) || !IsEffectAlive(*Tracked);
	}, EAllowShrinking::No);
	if (IsValid(Effect))
	{
		TrackedWorldEffects.AddUnique(Effect);
	}
}

void URRNiagaraPoolSubsystem::CollectWorldOwnedEffects(const UWorld& World, TSet<const UFXSystemComponent*>& Out)
{
	// SpawnSystemAtLocation outers its component to the world settings (engine 5.8 NiagaraFunctionLibrary); the engine FX
	// pool outers its components to the world. Only direct children of both.
	TArray<UObject*> Children;
	GetObjectsWithOuter(&World, Children, EGetObjectsFlags::None);
	if (const AWorldSettings* Settings = World.GetWorldSettings())
	{
		GetObjectsWithOuter(Settings, Children, EGetObjectsFlags::None);
	}
	for (const UObject* Child : Children)
	{
		const UFXSystemComponent* Effect = Cast<UFXSystemComponent>(Child);
		if (IsValid(Effect) && !Effect->IsBeingDestroyed() && IsEffectAlive(*Effect))
		{
			Out.Add(Effect);
		}
	}
}

int32 URRNiagaraPoolSubsystem::TrackWorldEffectsSpawnedSince(const TSet<const UFXSystemComponent*>& Before)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0;
	}
	TSet<const UFXSystemComponent*> After;
	CollectWorldOwnedEffects(*World, After);
	int32 Tracked = 0;
	for (const UFXSystemComponent* Effect : After)
	{
		if (!Before.Contains(Effect))
		{
			TrackWorldEffect(const_cast<UFXSystemComponent*>(Effect));
			++Tracked;
		}
	}
	return Tracked;
}

UNiagaraComponent* URRNiagaraPoolSubsystem::CreateComponent(UNiagaraSystem* System)
{
	UWorld* World = GetWorld();
	AWorldSettings* Settings = World ? World->GetWorldSettings() : nullptr;
	if (!Settings)
	{
		return nullptr;
	}
	// Same set-up as UNiagaraFunctionLibrary::SpawnSystemAtLocation, minus auto-destroy: owned by the world settings,
	// absolute transform, inactive until Spawn.
	UNiagaraComponent* Component = NewObject<UNiagaraComponent>(Settings, NAME_None, RF_Transient);
	Component->SetAsset(System);
	Component->bAutoActivate = false;
	Component->SetAutoDestroy(false);
	Component->SetVisibleInRayTracing(false);
	Component->SetAbsolute(true, true, true);
	Component->RegisterComponentWithWorld(World);
	Component->OnSystemFinished.AddDynamic(this, &URRNiagaraPoolSubsystem::HandleSystemFinished);
	return Component;
}

void URRNiagaraPoolSubsystem::ReclaimInactive(FRRNiagaraSystemPool& Pool)
{
	for (int32 Index = Pool.InUse.Num() - 1; Index >= 0; --Index)
	{
		UNiagaraComponent* Component = Pool.InUse[Index];
		if (!IsValid(Component))
		{
			Pool.InUse.RemoveAtSwap(Index, EAllowShrinking::No);
		}
		else if (!Component->IsActive() && !IsWaitingToStart(*Component))
		{
			UE_LOG(LogRogueRigs, Verbose, TEXT("Niagara pool %s: reclaimed %s (inactive, not waiting to start)."),
				*GetNameSafe(Component->GetAsset()), *Component->GetName());
			Pool.InUse.RemoveAtSwap(Index, EAllowShrinking::No);
			Pool.Free.Add(Component);
		}
	}
}

bool URRNiagaraPoolSubsystem::IsWaitingToStart(const UNiagaraComponent& Component)
{
	// UNiagaraComponent::ActivateInternal leaves a component that is not ready to run (system compiling, PSOs precaching)
	// inactive with its tick enabled, and the tick activates it once ready; every other path out of Activate that leaves it
	// inactive (cannot render, not allowed, completed) disables the tick. Read in engine 5.8 source (task-194-01).
	return !Component.IsActive() && Component.IsComponentTickEnabled();
}

bool URRNiagaraPoolSubsystem::IsEffectAlive(const UFXSystemComponent& Effect)
{
	const UNiagaraComponent* Niagara = Cast<UNiagaraComponent>(&Effect);
	return Effect.IsActive() || (Niagara && IsWaitingToStart(*Niagara));
}

void URRNiagaraPoolSubsystem::HandleSystemFinished(UNiagaraComponent* Component)
{
	if (!Component)
	{
		return;
	}
	FRRNiagaraSystemPool* Pool = Pools.Find(Component->GetAsset());
	if (Pool && Pool->InUse.RemoveSingleSwap(Component, EAllowShrinking::No) > 0)
	{
		Pool->Free.Add(Component);
	}
}
