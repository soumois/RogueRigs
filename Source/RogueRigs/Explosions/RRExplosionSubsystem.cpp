// Rogue Rigs - explosion world subsystem: queued explosions resolved one link at a time, terrain and character damage, FX.

#include "Explosions/RRExplosionSubsystem.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Audio/RRAudioSubsystem.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/RRCameraShakeLibrary.h"
#include "Camera/RRExplosionCameraShake.h"
#include "Damage/RRGE_DamageExplosion.h"
#include "Engine/DataTable.h"
#include "Enemies/RREnemy.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionFX.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Misc/App.h"
#include "Grid/RRBlockColors.h"
#include "Misc/PackageName.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "RogueRigs.h"
#include "Telegraph/RRDecalWarmUp.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRExplosionSubsystem)

const TCHAR* URRExplosionSubsystem::ExplosionStatsPath = TEXT("RogueRigs/Data/Tables/ExplosionStats.csv");

const FName URRExplosionSubsystem::WallDebrisCenterParameter(TEXT("User.ExplosionCenter"));
const FName URRExplosionSubsystem::WallDebrisPositionsParameter(TEXT("User.DebrisPositions"));
const FName URRExplosionSubsystem::WallDebrisColorsParameter(TEXT("User.DebrisColors"));
const FName URRExplosionSubsystem::WallDebrisCountParameter(TEXT("User.DebrisCount"));
const FName URRExplosionSubsystem::ExplosionSoundEvent(TEXT("Explosion"));

URRExplosionSubsystem::URRExplosionSubsystem()
{
	ExplosionShakeClass = URRExplosionCameraShake::StaticClass();
	WallDebrisSystem = TSoftObjectPtr<UNiagaraSystem>(FSoftObjectPath(TEXT("/Game/RogueRigs/FX/NS_ExplosionWallDebris.NS_ExplosionWallDebris")));
}

void URRExplosionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const UWorld* World = GetWorld();
	if (Pending.IsEmpty() || !World)
	{
		return;
	}
	// Small tolerance so float time steps do not push a link one frame late.
	const double Now = World->GetTimeSeconds();
	if (Now + 1.e-4 < NextExplosionTime)
	{
		return;
	}
	const FPendingExplosion Explosion = Pending[0];
	Pending.RemoveAt(0);
	NextExplosionTime = Now + ChainDelay;
	Explode(Explosion);
}

TStatId URRExplosionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRExplosionSubsystem, STATGROUP_Tickables);
}

void URRExplosionSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (!InWorld.IsGameWorld() || !FApp::CanEverRender())
	{
		return;
	}
	if (UNiagaraSystem* WallDebris = ResolveWallDebrisSystem())
	{
		if (URRNiagaraPoolSubsystem* FXPool = InWorld.GetSubsystem<URRNiagaraPoolSubsystem>())
		{
			FXPool->Prewarm(WallDebris, WallDebrisPoolSize, WallDebrisPoolSize);
		}
	}
	const UDataTable* Table = FRRBaseStatsLoader::GetCachedTable(ExplosionStatsPath, FRRExplosionStatsRow::StaticStruct());
	if (!Table)
	{
		return;
	}
	Table->ForeachRow<FRRExplosionStatsRow>(TEXT("URRExplosionSubsystem::OnWorldBeginPlay"), [this, &InWorld](const FName& RowName, const FRRExplosionStatsRow& Row)
	{
		UClass* FXClass = ResolveFXClass(Row.FXClass);
		if (!FXClass)
		{
			return;
		}
		PreloadedFXClasses.AddUnique(FXClass);
		ARRDecalWarmUp::WarmUp(&InWorld, FXClass->GetDefaultObject<ARRExplosionFX>()->GetShockwaveMaterial());
	});
}

bool URRExplosionSubsystem::QueueExplosion(FVector Center, FName RowName, AActor* Instigator, UObject* SourceObject)
{
	FRRExplosionStatsRow Stats;
	if (!GetExplosionStats(RowName, Stats))
	{
		return false;
	}
	return QueueExplosionWithStats(Center, RowName, Stats, Instigator, SourceObject);
}

bool URRExplosionSubsystem::QueueExplosionWithStats(FVector Center, FName RowName, const FRRExplosionStatsRow& Stats, AActor* Instigator, UObject* SourceObject)
{
	if (SourceObject)
	{
		bool bAlreadySpent = false;
		SpentSources.Add(FObjectKey(SourceObject), &bAlreadySpent);
		if (bAlreadySpent)
		{
			return false;
		}
	}
	FPendingExplosion& Explosion = Pending.AddDefaulted_GetRef();
	Explosion.Center = Center;
	Explosion.RowName = RowName;
	Explosion.Stats = Stats;
	Explosion.Instigator = Instigator;
	Explosion.SourceObject = SourceObject;
	return true;
}

bool URRExplosionSubsystem::GetExplosionStats(FName RowName, FRRExplosionStatsRow& OutStats)
{
	return FRRBaseStatsLoader::FindRow(ExplosionStatsPath, RowName, OutStats, TEXT("URRExplosionSubsystem"));
}

bool URRExplosionSubsystem::IsSourceSpent(const UObject* SourceObject) const
{
	return SourceObject && SpentSources.Contains(FObjectKey(SourceObject));
}

int32 URRExplosionSubsystem::ClearForNextLevel()
{
	const int32 Dropped = Pending.Num();
	Pending.Reset();
	SpentSources.Reset();
	return Dropped;
}

bool URRExplosionSubsystem::IsSourcePending(const UObject* SourceObject) const
{
	return SourceObject && Pending.ContainsByPredicate([SourceObject](const FPendingExplosion& Explosion)
	{
		return Explosion.SourceObject.Get() == SourceObject;
	});
}

bool URRExplosionSubsystem::ForgetSource(const UObject* SourceObject)
{
	if (!SourceObject || IsSourcePending(SourceObject))
	{
		return false;
	}
	SpentSources.Remove(FObjectKey(SourceObject));
	return true;
}

void URRExplosionSubsystem::Explode(const FPendingExplosion& Explosion)
{
	ARRArenaGrid* CurrentGrid = ResolveGrid();
	TArray<FRRExplosionBrokenBlock> BrokenBlocks;
	if (CurrentGrid)
	{
		CurrentGrid->QueueDestroyCellsInRadius(Explosion.Center, Explosion.Stats.Radius, Explosion.Stats.TerrainSource, &BrokenBlocks);
	}
	DamageCharacters(Explosion);
	SpawnFX(Explosion);
	if (CurrentGrid)
	{
		PlayWallDebris(Explosion, *CurrentGrid, BrokenBlocks);
	}
	PlayShake(Explosion);
	// Every explosion (barrel, kamikaze, chain links): one placed sound faded from the explosion edge (task-516, sys-audio).
	URRAudioSubsystem::PlayEventAt(this, ExplosionSoundEvent, Explosion.Center, NAME_None, Explosion.Stats.Radius);
	UE_LOG(LogRogueRigs, Verbose, TEXT("Explosion '%s' at %s (radius %.0f, damage %.0f, miner damage %.0f)"), *Explosion.RowName.ToString(), *Explosion.Center.ToString(), Explosion.Stats.Radius, Explosion.Stats.Damage, Explosion.Stats.MinerDamage);
	OnExplosion.Broadcast(Explosion.Center, Explosion.RowName);
}

void URRExplosionSubsystem::DamageCharacters(const FPendingExplosion& Explosion)
{
	UWorld* World = GetWorld();
	if (!World || (Explosion.Stats.Damage <= 0.f && Explosion.Stats.MinerDamage <= 0.f))
	{
		return;
	}

	// Collect first: applying damage can kill, spawn or destroy actors.
	const float RadiusSquared = FMath::Square(Explosion.Stats.Radius);
	TArray<TPair<TWeakObjectPtr<AActor>, TWeakObjectPtr<UAbilitySystemComponent>>> Targets;
	TSet<const UAbilitySystemComponent*> SeenASCs;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		// Robots parked idle in the actor pool are not in the game (sys-actor-pool).
		if (!IsValid(Actor) || FVector::DistSquared2D(Actor->GetActorLocation(), Explosion.Center) > RadiusSquared
			|| URRActorPoolSubsystem::IsPooledIdle(Actor))
		{
			continue;
		}
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
		if (!ASC || !ASC->HasAttributeSetForAttribute(URRHealthAttributeSet::GetHealthAttribute()))
		{
			continue;
		}
		bool bAlreadySeen = false;
		SeenASCs.Add(ASC, &bAlreadySeen);
		if (!bAlreadySeen)
		{
			Targets.Emplace(Actor, ASC);
		}
	}

	AActor* Instigator = Explosion.Instigator.Get();
	AActor* Causer = Cast<AActor>(Explosion.SourceObject.Get());
	for (const TPair<TWeakObjectPtr<AActor>, TWeakObjectPtr<UAbilitySystemComponent>>& Target : Targets)
	{
		AActor* Actor = Target.Key.Get();
		UAbilitySystemComponent* ASC = Target.Value.Get();
		if (!Actor || !ASC)
		{
			continue;
		}
		const float Damage = ComputeCharacterDamage(Actor, *ASC, Explosion.Stats);
		if (Damage <= 0.f)
		{
			continue;
		}
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddInstigator(Instigator, Causer ? Causer : Instigator);
		if (UObject* Source = Explosion.SourceObject.Get())
		{
			Context.AddSourceObject(Source);
		}
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(URRGE_DamageExplosion::StaticClass(), 1.f, Context);
		if (Spec.IsValid())
		{
			// Robots throw their death debris radially from the blast (enemy-death); a robot on the centre keeps its last hit.
			if (ARREnemy* Robot = Cast<ARREnemy>(Actor))
			{
				Robot->RecordHitDirection(Actor->GetActorLocation() - Explosion.Center);
			}
			Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Damage);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
		}
	}
}

float URRExplosionSubsystem::ComputeCharacterDamage(const AActor* Target, const UAbilitySystemComponent& TargetASC, const FRRExplosionStatsRow& Stats) const
{
	if (!Cast<ARRMiner>(Target))
	{
		return Stats.Damage;
	}
	// The miner takes his own value; the no-one-shot cap applies to it.
	const float Damage = Stats.MinerDamage;
	const float Health = TargetASC.GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	const float MaxHealth = TargetASC.GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	if (MaxHealth > 0.f && Health >= MaxHealth)
	{
		return FMath::Min(Damage, FMath::Max(Health - MinerHealthLeftAtFullHealth, 0.f));
	}
	return Damage;
}

void URRExplosionSubsystem::PlayShake(const FPendingExplosion& Explosion)
{
	// Resolved explosion only (a fuse never reaches Explode); each chain link plays it again (task-418 Q6).
	const float MaxDistance = Explosion.Stats.Radius * Explosion.Stats.ShakeMaxDistanceFactor;
	if (!ExplosionShakeClass || Explosion.Stats.ShakeScale <= 0.f || MaxDistance <= 0.f)
	{
		return;
	}
	URRCameraShakeLibrary::PlayCameraShakeAtLocation(this, Explosion.Center, MaxDistance, Explosion.Stats.ShakeScale, ExplosionShakeClass);
}

void URRExplosionSubsystem::SpawnFX(const FPendingExplosion& Explosion)
{
	UWorld* World = GetWorld();
	UClass* FXClass = World ? ResolveFXClass(Explosion.Stats.FXClass) : nullptr;
	if (!FXClass)
	{
		return;
	}
	const FTransform SpawnTransform(Explosion.Center);
	ARRExplosionFX* FX = World->SpawnActorDeferred<ARRExplosionFX>(FXClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (FX)
	{
		FX->SetRadius(GetVisualRadius(Explosion.Stats.Radius));
		FX->SetHoldSeconds(VisualHoldSeconds);
		FX->FinishSpawning(SpawnTransform);
	}
}

void URRExplosionSubsystem::PlayWallDebris(const FPendingExplosion& Explosion, const ARRArenaGrid& CurrentGrid, const TArray<FRRExplosionBrokenBlock>& Blocks)
{
	UWorld* World = GetWorld();
	URRNiagaraPoolSubsystem* FXPool = World ? World->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
	UNiagaraSystem* System = Blocks.IsEmpty() ? nullptr : ResolveWallDebrisSystem();
	const int32 Count = int32(FMath::Min<int64>(WallDebrisMaxParticles, int64(Blocks.Num()) * WallDebrisParticlesPerBlock));
	if (!FXPool || !System || Count <= 0)
	{
		return;
	}

	FRRWallDebrisRequest& Request = LastWallDebrisRequest;
	Request.Center = Explosion.Center;
	Request.Positions.Reset(Count);
	Request.Colors.Reset(Count);
	// Blocks are sorted nearest first: an even stride keeps debris from every distance when the maximum thins them.
	const float Jitter = CurrentGrid.GetCellSize() * WallDebrisCellJitter;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		const FRRExplosionBrokenBlock& Block = Blocks[int32(int64(Index) * Blocks.Num() / Count)];
		const FVector Offset(FMath::FRandRange(-Jitter, Jitter), FMath::FRandRange(-Jitter, Jitter), FMath::FRandRange(WallDebrisMinHeight, WallDebrisMaxHeight));
		Request.Positions.Add(CurrentGrid.GetCellCenter(Block.Cell.X, Block.Cell.Y) + Offset);
		Request.Colors.Add(RRBlockColors::ForType(Block.Type));
	}

	// Null = pool full: this link of the chain shows no debris.
	FXPool->Spawn(System, Explosion.Center, FRotator::ZeroRotator, WallDebrisPoolSize, [&Request, Count](UNiagaraComponent& Component)
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(&Component, WallDebrisPositionsParameter, Request.Positions);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayColor(&Component, WallDebrisColorsParameter, Request.Colors);
		Component.SetVariablePosition(WallDebrisCenterParameter, Request.Center);
		Component.SetVariableInt(WallDebrisCountParameter, Count);
	});
}

UNiagaraSystem* URRExplosionSubsystem::ResolveWallDebrisSystem()
{
	if (ResolvedWallDebrisSystem || bWallDebrisSystemMissing || WallDebrisSystem.IsNull())
	{
		return ResolvedWallDebrisSystem;
	}
	UNiagaraSystem* System = WallDebrisSystem.Get();
	// Check the package first: loading a missing asset logs a warning.
	if (!System && FPackageName::DoesPackageExist(WallDebrisSystem.ToSoftObjectPath().GetLongPackageName()))
	{
		System = WallDebrisSystem.LoadSynchronous();
	}
	if (!System)
	{
		bWallDebrisSystemMissing = true;
		UE_LOG(LogRogueRigs, Log, TEXT("URRExplosionSubsystem: wall debris system %s missing, explosions throw no wall debris"), *WallDebrisSystem.ToString());
		return nullptr;
	}
	ResolvedWallDebrisSystem = System;
	return System;
}

UClass* URRExplosionSubsystem::ResolveFXClass(const TSoftClassPtr<ARRExplosionFX>& FXClass)
{
	if (FXClass.IsNull() || UnusableFXPaths.Contains(FXClass.ToSoftObjectPath()))
	{
		return nullptr;
	}
	UClass* Class = FXClass.Get();
	if (!Class)
	{
		const FSoftObjectPath& Path = FXClass.ToSoftObjectPath();
		// Check the package first: loading a missing asset logs a warning on every attempt.
		if (FPackageName::DoesPackageExist(Path.GetLongPackageName()))
		{
			Class = FXClass.LoadSynchronous();
		}
	}
	if (!Class || Class->HasAnyClassFlags(CLASS_Abstract))
	{
		UnusableFXPaths.Add(FXClass.ToSoftObjectPath());
		UE_LOG(LogRogueRigs, Log, TEXT("URRExplosionSubsystem: FX class %s missing or abstract, explosions have no visual"), *FXClass.ToString());
		return nullptr;
	}
	return Class;
}

ARRArenaGrid* URRExplosionSubsystem::ResolveGrid()
{
	if (!Grid.IsValid())
	{
		Grid = RRWorld::FindArenaGrid(GetWorld());
	}
	return Grid.Get();
}
