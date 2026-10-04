// Rogue Rigs - stress test tooling: cheat commands that spawn robots in bulk around the miner and a timed frame-time sequencer.

#include "Debug/RRStressSubsystem.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageExplosion.h"
#include "Debug/RRCheats.h"
#include "Enemies/RRChaser.h"
#include "Enemies/RREnemy.h"
#include "Enemies/RREnemySpawner.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "GameFramework/Pawn.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "HAL/IConsoleManager.h"
#include "Misc/App.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"
#include "Player/RRMiner.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "ProfilingDebugging/MiscTrace.h"
#include "RogueRigs.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRStress
{
	TAutoConsoleVariable<int32> CVarMinCells(TEXT("rr.Stress.MinCells"), 3,
		TEXT("Minimum flow-field steps between the miner and a cell used by rr.Stress.Spawn."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarFloorClearance(TEXT("rr.Stress.FloorClearance"), 2.f,
		TEXT("Height (uu) between the floor and the capsule bottom of a robot spawned by rr.Stress.Spawn."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarSettleSeconds(TEXT("rr.Stress.SettleSeconds"), 5.f,
		TEXT("Default settle time (s) of each rr.Stress.Run step before sampling."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarMeasureSeconds(TEXT("rr.Stress.MeasureSeconds"), 20.f,
		TEXT("Default sampling time (s) of each rr.Stress.Run step."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarFireHealth(TEXT("rr.Stress.FireHealth"), 1000000.f,
		TEXT("Max health given to the stress robots in fire mode (rr.Stress.Run ... fire) so shots never kill them."), ECVF_Cheat);

	/** Damage of KillEnemy: far above any robot health. */
	constexpr float KillDamage = 1.0e9f;

	TAutoConsoleVariable<FString> CVarCounts(TEXT("rr.Stress.Counts"), TEXT("50,100,200,300,400"),
		TEXT("Default comma-separated robot counts of rr.Stress.Run."), ECVF_Cheat);

	/** Stress subsystem of World, null outside a game world. */
	URRStressSubsystem* Get(UWorld* World)
	{
		return World ? World->GetSubsystem<URRStressSubsystem>() : nullptr;
	}

	/** Parses "50,100,200" (or "50+100+200", usable inside -ExecCmds) into positive counts. */
	TArray<int32> ParseCounts(const FString& Text)
	{
		TArray<FString> Parts;
		const TCHAR* Delimiters[] = { TEXT(","), TEXT("+") };
		Text.ParseIntoArray(Parts, Delimiters, UE_ARRAY_COUNT(Delimiters), true);
		TArray<int32> Counts;
		for (const FString& Part : Parts)
		{
			const int32 Value = FCString::Atoi(*Part.TrimStartAndEnd());
			if (Value > 0)
			{
				Counts.Add(Value);
			}
		}
		return Counts;
	}

	/** Average and 99th percentile (nearest rank) of Samples; Samples gets sorted. */
	void Summarize(TArray<float>& Samples, float& OutAvg, float& OutP99)
	{
		OutAvg = 0.f;
		OutP99 = 0.f;
		if (Samples.IsEmpty())
		{
			return;
		}
		double Sum = 0.0;
		for (const float Value : Samples)
		{
			Sum += Value;
		}
		OutAvg = static_cast<float>(Sum / Samples.Num());
		Samples.Sort();
		const int32 Rank = FMath::Clamp(FMath::CeilToInt(0.99 * Samples.Num()) - 1, 0, Samples.Num() - 1);
		OutP99 = Samples[Rank];
	}

	void HandleSpawn(const TArray<FString>& Args, UWorld* World)
	{
		URRStressSubsystem* Stress = Get(World);
		if (!Stress || Args.IsEmpty())
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Stress.Spawn <N> [Class]: needs a count and a game world."));
			return;
		}
		const int32 Count = FCString::Atoi(*Args[0]);
		const TSubclassOf<ARREnemy> EnemyClass = Stress->ResolveEnemyClass(Args.IsValidIndex(1) ? Args[1] : FString());
		if (!EnemyClass)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Stress.Spawn: no robot class matches '%s'."), Args.IsValidIndex(1) ? *Args[1] : TEXT("<default>"));
			return;
		}
		Stress->SpawnStressEnemies(Count, EnemyClass);
	}

	void HandleClear(const TArray<FString>& Args, UWorld* World)
	{
		if (URRStressSubsystem* Stress = Get(World))
		{
			Stress->ClearStressEnemies();
		}
	}

	void HandlePeace(const TArray<FString>& Args, UWorld* World)
	{
		if (URRStressSubsystem* Stress = Get(World))
		{
			Stress->SetPeace(Args.IsEmpty() || FCString::Atoi(*Args[0]) != 0);
		}
	}

	void HandleRun(const TArray<FString>& Args, UWorld* World)
	{
		URRStressSubsystem* Stress = Get(World);
		if (!Stress)
		{
			return;
		}
		const TArray<int32> Counts = ParseCounts(Args.IsValidIndex(0) ? Args[0] : CVarCounts.GetValueOnGameThread());
		const float Settle = Args.IsValidIndex(1) ? FCString::Atof(*Args[1]) : CVarSettleSeconds.GetValueOnGameThread();
		const float Measure = Args.IsValidIndex(2) ? FCString::Atof(*Args[2]) : CVarMeasureSeconds.GetValueOnGameThread();
		const bool bQuit = Args.IsValidIndex(3) && FCString::Atoi(*Args[3]) != 0;
		ERRStressMode Mode = ERRStressMode::Peace;
		if (Args.IsValidIndex(4) && !URRStressSubsystem::ParseMode(Args[4], Mode))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Stress.Run: unknown mode '%s' (peace, fire, burst)."), *Args[4]);
			return;
		}
		const TSubclassOf<ARREnemy> EnemyClass = Stress->ResolveEnemyClass(Args.IsValidIndex(5) ? Args[5] : FString());
		if (!EnemyClass)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Stress.Run: no robot class matches '%s'."), Args.IsValidIndex(5) ? *Args[5] : TEXT("<default>"));
			return;
		}
		Stress->StartRun(Counts, Settle, Measure, bQuit, Mode, EnemyClass);
	}

	FAutoConsoleCommandWithWorldAndArgs CmdSpawn(TEXT("rr.Stress.Spawn"),
		TEXT("rr.Stress.Spawn <N> [Class]: spawns N robots (default chaser) around the miner, ignoring MaxAlive."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleSpawn), ECVF_Cheat);

	FAutoConsoleCommandWithWorldAndArgs CmdClear(TEXT("rr.Stress.Clear"),
		TEXT("rr.Stress.Clear: destroys every robot spawned by rr.Stress.Spawn."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleClear), ECVF_Cheat);

	FAutoConsoleCommandWithWorldAndArgs CmdPeace(TEXT("rr.Stress.Peace"),
		TEXT("rr.Stress.Peace <0|1>: 1 = miner invulnerable and weapons stopped."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandlePeace), ECVF_Cheat);

	FAutoConsoleCommandWithWorldAndArgs CmdRun(TEXT("rr.Stress.Run"),
		TEXT("rr.Stress.Run [Counts] [SettleSeconds] [MeasureSeconds] [Quit] [peace|fire|burst] [Class]: timed frame-time steps, results in Saved/Stress."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleRun), ECVF_Cheat);
}

bool URRStressSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId URRStressSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRStressSubsystem, STATGROUP_Tickables);
}

TSubclassOf<ARREnemy> URRStressSubsystem::GetDefaultEnemyClass() const
{
	for (TActorIterator<ARREnemySpawner> It(GetWorld()); It; ++It)
	{
		TArray<TSubclassOf<ARREnemy>> Classes;
		It->GetSpawnableClasses(Classes);
		for (const TSubclassOf<ARREnemy>& Class : Classes)
		{
			if (Class->IsChildOf(ARRChaser::StaticClass()))
			{
				return Class;
			}
		}
		if (!Classes.IsEmpty())
		{
			return Classes[0];
		}
	}
	return nullptr;
}

TSubclassOf<ARREnemy> URRStressSubsystem::ResolveEnemyClass(const FString& Name) const
{
	if (Name.IsEmpty())
	{
		return GetDefaultEnemyClass();
	}
	for (TActorIterator<ARREnemySpawner> It(GetWorld()); It; ++It)
	{
		TArray<TSubclassOf<ARREnemy>> Classes;
		It->GetSpawnableClasses(Classes);
		for (const TSubclassOf<ARREnemy>& Class : Classes)
		{
			if (Class->GetName().Contains(Name, ESearchCase::IgnoreCase))
			{
				return Class;
			}
		}
	}
	return LoadClass<ARREnemy>(nullptr, *Name);
}

int32 URRStressSubsystem::SpawnStressEnemies(int32 Count, TSubclassOf<ARREnemy> EnemyClass)
{
	UWorld* World = GetWorld();
	if (!EnemyClass)
	{
		EnemyClass = GetDefaultEnemyClass();
	}
	URRFlowFieldSubsystem* FlowField = World ? World->GetSubsystem<URRFlowFieldSubsystem>() : nullptr;
	if (!World || !EnemyClass || !FlowField || Count <= 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: cannot spawn (world, class, flow field or count missing)."));
		return 0;
	}
	FlowField->UpdateField();
	const ARRArenaGrid* Grid = FlowField->GetGrid();
	if (!Grid)
	{
		return 0;
	}

	TSet<FIntPoint> Occupied;
	FlowField->GatherPawnCells(Occupied);

	// Reachable free walkable cells at least MinCells steps from the miner, nearest first.
	const int32 MinCells = FMath::Max(0, RRStress::CVarMinCells.GetValueOnGameThread());
	TArray<TPair<int32, FIntPoint>> Cells;
	TArray<FIntPoint> Reachable;
	FlowField->GatherReachableCells(Reachable, [MinCells, &Cells](FIntPoint Cell, int32 Steps)
	{
		if (Steps < MinCells)
		{
			return false;
		}
		Cells.Emplace(Steps, Cell);
		return true;
	}, /*bFloorOnly*/ false, &Occupied);
	if (Cells.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: no reachable cell to spawn on."));
		return 0;
	}
	Cells.Sort([](const TPair<int32, FIntPoint>& A, const TPair<int32, FIntPoint>& B) { return A.Key < B.Key; });

	const float HalfHeight = EnemyClass->GetDefaultObject<ARREnemy>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float Quarter = Grid->GetCellSize() * 0.25f;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	int32 Spawned = 0;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		// More robots than cells: wrap around with an offset toward a cell corner on each pass.
		const int32 Pass = Index / Cells.Num();
		const FIntPoint Cell = Cells[Index % Cells.Num()].Value;
		FVector Offset = FVector::ZeroVector;
		if (Pass > 0)
		{
			const int32 Corner = (Pass - 1) % 4;
			Offset = FVector((Corner & 1) ? Quarter : -Quarter, (Corner & 2) ? Quarter : -Quarter, 0.f);
		}
		const FVector Location = Grid->GetCellCenter(Cell.X, Cell.Y) + Offset + FVector(0.f, 0.f, HalfHeight + RRStress::CVarFloorClearance.GetValueOnGameThread());
		if (ARREnemy* Enemy = World->SpawnActor<ARREnemy>(EnemyClass, Location, FRotator::ZeroRotator, Params))
		{
			StressEnemies.Add(Enemy);
			++Spawned;
		}
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: spawned %d %s on %d candidate cells, %d stress robots alive."), Spawned,
		*EnemyClass->GetName(), Cells.Num(), GetStressEnemyCount());
	return Spawned;
}

int32 URRStressSubsystem::ClearStressEnemies()
{
	int32 Destroyed = 0;
	for (const TWeakObjectPtr<ARREnemy>& Enemy : StressEnemies)
	{
		if (Enemy.IsValid() && !Enemy->IsActorBeingDestroyed())
		{
			Enemy->Destroy();
			++Destroyed;
		}
	}
	StressEnemies.Reset();
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: destroyed %d robots."), Destroyed);
	return Destroyed;
}

int32 URRStressSubsystem::GetStressEnemyCount()
{
	StressEnemies.RemoveAll([](const TWeakObjectPtr<ARREnemy>& Enemy)
	{
		return !Enemy.IsValid() || Enemy->IsActorBeingDestroyed() || Enemy->IsDead();
	});
	return StressEnemies.Num();
}

void URRStressSubsystem::SetPeace(bool bInPeace)
{
	bPeace = bInPeace;
	SetMinersInvulnerable(bPeace);
	SetMinerWeaponsFiring(!bPeace);
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: peace %s."), bPeace ? TEXT("on") : TEXT("off"));
}

void URRStressSubsystem::SetMinersInvulnerable(bool bInvulnerable)
{
	for (TActorIterator<ARRMiner> It(GetWorld()); It; ++It)
	{
		if (UAbilitySystemComponent* ASC = It->GetAbilitySystemComponent())
		{
			if (bInvulnerable && !ASC->HasMatchingGameplayTag(RRGameplayTags::State_Invulnerable))
			{
				ASC->AddLooseGameplayTag(RRGameplayTags::State_Invulnerable);
			}
			else if (!bInvulnerable)
			{
				ASC->RemoveLooseGameplayTag(RRGameplayTags::State_Invulnerable);
			}
		}
	}
}

void URRStressSubsystem::SetMinerWeaponsFiring(bool bFiring)
{
	for (TActorIterator<ARRMiner> It(GetWorld()); It; ++It)
	{
		for (ARRWeapon* Weapon : It->GetWeapons())
		{
			if (URRAutoFireComponent* AutoFire = Weapon ? Weapon->GetAutoFireComponent() : nullptr)
			{
				bFiring ? AutoFire->StartFiring() : AutoFire->StopFiring();
			}
		}
	}
}

bool URRStressSubsystem::ParseMode(const FString& Text, ERRStressMode& OutMode)
{
	const FString Mode = Text.TrimStartAndEnd();
	if (Mode.Equals(TEXT("peace"), ESearchCase::IgnoreCase) || Mode == TEXT("0"))
	{
		OutMode = ERRStressMode::Peace;
		return true;
	}
	if (Mode.Equals(TEXT("fire"), ESearchCase::IgnoreCase) || Mode == TEXT("1"))
	{
		OutMode = ERRStressMode::Fire;
		return true;
	}
	if (Mode.Equals(TEXT("burst"), ESearchCase::IgnoreCase) || Mode == TEXT("2"))
	{
		OutMode = ERRStressMode::Burst;
		return true;
	}
	return false;
}

bool URRStressSubsystem::KillEnemy(ARREnemy* Enemy)
{
	UAbilitySystemComponent* ASC = IsValid(Enemy) && !Enemy->IsDead() ? Enemy->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		return false;
	}
	// Same path as an explosion hit: the health set sees the damage and broadcasts the death (XP, kamikaze explosion).
	FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(URRGE_DamageExplosion::StaticClass(), 1.f, ASC->MakeEffectContext());
	if (!Spec.IsValid())
	{
		return false;
	}
	Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, RRStress::KillDamage);
	ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
	return Enemy->IsDead();
}

int32 URRStressSubsystem::DestroyAllBarrels(ARRArenaGrid* Grid)
{
	if (!Grid)
	{
		return 0;
	}
	int32 Destroyed = 0;
	const FIntPoint Size = Grid->GetGridSize();
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (Grid->GetCellType(Cell) == ERRCellType::Barrel && Grid->DestroyCell(Cell, ERRTerrainDamageSource::Barrel))
			{
				++Destroyed;
			}
		}
	}
	return Destroyed;
}

void URRStressSubsystem::MakeStressEnemiesTough()
{
	const float Health = FMath::Max(1.f, RRStress::CVarFireHealth.GetValueOnGameThread());
	for (const TWeakObjectPtr<ARREnemy>& Enemy : StressEnemies)
	{
		UAbilitySystemComponent* ASC = Enemy.IsValid() && !Enemy->IsDead() ? Enemy->GetAbilitySystemComponent() : nullptr;
		if (ASC && ASC->GetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute()) < Health)
		{
			// MaxHealth first: Health is clamped against the current MaxHealth.
			ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), Health);
			ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), Health);
		}
	}
}

void URRStressSubsystem::TriggerBurst()
{
	int32 Kills = 0;
	for (const TWeakObjectPtr<ARREnemy>& Enemy : StressEnemies)
	{
		Kills += KillEnemy(Enemy.Get()) ? 1 : 0;
	}
	TActorIterator<ARRArenaGrid> GridIt(GetWorld());
	StepBurstKills = Kills;
	StepBurstBarrels = DestroyAllBarrels(GridIt ? *GridIt : nullptr);
	const URRExplosionSubsystem* Explosions = GetWorld()->GetSubsystem<URRExplosionSubsystem>();
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: burst killed %d robots, destroyed %d barrels, %d explosions queued."), StepBurstKills,
		StepBurstBarrels, Explosions ? Explosions->GetPendingCount() : 0);
}

void URRStressSubsystem::HandleExplosion(FVector Center, FName RowName)
{
	++StepExplosions;
}

void URRStressSubsystem::StartRun(const TArray<int32>& Counts, float SettleSeconds, float MeasureSeconds, bool bQuit,
	ERRStressMode Mode, TSubclassOf<ARREnemy> EnemyClass)
{
	if (Counts.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: run needs at least one count."));
		return;
	}
	IConsoleManager::Get().ProcessUserConsoleInput(TEXT("rr.Spawner.Enabled 0"), *GLog, GetWorld());
	if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport(); Viewport && !Viewport->IsStatEnabled(TEXT("Unit")))
	{
		GEngine->Exec(GetWorld(), TEXT("stat unit"));
	}
	RunMode = Mode;
	RunEnemyClass = EnemyClass ? EnemyClass : GetDefaultEnemyClass();
	if (RunMode == ERRStressMode::Fire)
	{
		// S2: the miner stays alive another way than peace, its weapons keep firing.
		bPeace = false;
		SetMinersInvulnerable(true);
		SetMinerWeaponsFiring(true);
	}
	else
	{
		SetPeace(true);
	}
	if (URRExplosionSubsystem* Explosions = GetWorld()->GetSubsystem<URRExplosionSubsystem>(); Explosions && !bExplosionBound)
	{
		Explosions->OnExplosion.AddDynamic(this, &URRStressSubsystem::HandleExplosion);
		bExplosionBound = true;
	}
	RunCounts = Counts;
	RunSettleSeconds = FMath::Max(0.f, SettleSeconds);
	RunMeasureSeconds = FMath::Max(0.1f, MeasureSeconds);
	bQuitWhenDone = bQuit;
	Results.Reset();
	bRunning = true;
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: run started, %d steps, settle %.1f s, measure %.1f s, mode %s, class %s."), RunCounts.Num(),
		RunSettleSeconds, RunMeasureSeconds, *UEnum::GetValueAsString(RunMode), *GetNameSafe(RunEnemyClass));
	BeginStep(0);
}

void URRStressSubsystem::BeginStep(int32 InStepIndex)
{
	StepIndex = InStepIndex;
	if (!RunCounts.IsValidIndex(StepIndex))
	{
		bRunning = false;
		WriteResults();
		if (bQuitWhenDone)
		{
			FPlatformMisc::RequestExit(false, TEXT("rr.Stress.Run"));
		}
		return;
	}
	if (RunMode == ERRStressMode::Burst)
	{
		// Each burst step starts from a fresh set of robots (the previous ones are dead or exploding).
		ClearStressEnemies();
	}
	const int32 Missing = RunCounts[StepIndex] - GetStressEnemyCount();
	if (Missing > 0)
	{
		SpawnStressEnemies(Missing, RunEnemyClass);
	}
	if (RunMode == ERRStressMode::Fire)
	{
		MakeStressEnemiesTough();
	}
	StepBurstKills = 0;
	StepBurstBarrels = 0;
	StepExplosions = 0;
	StepElapsed = 0.0;
	FrameSamples.Reset();
	GameSamples.Reset();
	DrawSamples.Reset();
	GpuSamples.Reset();
	CSV_EVENT_GLOBAL(TEXT("Stress_%d_Settle"), RunCounts[StepIndex]);
}

void URRStressSubsystem::Tick(float DeltaTime)
{
	if (!bRunning)
	{
		return;
	}
	if (GetWorld()->IsPaused())
	{
		// A level-up screen (XP of killed robots) would hold the run: pick the first card; paused frames are not sampled.
		RRCheats::ChooseUpgrade(GetWorld(), 0, TEXT("rr.Stress.Run"));
		return;
	}
	const double RealDelta = FApp::GetDeltaTime();
	const bool bWasSettling = StepElapsed < RunSettleSeconds;
	StepElapsed += RealDelta;
	if (StepElapsed < RunSettleSeconds)
	{
		return;
	}
	if (bWasSettling)
	{
		// First frame after the settle window: the delta still belongs to it.
		CSV_EVENT_GLOBAL(TEXT("Stress_%d_Measure"), RunCounts[StepIndex]);
		// Insights timing region (with -trace=...,region): TimingInsights exports can filter with -region=Stress_<N>_Measure.
		TRACE_BEGIN_REGION(*FString::Printf(TEXT("Stress_%d_Measure"), RunCounts[StepIndex]), TEXT("Stress"));
		StepExplosions = 0;
		if (RunMode == ERRStressMode::Burst)
		{
			TriggerBurst();
		}
		return;
	}
	// stat unit raw values (FStatUnitData, refreshed each frame while stat unit is shown, which StartRun ensures).
	const UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	const FStatUnitData* Unit = Viewport ? Viewport->GetStatUnitData() : nullptr;
	FrameSamples.Add(static_cast<float>(RealDelta * 1000.0));
	GameSamples.Add(Unit ? Unit->RawGameThreadTime : 0.f);
	DrawSamples.Add(Unit ? Unit->RawRenderThreadTime : 0.f);
	GpuSamples.Add(Unit ? Unit->RawGPUFrameTime[0] : 0.f);
	if (StepElapsed >= RunSettleSeconds + RunMeasureSeconds)
	{
		FinishStep();
		BeginStep(StepIndex + 1);
	}
}

void URRStressSubsystem::FinishStep()
{
	FRRStressStepResult Result;
	Result.EnemyCount = GetStressEnemyCount();
	Result.FrameCount = FrameSamples.Num();
	Result.FrameMax = FrameSamples.IsEmpty() ? 0.f : FMath::Max(FrameSamples);
	Result.GameMax = GameSamples.IsEmpty() ? 0.f : FMath::Max(GameSamples);
	Result.BurstKills = StepBurstKills;
	Result.BurstBarrels = StepBurstBarrels;
	Result.Explosions = StepExplosions;
	RRStress::Summarize(FrameSamples, Result.FrameAvg, Result.FrameP99);
	RRStress::Summarize(GameSamples, Result.GameAvg, Result.GameP99);
	RRStress::Summarize(DrawSamples, Result.DrawAvg, Result.DrawP99);
	RRStress::Summarize(GpuSamples, Result.GpuAvg, Result.GpuP99);
	Results.Add(Result);
	TRACE_END_REGION(*FString::Printf(TEXT("Stress_%d_Measure"), RunCounts[StepIndex]));
	UE_LOG(LogRogueRigs, Display,
		TEXT("Stress step: enemies %d, frames %d, frame avg %.2f p99 %.2f max %.2f, game avg %.2f p99 %.2f max %.2f, draw avg %.2f p99 %.2f, gpu avg %.2f p99 %.2f ms, burst kills %d barrels %d, explosions %d"),
		Result.EnemyCount, Result.FrameCount, Result.FrameAvg, Result.FrameP99, Result.FrameMax, Result.GameAvg, Result.GameP99,
		Result.GameMax, Result.DrawAvg, Result.DrawP99, Result.GpuAvg, Result.GpuP99, Result.BurstKills, Result.BurstBarrels, Result.Explosions);
}

void URRStressSubsystem::WriteResults() const
{
	FString Csv = TEXT("Enemies,Frames,FrameAvg,FrameP99,FrameMax,GameAvg,GameP99,GameMax,DrawAvg,DrawP99,GpuAvg,GpuP99,Kills,Barrels,Explosions\n");
	for (const FRRStressStepResult& R : Results)
	{
		Csv += FString::Printf(TEXT("%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%d,%d,%d\n"), R.EnemyCount, R.FrameCount, R.FrameAvg,
			R.FrameP99, R.FrameMax, R.GameAvg, R.GameP99, R.GameMax, R.DrawAvg, R.DrawP99, R.GpuAvg, R.GpuP99, R.BurstKills, R.BurstBarrels,
			R.Explosions);
	}
	const FString Path = FPaths::ProjectSavedDir() / TEXT("Stress") /
		FString::Printf(TEXT("Stress_%s.csv"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	const bool bSaved = FFileHelper::SaveStringToFile(Csv, *Path);
	UE_LOG(LogRogueRigs, Display, TEXT("Stress: run finished, %d steps, results %s %s"), Results.Num(),
		bSaved ? TEXT("written to") : TEXT("NOT written to"), *FPaths::ConvertRelativePathToFull(Path));
}
