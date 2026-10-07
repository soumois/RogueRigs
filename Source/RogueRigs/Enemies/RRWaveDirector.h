// Rogue Rigs - wave director of the enemy spawner: data (phases, pack types, hordes) and the world-free scheduler that turns run time and alert into spawn orders.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "RRWaveDirector.generated.h"

class ARREnemy;

/** What starts a horde: the spawner run time or the alert bar of the run flow. */
UENUM(BlueprintType)
enum class ERRHordeTrigger : uint8
{
	/** The front starts at FRRDirectorHorde::TriggerTime seconds of spawning; the lull starts LullSeconds before. */
	RunTime,
	/** The lull starts when the run-flow alert reaches FRRDirectorHorde::TriggerAlert (0..1); the front follows LullSeconds later. */
	Alert
};

/** Progress of one horde. */
UENUM(BlueprintType)
enum class ERRHordeState : uint8
{
	/** Not triggered yet. */
	Pending,
	/** Lull: nothing spawns, banner not shown yet. */
	Lull,
	/** Banner shown ("HORDE INCOMING!"), still nothing spawns until the front. */
	Announced,
	/** The front is spawning, over SpreadSeconds. */
	Spawning,
	/** Over. */
	Done
};

/** One group of a pack or horde: a robot class and how many of it. */
USTRUCT(BlueprintType)
struct FRRDirectorPackMember
{
	GENERATED_BODY()

	/**
	 * Robot class. A native class (ARRChaser...) resolves to the spawner WeightedEnemyClasses entry (or EnemyClass) whose nearest
	 * native class is this one (the Blueprint with the mesh); without such an entry the class itself is spawned. None = ignored.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TSubclassOf<ARREnemy> RobotClass;

	/** Fewest robots of this group. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0))
	int32 CountMin = 1;

	/** Most robots of this group (below CountMin it counts as CountMin). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0))
	int32 CountMax = 1;

	/** True: these robots take the cells nearest to the view (front of the pack, spawned first). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	bool bFront = false;

	/**
	 * True (and bFront false): these robots never lead the pack; they are placed at random in the back half of the mixed members
	 * (the middle or the back of the pack), e.g. 1-2 shooters riding in a chaser pack.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	bool bBehind = false;
};

/** A pack type: its member groups, all spawned at once on adjacent cells of one side of the view. */
USTRUCT(BlueprintType)
struct FRRDirectorPackType
{
	GENERATED_BODY()

	/** Name the phase weights refer to (Chasers, Shooters...). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	FName Name;

	/** Groups of the pack (front groups first in space and time). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TArray<FRRDirectorPackMember> Members;
};

/** Draw weight of a pack type during a phase. */
USTRUCT(BlueprintType)
struct FRRDirectorPackWeight
{
	GENERATED_BODY()

	/** FRRDirectorPackType::Name; an unknown name is ignored. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	FName PackType;

	/** Relative weight; 0 or less = never drawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0))
	float Weight = 1.f;
};

/** A stretch of the run, from StartTime to the next phase: lone robots and/or waves of packs. */
USTRUCT(BlueprintType)
struct FRRDirectorPhase
{
	GENERATED_BODY()

	/** Name shown in logs and sent with OnDirectorPhaseChanged. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	FName Name;

	/** Seconds of spawning at which the phase starts (phases sorted by start time). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float StartTime = 0.f;

	/** Seconds between two lone robots, the first one as the phase starts. 0 = none. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float SingleInterval = 0.f;

	/** Class of the lone robots (resolved like a pack member). None = the spawner weighted mix (PickEnemyClass). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TSubclassOf<ARREnemy> SingleClass;

	/** True: a wave comes as the phase starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	bool bWaveOnStart = false;

	/**
	 * True: entering this phase keeps the wave timer of the previous one instead of restarting it, so a phase that only changes
	 * the pack mix (FlowShields) does not shift the wave rhythm. Set its WaveIntervalStart to the previous phase interval at its start.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	bool bKeepWaveTimer = false;

	/** Seconds between two waves at the phase start. 0 = no timed wave (only the start wave). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float WaveIntervalStart = 0.f;

	/** Seconds between two waves after WaveIntervalRampSeconds in the phase (linear in between). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float WaveIntervalEnd = 0.f;

	/** Seconds over which the wave interval goes from WaveIntervalStart to WaveIntervalEnd. 0 = WaveIntervalEnd at once. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float WaveIntervalRampSeconds = 0.f;

	/** Fewest packs per wave, each from a distinct side of the view (1 to 4). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 1, ClampMax = 4))
	int32 SidesMin = 1;

	/** Most packs per wave (1 to 4; below SidesMin it counts as SidesMin). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 1, ClampMax = 4))
	int32 SidesMax = 1;

	/** Pack types drawn for each pack of a wave, with their weights. Empty = no wave. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TArray<FRRDirectorPackWeight> PackWeights;
};

/** An announced horde: lull, banner, then a front of robots along one side of the view. */
USTRUCT(BlueprintType)
struct FRRDirectorHorde
{
	GENERATED_BODY()

	/** RunTime trigger: seconds of spawning at which the front starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float TriggerTime = 95.f;

	/** Alert trigger: alert (0..1) at which the lull starts. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, ClampMax = 1))
	float TriggerAlert = 0.4f;

	/** Seconds without any spawn before the front. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float LullSeconds = 5.f;

	/** Seconds between the banner (OnHordeIncoming) and the front; at most LullSeconds. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float WarningSeconds = 4.f;

	/** Seconds over which the front spawns (front rows and listed members first). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0, Units = "s"))
	float SpreadSeconds = 2.f;

	/** Fewest robots of the front (before the clamp to the free slots). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 1))
	int32 SizeMin = 40;

	/** Most robots of the front (below SizeMin it counts as SizeMin). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 1))
	int32 SizeMax = 60;

	/** Listed groups (front groups take the nearest cells); the rest of the size is FillClass. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TArray<FRRDirectorPackMember> Members;

	/** Class filling the front up to its size (the carpet behind). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TSubclassOf<ARREnemy> FillClass;
};

/**
 * Overtime (difficulty, owner 2026-10-06 third test, provisional): from StartTime seconds of spawning (level time), a horde
 * (Horde template) whose lull starts every HordeInterval seconds (StartTime, StartTime + HordeInterval...), and the wave interval
 * multiplied by WaveIntervalMultiplier once per overtime step begun (x0.5 from StartTime, x0.25 from StartTime + StepSeconds...).
 */
USTRUCT(BlueprintType)
struct FRRDirectorOvertime
{
	GENERATED_BODY()

	/** True: overtime starts at StartTime. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director|Overtime")
	bool bEnabled = true;

	/** Seconds of spawning at which overtime starts (210 = 3:30). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director|Overtime", meta = (ClampMin = 0, Units = "s"))
	float StartTime = 210.f;

	/** Seconds between the lull starts of two overtime hordes (30). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director|Overtime", meta = (ClampMin = 1, Units = "s"))
	float HordeInterval = 30.f;

	/** Length of one overtime step (60 s: one overtime minute). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director|Overtime", meta = (ClampMin = 1, Units = "s"))
	float StepSeconds = 60.f;

	/** Factor applied to the wave interval per overtime step begun (0.5: halved at each overtime minute). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director|Overtime", meta = (ClampMin = 0.01, ClampMax = 1))
	float WaveIntervalMultiplier = 0.5f;

	/** Shortest wave interval overtime may reach (s); 0 = no floor. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director|Overtime", meta = (ClampMin = 0, Units = "s"))
	float MinWaveInterval = 0.f;

	/** Template of every overtime horde (its TriggerTime and TriggerAlert are unused). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director|Overtime")
	FRRDirectorHorde Horde;
};

/** Every tunable of the wave director (edited on the spawner, ARREnemySpawner::DirectorConfig). */
USTRUCT(BlueprintType)
struct FRRWaveDirectorConfig
{
	GENERATED_BODY()

	/** True: the director drives the spawner. False: the older trickle-then-packs timers (SpawnInterval, TrickleDuration...). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	bool bEnabled = true;

	/** What starts the hordes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	ERRHordeTrigger HordeTrigger = ERRHordeTrigger::RunTime;

	/** Phases, sorted by StartTime. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TArray<FRRDirectorPhase> Phases;

	/** Pack types the phases draw. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TArray<FRRDirectorPackType> PackTypes;

	/** Hordes, in trigger order (one at a time; a later one waits for the current one to end). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TArray<FRRDirectorHorde> Hordes;

	/**
	 * Shooter counter (owner 2026-10-06, 4 since the fourth test, provisional): at most this many shooters (ARRShooter and subclasses) alive when a pack
	 * is drawn; the shooters of a pack beyond it become ShooterCapReplacement robots in the same places. Shooters fire from afar
	 * and pile up while the miner backs away firing. Hordes and lone robots carry no shooters.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director", meta = (ClampMin = 0))
	int32 ShooterMaxAlive = 4;

	/** Robot that takes the place of a shooter drawn at the cap; none = ARRChaser (resolved like any member class). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	TSubclassOf<ARREnemy> ShooterCapReplacement;

	/** Overtime: hordes every 30 s and a shrinking wave interval from 3:30 of level time. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Director")
	FRRDirectorOvertime Overtime;

	/** Index of the pack type called Name; INDEX_NONE when none. */
	int32 FindPackType(FName Name) const;

	/**
	 * Horde of index Index: Hordes[Index], or the overtime template for Index >= Hordes.Num() while overtime is enabled (overtime
	 * horde Index - Hordes.Num()). Null otherwise.
	 */
	const FRRDirectorHorde* FindHorde(int32 Index) const;

	/** True when Index is an overtime horde. */
	bool IsOvertimeHorde(int32 Index) const { return Overtime.bEnabled && Index >= Hordes.Num(); }

	/** Seconds of spawning at which the front of horde Index starts (RunTime trigger or overtime); -1 when none. */
	float GetHordeFrontTime(int32 Index) const;

	/** Provisional defaults (owner 2026-10-06, enemies-index): a lone chaser from 1 s, first pack at 15 s, multi-side flow (packs of 6-10 until 90 s, then 0-1 shooter riding behind), hordes at 95 s and 175 s. */
	static FRRWaveDirectorConfig MakeDefault();
};

/** One order of the scheduler for the spawner. */
struct FRRDirectorAction
{
	/** Kind of order. */
	enum class EType : uint8
	{
		/** Index = new phase. */
		PhaseChanged,
		/** Spawn one robot of SingleClass (none = weighted mix). */
		SpawnSingle,
		/** Spawn one pack per PackTypes entry, each from a distinct side. */
		SpawnWave,
		/** Index = horde whose lull starts. */
		HordeLull,
		/** Index = horde announced; Seconds = seconds until its front. */
		HordeAnnounced,
		/** Index = horde whose front starts spawning. */
		HordeStarted,
		/** Index = horde whose front is fully spawned. */
		HordeFinished,
		/** Overtime starts (once). */
		OvertimeStarted
	};

	/** Kind of order. */
	EType Type = EType::PhaseChanged;

	/** Phase or horde index. */
	int32 Index = INDEX_NONE;

	/** SpawnSingle: class to resolve (none = weighted mix). */
	TSubclassOf<ARREnemy> SingleClass;

	/** SpawnWave: pack type index of each pack. */
	TArray<int32, TInlineAllocator<4>> PackTypes;

	/** HordeAnnounced: seconds until the front. */
	float Seconds = 0.f;
};

/**
 * World-free scheduler of the wave director: Advance adds the frame time and returns the orders due (phase changes, lone robots,
 * waves, horde steps). While a horde is in its lull, announced or spawning, no lone robot and no wave is ordered and the wave timer
 * stands; it restarts from zero when the horde ends. The spawner owns one and executes the orders.
 */
struct ROGUERIGS_API FRRWaveDirector
{
	/** Back to the start of a run (time 0, no phase, every horde pending). */
	void Reset();

	/** Advances DeltaSeconds of spawning with the current Alert (0..1) and appends the orders due to OutActions. */
	void Advance(const FRRWaveDirectorConfig& Config, float DeltaSeconds, float Alert, FRandomStream& Stream, TArray<FRRDirectorAction>& OutActions);

	/** Seconds of spawning since Reset. */
	float GetRunTime() const { return RunTime; }

	/** Current phase index; INDEX_NONE before the first phase starts. */
	int32 GetPhaseIndex() const { return PhaseIndex; }

	/** State of horde Index (Pending when out of range or not reached yet). */
	ERRHordeState GetHordeState(int32 Index) const;

	/** Horde in its lull, announced or spawning; INDEX_NONE when none. */
	int32 GetActiveHorde() const { return ActiveHorde; }

	/** Spawning horde: fraction (0..1) of its front that should exist by now. 0 when no horde spawns. */
	float GetHordeSpawnFraction(const FRRWaveDirectorConfig& Config) const;

	/** Seconds until the front of the active horde (0 once spawning); -1 without an active horde. */
	float GetSecondsUntilHordeFront(const FRRWaveDirectorConfig& Config) const;

	/** True while a horde silences the lone robots and waves. */
	bool IsQuiet() const { return ActiveHorde != INDEX_NONE; }

	/** Wave interval of Phase at PhaseSeconds into it (linear ramp from WaveIntervalStart to WaveIntervalEnd). */
	static float GetWaveInterval(const FRRDirectorPhase& Phase, float PhaseSeconds);

	/** True once RunTime reached Config.Overtime.StartTime (overtime enabled). */
	bool IsInOvertime(const FRRWaveDirectorConfig& Config) const;

	/** Factor on the wave interval at RunTime: 1 before overtime, WaveIntervalMultiplier ^ (overtime steps begun) after. */
	float GetOvertimeWaveFactor(const FRRWaveDirectorConfig& Config) const;

	/** Wave interval used now (current phase ramp times the overtime factor, floored at MinWaveInterval); 0 without timed waves. */
	float GetCurrentWaveInterval(const FRRWaveDirectorConfig& Config) const;

protected:
	/** Steps the hordes (trigger, lull, banner, front, end). */
	void AdvanceHordes(const FRRWaveDirectorConfig& Config, float DeltaSeconds, float Alert, TArray<FRRDirectorAction>& OutActions);

	/** Draws the pack types of one wave of Phase; empty when the phase has no valid weight. */
	static void DrawWave(const FRRWaveDirectorConfig& Config, const FRRDirectorPhase& Phase, FRandomStream& Stream, FRRDirectorAction& OutWave);

	/** Seconds of spawning since Reset. */
	float RunTime = 0.f;

	/** Current phase; INDEX_NONE before the first. */
	int32 PhaseIndex = INDEX_NONE;

	/** Seconds accumulated toward the next lone robot. */
	float SingleTimer = 0.f;

	/** Seconds accumulated toward the next wave. */
	float WaveTimer = 0.f;

	/** State of each horde (grown to the config size). */
	TArray<ERRHordeState> HordeStates;

	/** Horde in progress; INDEX_NONE when none. */
	int32 ActiveHorde = INDEX_NONE;

	/** Seconds since the lull of ActiveHorde started. */
	float HordeClock = 0.f;

	/** True once OvertimeStarted was ordered. */
	bool bOvertimeAnnounced = false;
};
