// Rogue Rigs - wave director of the enemy spawner: data (phases, pack types, hordes) and the world-free scheduler that turns run time and alert into spawn orders.

#include "Enemies/RRWaveDirector.h"

#include "Enemies/RRChaser.h"
#include "Enemies/RRHeavy.h"
#include "Enemies/RRKamikaze.h"
#include "Enemies/RRShieldChaser.h"
#include "Enemies/RRShooter.h"

namespace RRWaveDirectorDefaults
{
	FRRDirectorPackMember Member(TSubclassOf<ARREnemy> RobotClass, int32 CountMin, int32 CountMax, bool bFront = false)
	{
		FRRDirectorPackMember Result;
		Result.RobotClass = RobotClass;
		Result.CountMin = CountMin;
		Result.CountMax = CountMax;
		Result.bFront = bFront;
		return Result;
	}

	FRRDirectorPackMember Behind(TSubclassOf<ARREnemy> RobotClass, int32 CountMin, int32 CountMax)
	{
		FRRDirectorPackMember Result = Member(RobotClass, CountMin, CountMax);
		Result.bBehind = true;
		return Result;
	}

	FRRDirectorPackWeight Weight(FName PackType, float Value)
	{
		FRRDirectorPackWeight Result;
		Result.PackType = PackType;
		Result.Weight = Value;
		return Result;
	}
}

int32 FRRWaveDirectorConfig::FindPackType(FName Name) const
{
	return PackTypes.IndexOfByPredicate([Name](const FRRDirectorPackType& Type) { return Type.Name == Name; });
}

FRRWaveDirectorConfig FRRWaveDirectorConfig::MakeDefault()
{
	using namespace RRWaveDirectorDefaults;
	FRRWaveDirectorConfig Config;

	// Pack types: no shooter-only pack; 1-2 shooters ride behind the front of every flow pack (about 10 % of the robots);
	// kamikazes ride with the chasers; heavies lead a chaser pack.
	FRRDirectorPackType& FirstChasers = Config.PackTypes.AddDefaulted_GetRef();
	FirstChasers.Name = TEXT("FirstChasers");
	FirstChasers.Members = { Member(ARRChaser::StaticClass(), 6, 8) };
	FRRDirectorPackType& Chasers = Config.PackTypes.AddDefaulted_GetRef();
	Chasers.Name = TEXT("Chasers");
	Chasers.Members = { Member(ARRChaser::StaticClass(), 8, 14), Member(ARRKamikaze::StaticClass(), 2, 3), Behind(ARRShooter::StaticClass(), 1, 2) };
	FRRDirectorPackType& Heavies = Config.PackTypes.AddDefaulted_GetRef();
	Heavies.Name = TEXT("Heavies");
	Heavies.Members = { Member(ARRHeavy::StaticClass(), 2, 3, /*bFront*/ true), Member(ARRChaser::StaticClass(), 8, 12), Behind(ARRShooter::StaticClass(), 1, 2) };
	// Chasers after 1:00 (enemy-chaser shield variant): a quarter of the pack chasers carry a shield (2-4 of 8-16).
	FRRDirectorPackType& ShieldChasers = Config.PackTypes.AddDefaulted_GetRef();
	ShieldChasers.Name = TEXT("ShieldChasers");
	ShieldChasers.Members = { Member(ARRChaser::StaticClass(), 6, 12), Member(ARRShieldChaser::StaticClass(), 2, 4), Member(ARRKamikaze::StaticClass(), 2, 3),
		Behind(ARRShooter::StaticClass(), 1, 2) };

	// Phases (no calm): from 1 s a lone chaser every 2 s; 15 s first pack from one side; from 21 s waves of 1-2 packs from
	// distinct sides, the interval shrinking from 6 s to 3.5 s over 3 min 30; from 60 s the chaser packs carry shield chasers;
	// from 90 s waves of 2-3 packs. The later phases only change the mix or the sides: they keep the wave timer and the ramp.
	FRRDirectorPhase& Isolated = Config.Phases.AddDefaulted_GetRef();
	Isolated.Name = TEXT("Isolated");
	Isolated.StartTime = 1.f;
	Isolated.SingleInterval = 2.f;
	Isolated.SingleClass = ARRChaser::StaticClass();
	FRRDirectorPhase& FirstPack = Config.Phases.AddDefaulted_GetRef();
	FirstPack.Name = TEXT("FirstPack");
	FirstPack.StartTime = 15.f;
	FirstPack.bWaveOnStart = true;
	FirstPack.PackWeights = { Weight(TEXT("FirstChasers"), 1.f) };
	FRRDirectorPhase& Flow = Config.Phases.AddDefaulted_GetRef();
	Flow.Name = TEXT("Flow");
	Flow.StartTime = 21.f;
	Flow.bWaveOnStart = true;
	Flow.WaveIntervalStart = 6.f;
	Flow.WaveIntervalEnd = 3.5f;
	Flow.WaveIntervalRampSeconds = 210.f;
	Flow.SidesMin = 1;
	Flow.SidesMax = 2;
	Flow.PackWeights = { Weight(TEXT("Chasers"), 92.f), Weight(TEXT("Heavies"), 8.f) };

	const FRRDirectorPhase FlowCopy = Flow;
	auto ContinueFlow = [&Config, &FlowCopy](FName Name, float StartTime) -> FRRDirectorPhase&
	{
		FRRDirectorPhase& Phase = Config.Phases.Add_GetRef(FlowCopy);
		Phase.Name = Name;
		Phase.StartTime = StartTime;
		Phase.bWaveOnStart = false;
		Phase.bKeepWaveTimer = true;
		const float FlowSeconds = StartTime - FlowCopy.StartTime;
		Phase.WaveIntervalStart = FRRWaveDirector::GetWaveInterval(FlowCopy, FlowSeconds);
		Phase.WaveIntervalRampSeconds = FMath::Max(FlowCopy.WaveIntervalRampSeconds - FlowSeconds, 0.f);
		return Phase;
	};
	FRRDirectorPhase& FlowShields = ContinueFlow(TEXT("FlowShields"), 60.f);
	FlowShields.PackWeights = { Weight(TEXT("ShieldChasers"), 92.f), Weight(TEXT("Heavies"), 8.f) };
	FRRDirectorPhase& FlowWide = ContinueFlow(TEXT("FlowWide"), 90.f);
	FlowWide.SidesMin = 2;
	FlowWide.SidesMax = 3;
	FlowWide.PackWeights = { Weight(TEXT("ShieldChasers"), 92.f), Weight(TEXT("Heavies"), 8.f) };

	// Hordes: fronts at 1:35 and 2:55, heavies in front, kamikazes among them, a carpet of chasers behind.
	for (const float FrontTime : { 95.f, 175.f })
	{
		FRRDirectorHorde& Horde = Config.Hordes.AddDefaulted_GetRef();
		Horde.TriggerTime = FrontTime;
		Horde.TriggerAlert = FrontTime < 100.f ? 0.4f : 0.75f;
		Horde.Members = { Member(ARRHeavy::StaticClass(), 3, 5, /*bFront*/ true), Member(ARRKamikaze::StaticClass(), 3, 5) };
		Horde.FillClass = ARRChaser::StaticClass();
	}
	// Shooter counter (owner 2026-10-06): at most 6 alive; a pack drawn at the cap gets chasers in place of its shooters.
	Config.ShooterMaxAlive = 6;
	Config.ShooterCapReplacement = ARRChaser::StaticClass();

	return Config;
}

void FRRWaveDirector::Reset()
{
	RunTime = 0.f;
	PhaseIndex = INDEX_NONE;
	SingleTimer = 0.f;
	WaveTimer = 0.f;
	HordeStates.Reset();
	ActiveHorde = INDEX_NONE;
	HordeClock = 0.f;
}

ERRHordeState FRRWaveDirector::GetHordeState(int32 Index) const
{
	return HordeStates.IsValidIndex(Index) ? HordeStates[Index] : ERRHordeState::Pending;
}

float FRRWaveDirector::GetWaveInterval(const FRRDirectorPhase& Phase, float PhaseSeconds)
{
	if (Phase.WaveIntervalStart <= 0.f)
	{
		return 0.f;
	}
	const float Alpha = Phase.WaveIntervalRampSeconds > 0.f ? FMath::Clamp(PhaseSeconds / Phase.WaveIntervalRampSeconds, 0.f, 1.f) : 1.f;
	const float End = Phase.WaveIntervalEnd > 0.f ? Phase.WaveIntervalEnd : Phase.WaveIntervalStart;
	return FMath::Lerp(Phase.WaveIntervalStart, End, Alpha);
}

float FRRWaveDirector::GetHordeSpawnFraction(const FRRWaveDirectorConfig& Config) const
{
	if (!Config.Hordes.IsValidIndex(ActiveHorde) || GetHordeState(ActiveHorde) != ERRHordeState::Spawning)
	{
		return 0.f;
	}
	const FRRDirectorHorde& Horde = Config.Hordes[ActiveHorde];
	const float Lull = FMath::Max(Horde.LullSeconds, 0.f);
	return Horde.SpreadSeconds > 0.f ? FMath::Clamp((HordeClock - Lull) / Horde.SpreadSeconds, 0.f, 1.f) : 1.f;
}

float FRRWaveDirector::GetSecondsUntilHordeFront(const FRRWaveDirectorConfig& Config) const
{
	if (!Config.Hordes.IsValidIndex(ActiveHorde))
	{
		return -1.f;
	}
	return FMath::Max(FMath::Max(Config.Hordes[ActiveHorde].LullSeconds, 0.f) - HordeClock, 0.f);
}

void FRRWaveDirector::Advance(const FRRWaveDirectorConfig& Config, float DeltaSeconds, float Alert, FRandomStream& Stream, TArray<FRRDirectorAction>& OutActions)
{
	while (HordeStates.Num() < Config.Hordes.Num())
	{
		HordeStates.Add(ERRHordeState::Pending);
	}
	RunTime += DeltaSeconds;

	// Phase: the last one whose start time is reached.
	int32 NewPhase = INDEX_NONE;
	for (int32 Index = 0; Index < Config.Phases.Num(); ++Index)
	{
		if (Config.Phases[Index].StartTime <= RunTime)
		{
			NewPhase = Index;
		}
	}
	bool bPhaseStarted = false;
	if (NewPhase != PhaseIndex)
	{
		PhaseIndex = NewPhase;
		bPhaseStarted = PhaseIndex != INDEX_NONE;
		SingleTimer = 0.f;
		if (!Config.Phases.IsValidIndex(PhaseIndex) || !Config.Phases[PhaseIndex].bKeepWaveTimer)
		{
			WaveTimer = 0.f;
		}
		FRRDirectorAction& Changed = OutActions.AddDefaulted_GetRef();
		Changed.Type = FRRDirectorAction::EType::PhaseChanged;
		Changed.Index = PhaseIndex;
	}

	AdvanceHordes(Config, DeltaSeconds, Alert, OutActions);
	if (!Config.Phases.IsValidIndex(PhaseIndex) || IsQuiet())
	{
		return;
	}
	const FRRDirectorPhase& Phase = Config.Phases[PhaseIndex];

	// Lone robots: one as the phase starts, then every SingleInterval.
	if (Phase.SingleInterval > 0.f)
	{
		bool bSingleDue = bPhaseStarted;
		if (!bPhaseStarted)
		{
			SingleTimer += DeltaSeconds;
			if (SingleTimer >= Phase.SingleInterval)
			{
				SingleTimer = FMath::Fmod(SingleTimer, Phase.SingleInterval);
				bSingleDue = true;
			}
		}
		if (bSingleDue)
		{
			FRRDirectorAction& Single = OutActions.AddDefaulted_GetRef();
			Single.Type = FRRDirectorAction::EType::SpawnSingle;
			Single.Index = PhaseIndex;
			Single.SingleClass = Phase.SingleClass;
		}
	}

	// Waves: one as the phase starts (bWaveOnStart), then every (ramping) interval.
	bool bWaveDue = bPhaseStarted && Phase.bWaveOnStart;
	if (!bPhaseStarted)
	{
		const float Interval = GetWaveInterval(Phase, RunTime - Phase.StartTime);
		if (Interval > 0.f)
		{
			WaveTimer += DeltaSeconds;
			if (WaveTimer >= Interval)
			{
				WaveTimer = FMath::Fmod(WaveTimer, Interval);
				bWaveDue = true;
			}
		}
	}
	if (bWaveDue)
	{
		FRRDirectorAction Wave;
		Wave.Type = FRRDirectorAction::EType::SpawnWave;
		Wave.Index = PhaseIndex;
		DrawWave(Config, Phase, Stream, Wave);
		if (!Wave.PackTypes.IsEmpty())
		{
			OutActions.Add(MoveTemp(Wave));
		}
	}
}

void FRRWaveDirector::AdvanceHordes(const FRRWaveDirectorConfig& Config, float DeltaSeconds, float Alert, TArray<FRRDirectorAction>& OutActions)
{
	if (ActiveHorde == INDEX_NONE)
	{
		// Only the first pending horde may trigger: hordes come in order, one at a time.
		const int32 Next = HordeStates.IndexOfByKey(ERRHordeState::Pending);
		if (!Config.Hordes.IsValidIndex(Next))
		{
			return;
		}
		const FRRDirectorHorde& Horde = Config.Hordes[Next];
		const float LullStart = Horde.TriggerTime - FMath::Max(Horde.LullSeconds, 0.f);
		const bool bTriggered = Config.HordeTrigger == ERRHordeTrigger::RunTime ? RunTime >= LullStart : Alert >= Horde.TriggerAlert;
		if (!bTriggered)
		{
			return;
		}
		ActiveHorde = Next;
		HordeClock = Config.HordeTrigger == ERRHordeTrigger::RunTime ? FMath::Max(RunTime - LullStart, 0.f) : 0.f;
		HordeStates[Next] = ERRHordeState::Lull;
		FRRDirectorAction& Lull = OutActions.AddDefaulted_GetRef();
		Lull.Type = FRRDirectorAction::EType::HordeLull;
		Lull.Index = Next;
	}
	else
	{
		HordeClock += DeltaSeconds;
	}

	const FRRDirectorHorde& Horde = Config.Hordes[ActiveHorde];
	const float Lull = FMath::Max(Horde.LullSeconds, 0.f);
	const float Warning = FMath::Clamp(Horde.WarningSeconds, 0.f, Lull);
	ERRHordeState& State = HordeStates[ActiveHorde];
	if (State == ERRHordeState::Lull && HordeClock >= Lull - Warning)
	{
		State = ERRHordeState::Announced;
		FRRDirectorAction& Announced = OutActions.AddDefaulted_GetRef();
		Announced.Type = FRRDirectorAction::EType::HordeAnnounced;
		Announced.Index = ActiveHorde;
		Announced.Seconds = FMath::Max(Lull - HordeClock, 0.f);
	}
	if (State == ERRHordeState::Announced && HordeClock >= Lull)
	{
		State = ERRHordeState::Spawning;
		FRRDirectorAction& Started = OutActions.AddDefaulted_GetRef();
		Started.Type = FRRDirectorAction::EType::HordeStarted;
		Started.Index = ActiveHorde;
	}
	if (State == ERRHordeState::Spawning && HordeClock >= Lull + FMath::Max(Horde.SpreadSeconds, 0.f))
	{
		State = ERRHordeState::Done;
		FRRDirectorAction& Finished = OutActions.AddDefaulted_GetRef();
		Finished.Type = FRRDirectorAction::EType::HordeFinished;
		Finished.Index = ActiveHorde;
		ActiveHorde = INDEX_NONE;
		HordeClock = 0.f;
		// The flow resumes one full interval after the front.
		WaveTimer = 0.f;
		SingleTimer = 0.f;
	}
}

void FRRWaveDirector::DrawWave(const FRRWaveDirectorConfig& Config, const FRRDirectorPhase& Phase, FRandomStream& Stream, FRRDirectorAction& OutWave)
{
	TArray<TPair<int32, float>, TInlineAllocator<8>> Options;
	float TotalWeight = 0.f;
	for (const FRRDirectorPackWeight& Entry : Phase.PackWeights)
	{
		const int32 TypeIndex = Config.FindPackType(Entry.PackType);
		if (TypeIndex != INDEX_NONE && Entry.Weight > 0.f)
		{
			Options.Emplace(TypeIndex, Entry.Weight);
			TotalWeight += Entry.Weight;
		}
	}
	if (Options.IsEmpty())
	{
		return;
	}
	const int32 SidesMin = FMath::Clamp(Phase.SidesMin, 1, 4);
	const int32 PackCount = Stream.RandRange(SidesMin, FMath::Clamp(Phase.SidesMax, SidesMin, 4));
	for (int32 Pack = 0; Pack < PackCount; ++Pack)
	{
		float Draw = Stream.FRand() * TotalWeight;
		int32 Picked = Options.Last().Key;
		for (const TPair<int32, float>& Option : Options)
		{
			if (Draw < Option.Value)
			{
				Picked = Option.Key;
				break;
			}
			Draw -= Option.Value;
		}
		OutWave.PackTypes.Add(Picked);
	}
}
