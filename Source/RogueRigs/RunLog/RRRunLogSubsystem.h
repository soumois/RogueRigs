// Rogue Rigs - run log: per-level counters (terrain, damage, kills, upgrades, XP) fed by game events, a level entry appended at
// each level end and the run JSON file in Saved/RunLogs/ (run-log-schema).

#pragma once

#include "CoreMinimal.h"
#include "RunLog/RRRunLogTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RRRunLogSubsystem.generated.h"

class ARRArenaGrid;
class ARRMiner;
class ARRShuttle;
class URRRunFlowSubsystem;
enum class ERRCellType : uint8;
enum class ERRTerrainDamageSource : uint8;
struct FRRDamageEventInfo;
struct FRRUpgradeCard;

/**
 * Run log collector (sys-run-log). Lives on the game instance, so it survives the map travel between levels; its counters
 * cover the current level only. Level start = the first miner BeginPlay of a world (ARRMiner::BeginPlay, after the run snapshot
 * replay, so replayed cards are not counted): counters reset, the world's grid, the miner's upgrade and progression components
 * bound. URRRunTransitionSubsystem::OnLevelReady (after a travel) restamps the start time to the end of the loading hold.
 * Sources (events only, no polling): ARRArenaGrid::OnCellDestroyed (terrain), URRHealthAttributeSet::OnAnyDamageReceived
 * (damage dealt, kills, damage taken), URRUpgradeComponent::OnUpgradeApplied (cards), URRProgressionComponent::OnXPGained (XP).
 * Writer (task-212-02): a run record opens at the start of a run's level 1 (Saved/RunLogs/<yyyyMMdd-HHmmss>.json created at once);
 * at each level end (HandleLevelEnded from URRRunFlowSubsystem::EndRun, or the miner's OnDeath) the counters are snapshotted
 * with the outcome, the level time, the shuttle time left, the risk-window ore and the alert events met, appended and the file
 * rewritten. The run ends on a death, a missed shuttle or the last level's extraction; URRRunSubsystem::OnRunReset closes an
 * unfinished run as abandoned. No file is written during automation tests. Debug: rr.RunLog.Dump (not in shipping).
 */
UCLASS()
class ROGUERIGS_API URRRunLogSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** The subsystem of WorldContext's game instance; null without a world or a game instance (some test worlds). */
	static URRRunLogSubsystem* Get(const UObject* WorldContext);

	/** Binds the global damage event and the transition subsystem's OnLevelReady. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Unbinds the global damage event. */
	virtual void Deinitialize() override;

	/**
	 * Level start, called at the end of ARRMiner::BeginPlay: for the first miner of a world, resets the counters, records the
	 * run level and layout and binds the world's grid and the miner's components. Later miners of the same world are ignored.
	 */
	void HandleMinerBeginPlay(ARRMiner* Miner);

	/**
	 * Next run level started in the same world (task-318-01): the level start of HandleMinerBeginPlay for the same miner again
	 * (counters reset, new run level and layout recorded; bindings are unique, so nothing is bound twice).
	 */
	void HandleLevelStartedInPlace(ARRMiner* Miner);

	/** Copy of the counters of the current level (what the writer serializes at level end). */
	UFUNCTION(BlueprintPure, Category = "Run Log")
	FRRRunLevelStats GetCurrentLevelStats() const;

	/**
	 * Level end (extracted, death or missed shuttle) of World: appends the level entry to the run record and rewrites the run
	 * file; a death, a missed shuttle or the last level's extraction (or no run in progress) also ends the run. Called by
	 * URRRunFlowSubsystem::EndRun before the intermission, and by the miner's OnDeath. Once per level; other worlds ignored.
	 */
	void HandleLevelEnded(UWorld* World, ERRRunLevelOutcome Outcome);

	/** The current (or last) run record. */
	const FRRRunLogRecord& GetRunRecord() const { return Run; }

	/** Run-log name of a level outcome (extracted, death, missed_shuttle). */
	static FString GetLevelOutcomeName(ERRRunLevelOutcome Outcome);

	/** Logs every counter of the current level on LogRogueRigs (rr.RunLog.Dump); Context prefixes each line. */
	void LogCurrentLevelStats(const FString& Context) const;

	/** Run-log id of a card: "<Stat>_<Tier>@<Target>" (Target Player, AllWeapons or the weapon class name), "NewWeapon@<class>". */
	static FString MakeCardId(const FRRUpgradeCard& Card);

	/** Run-log name of a terrain source (barrel, kamikaze, drone, shot, extractor). */
	static FString GetTerrainSourceName(ERRTerrainDamageSource Source);

	/** Run-log name of a block type (rock, ore, heal_ore, special_ore, barrel; other types lower case). */
	static FString GetCellTypeName(ERRCellType Type);

protected:
	/** Opens a new run record (id and file name from the local time) and writes its file; closes an open one as abandoned first. */
	void BeginRunRecord();

	/** Ends the run record with RunOutcome (won, death, missed_shuttle, abandoned) and stamps the end time; the caller writes. */
	void EndRunRecord(const FString& RunOutcome);

	/** Writes the run file (skipped during automation tests). */
	void WriteRunFile() const;

	/** Alert events of the level that happened (console, bonus chest, hordes) with their outcome now. */
	void CollectEventsMet(const URRRunFlowSubsystem* RunFlow, TArray<FRRRunLogEvent>& OutEvents) const;

	/** Miner OnDeath: the level ends with a death. */
	UFUNCTION()
	void HandleMinerDeath(AActor* DeathInstigator);

	/** Run flow OnShuttleLanded: the risk window opens. */
	UFUNCTION()
	void HandleShuttleLanded(ARRShuttle* Shuttle);

	/** Run flow OnOreChanged: carried ore gained during the risk window is counted. */
	UFUNCTION()
	void HandleOreChanged(int32 CarriedOre, int32 SecuredOre);

	/** URRRunSubsystem::OnRunReset: an unfinished run record ends as abandoned (a restart starts a new file). */
	UFUNCTION()
	void HandleRunReset();

	/** Clears the counters and stamps the level start (index, layout, world time) for World. */
	void ResetForLevel(UWorld* World);

	/** Grid event: one destroyed block, counted by source and by type. */
	UFUNCTION()
	void HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source);

	/** Upgrade event: appends the card id to the picked list. */
	UFUNCTION()
	void HandleUpgradeApplied(const FRRUpgradeCard& Card);

	/** Progression event: adds the gained XP. */
	UFUNCTION()
	void HandleXPGained(float Amount);

	/** Transition event: the level's gameplay starts now (end of the loading hold); restamps LevelStartWorldSeconds. */
	UFUNCTION()
	void HandleLevelReady(int32 LevelIndex, const FString& LayoutName);

	/** Global damage event: damage dealt per weapon and kills (robot targets), damage taken (miner target), tracked world only. */
	void HandleAnyDamage(const FRRDamageEventInfo& Info);

	/** Weapon credited for a hit: the instigator or causer weapon; a turret's hits go to its owning launcher. Null for non-weapon hits. */
	static const AActor* FindCreditedWeapon(const FRRDamageEventInfo& Info);

	/** World whose level the counters describe (the world of the first miner seen). */
	TWeakObjectPtr<UWorld> TrackedWorld;

	/** Grid bound for terrain events in the tracked world. */
	TWeakObjectPtr<ARRArenaGrid> BoundGrid;

	/** Handle of the binding on URRHealthAttributeSet::OnAnyDamageReceived. */
	FDelegateHandle AnyDamageHandle;

	/** Counters of the current level. Internal maps below are keyed by FName (no string build per hit) and copied out by GetCurrentLevelStats. */
	FRRRunLevelStats Stats;

	/** Damage dealt per weapon class name (internal, FName keys). */
	TMap<FName, float> DamageDealtByWeapon;

	/** Kills per robot type (internal, FName keys). */
	TMap<FName, int32> KillsByRobotType;

	/** Damage taken per source (internal, FName keys). */
	TMap<FName, float> DamageTakenBySource;

	/** Run record being written (or the last one once ended). */
	FRRRunLogRecord Run;

	/** True while Run is open (started, not ended). */
	bool bRunOpen = false;

	/** True once the tracked level ended (its entry is in Run); later ends of the same world are ignored. */
	bool bLevelEnded = false;

	/** True once the shuttle of the tracked level landed (risk window open). */
	bool bShuttleLanded = false;

	/** Carried ore at the last OnOreChanged (deltas above it count in the risk window). */
	int32 LastCarriedOre = 0;

	/** Carried ore gained since the shuttle landed. */
	int32 OreInRiskWindow = 0;

	/** Run flow bound for the tracked level (shuttle landing, ore). */
	TWeakObjectPtr<URRRunFlowSubsystem> BoundRunFlow;
};
