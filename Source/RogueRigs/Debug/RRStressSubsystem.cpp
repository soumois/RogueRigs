// Rogue Rigs - stress test tooling: cheat commands that spawn robots in bulk around the miner and a timed frame-time sequencer.

#include "Debug/RRStressSubsystem.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Damage/RRGE_DamageExplosion.h"
#include "Debug/RRCheats.h"
#include "Debug/RRGridStress.h"
#include "Debug/RRPacingBotSubsystem.h"
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
#include "Pickups/RRPickup.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "NiagaraSystem.h"
#include "RogueRigs.h"
#include "Progression/RRProgressionComponent.h"
#include "UI/RRHUDWidget.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UObjectIterator.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"
#include "World/RRWorld.h"

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

	TAutoConsoleVariable<int32> CVarChainCount(TEXT("rr.Stress.ChainCount"), 30,
		TEXT("Chain mode: explosions queued at the end of the settle time (resolved one per ChainDelay)."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarChainMinRange(TEXT("rr.Stress.ChainMinRange"), 1500.f,
		TEXT("Chain mode: inner radius (uu) around the miner of the ring where explosion points are drawn."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarChainMaxRange(TEXT("rr.Stress.ChainMaxRange"), 5000.f,
		TEXT("Chain mode: outer radius (uu) around the miner of the ring where explosion points are drawn."), ECVF_Cheat);

	TAutoConsoleVariable<FString> CVarChainRow(TEXT("rr.Stress.ChainRow"), TEXT("Barrel"),
		TEXT("Chain mode: ExplosionStats row of the queued explosions."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarDigRate(TEXT("rr.Stress.DigRate"), 8.f,
		TEXT("Dig mode: block area destroyed per second, in cells of rr.Stress.DigReferenceCellSize (same world area at any cell size)."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarDigReferenceCellSize(TEXT("rr.Stress.DigReferenceCellSize"), 200.f,
		TEXT("Dig mode: cell size (uu) in which rr.Stress.DigRate is expressed."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarWalkRadius(TEXT("rr.Stress.WalkRadius"), 800.f,
		TEXT("Walk mode: radius (uu) of the circle the miner is moved along."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarWalkSpeed(TEXT("rr.Stress.WalkSpeed"), 600.f,
		TEXT("Walk mode: miner speed (uu/s) along the circle."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarSpikeMs(TEXT("rr.Stress.SpikeMs"), 25.f,
		TEXT("Measured frames longer than this (ms) are counted and logged as 'Stress spike:' lines with their frame, game, draw, GPU, flow ms and robots alive."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarGameProfile(TEXT("rr.Stress.GameProfile"), 3,
		TEXT("Game mode: pacing bot profile driving the miner (0 still, 1 collect, 2 run, 3 run-and-collect)."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarGameSeed(TEXT("rr.Stress.GameSeed"), 1,
		TEXT("Game mode: seed of the pacing bot (spawner stream and global random)."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarGameFillInterval(TEXT("rr.Stress.GameFillInterval"), 0.5f,
		TEXT("Game mode: seconds between two added director waves while the robots alive are under the cap (<= 0: no added wave, director only)."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarGameFillPacks(TEXT("rr.Stress.GameFillPacks"), 2,
		TEXT("Game mode: packs per added director wave (pack types drawn from the current phase weights)."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarGameHordeMin(TEXT("rr.Stress.GameHordeMin"), 40,
		TEXT("Game mode: minimum size of the forced horde."), ECVF_Cheat);

	TAutoConsoleVariable<int32> CVarGameHordeMax(TEXT("rr.Stress.GameHordeMax"), 60,
		TEXT("Game mode: maximum size of the forced horde (room kept under the cap until its lull)."), ECVF_Cheat);

	TAutoConsoleVariable<float> CVarGameHordeAt(TEXT("rr.Stress.GameHordeAt"), 20.f,
		TEXT("Game mode: seconds after the start of the first measure window at which the forced horde front starts (< 0: no forced horde)."), ECVF_Cheat);

	TAutoConsoleVariable<FString> CVarCounts(TEXT("rr.Stress.Counts"), TEXT("50,100,200,300,400"),
		TEXT("Default comma-separated robot counts of rr.Stress.Run."), ECVF_Cheat);

	TAutoConsoleVariable<FString> CVarPickupClass(TEXT("rr.Stress.PickupClass"), TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupXP.BP_PickupXP_C"),
		TEXT("Class path of the XP nuts spawned by rr.Stress.Pickups."), ECVF_Cheat);

	TAutoConsoleVariable<FString> CVarPickupBigClass(TEXT("rr.Stress.PickupBigClass"), TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupXPBig.BP_PickupXPBig_C"),
		TEXT("Class path of the big XP chips spawned by rr.Stress.Pickups."), ECVF_Cheat);

	/** Protected pickup members read by rr.Stress.Pickups (same idiom as FSpawnerAccess; the pickup class is unchanged). */
	struct FPickupAccess : public ARRPickup
	{
		static float PopMaxDistanceOf(const ARRPickup* Pickup) { return Pickup->*(&FPickupAccess::PopMaxDistance); }
	};

	/** Protected spawner members the game mode drives (same idiom as Tests/RRTestAccess.h; the spawner class is unchanged). */
	struct FSpawnerAccess : public ARREnemySpawner
	{
		static int32& MaxAliveOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::MaxAlive); }
		static FRRWaveDirectorConfig& DirectorOf(ARREnemySpawner* Spawner) { return Spawner->*(&FSpawnerAccess::DirectorConfig); }
	};

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

	void HandlePickups(const TArray<FString>& Args, UWorld* World)
	{
		URRStressSubsystem* Stress = Get(World);
		if (!Stress || Args.IsEmpty())
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Stress.Pickups <Nuts> [Chips]: needs a count and a game world."));
			return;
		}
		Stress->SpawnStressPickups(FCString::Atoi(*Args[0]), Args.IsValidIndex(1) ? FCString::Atoi(*Args[1]) : 0);
	}

	void HandleClearPickups(const TArray<FString>& Args, UWorld* World)
	{
		if (URRStressSubsystem* Stress = Get(World))
		{
			Stress->ClearStressPickups();
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
			UE_LOG(LogRogueRigs, Warning, TEXT("rr.Stress.Run: unknown mode '%s' (peace, fire, burst, chain, dig, walk, game)."), *Args[4]);
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

	FAutoConsoleCommandWithWorldAndArgs CmdPickups(TEXT("rr.Stress.Pickups"),
		TEXT("rr.Stress.Pickups <Nuts> [Chips]: spawns XP nuts and big XP chips through the pickup pool around the miner, outside its pickup radius (never collected)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandlePickups), ECVF_Cheat);

	FAutoConsoleCommandWithWorldAndArgs CmdClearPickups(TEXT("rr.Stress.ClearPickups"),
		TEXT("rr.Stress.ClearPickups: returns every pickup spawned by rr.Stress.Pickups to the pool (no collection, no XP)."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleClearPickups), ECVF_Cheat);

	FAutoConsoleCommandWithWorldAndArgs CmdPeace(TEXT("rr.Stress.Peace"),
		TEXT("rr.Stress.Peace <0|1>: 1 = miner invulnerable and weapons stopped."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandlePeace), ECVF_Cheat);

	FAutoConsoleCommandWithWorldAndArgs CmdRun(TEXT("rr.Stress.Run"),
		TEXT("rr.Stress.Run [Counts] [SettleSeconds] [MeasureSeconds] [Quit] [peace|fire|burst|chain|dig|walk|game] [Class]: timed frame-time steps (game: counts are MaxAlive caps of the live director), results in Saved/Stress."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&HandleRun), ECVF_Cheat);
}

bool URRStressSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	// Stress tooling never exists in shipping builds (same gate as URRCheatSubsystem).
#if UE_BUILD_SHIPPING
	return false;
#else
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
#endif
}

void URRStressSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGarbageCollectHandle);
	PostGarbageCollectHandle.Reset();
	Super::Deinitialize();
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

int32 URRStressSubsystem::SpawnStressPickups(int32 Nuts, int32 Chips)
{
	UWorld* World = GetWorld();
	Nuts = FMath::Max(0, Nuts);
	Chips = FMath::Max(0, Chips);
	const TSubclassOf<ARRPickup> NutClass = LoadClass<ARRPickup>(nullptr, *RRStress::CVarPickupClass.GetValueOnGameThread());
	const TSubclassOf<ARRPickup> ChipClass = LoadClass<ARRPickup>(nullptr, *RRStress::CVarPickupBigClass.GetValueOnGameThread());
	URRFlowFieldSubsystem* FlowField = World ? World->GetSubsystem<URRFlowFieldSubsystem>() : nullptr;
	URRPickupSubsystem* PickupSubsystem = World ? World->GetSubsystem<URRPickupSubsystem>() : nullptr;
	const ARRMiner* Miner = World ? RRWorld::FindLivingMiner(World) : nullptr;
	if (!FlowField || !PickupSubsystem || !Miner || (Nuts > 0 && !NutClass) || (Chips > 0 && !ChipClass) || Nuts + Chips <= 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: cannot spawn pickups (world, flow field, pickup subsystem, miner, class or count missing)."));
		return 0;
	}
	FlowField->UpdateField();
	const ARRArenaGrid* Grid = FlowField->GetGrid();
	if (!Grid)
	{
		return 0;
	}

	// Keep out of the magnet: the pop moves a pickup up to PopMaxDistance from its cell center, a wrap pass up to a quarter-cell
	// corner offset; the magnet latches within the miner's PickupRadius (2D).
	const float Quarter = Grid->GetCellSize() * 0.25f;
	float PopMax = 0.f;
	for (const TSubclassOf<ARRPickup>& Class : { NutClass, ChipClass })
	{
		if (Class)
		{
			PopMax = FMath::Max(PopMax, RRStress::FPickupAccess::PopMaxDistanceOf(GetDefault<ARRPickup>(Class)));
		}
	}
	const float PickupRadius = Miner->GetPickupRadius();
	const float MinDistance = PickupRadius + PopMax + Quarter * UE_SQRT_2;
	const FVector MinerLocation = Miner->GetActorLocation();

	// Reachable walkable cells at least MinCells steps from the miner and outside MinDistance, nearest first.
	const int32 MinCells = FMath::Max(0, RRStress::CVarMinCells.GetValueOnGameThread());
	TArray<TPair<int32, FIntPoint>> Cells;
	TArray<FIntPoint> Reachable;
	FlowField->GatherReachableCells(Reachable, [MinCells, Grid, &MinerLocation, MinDistance, &Cells](FIntPoint Cell, int32 Steps)
	{
		if (Steps < MinCells || FVector::DistSquared2D(Grid->GetCellCenter(Cell.X, Cell.Y), MinerLocation) <= FMath::Square(MinDistance))
		{
			return false;
		}
		Cells.Emplace(Steps, Cell);
		return true;
	});
	if (Cells.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: no reachable cell farther than %.0f uu from the miner to spawn pickups on."), MinDistance);
		return 0;
	}
	Cells.Sort([](const TPair<int32, FIntPoint>& A, const TPair<int32, FIntPoint>& B) { return A.Key < B.Key; });

	const int32 Total = Nuts + Chips;
	if (Total > Cells.Num())
	{
		UE_LOG(LogRogueRigs, Log, TEXT("Stress: %d pickups for %d cells outside the radius: wrapping with quarter-cell corner offsets."), Total, Cells.Num());
	}

	// Synchronous pool delivery (the test setting, SpawnMaxFrames 0) so every pickup is known now; the reuse and reset path is the real one.
	const int32 SavedMaxFrames = PickupSubsystem->GetSpawnMaxFrames();
	PickupSubsystem->SetSpawnMaxFrames(0);
	int32 SpawnedNuts = 0;
	int32 SpawnedChips = 0;
	for (int32 Index = 0; Index < Total; ++Index)
	{
		// Chips spread evenly among the nuts (index i is a chip when floor((i + 1) * Chips / Total) steps up).
		const bool bChip = (Index + 1) * Chips / Total > Index * Chips / Total;
		const TSubclassOf<ARRPickup> Class = bChip ? ChipClass : NutClass;
		const int32 Pass = Index / Cells.Num();
		const FIntPoint Cell = Cells[Index % Cells.Num()].Value;
		FVector Offset = FVector::ZeroVector;
		if (Pass > 0)
		{
			const int32 Corner = (Pass - 1) % 4;
			Offset = FVector((Corner & 1) ? Quarter : -Quarter, (Corner & 2) ? Quarter : -Quarter, 0.f);
		}
		// Same request as a grid drop at a cell center (pooled acquire, reset hooks, root template scale); no XP merge.
		const FVector Location = Grid->GetCellCenter(Cell.X, Cell.Y) + Offset;
		if (ARRPickup* Pickup = PickupSubsystem->RequestSpawn(Class, Location, nullptr, GetDefault<ARRPickup>(Class)->GetOreValue()))
		{
			StressPickups.Add(Pickup);
			++(bChip ? SpawnedChips : SpawnedNuts);
		}
	}
	PickupSubsystem->SetSpawnMaxFrames(SavedMaxFrames);

	int32 AliveChips = 0;
	int32 Magnetized = 0;
	const int32 Alive = GetStressPickupCount(&AliveChips, &Magnetized);
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: spawned %d %s and %d %s on %d candidate cells farther than %.0f uu (pickup radius %.0f + pop %.0f + corner %.0f); stress pickups alive %d (nuts %d, chips %d, magnetized %d), pickups registered %d."),
		SpawnedNuts, NutClass ? *NutClass->GetName() : TEXT("-"), SpawnedChips, ChipClass ? *ChipClass->GetName() : TEXT("-"), Cells.Num(),
		MinDistance, PickupRadius, PopMax, Quarter * UE_SQRT_2, Alive, Alive - AliveChips, AliveChips, Magnetized, PickupSubsystem->GetPickupCount());
	return SpawnedNuts + SpawnedChips;
}

int32 URRStressSubsystem::ClearStressPickups()
{
	int32 Chips = 0;
	int32 Magnetized = 0;
	const int32 Alive = GetStressPickupCount(&Chips, &Magnetized);
	URRActorPoolSubsystem* Pool = GetWorld() ? GetWorld()->GetSubsystem<URRActorPoolSubsystem>() : nullptr;
	int32 Removed = 0;
	for (const TWeakObjectPtr<ARRPickup>& Pickup : StressPickups)
	{
		// The pool release runs OnPoolReleased (unregister, reset), the end of a collection without its XP and events.
		if (Pool && Pool->IsPoolMember(Pickup.Get()) ? Pool->Release(Pickup.Get()) : Pickup->Destroy())
		{
			++Removed;
		}
	}
	StressPickups.Reset();
	const URRPickupSubsystem* PickupSubsystem = GetWorld() ? GetWorld()->GetSubsystem<URRPickupSubsystem>() : nullptr;
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: removed %d stress pickups (alive before: %d, nuts %d, chips %d, magnetized %d); pickups registered %d."),
		Removed, Alive, Alive - Chips, Chips, Magnetized, PickupSubsystem ? PickupSubsystem->GetPickupCount() : 0);
	return Removed;
}

int32 URRStressSubsystem::GetStressPickupCount(int32* OutChips, int32* OutMagnetized)
{
	StressPickups.RemoveAll([](const TWeakObjectPtr<ARRPickup>& Pickup)
	{
		return !Pickup.IsValid() || Pickup->IsActorBeingDestroyed() || URRActorPoolSubsystem::IsPooledIdle(Pickup.Get())
			|| Pickup->GetPhase() == ERRPickupPhase::Collected;
	});
	const UClass* ChipClass = LoadClass<ARRPickup>(nullptr, *RRStress::CVarPickupBigClass.GetValueOnGameThread());
	int32 Chips = 0;
	int32 Magnetized = 0;
	for (const TWeakObjectPtr<ARRPickup>& Pickup : StressPickups)
	{
		Chips += ChipClass && Pickup->GetClass() == ChipClass ? 1 : 0;
		Magnetized += Pickup->IsMagnetized() ? 1 : 0;
	}
	if (OutChips)
	{
		*OutChips = Chips;
	}
	if (OutMagnetized)
	{
		*OutMagnetized = Magnetized;
	}
	return StressPickups.Num();
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
	if (Mode.Equals(TEXT("chain"), ESearchCase::IgnoreCase) || Mode == TEXT("3"))
	{
		OutMode = ERRStressMode::Chain;
		return true;
	}
	if (Mode.Equals(TEXT("dig"), ESearchCase::IgnoreCase) || Mode == TEXT("4"))
	{
		OutMode = ERRStressMode::Dig;
		return true;
	}
	if (Mode.Equals(TEXT("walk"), ESearchCase::IgnoreCase) || Mode == TEXT("5"))
	{
		OutMode = ERRStressMode::Walk;
		return true;
	}
	if (Mode.Equals(TEXT("game"), ESearchCase::IgnoreCase) || Mode == TEXT("6"))
	{
		OutMode = ERRStressMode::Game;
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
	StepBurstKills = Kills;
	StepBurstBarrels = DestroyAllBarrels(RRWorld::FindArenaGrid(GetWorld()));
	const URRExplosionSubsystem* Explosions = GetWorld()->GetSubsystem<URRExplosionSubsystem>();
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: burst killed %d robots, destroyed %d barrels, %d explosions queued."), StepBurstKills,
		StepBurstBarrels, Explosions ? Explosions->GetPendingCount() : 0);
}

void URRStressSubsystem::HandleExplosion(FVector Center, FName RowName)
{
	++StepExplosions;
}

void URRStressSubsystem::HandleCellDestroyed(FIntPoint Cell, ERRCellType Type, ERRTerrainDamageSource Source)
{
	++StepBlocksDestroyed;
}

void URRStressSubsystem::TriggerChain()
{
	UWorld* World = GetWorld();
	ARRArenaGrid* Grid = RRWorld::FindArenaGrid(World);
	const ARRMiner* Miner = RRWorld::FindLivingMiner(World);
	URRExplosionSubsystem* Explosions = World->GetSubsystem<URRExplosionSubsystem>();
	if (!Grid || !Miner || !Explosions)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: chain needs a grid, a living miner and the explosion subsystem."));
		return;
	}
	const FName Row(*RRStress::CVarChainRow.GetValueOnGameThread());
	const TArray<FVector> Points = RRGridStress::DrawBlockPoints(Grid, Miner->GetActorLocation(), RRStress::CVarChainCount.GetValueOnGameThread(),
		RRStress::CVarChainMinRange.GetValueOnGameThread(), RRStress::CVarChainMaxRange.GetValueOnGameThread(), RRGridStress::BenchSeed);
	int32 Queued = 0;
	for (const FVector& Point : Points)
	{
		// No source object: never deduplicated, every point explodes.
		Queued += Explosions->QueueExplosion(Point, Row, Grid, nullptr) ? 1 : 0;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: chain queued %d %s explosions."), Queued, *Row.ToString());
}

void URRStressSubsystem::BuildDigQueue()
{
	DigQueue.Reset();
	DigIndex = 0;
	DigAccumulator = 0.f;
	const ARRArenaGrid* Grid = RRWorld::FindArenaGrid(GetWorld());
	const ARRMiner* Miner = RRWorld::FindLivingMiner(GetWorld());
	if (!Grid || !Miner)
	{
		return;
	}
	const FVector Origin = Miner->GetActorLocation();
	TArray<TPair<double, FIntPoint>> Blocks;
	const FIntPoint Size = Grid->GetGridSize();
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (Grid->IsBlockCell(Cell) && Grid->GetCellType(Cell) != ERRCellType::Barrel)
			{
				Blocks.Emplace(FVector::DistSquared2D(Grid->GetCellCenter(X, Y), Origin), Cell);
			}
		}
	}
	Blocks.Sort([](const TPair<double, FIntPoint>& A, const TPair<double, FIntPoint>& B) { return A.Key < B.Key; });
	for (const TPair<double, FIntPoint>& Block : Blocks)
	{
		DigQueue.Add(Block.Value);
	}
}

void URRStressSubsystem::TickDig(float DeltaSeconds)
{
	ARRArenaGrid* Grid = RRWorld::FindArenaGrid(GetWorld());
	if (!Grid || Grid->GetCellSize() <= 0.f)
	{
		return;
	}
	const float AreaScale = FMath::Square(RRStress::CVarDigReferenceCellSize.GetValueOnGameThread() / Grid->GetCellSize());
	DigAccumulator += DeltaSeconds * RRStress::CVarDigRate.GetValueOnGameThread() * AreaScale;
	while (DigAccumulator >= 1.f && DigQueue.IsValidIndex(DigIndex))
	{
		Grid->DestroyCell(DigQueue[DigIndex++], ERRTerrainDamageSource::Drone);
		DigAccumulator -= 1.f;
	}
}

void URRStressSubsystem::TickWalk(float DeltaSeconds)
{
	const ARRArenaGrid* Grid = RRWorld::FindArenaGrid(GetWorld());
	ARRMiner* Miner = RRWorld::FindLivingMiner(GetWorld());
	const float Radius = RRStress::CVarWalkRadius.GetValueOnGameThread();
	if (!Grid || !Miner || Radius <= 0.f)
	{
		return;
	}
	WalkAngle = FMath::Fmod(WalkAngle + DeltaSeconds * RRStress::CVarWalkSpeed.GetValueOnGameThread() / Radius, UE_TWO_PI);
	const FVector Target(WalkCenter.X + Radius * FMath::Cos(WalkAngle), WalkCenter.Y + Radius * FMath::Sin(WalkAngle), Miner->GetActorLocation().Z);
	const ERRCellType Type = Grid->GetCellType(Grid->GetCellAtWorld(Target));
	if (Type == ERRCellType::Floor || Type == ERRCellType::Spawn)
	{
		Miner->SetActorLocation(Target, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void URRStressSubsystem::SampleGridEvents(float FrameMs)
{
	// An event's cost lands in the frame that resolves it or the next one (tick order): keep the worst of the two frames after it.
	if (PendingEventFrames > 0)
	{
		PendingEventMax = FMath::Max(PendingEventMax, FrameMs);
		if (--PendingEventFrames == 0)
		{
			EventSamples.Add(PendingEventMax);
		}
	}
	const int32 EventCounter = StepExplosions + StepBlocksDestroyed;
	if (EventCounter != LastEventCounter)
	{
		LastEventCounter = EventCounter;
		if (PendingEventFrames == 0)
		{
			PendingEventFrames = 2;
			PendingEventMax = 0.f;
		}
	}
	if (const URRFlowFieldSubsystem* Flow = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>(); Flow && Flow->GetWorkFrameCount() != LastFlowCount)
	{
		LastFlowCount = Flow->GetWorkFrameCount();
		FlowSamples.Add(Flow->GetLastFrameWorkMilliseconds());
	}
}

void URRStressSubsystem::StartRun(const TArray<int32>& Counts, float SettleSeconds, float MeasureSeconds, bool bQuit,
	ERRStressMode Mode, TSubclassOf<ARREnemy> EnemyClass)
{
	if (Counts.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: run needs at least one count."));
		return;
	}
	if (Mode != ERRStressMode::Game)
	{
		IConsoleManager::Get().ProcessUserConsoleInput(TEXT("rr.Spawner.Enabled 0"), *GLog, GetWorld());
	}
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
	else if (RunMode != ERRStressMode::Game)
	{
		SetPeace(true);
	}
	if (URRExplosionSubsystem* Explosions = GetWorld()->GetSubsystem<URRExplosionSubsystem>(); Explosions && !bExplosionBound)
	{
		Explosions->OnExplosion.AddDynamic(this, &URRStressSubsystem::HandleExplosion);
		bExplosionBound = true;
	}
	if (ARRArenaGrid* Grid = RRWorld::FindArenaGrid(GetWorld()); Grid && BoundGrid.Get() != Grid)
	{
		Grid->OnCellDestroyed.AddUniqueDynamic(this, &URRStressSubsystem::HandleCellDestroyed);
		BoundGrid = Grid;
	}
	RunCounts = Counts;
	RunSettleSeconds = FMath::Max(0.f, SettleSeconds);
	RunMeasureSeconds = FMath::Max(0.1f, MeasureSeconds);
	bQuitWhenDone = bQuit;
	Results.Reset();
	bRunning = true;
	if (!PostGarbageCollectHandle.IsValid())
	{
		PostGarbageCollectHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddWeakLambda(this, [this]() { LastGarbageCollectFrame = GFrameCounter; });
	}
	if (RunMode == ERRStressMode::Game)
	{
		StartGameMode();
	}
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
		if (RunMode == ERRStressMode::Game)
		{
			EndGameMode();
		}
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
	if (RunMode == ERRStressMode::Game)
	{
		// The live director spawns; the step count is its cap.
		ApplyGameStep();
	}
	else if (Missing > 0)
	{
		SpawnStressEnemies(Missing, RunEnemyClass);
	}
	if (RunMode == ERRStressMode::Fire || RunMode == ERRStressMode::Chain || RunMode == ERRStressMode::Dig || RunMode == ERRStressMode::Walk)
	{
		// Chain and dig: explosions must not change the robot count either.
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
	AliveSamples.Reset();
	if (URRActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<URRActorPoolSubsystem>())
	{
		Pool->ResetStats();
	}
	LastPoolCreatedTotal = 0;
	StepSpikes = 0;
	StepSpikesFill = 0;
	StepSpikesGarbageCollect = 0;
	StepSpikesPause = 0;
	StepSpikesXP = 0;
	StepHordeSpawned = 0;
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
		LastPausedFrame = GFrameCounter;
		return;
	}
	const double RealDelta = FApp::GetDeltaTime();
	if (RunMode == ERRStressMode::Game)
	{
		TickGameFill(static_cast<float>(RealDelta));
	}
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
		StepBlocksDestroyed = 0;
		LastEventCounter = 0;
		PendingEventFrames = 0;
		EventSamples.Reset();
		FlowSamples.Reset();
		if (const URRFlowFieldSubsystem* Flow = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>())
		{
			LastFlowCount = Flow->GetWorkFrameCount();
		}
		// Before the burst: its deaths play the puff and debris inside this window.
		NiagaraCountsAtMeasure = ReadNiagaraCounts();
		if (RunMode == ERRStressMode::Burst)
		{
			TriggerBurst();
		}
		else if (RunMode == ERRStressMode::Chain)
		{
			TriggerChain();
		}
		else if (RunMode == ERRStressMode::Dig)
		{
			BuildDigQueue();
		}
		else if (RunMode == ERRStressMode::Walk)
		{
			const AActor* Miner = RRWorld::FindLivingMiner(GetWorld());
			WalkCenter = Miner ? Miner->GetActorLocation() : FVector::ZeroVector;
			WalkAngle = 0.f;
		}
		return;
	}
	if (RunMode == ERRStressMode::Dig)
	{
		TickDig(static_cast<float>(RealDelta));
	}
	else if (RunMode == ERRStressMode::Walk)
	{
		TickWalk(static_cast<float>(RealDelta));
	}
	SampleGridEvents(static_cast<float>(RealDelta * 1000.0));
	// stat unit raw values (FStatUnitData, refreshed each frame while stat unit is shown, which StartRun ensures).
	const UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	const FStatUnitData* Unit = Viewport ? Viewport->GetStatUnitData() : nullptr;
	const float FrameMs = static_cast<float>(RealDelta * 1000.0);
	const float GameMs = Unit ? Unit->RawGameThreadTime : 0.f;
	const float DrawMs = Unit ? Unit->RawRenderThreadTime : 0.f;
	const float GpuMs = Unit ? Unit->RawGPUFrameTime[0] : 0.f;
	const int32 Alive = GetWorldAliveCount();
	// Robots created by the pool since the previous sample (task-091-01: a wave is created over up to 20 frames).
	const URRActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<URRActorPoolSubsystem>();
	const URRPickupSubsystem* PickupSystem = GetWorld()->GetSubsystem<URRPickupSubsystem>();
	// task-092-01: pickups share the pool; only robot creations count as wave spawns.
	const int32 PoolCreatedTotal = Pool ? Pool->GetStats().TotalCreated - (PickupSystem ? PickupSystem->GetPoolStats().TotalCreated : 0) : 0;
	const int32 XPRequestedTotal = PickupSystem ? PickupSystem->GetTotalXPCubesRequested() : 0;
	XPRequestedPrevious = XPRequestedLatest;
	XPRequestedLatest = LastXPRequestedTotal >= 0 ? FMath::Max(XPRequestedTotal - LastXPRequestedTotal, 0) : 0;
	LastXPRequestedTotal = XPRequestedTotal;
	PoolCreatedPrevious = PoolCreatedLatest;
	PoolCreatedLatest = FMath::Max(PoolCreatedTotal - LastPoolCreatedTotal, 0);
	LastPoolCreatedTotal = PoolCreatedTotal;
	FrameSamples.Add(FrameMs);
	GameSamples.Add(GameMs);
	DrawSamples.Add(DrawMs);
	GpuSamples.Add(GpuMs);
	AliveSamples.Add(Alive);
	if (FrameMs > RRStress::CVarSpikeMs.GetValueOnGameThread())
	{
		// The sampled delta is the previous frame's: a cause tagged on the previous (or this) frame counter is the spike's.
		const bool bGarbageCollect = GFrameCounter - LastGarbageCollectFrame <= 1;
		const bool bAfterPause = GFrameCounter - LastPausedFrame <= 2;
		const int32 FillRobots = GFrameCounter - GameFillFrame <= 1 ? GameFillSpawned : 0;
		const int32 PoolCreated = PoolCreatedLatest + PoolCreatedPrevious;
		// Wave spawn tag: a fill wave requested on the previous frame, or robots created by the pool on the last two samples.
		const bool bSpawn = FillRobots > 0 || PoolCreated > 0;
		const int32 XPCubes = XPRequestedLatest + XPRequestedPrevious;
		++StepSpikes;
		StepSpikesXP += XPCubes > 0 && !bGarbageCollect ? 1 : 0;
		StepSpikesGarbageCollect += bGarbageCollect ? 1 : 0;
		StepSpikesFill += bSpawn && !bGarbageCollect ? 1 : 0;
		StepSpikesPause += bAfterPause && !bGarbageCollect && !bSpawn ? 1 : 0;
		const URRFlowFieldSubsystem* Flow = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>();
		// stat unit values are those of the last completed frame of each thread; the flow ms is that of the last frame with flow work.
		UE_LOG(LogRogueRigs, Display, TEXT("Stress spike: step %d, t %.2f s, frame %.2f, game %.2f, draw %.2f, gpu %.2f ms, flow last %.3f ms (work frames %d), robots %d, blocks %d, explosions %d, fill robots %d, pool created %d, xp drop %d, gc %d, after pause %d"),
			RunCounts[StepIndex], StepElapsed - RunSettleSeconds, FrameMs, GameMs, DrawMs, GpuMs, Flow ? Flow->GetLastFrameWorkMilliseconds() : 0.f,
			Flow ? Flow->GetWorkFrameCount() : 0, Alive, StepBlocksDestroyed, StepExplosions, FillRobots, PoolCreated, XPCubes, bGarbageCollect ? 1 : 0, bAfterPause ? 1 : 0);
	}
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
	Result.BlocksDestroyed = StepBlocksDestroyed;
	Result.EventFrames = EventSamples.Num();
	Result.EventFrameMax = EventSamples.IsEmpty() ? 0.f : FMath::Max(EventSamples);
	Result.FlowComputes = FlowSamples.Num();
	Result.FlowMax = FlowSamples.IsEmpty() ? 0.f : FMath::Max(FlowSamples);
	if (!AliveSamples.IsEmpty())
	{
		int64 AliveSum = 0;
		for (const int32 Alive : AliveSamples)
		{
			AliveSum += Alive;
		}
		Result.AliveAvg = static_cast<float>(static_cast<double>(AliveSum) / AliveSamples.Num());
		Result.AliveMin = FMath::Min(AliveSamples);
		Result.AliveMax = FMath::Max(AliveSamples);
	}
	Result.Spikes = StepSpikes;
	Result.SpikesFill = StepSpikesFill;
	Result.SpikesGarbageCollect = StepSpikesGarbageCollect;
	Result.SpikesPause = StepSpikesPause;
	Result.HordeSpawned = StepHordeSpawned;
	if (const ARRMiner* Miner = RRWorld::FindLivingMiner(GetWorld()); Miner && Miner->GetProgression())
	{
		Result.MinerLevel = Miner->GetProgression()->GetLevel();
	}
	if (RunMode == ERRStressMode::Game)
	{
		// The step count is the cap; the robots reached are AliveAvg / AliveMax.
		Result.EnemyCount = RunCounts[StepIndex];
		// Real-game check: the HUD is on screen and its minimap texture exists.
		int32 HudsShown = 0;
		bool bMinimap = false;
		for (TObjectIterator<URRHUDWidget> It; It; ++It)
		{
			if (It->GetWorld() == GetWorld() && It->IsInViewport())
			{
				++HudsShown;
				bMinimap |= It->GetMinimapTexture() != nullptr;
			}
		}
		UE_LOG(LogRogueRigs, Display, TEXT("Stress: game step %d, HUD widgets in viewport %d, minimap texture %s."), RunCounts[StepIndex], HudsShown,
			bMinimap ? TEXT("yes") : TEXT("no"));
	}
	if (const URRActorPoolSubsystem* Pool = GetWorld()->GetSubsystem<URRActorPoolSubsystem>())
	{
		const FRRActorPoolStats PoolStats = Pool->GetStats();
		UE_LOG(LogRogueRigs, Display, TEXT("Stress pool: step %d, created %d, reused %d, longest wait %d frames, pool size %d (idle %d, acquired %d, pending %d)."),
			RunCounts[StepIndex], PoolStats.TotalCreated, PoolStats.TotalReused, PoolStats.LongestWaitFrames, PoolStats.PoolSize, PoolStats.Idle,
			PoolStats.Acquired, PoolStats.Pending);
	}
	if (const URRPickupSubsystem* PickupSystem = GetWorld()->GetSubsystem<URRPickupSubsystem>())
	{
		const FRRActorPoolStats PickupStats = PickupSystem->GetPoolStats();
		UE_LOG(LogRogueRigs, Display, TEXT("Stress pickup pool: step %d, created %d, reused %d, longest wait %d frames, pool size %d (idle %d, acquired %d, pending %d), xp drop spikes %d."),
			RunCounts[StepIndex], PickupStats.TotalCreated, PickupStats.TotalReused, PickupStats.LongestWaitFrames, PickupStats.PoolSize,
			PickupStats.Idle, PickupStats.Acquired, PickupStats.Pending, StepSpikesXP);
	}
	// Puff / debris plays and skips of the measure window (task-181-01): totals now minus totals at the window start.
	FString NiagaraText;
	for (FRRStressNiagaraCount Count : ReadNiagaraCounts())
	{
		if (const FRRStressNiagaraCount* Start = NiagaraCountsAtMeasure.FindByPredicate(
				[&Count](const FRRStressNiagaraCount& Item) { return Item.System == Count.System; }))
		{
			Count.Plays -= Start->Plays;
			Count.Skips -= Start->Skips;
		}
		NiagaraText += FString::Printf(TEXT(", %s plays %d skips %d"), *Count.System.ToString(), Count.Plays, Count.Skips);
		Result.NiagaraCounts.Add(Count);
	}
	float UnusedP99 = 0.f;
	RRStress::Summarize(EventSamples, Result.EventFrameAvg, UnusedP99);
	RRStress::Summarize(FlowSamples, Result.FlowAvg, UnusedP99);
	RRStress::Summarize(FrameSamples, Result.FrameAvg, Result.FrameP99);
	RRStress::Summarize(GameSamples, Result.GameAvg, Result.GameP99);
	RRStress::Summarize(DrawSamples, Result.DrawAvg, Result.DrawP99);
	RRStress::Summarize(GpuSamples, Result.GpuAvg, Result.GpuP99);
	Results.Add(Result);
	TRACE_END_REGION(*FString::Printf(TEXT("Stress_%d_Measure"), RunCounts[StepIndex]));
	UE_LOG(LogRogueRigs, Display,
		TEXT("Stress step: enemies %d, frames %d, frame avg %.2f p99 %.2f max %.2f, game avg %.2f p99 %.2f max %.2f, draw avg %.2f p99 %.2f, gpu avg %.2f p99 %.2f ms, burst kills %d barrels %d, explosions %d, blocks %d, event frames %d avg %.2f max %.2f, flow computes %d avg %.3f max %.3f ms, alive avg %.1f min %d max %d, spikes %d (fill %d, gc %d, after pause %d), horde %d, level %d, niagara pools%s"),
		Result.EnemyCount, Result.FrameCount, Result.FrameAvg, Result.FrameP99, Result.FrameMax, Result.GameAvg, Result.GameP99,
		Result.GameMax, Result.DrawAvg, Result.DrawP99, Result.GpuAvg, Result.GpuP99, Result.BurstKills, Result.BurstBarrels, Result.Explosions,
		Result.BlocksDestroyed, Result.EventFrames, Result.EventFrameAvg, Result.EventFrameMax, Result.FlowComputes, Result.FlowAvg, Result.FlowMax, Result.AliveAvg, Result.AliveMin, Result.AliveMax,
		Result.Spikes, Result.SpikesFill, Result.SpikesGarbageCollect, Result.SpikesPause, Result.HordeSpawned, Result.MinerLevel,
		NiagaraText.IsEmpty() ? TEXT(" none") : *NiagaraText);
}

TArray<FRRStressNiagaraCount> URRStressSubsystem::ReadNiagaraCounts() const
{
	TArray<FRRStressNiagaraCount> Counts;
	const URRNiagaraPoolSubsystem* NiagaraPool = GetWorld() ? GetWorld()->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
	if (!NiagaraPool)
	{
		return Counts;
	}
	TArray<UNiagaraSystem*> Systems;
	NiagaraPool->GetPooledSystems(Systems);
	for (const UNiagaraSystem* System : Systems)
	{
		FRRStressNiagaraCount& Count = Counts.AddDefaulted_GetRef();
		Count.System = System->GetFName();
		Count.Plays = NiagaraPool->GetPlayedCount(System);
		Count.Skips = NiagaraPool->GetSkippedCount(System);
	}
	Counts.Sort([](const FRRStressNiagaraCount& A, const FRRStressNiagaraCount& B) { return A.System.LexicalLess(B.System); });
	return Counts;
}

void URRStressSubsystem::WriteResults() const
{
	// task-181-01: one <System>Plays,<System>Skips column pair per pooled Niagara system seen in any step (e.g. NS_RobotDeathPuffPlays).
	TArray<FName> NiagaraSystems;
	for (const FRRStressStepResult& R : Results)
	{
		for (const FRRStressNiagaraCount& Count : R.NiagaraCounts)
		{
			NiagaraSystems.AddUnique(Count.System);
		}
	}
	NiagaraSystems.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });
	FString Csv = TEXT("Enemies,Frames,FrameAvg,FrameP99,FrameMax,GameAvg,GameP99,GameMax,DrawAvg,DrawP99,GpuAvg,GpuP99,Kills,Barrels,Explosions,Blocks,EventFrames,EventAvg,EventMax,FlowComputes,FlowAvg,FlowMax,AliveAvg,AliveMin,AliveMax,Spikes,SpikesFill,SpikesGC,SpikesPause,Horde,Level");
	for (const FName& System : NiagaraSystems)
	{
		Csv += FString::Printf(TEXT(",%sPlays,%sSkips"), *System.ToString(), *System.ToString());
	}
	Csv += TEXT("\n");
	for (const FRRStressStepResult& R : Results)
	{
		Csv += FString::Printf(TEXT("%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%d,%d,%d,%d,%d,%.3f,%.3f,%d,%.3f,%.3f,%.1f,%d,%d,%d,%d,%d,%d,%d,%d"), R.EnemyCount,
			R.FrameCount, R.FrameAvg, R.FrameP99, R.FrameMax, R.GameAvg, R.GameP99, R.GameMax, R.DrawAvg, R.DrawP99, R.GpuAvg, R.GpuP99,
			R.BurstKills, R.BurstBarrels, R.Explosions, R.BlocksDestroyed, R.EventFrames, R.EventFrameAvg, R.EventFrameMax, R.FlowComputes,
			R.FlowAvg, R.FlowMax, R.AliveAvg, R.AliveMin, R.AliveMax, R.Spikes, R.SpikesFill, R.SpikesGarbageCollect, R.SpikesPause, R.HordeSpawned, R.MinerLevel);
		for (const FName& System : NiagaraSystems)
		{
			const FRRStressNiagaraCount* Count = R.NiagaraCounts.FindByPredicate([&System](const FRRStressNiagaraCount& Item) { return Item.System == System; });
			Csv += FString::Printf(TEXT(",%d,%d"), Count ? Count->Plays : 0, Count ? Count->Skips : 0);
		}
		Csv += TEXT("\n");
	}
	const FString Path = FPaths::ProjectSavedDir() / TEXT("Stress") /
		FString::Printf(TEXT("Stress_%s.csv"), *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
	const bool bSaved = FFileHelper::SaveStringToFile(Csv, *Path);
	UE_LOG(LogRogueRigs, Display, TEXT("Stress: run finished, %d steps, results %s %s"), Results.Num(),
		bSaved ? TEXT("written to") : TEXT("NOT written to"), *FPaths::ConvertRelativePathToFull(Path));
}

// --- Game mode (task-067-01) ---

void URRStressSubsystem::StartGameMode()
{
	TActorIterator<ARREnemySpawner> It(GetWorld());
	ARREnemySpawner* Spawner = It ? *It : nullptr;
	GameSpawner = Spawner;
	GameHordeIndex = INDEX_NONE;
	GameHordeRoom = 0;
	bGameHordeLull = false;
	bGameHordeDone = false;
	GameFillTimer = 0.f;
	if (!Spawner)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Stress: game mode needs an ARREnemySpawner in the level."));
		return;
	}
	IConsoleManager::Get().ProcessUserConsoleInput(TEXT("rr.Spawner.Enabled 1"), *GLog, GetWorld());
	GameOriginalMaxAlive = RRStress::FSpawnerAccess::MaxAliveOf(Spawner);
	Spawner->OnHordeLull.AddUniqueDynamic(this, &URRStressSubsystem::HandleHordeLull);
	Spawner->OnHordeFinished.AddUniqueDynamic(this, &URRStressSubsystem::HandleHordeFinished);
	SetMinerWeaponsFiring(true);

	// The pacing bot drives the miner for the whole run (and a margin): invulnerable, aims at the nearest robot, picks card 0.
	const float RunSeconds = RunCounts.Num() * (RunSettleSeconds + RunMeasureSeconds) + 60.f;
	const int32 Profile = FMath::Clamp(RRStress::CVarGameProfile.GetValueOnGameThread(), 0, 3);
	URRPacingBotSubsystem* Bot = GetWorld()->GetSubsystem<URRPacingBotSubsystem>();
	const bool bBot = Bot && (Bot->IsRunning() || Bot->StartRun(RunSeconds, RRStress::CVarGameSeed.GetValueOnGameThread(), false, static_cast<ERRPacingProfile>(Profile)));
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: game mode, spawner %s (MaxAlive %d, clock %.1f s), pacing bot profile %d %s."), *Spawner->GetName(),
		GameOriginalMaxAlive, Spawner->GetRunTime(), Profile, bBot ? TEXT("driving") : TEXT("NOT started"));
}

void URRStressSubsystem::ApplyGameStep()
{
	ARREnemySpawner* Spawner = GameSpawner.Get();
	if (!Spawner || !RunCounts.IsValidIndex(StepIndex))
	{
		return;
	}
	GameStepCap = RunCounts[StepIndex];
	const float HordeAt = RRStress::CVarGameHordeAt.GetValueOnGameThread();
	if (GameHordeIndex == INDEX_NONE && HordeAt >= 0.f)
	{
		// Force the first pending listed horde: its front lands HordeAt s into this step's measure window, at the forced size.
		FRRWaveDirectorConfig& Config = RRStress::FSpawnerAccess::DirectorOf(Spawner);
		for (int32 Index = 0; Index < Config.Hordes.Num(); ++Index)
		{
			if (Spawner->GetHordeState(Index) != ERRHordeState::Pending)
			{
				continue;
			}
			FRRDirectorHorde& Horde = Config.Hordes[Index];
			Config.HordeTrigger = ERRHordeTrigger::RunTime;
			Horde.TriggerTime = Spawner->GetRunTime() + RunSettleSeconds + HordeAt;
			Horde.ConsoleLandedDelaySeconds = -1.f; // forced time, not the console landing (task-524)
			Horde.SizeMin = FMath::Max(1, RRStress::CVarGameHordeMin.GetValueOnGameThread());
			Horde.SizeMax = FMath::Max(Horde.SizeMin, RRStress::CVarGameHordeMax.GetValueOnGameThread());
			GameHordeIndex = Index;
			GameHordeRoom = Horde.SizeMax;
			UE_LOG(LogRogueRigs, Log, TEXT("Stress: forced horde %d, size %d-%d, front at %.1f s of spawner clock (lull %.1f s before)."), Index,
				Horde.SizeMin, Horde.SizeMax, Horde.TriggerTime, Horde.LullSeconds);
			break;
		}
	}
	const bool bKeepRoom = GameHordeIndex != INDEX_NONE && !bGameHordeLull && !bGameHordeDone;
	RRStress::FSpawnerAccess::MaxAliveOf(Spawner) = bKeepRoom ? FMath::Max(GameStepCap - GameHordeRoom, 0) : GameStepCap;
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: game step cap %d, spawner MaxAlive %d, %d robots alive."), GameStepCap,
		RRStress::FSpawnerAccess::MaxAliveOf(Spawner), Spawner->GetAliveCount());
}

void URRStressSubsystem::TickGameFill(float DeltaSeconds)
{
	ARREnemySpawner* Spawner = GameSpawner.Get();
	const float Interval = RRStress::CVarGameFillInterval.GetValueOnGameThread();
	if (!Spawner || Interval <= 0.f)
	{
		return;
	}
	GameFillTimer -= DeltaSeconds;
	if (GameFillTimer > 0.f)
	{
		return;
	}
	GameFillTimer = Interval;
	// The director silences waves from a horde lull to the end of its front: so does the fill.
	if (Spawner->GetActiveHorde() != INDEX_NONE || Spawner->GetOccupiedCount() >= RRStress::FSpawnerAccess::MaxAliveOf(Spawner))
	{
		return;
	}
	// Pack mix of the current phase, else of the next phase that has one (the opening phases spawn lone robots only).
	const FRRWaveDirectorConfig& Config = Spawner->GetDirectorConfig();
	const FRRDirectorPhase* Phase = nullptr;
	for (int32 Index = FMath::Max(Spawner->GetDirectorPhase(), 0); Index < Config.Phases.Num() && !Phase; ++Index)
	{
		Phase = Config.Phases[Index].PackWeights.IsEmpty() ? nullptr : &Config.Phases[Index];
	}
	if (!Phase)
	{
		return;
	}
	float TotalWeight = 0.f;
	for (const FRRDirectorPackWeight& Entry : Phase->PackWeights)
	{
		TotalWeight += Config.FindPackType(Entry.PackType) != INDEX_NONE ? FMath::Max(Entry.Weight, 0.f) : 0.f;
	}
	if (TotalWeight <= 0.f)
	{
		return;
	}
	TArray<int32> PackTypes;
	const int32 Packs = FMath::Clamp(RRStress::CVarGameFillPacks.GetValueOnGameThread(), 1, 4);
	for (int32 Pack = 0; Pack < Packs; ++Pack)
	{
		float Draw = FMath::FRand() * TotalWeight;
		for (const FRRDirectorPackWeight& Entry : Phase->PackWeights)
		{
			const int32 TypeIndex = Config.FindPackType(Entry.PackType);
			const float Weight = TypeIndex != INDEX_NONE ? FMath::Max(Entry.Weight, 0.f) : 0.f;
			if (Weight > 0.f && (Draw < Weight || &Entry == &Phase->PackWeights.Last()))
			{
				PackTypes.Add(TypeIndex);
				break;
			}
			Draw -= Weight;
		}
	}
	const int32 OccupiedBefore = Spawner->GetOccupiedCount();
	Spawner->TrySpawnDirectorWave(PackTypes);
	GameFillSpawned = Spawner->GetOccupiedCount() - OccupiedBefore;
	GameFillFrame = GFrameCounter;
}

void URRStressSubsystem::EndGameMode()
{
	if (ARREnemySpawner* Spawner = GameSpawner.Get())
	{
		RRStress::FSpawnerAccess::MaxAliveOf(Spawner) = GameOriginalMaxAlive;
	}
}

void URRStressSubsystem::HandleHordeLull(int32 HordeIndex)
{
	if (HordeIndex != GameHordeIndex || !GameSpawner.IsValid())
	{
		return;
	}
	bGameHordeLull = true;
	RRStress::FSpawnerAccess::MaxAliveOf(GameSpawner.Get()) = GameStepCap;
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: forced horde %d lull, %d robots alive, cap %d."), HordeIndex, GameSpawner->GetAliveCount(), GameStepCap);
}

void URRStressSubsystem::HandleHordeFinished(int32 HordeIndex, int32 Spawned)
{
	if (HordeIndex != GameHordeIndex)
	{
		return;
	}
	bGameHordeDone = true;
	StepHordeSpawned = Spawned;
	UE_LOG(LogRogueRigs, Log, TEXT("Stress: forced horde %d front over, %d robots spawned, %d alive."), HordeIndex, Spawned,
		GameSpawner.IsValid() ? GameSpawner->GetAliveCount() : 0);
}

int32 URRStressSubsystem::GetWorldAliveCount()
{
	if (RunMode == ERRStressMode::Game)
	{
		ARREnemySpawner* Spawner = GameSpawner.Get();
		return Spawner ? Spawner->GetAliveCount() : 0;
	}
	return GetStressEnemyCount();
}
