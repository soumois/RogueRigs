// Rogue Rigs - run log collector: per-level counters (terrain, damage, kills, upgrades, XP) fed by game events.

#include "RunLog/RRRunLogSubsystem.h"

#include "Damage/RRGE_DamageExplosion.h"
#include "Enemies/RREnemy.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RRCellType.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "Run/RRRunSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "Upgrades/RRUpgradeCard.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRWeapon.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRRunLogSubsystem)

namespace RRRunLog
{
	/** Kill source names (run-log-schema "by source"). */
	const FName KillSourceWeapon(TEXT("weapon"));
	const FName KillSourceExplosion(TEXT("explosion"));
	const FName KillSourceOther(TEXT("other"));

	/** Copies an FName-keyed map into the FString-keyed map of the snapshot. */
	template <typename ValueType>
	void CopyToStringMap(const TMap<FName, ValueType>& From, TMap<FString, ValueType>& To)
	{
		To.Reset();
		for (const TPair<FName, ValueType>& Pair : From)
		{
			To.Add(Pair.Key.ToString(), Pair.Value);
		}
	}

	/** "key=value, key=value" of a map, for the dump. */
	template <typename ValueType>
	FString JoinMap(const TMap<FString, ValueType>& Map)
	{
		TArray<FString> Parts;
		for (const TPair<FString, ValueType>& Pair : Map)
		{
			Parts.Add(FString::Printf(TEXT("%s=%s"), *Pair.Key, *LexToSanitizedString(Pair.Value)));
		}
		return Parts.Num() > 0 ? FString::Join(Parts, TEXT(", ")) : FString(TEXT("(none)"));
	}

	/** True when the hit came from an explosion (URRGE_DamageExplosion or a child). */
	bool IsExplosionHit(const FRRDamageEventInfo& Info)
	{
		return Info.EffectClass && Info.EffectClass->IsChildOf(URRGE_DamageExplosion::StaticClass());
	}

	/** Robot type of an enemy: its EnemyStats.csv row, else its class name. */
	FName GetRobotType(const ARREnemy& Enemy)
	{
		const FName Row = Enemy.GetStatsRowName();
		return Row.IsNone() ? Enemy.GetClass()->GetFName() : Row;
	}
}

URRRunLogSubsystem* URRRunLogSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext && GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URRRunLogSubsystem>() : nullptr;
}

void URRRunLogSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AnyDamageHandle = URRHealthAttributeSet::OnAnyDamageReceived.AddUObject(this, &URRRunLogSubsystem::HandleAnyDamage);

	if (URRRunTransitionSubsystem* Transition = Collection.InitializeDependency<URRRunTransitionSubsystem>())
	{
		Transition->OnLevelReady.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleLevelReady);
	}
}

void URRRunLogSubsystem::Deinitialize()
{
	URRHealthAttributeSet::OnAnyDamageReceived.Remove(AnyDamageHandle);
	AnyDamageHandle.Reset();
	Super::Deinitialize();
}

void URRRunLogSubsystem::HandleMinerBeginPlay(ARRMiner* Miner)
{
	UWorld* World = Miner ? Miner->GetWorld() : nullptr;
	if (!World || TrackedWorld.Get() == World)
	{
		return;
	}

	ResetForLevel(World);

	if (ARRArenaGrid* Grid = RRWorld::FindArenaGrid(World))
	{
		Grid->OnCellDestroyed.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleCellDestroyed);
		BoundGrid = Grid;
	}
	if (URRUpgradeComponent* Upgrades = Miner->GetUpgrades())
	{
		Upgrades->OnUpgradeApplied.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleUpgradeApplied);
	}
	if (URRProgressionComponent* Progression = Miner->GetProgression())
	{
		Progression->OnXPGained.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleXPGained);
	}

	UE_LOG(LogRogueRigs, Log, TEXT("RunLog: level %d (%s) started, counters reset (grid %s)."),
		Stats.LevelIndex, *Stats.LayoutName, BoundGrid.IsValid() ? TEXT("bound") : TEXT("missing"));
}

void URRRunLogSubsystem::ResetForLevel(UWorld* World)
{
	if (ARRArenaGrid* OldGrid = BoundGrid.Get())
	{
		OldGrid->OnCellDestroyed.RemoveDynamic(this, &URRRunLogSubsystem::HandleCellDestroyed);
	}
	BoundGrid.Reset();
	TrackedWorld = World;

	Stats = FRRRunLevelStats();
	DamageDealtByWeapon.Reset();
	KillsByRobotType.Reset();
	DamageTakenBySource.Reset();

	if (const URRRunSubsystem* Run = URRRunSubsystem::Get(World))
	{
		Stats.LevelIndex = Run->GetLevelIndex();
		Stats.LayoutName = Run->GetCurrentLayoutName();
	}
	Stats.LevelStartWorldSeconds = World ? World->GetTimeSeconds() : 0.0;
}

FRRRunLevelStats URRRunLogSubsystem::GetCurrentLevelStats() const
{
	FRRRunLevelStats Copy = Stats;
	RRRunLog::CopyToStringMap(DamageDealtByWeapon, Copy.DamageDealtByWeapon);
	RRRunLog::CopyToStringMap(KillsByRobotType, Copy.KillsByRobotType);
	RRRunLog::CopyToStringMap(DamageTakenBySource, Copy.DamageTakenBySource);
	return Copy;
}

void URRRunLogSubsystem::LogCurrentLevelStats(const FString& Context) const
{
	const FRRRunLevelStats Current = GetCurrentLevelStats();
	const UWorld* World = TrackedWorld.Get();
	const double Elapsed = World ? World->GetTimeSeconds() - Current.LevelStartWorldSeconds : 0.0;
	UE_LOG(LogRogueRigs, Log, TEXT("%s: level %d (%s), %.1f s since level start."), *Context, Current.LevelIndex, *Current.LayoutName, Elapsed);
	UE_LOG(LogRogueRigs, Log, TEXT("%s: terrain_destroyed by source: %s"), *Context, *RRRunLog::JoinMap(Current.TerrainDestroyedBySource));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: terrain_destroyed by type: %s"), *Context, *RRRunLog::JoinMap(Current.TerrainDestroyedByType));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: damage_dealt by weapon: %s"), *Context, *RRRunLog::JoinMap(Current.DamageDealtByWeapon));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: kills by robot type: %s"), *Context, *RRRunLog::JoinMap(Current.KillsByRobotType));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: kills by source: %s"), *Context, *RRRunLog::JoinMap(Current.KillsBySource));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: upgrades_picked: %s"), *Context,
		Current.UpgradesPicked.Num() > 0 ? *FString::Join(Current.UpgradesPicked, TEXT(", ")) : TEXT("(none)"));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: damage_taken by source: %s"), *Context, *RRRunLog::JoinMap(Current.DamageTakenBySource));
	UE_LOG(LogRogueRigs, Log, TEXT("%s: xp_collected: %.2f"), *Context, Current.XPCollected);
}

FString URRRunLogSubsystem::MakeCardId(const FRRUpgradeCard& Card)
{
	if (Card.CardType == ERRUpgradeCardType::NewWeapon)
	{
		return FString::Printf(TEXT("NewWeapon@%s"), Card.WeaponClass ? *Card.WeaponClass->GetName() : TEXT("None"));
	}

	FString Target;
	switch (Card.Target)
	{
	case ERRUpgradeTarget::Player: Target = TEXT("Player"); break;
	case ERRUpgradeTarget::AllWeapons: Target = TEXT("AllWeapons"); break;
	default: Target = Card.TargetWeapon ? Card.TargetWeapon->GetClass()->GetName() : FString(TEXT("Weapon")); break;
	}
	return FString::Printf(TEXT("%s_%s@%s"), *StaticEnum<ERRUpgradeStat>()->GetNameStringByValue(static_cast<int64>(Card.Stat)),
		*StaticEnum<ERRUpgradeRarity>()->GetNameStringByValue(static_cast<int64>(Card.Tier)), *Target);
}

FString URRRunLogSubsystem::GetTerrainSourceName(ERRTerrainDamageSource Source)
{
	switch (Source)
	{
	case ERRTerrainDamageSource::Drone: return TEXT("drone");
	case ERRTerrainDamageSource::Barrel: return TEXT("barrel");
	case ERRTerrainDamageSource::Kamikaze: return TEXT("kamikaze");
	case ERRTerrainDamageSource::Shot: return TEXT("shot");
	default: return StaticEnum<ERRTerrainDamageSource>()->GetNameStringByValue(static_cast<int64>(Source)).ToLower();
	}
}

FString URRRunLogSubsystem::GetCellTypeName(ERRCellType Type)
{
	switch (Type)
	{
	case ERRCellType::Rock: return TEXT("rock");
	case ERRCellType::Ore: return TEXT("ore");
	case ERRCellType::HealOre: return TEXT("heal_ore");
	case ERRCellType::SpecialOre: return TEXT("special_ore");
	case ERRCellType::Barrel: return TEXT("barrel");
	default: return StaticEnum<ERRCellType>()->GetNameStringByValue(static_cast<int64>(Type)).ToLower();
	}
}

void URRRunLogSubsystem::HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source)
{
	++Stats.TerrainDestroyedBySource.FindOrAdd(GetTerrainSourceName(Source));
	++Stats.TerrainDestroyedByType.FindOrAdd(GetCellTypeName(Type));
}

void URRRunLogSubsystem::HandleUpgradeApplied(const FRRUpgradeCard& Card)
{
	Stats.UpgradesPicked.Add(MakeCardId(Card));
}

void URRRunLogSubsystem::HandleXPGained(float Amount)
{
	Stats.XPCollected += Amount;
}

void URRRunLogSubsystem::HandleLevelReady(int32 LevelIndex, const FString& LayoutName)
{
	const UWorld* World = TrackedWorld.Get();
	if (World)
	{
		Stats.LevelStartWorldSeconds = World->GetTimeSeconds();
	}
	UE_LOG(LogRogueRigs, Log, TEXT("RunLog: level %d (%s) ready, start time restamped to %.2f s."), LevelIndex, *LayoutName, Stats.LevelStartWorldSeconds);
}

const AActor* URRRunLogSubsystem::FindCreditedWeapon(const FRRDamageEventInfo& Info)
{
	const AActor* Weapon = Cast<ARRWeapon>(Info.Instigator);
	if (!Weapon)
	{
		Weapon = Cast<ARRWeapon>(Info.EffectCauser);
	}
	// A turret (ARRTurret is a weapon) is owned by its launcher: its hits count for the launcher.
	if (Weapon && Cast<ARRWeapon>(Weapon->GetOwner()))
	{
		Weapon = Weapon->GetOwner();
	}
	return Weapon;
}

void URRRunLogSubsystem::HandleAnyDamage(const FRRDamageEventInfo& Info)
{
	if (!Info.Target || !TrackedWorld.IsValid() || Info.Target->GetWorld() != TrackedWorld.Get())
	{
		return;
	}

	const bool bExplosion = RRRunLog::IsExplosionHit(Info);
	if (const ARREnemy* Robot = Cast<ARREnemy>(Info.Target))
	{
		const AActor* Weapon = bExplosion ? nullptr : FindCreditedWeapon(Info);
		if (Weapon)
		{
			DamageDealtByWeapon.FindOrAdd(Weapon->GetClass()->GetFName()) += Info.Damage;
		}
		if (Info.bKilled)
		{
			++KillsByRobotType.FindOrAdd(RRRunLog::GetRobotType(*Robot));
			const FName Source = bExplosion ? RRRunLog::KillSourceExplosion : (Weapon ? RRRunLog::KillSourceWeapon : RRRunLog::KillSourceOther);
			++Stats.KillsBySource.FindOrAdd(Source.ToString());
		}
	}
	else if (Cast<ARRMiner>(Info.Target))
	{
		FName Source = RRRunLog::KillSourceOther;
		if (bExplosion)
		{
			Source = RRRunLog::KillSourceExplosion;
		}
		else if (const ARREnemy* Attacker = Cast<ARREnemy>(Info.Instigator))
		{
			Source = RRRunLog::GetRobotType(*Attacker);
		}
		DamageTakenBySource.FindOrAdd(Source) += Info.Damage;
	}
}
