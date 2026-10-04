// Rogue Rigs - explosion world subsystem: queued explosions resolved one link at a time, terrain and character damage, FX.

#include "Explosions/RRExplosionSubsystem.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Damage/RRGE_DamageExplosion.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionFX.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Misc/PackageName.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRExplosionSubsystem)

const TCHAR* URRExplosionSubsystem::ExplosionStatsPath = TEXT("RogueRigs/Data/Tables/ExplosionStats.csv");

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
	if (!StatsTable)
	{
		StatsTable = FRRBaseStatsLoader::LoadTableFromCSV(ExplosionStatsPath, FRRExplosionStatsRow::StaticStruct(), this);
		if (!StatsTable)
		{
			return false;
		}
	}
	const FRRExplosionStatsRow* Row = StatsTable->FindRow<FRRExplosionStatsRow>(RowName, TEXT("URRExplosionSubsystem"), false);
	if (!Row)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRExplosionSubsystem: row '%s' not found in %s"), *RowName.ToString(), ExplosionStatsPath);
		return false;
	}
	OutStats = *Row;
	return true;
}

bool URRExplosionSubsystem::IsSourceSpent(const UObject* SourceObject) const
{
	return SourceObject && SpentSources.Contains(FObjectKey(SourceObject));
}

void URRExplosionSubsystem::Explode(const FPendingExplosion& Explosion)
{
	if (ARRArenaGrid* CurrentGrid = ResolveGrid())
	{
		CurrentGrid->DestroyCellsInRadius(Explosion.Center, Explosion.Stats.Radius, Explosion.Stats.TerrainSource);
	}
	DamageCharacters(Explosion);
	SpawnFX(Explosion);
	UE_LOG(LogRogueRigs, Verbose, TEXT("Explosion '%s' at %s (radius %.0f, damage %.0f)"), *Explosion.RowName.ToString(), *Explosion.Center.ToString(), Explosion.Stats.Radius, Explosion.Stats.Damage);
	OnExplosion.Broadcast(Explosion.Center, Explosion.RowName);
}

void URRExplosionSubsystem::DamageCharacters(const FPendingExplosion& Explosion)
{
	UWorld* World = GetWorld();
	if (!World || Explosion.Stats.Damage <= 0.f)
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
		if (!IsValid(Actor) || FVector::DistSquared2D(Actor->GetActorLocation(), Explosion.Center) > RadiusSquared)
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
		const float Damage = ComputeCharacterDamage(Actor, *ASC, Explosion.Stats.Damage);
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
			Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Damage);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
		}
	}
}

float URRExplosionSubsystem::ComputeCharacterDamage(const AActor* Target, const UAbilitySystemComponent& TargetASC, float Damage) const
{
	if (!Cast<ARRMiner>(Target))
	{
		return Damage;
	}
	const float Health = TargetASC.GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute());
	const float MaxHealth = TargetASC.GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	if (MaxHealth > 0.f && Health >= MaxHealth)
	{
		return FMath::Min(Damage, FMath::Max(Health - MinerHealthLeftAtFullHealth, 0.f));
	}
	return Damage;
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
		FX->SetRadius(Explosion.Stats.Radius);
		FX->FinishSpawning(SpawnTransform);
	}
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
