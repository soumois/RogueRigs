// Rogue Rigs - run log collector: per-level counters (terrain, damage, kills, upgrades, XP) fed by game events, read by the
// run log writer at level end (run-log-schema).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RRRunLogSubsystem.generated.h"

class ARRArenaGrid;
class ARRMiner;
enum class ERRCellType : uint8;
enum class ERRTerrainDamageSource : uint8;
struct FRRDamageEventInfo;
struct FRRUpgradeCard;

/**
 * Counters of one level of a run (run-log-schema), as collected so far. Keys are run-log values: terrain sources
 * barrel / kamikaze / drone / shot, terrain types rock / ore / heal_ore / special_ore / barrel, weapon class names, robot types
 * (EnemyStats.csv row: Chaser, Kamikaze...), kill sources weapon / explosion / other, damage-taken sources robot type /
 * explosion / other. Copy taken by URRRunLogSubsystem::GetCurrentLevelStats; the writer serializes it.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRRunLevelStats
{
	GENERATED_BODY()

	/** Run level index (1..N) of these counters; 0 when no run was in progress at level start. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	int32 LevelIndex = 0;

	/** Layout name of the level (URRRunSubsystem::GetCurrentLayoutName at level start). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	FString LayoutName;

	/** World time (seconds, pauses excluded) at which the level started: first miner BeginPlay, then OnLevelReady after a travel. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	double LevelStartWorldSeconds = 0.0;

	/** Destroyed blocks per destruction source (barrel, kamikaze, drone, shot). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> TerrainDestroyedBySource;

	/** Destroyed blocks per block type (rock, ore, heal_ore, special_ore, barrel). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> TerrainDestroyedByType;

	/** Raw damage dealt to robots per weapon class name (turret shots count for their launcher). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, float> DamageDealtByWeapon;

	/** Robots killed per robot type (EnemyStats.csv row name, class name when the row is empty). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> KillsByRobotType;

	/** Robots killed per kill source (weapon, explosion, other). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, int32> KillsBySource;

	/** Upgrade cards picked during the level, in pick order (card ids, see URRRunLogSubsystem::MakeCardId). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TArray<FString> UpgradesPicked;

	/** Raw damage taken by the miner per source (robot type, explosion, other). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	TMap<FString, float> DamageTakenBySource;

	/** XP gained by the miner during the level (after the XPGain multiplier; cheat grants included). */
	UPROPERTY(BlueprintReadOnly, Category = "Run Log")
	float XPCollected = 0.f;
};

/**
 * Run log collector (sys-run-log). Lives on the game instance, so it survives the map travel between levels; its counters
 * cover the current level only. Level start = the first miner BeginPlay of a world (ARRMiner::BeginPlay, after the run snapshot
 * replay, so replayed cards are not counted): counters reset, the world's grid, the miner's upgrade and progression components
 * bound. URRRunTransitionSubsystem::OnLevelReady (after a travel) restamps the start time to the end of the loading hold.
 * Sources (events only, no polling): ARRArenaGrid::OnCellDestroyed (terrain), URRHealthAttributeSet::OnAnyDamageReceived
 * (damage dealt, kills, damage taken), URRUpgradeComponent::OnUpgradeApplied (cards), URRProgressionComponent::OnXPGained (XP).
 * The writer (task-212-02) snapshots GetCurrentLevelStats at level end. Debug: rr.RunLog.Dump (not in shipping).
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

	/** Copy of the counters of the current level (what the writer serializes at level end). */
	UFUNCTION(BlueprintPure, Category = "Run Log")
	FRRRunLevelStats GetCurrentLevelStats() const;

	/** Logs every counter of the current level on LogRogueRigs (rr.RunLog.Dump); Context prefixes each line. */
	void LogCurrentLevelStats(const FString& Context) const;

	/** Run-log id of a card: "<Stat>_<Tier>@<Target>" (Target Player, AllWeapons or the weapon class name), "NewWeapon@<class>". */
	static FString MakeCardId(const FRRUpgradeCard& Card);

	/** Run-log name of a terrain source (barrel, kamikaze, drone, shot). */
	static FString GetTerrainSourceName(ERRTerrainDamageSource Source);

	/** Run-log name of a block type (rock, ore, heal_ore, special_ore, barrel; other types lower case). */
	static FString GetCellTypeName(ERRCellType Type);

protected:
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
};
