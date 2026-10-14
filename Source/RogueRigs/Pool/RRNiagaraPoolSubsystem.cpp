// Rogue Rigs - Niagara component pool world subsystem (sys-actor-pool): capped per-system pools of reusable one-shot Niagara components (robot death puff, debris).

#include "Pool/RRNiagaraPoolSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "RogueRigs.h"

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
		return nullptr;
	}

	Pool.InUse.Add(Component);
	Component->SetWorldLocationAndRotation(Location, Rotation);
	BeforeActivate(*Component);
	// A system that cannot run completes inside Activate: the finish callback has already put the component back.
	Component->Activate(/*bReset*/ true);
	return Pool.InUse.Contains(Component) ? Component : nullptr;
}

void URRNiagaraPoolSubsystem::Prewarm(UNiagaraSystem* System, int32 Count, int32 Cap)
{
	if (!System || Cap <= 0)
	{
		return;
	}
	FRRNiagaraSystemPool& Pool = Pools.FindOrAdd(System);
	Pool.Cap = Cap;
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
		else if (!Component->IsActive())
		{
			Pool.InUse.RemoveAtSwap(Index, EAllowShrinking::No);
			Pool.Free.Add(Component);
		}
	}
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
