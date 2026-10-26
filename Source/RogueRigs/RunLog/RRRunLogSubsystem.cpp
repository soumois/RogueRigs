// Rogue Rigs - run log: per-level counters fed by game events, level entries at level end and the run JSON file (run-log-schema).

#include "RunLog/RRRunLogSubsystem.h"

#include "Damage/RRGE_DamageExplosion.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RRCellType.h"
#include "Misc/App.h"
#include "Misc/Paths.h"
#include "Objectives/RRBonusChest.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"
#include "Run/RRRunSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RunLog/RRRunLogWriter.h"
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
	if (URRRunSubsystem* RunSubsystem = Collection.InitializeDependency<URRRunSubsystem>())
	{
		RunSubsystem->OnRunReset.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleRunReset);
	}
}

void URRRunLogSubsystem::Deinitialize()
{
	URRHealthAttributeSet::OnAnyDamageReceived.Remove(AnyDamageHandle);
	AnyDamageHandle.Reset();
	Super::Deinitialize();
}

void URRRunLogSubsystem::HandleLevelStartedInPlace(ARRMiner* Miner)
{
	TrackedWorld.Reset();
	HandleMinerBeginPlay(Miner);
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
	Miner->OnDeath.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleMinerDeath);
	if (URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>())
	{
		RunFlow->OnShuttleLanded.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleShuttleLanded);
		RunFlow->OnOreChanged.AddUniqueDynamic(this, &URRRunLogSubsystem::HandleOreChanged);
		LastCarriedOre = RunFlow->GetCarriedOre();
		BoundRunFlow = RunFlow;
	}

	// A run's level 1 (or a level seen without an open run) starts a new run record and file.
	if (!bRunOpen || Stats.LevelIndex <= 1)
	{
		BeginRunRecord();
	}
	Run.Layouts.Add(Stats.LayoutName);

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
	if (URRRunFlowSubsystem* OldRunFlow = BoundRunFlow.Get())
	{
		OldRunFlow->OnShuttleLanded.RemoveDynamic(this, &URRRunLogSubsystem::HandleShuttleLanded);
		OldRunFlow->OnOreChanged.RemoveDynamic(this, &URRRunLogSubsystem::HandleOreChanged);
	}
	BoundRunFlow.Reset();
	TrackedWorld = World;
	bLevelEnded = false;
	bShuttleLanded = false;
	LastCarriedOre = 0;
	OreInRiskWindow = 0;

	Stats = FRRRunLevelStats();
	DamageDealtByWeapon.Reset();
	KillsByRobotType.Reset();
	DamageTakenBySource.Reset();

	if (const URRRunSubsystem* RunSubsystem = URRRunSubsystem::Get(World))
	{
		Stats.LevelIndex = RunSubsystem->GetLevelIndex();
		Stats.LayoutName = RunSubsystem->GetCurrentLayoutName();
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

FString URRRunLogSubsystem::GetLevelOutcomeName(ERRRunLevelOutcome Outcome)
{
	switch (Outcome)
	{
	case ERRRunLevelOutcome::Extracted: return TEXT("extracted");
	case ERRRunLevelOutcome::Death: return TEXT("death");
	case ERRRunLevelOutcome::MissedShuttle: return TEXT("missed_shuttle");
	default: return TEXT("unknown");
	}
}

void URRRunLogSubsystem::HandleLevelEnded(UWorld* World, ERRRunLevelOutcome Outcome)
{
	if (!World || World != TrackedWorld.Get() || bLevelEnded)
	{
		return;
	}
	bLevelEnded = true;

	FRRRunLogLevelEntry Entry;
	Entry.Stats = GetCurrentLevelStats();
	Entry.LevelOutcome = GetLevelOutcomeName(Outcome);
	Entry.TimePerLevelSeconds = static_cast<float>(World->GetTimeSeconds() - Entry.Stats.LevelStartWorldSeconds);
	Entry.OreCollectedInRiskWindow = OreInRiskWindow;

	const URRRunFlowSubsystem* RunFlow = World->GetSubsystem<URRRunFlowSubsystem>();
	const ARRShuttle* Shuttle = RunFlow ? RunFlow->GetShuttle() : nullptr;
	if (Outcome == ERRRunLevelOutcome::Extracted && Shuttle && Shuttle->IsFinished() && Shuttle->WasExtracted())
	{
		Entry.bReachedShuttle = true;
		Entry.TimeLeftAtShuttleSeconds = Shuttle->GetRemainingSeconds();
	}
	CollectEventsMet(RunFlow, Entry.EventsMet);

	if (!bRunOpen)
	{
		BeginRunRecord();
		Run.Layouts.Add(Entry.Stats.LayoutName);
	}
	Run.Levels.Add(Entry);

	// The run goes on only after an extraction from a level that is not the run's last (the intermission follows).
	const URRRunSubsystem* RunSubsystem = URRRunSubsystem::Get(World);
	const bool bRunEnds = Outcome != ERRRunLevelOutcome::Extracted || !RunSubsystem || !RunSubsystem->IsRunInProgress() || RunSubsystem->IsLastLevel();
	if (bRunEnds)
	{
		EndRunRecord(Outcome == ERRRunLevelOutcome::Extracted ? FString(TEXT("won")) : Entry.LevelOutcome);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("RunLog: level %d (%s) ended: %s after %.1f s, shuttle time left %s, risk-window ore %d, %d event(s) met; run %s."),
		Entry.Stats.LevelIndex, *Entry.Stats.LayoutName, *Entry.LevelOutcome, Entry.TimePerLevelSeconds,
		Entry.bReachedShuttle ? *FString::Printf(TEXT("%.1f s"), Entry.TimeLeftAtShuttleSeconds) : TEXT("null"),
		Entry.OreCollectedInRiskWindow, Entry.EventsMet.Num(), *Run.Outcome);
	WriteRunFile();
}

void URRRunLogSubsystem::BeginRunRecord()
{
	if (bRunOpen)
	{
		EndRunRecord(TEXT("abandoned"));
		WriteRunFile();
	}

	Run = FRRRunLogRecord();
	Run.RunId = FDateTime::Now().ToString(TEXT("%Y%m%d-%H%M%S"));
	const FString Directory = FRRRunLogWriter::GetRunLogDirectory();
	Run.FilePath = FPaths::Combine(Directory, Run.RunId + TEXT(".json"));
	for (int32 Suffix = 2; FPaths::FileExists(Run.FilePath); ++Suffix)
	{
		Run.FilePath = FPaths::Combine(Directory, FString::Printf(TEXT("%s_%d.json"), *Run.RunId, Suffix));
	}
	Run.StartTimeUtc = FDateTime::UtcNow();
	Run.BuildVersion = FApp::GetBuildVersion();
	Run.BuildConfiguration = LexToString(FApp::GetBuildConfiguration());
	Run.Outcome = TEXT("in_progress");
	bRunOpen = true;
	UE_LOG(LogRogueRigs, Log, TEXT("RunLog: run %s started (file %s)."), *Run.RunId, *Run.FilePath);
	WriteRunFile();
}

void URRRunLogSubsystem::EndRunRecord(const FString& RunOutcome)
{
	if (!bRunOpen)
	{
		return;
	}
	bRunOpen = false;
	Run.bEnded = true;
	Run.EndTimeUtc = FDateTime::UtcNow();
	Run.Outcome = RunOutcome;
}

void URRRunLogSubsystem::WriteRunFile() const
{
	// Automation tests end levels in throwaway worlds: no file.
	if (GIsAutomationTesting)
	{
		return;
	}
	FRRRunLogWriter::WriteFile(Run);
}

void URRRunLogSubsystem::CollectEventsMet(const URRRunFlowSubsystem* RunFlow, TArray<FRRRunLogEvent>& OutEvents) const
{
	OutEvents.Reset();
	if (!RunFlow)
	{
		return;
	}
	if (const ARRConsole* Console = RunFlow->GetConsole())
	{
		OutEvents.Add({ TEXT("console"), INDEX_NONE, Console->IsComplete() ? TEXT("hacked") : TEXT("not_hacked") });
	}
	if (const ARRBonusChest* Chest = RunFlow->GetBonusChest())
	{
		OutEvents.Add({ TEXT("bonus_chest"), INDEX_NONE, Chest->IsComplete() ? TEXT("opened") : TEXT("not_opened") });
	}
	if (const ARREnemySpawner* Spawner = RunFlow->GetSpawner())
	{
		for (int32 Index = 0; Index < Spawner->GetHordeCount(); ++Index)
		{
			const ERRHordeState State = Spawner->GetHordeState(Index);
			if (State != ERRHordeState::Pending)
			{
				OutEvents.Add({ TEXT("horde"), Index, StaticEnum<ERRHordeState>()->GetNameStringByValue(static_cast<int64>(State)).ToLower() });
			}
		}
	}
}

void URRRunLogSubsystem::HandleMinerDeath(AActor* DeathInstigator)
{
	HandleLevelEnded(TrackedWorld.Get(), ERRRunLevelOutcome::Death);
}

void URRRunLogSubsystem::HandleShuttleLanded(ARRShuttle* Shuttle)
{
	bShuttleLanded = true;
	if (const URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get())
	{
		LastCarriedOre = RunFlow->GetCarriedOre();
	}
}

void URRRunLogSubsystem::HandleOreChanged(int32 CarriedOre, int32 SecuredOre)
{
	if (bShuttleLanded && !bLevelEnded && CarriedOre > LastCarriedOre)
	{
		OreInRiskWindow += CarriedOre - LastCarriedOre;
	}
	LastCarriedOre = CarriedOre;
}

void URRRunLogSubsystem::HandleRunReset()
{
	if (bRunOpen)
	{
		EndRunRecord(TEXT("abandoned"));
		WriteRunFile();
	}
}
