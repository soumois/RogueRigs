// Rogue Rigs - run flow of a level: alert bar from time and kills, console drop and pause, extraction shuttle and run end.

#include "Objectives/RRRunFlowSubsystem.h"

#include "Debug/RRCheatSettings.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "Grid/RRArenaGrid.h"
#include "Enemies/RRWaveDirector.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Objectives/RRBonusChest.h"
#include "Objectives/RRLandingFootprint.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "RogueRigs.h"
#include "Text/RRSubtitleSubsystem.h"
#include "Text/RRTextContentSubsystem.h"
#include "World/RRWorld.h"

void URRRunFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RandomStream.Initialize(FMath::Rand());
}

bool URRRunFlowSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId URRRunFlowSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRRunFlowSubsystem, STATGROUP_Tickables);
}

void URRRunFlowSubsystem::SetConfig(const FRRRunFlowConfig& InConfig)
{
	Config = InConfig;
	bConfigResolved = true;
}

float URRRunFlowSubsystem::GetAlert() const
{
	return FMath::Clamp(AlertSeconds / FMath::Max(Config.AlertFillSeconds, 1.f), 0.f, 1.f);
}

ERRRunPhase URRRunFlowSubsystem::GetPhase() const
{
	if (bRunEnded)
	{
		return ERRRunPhase::Ended;
	}
	if (bExtractionReady)
	{
		return ERRRunPhase::ExtractionReady;
	}
	return HasPendingObjective() ? ERRRunPhase::ObjectivePending : ERRRunPhase::Filling;
}

void URRRunFlowSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickRunFlow(DeltaTime);
	if (bRunFlowEnabled && !bRunEnded)
	{
		// A pending horde's estimate moves with time while the bar is paused: recomputed here, sent only on change.
		RefreshAlertBarEvents();
	}
}

void URRRunFlowSubsystem::TickRunFlow(float DeltaTime)
{

	if (!bRunFlowEnabled || bRunEnded)
	{
		return;
	}
	ResolveConfig();
	if (!ResolveRunActors())
	{
		return;
	}
	if (bExtractionReady)
	{
		// Landing retried until a reachable cell exists; the shuttle then owns the countdown.
		if (!Shuttle)
		{
			LandShuttle();
		}
		return;
	}

	const float FillSeconds = FMath::Max(Config.AlertFillSeconds, 1.f);
	if (!HasPendingObjective())
	{
		AlertSeconds = FMath::Min(AlertSeconds + DeltaTime * GetAlertGainMultiplier(), FillSeconds);
	}

	// Console threshold: clamp the bar there and drop (retried every tick until a cell qualifies).
	if (!bConsoleDone && GetAlert() >= Config.ConsoleAlertThreshold)
	{
		if (!bConsoleTriggered)
		{
			bConsoleTriggered = true;
			AlertSeconds = Config.ConsoleAlertThreshold * FillSeconds;
		}
		if (!Console)
		{
			DropConsole();
		}
	}

	CheckExtraction();

	// Bonus chest: once per level at its threshold, optional (nothing pauses); retried every tick until a cell qualifies.
	if (!bExtractionReady && !BonusChest && Config.BonusChestAlertThreshold > 0.f && GetAlert() >= Config.BonusChestAlertThreshold)
	{
		bBonusChestTriggered = true;
		DropBonusChest();
	}
	BroadcastAlertIfChanged();
}

void URRRunFlowSubsystem::NotifyEnemyKilled(AActor* Enemy, AActor* DeathInstigator)
{
	if (!bRunFlowEnabled || (Enemy && DeathInstigator == Enemy))
	{
		return;
	}
	++KillCount;
	if (HasPendingObjective() || bExtractionReady)
	{
		return;
	}
	ResolveConfig();
	const float FillSeconds = FMath::Max(Config.AlertFillSeconds, 1.f);
	AlertSeconds = FMath::Min(AlertSeconds + Config.KillAlertSeconds * GetAlertGainMultiplier(), FillSeconds);
	// A kill never jumps over the console threshold: the drop happens on the next tick.
	if (!bConsoleTriggered && !bConsoleDone)
	{
		AlertSeconds = FMath::Min(AlertSeconds, Config.ConsoleAlertThreshold * FillSeconds);
	}
	BroadcastAlertIfChanged();
}

//~ Begin cheats: bodies compiled out of shipping builds.

void URRRunFlowSubsystem::SetAlert(float NewAlert)
{
#if !UE_BUILD_SHIPPING
	ResolveConfig();
	AlertSeconds = FMath::Clamp(NewAlert, 0.f, 1.f) * FMath::Max(Config.AlertFillSeconds, 1.f);
	BroadcastAlertIfChanged();
#endif
}

ARRConsole* URRRunFlowSubsystem::SkipToConsole()
{
#if UE_BUILD_SHIPPING
	return nullptr;
#else
	if (Console || bConsoleDone)
	{
		return Console;
	}
	ResolveConfig();
	if (!ResolveRunActors())
	{
		return nullptr;
	}
	AlertSeconds = Config.ConsoleAlertThreshold * FMath::Max(Config.AlertFillSeconds, 1.f);
	bConsoleTriggered = true;
	ARRConsole* Dropped = DropConsole();
	BroadcastAlertIfChanged();
	return Dropped;
#endif
}

bool URRRunFlowSubsystem::CompleteConsoleHack()
{
#if UE_BUILD_SHIPPING
	return false;
#else
	if (!Console || Console->IsComplete())
	{
		return false;
	}
	Console->ForceComplete();
	return true;
#endif
}

bool URRRunFlowSubsystem::SkipToExtraction()
{
#if UE_BUILD_SHIPPING
	return false;
#else
	ResolveConfig();
	if (!ResolveRunActors())
	{
		return false;
	}
	if (Console && !Console->IsComplete())
	{
		Console->ForceComplete();
	}
	bConsoleTriggered = true;
	bConsoleDone = true;
	AlertSeconds = FMath::Max(Config.AlertFillSeconds, 1.f);
	CheckExtraction();
	BroadcastAlertIfChanged();
	return bExtractionReady;
#endif
}

//~ End cheats

bool URRRunFlowSubsystem::ResolveRunActors()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	if (!IsValid(Grid))
	{
		Grid = RRWorld::FindArenaGrid(World);
	}
	if (!IsValid(Miner) || Miner->IsActorBeingDestroyed())
	{
		Miner = Cast<ARRMiner>(UGameplayStatics::GetPlayerPawn(World, 0));
		if (!Miner)
		{
			Miner = RRWorld::FindLivingMiner(World);
		}
	}
	return Grid != nullptr && Miner != nullptr;
}

void URRRunFlowSubsystem::ResolveConfig()
{
	if (bConfigResolved)
	{
		return;
	}
	bConfigResolved = true;
	if (const ARRGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ARRGameMode>() : nullptr)
	{
		Config = GameMode->GetRunFlowConfig();
	}
}

bool URRRunFlowSubsystem::FindConsoleCell(FIntPoint& OutCell)
{
	const UClass* ConsoleClass = Config.ConsoleClass ? Config.ConsoleClass.Get() : ARRConsole::StaticClass();
	return FindObjectiveCell(ConsoleClass->GetDefaultObject<ARRConsole>(), Config.ConsoleMinDistanceCells, Config.ConsoleMaxDistanceCells,
		Config.bPreferCellsWithMarkedRocks, TEXT("console"), OutCell);
}

bool URRRunFlowSubsystem::FindObjectiveCell(const ARRConsole* Defaults, float MinCells, float MaxCells, bool bPreferMarkedRocks, const TCHAR* What, FIntPoint& OutCell)
{
	if (!Grid || !Miner || !Defaults)
	{
		return false;
	}
	URRFlowFieldSubsystem* FlowField = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>();
	if (!FlowField)
	{
		return false;
	}
	const FIntPoint MinerCell = Grid->GetCellAtWorld(Miner->GetActorLocation());
	const float MinSquared = FMath::Square(MinCells);
	const float MaxSquared = FMath::Square(MaxCells);
	const FIntPoint Footprint = Defaults->GetFootprintCells();

	// Reachable floor cells in the distance ring around the miner whose whole footprint is reachable floor.
	TSet<FIntPoint> ReachableFloor;
	GatherReachableFloor(*FlowField, ReachableFloor);
	TArray<FIntPoint> Ring;
	FlowField->GatherReachableCells(Ring, [MinerCell, MinSquared, MaxSquared](FIntPoint Cell, int32 Steps)
	{
		const float DistanceSquared = static_cast<float>(FMath::Square(Cell.X - MinerCell.X) + FMath::Square(Cell.Y - MinerCell.Y));
		return DistanceSquared >= MinSquared && DistanceSquared <= MaxSquared;
	}, /*bFloorOnly*/ true);
	if (Ring.IsEmpty())
	{
		return false;
	}
	TArray<FIntPoint> Candidates = Ring.FilterByPredicate([&ReachableFloor, Footprint](const FIntPoint& Cell)
	{
		return FootprintFits(ReachableFloor, Cell, Footprint);
	});
	if (Candidates.IsEmpty())
	{
		// Fallback: no spot of the ring fits the footprint; land on a ring cell anyway (the mesh may overlap blocks).
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no %d x %d cell spot of reachable floor %.0f to %.0f cells from the miner; %s lands on a single reachable floor cell (mesh may overlap blocks)."),
			Footprint.X, Footprint.Y, MinCells, MaxCells, What);
		Candidates = MoveTemp(Ring);
	}

	// Prefer cells with rocks to mark, so the console has something to clear.
	if (bPreferMarkedRocks)
	{
		TArray<FIntPoint> WithRocks = Candidates.FilterByPredicate([this, Defaults](const FIntPoint& Cell)
		{
			return !Defaults->FindMarkableCells(Grid, Cell).IsEmpty();
		});
		if (!WithRocks.IsEmpty())
		{
			Candidates = MoveTemp(WithRocks);
		}
	}
	OutCell = Candidates[RandomStream.RandRange(0, Candidates.Num() - 1)];
	return true;
}

void URRRunFlowSubsystem::GatherReachableFloor(URRFlowFieldSubsystem& FlowField, TSet<FIntPoint>& OutCells)
{
	TArray<FIntPoint> Cells;
	FlowField.GatherReachableCells(Cells, [](FIntPoint Cell, int32 Steps) { return true; }, /*bFloorOnly*/ true);
	OutCells.Reset();
	OutCells.Append(Cells);
}

bool URRRunFlowSubsystem::FootprintFits(const TSet<FIntPoint>& ReachableFloor, FIntPoint Anchor, FIntPoint Footprint)
{
	return RRLandingFootprint::Fits(Anchor, Footprint, [&ReachableFloor](FIntPoint Cell) { return ReachableFloor.Contains(Cell); });
}

ARRConsole* URRRunFlowSubsystem::DropConsole()
{
	FIntPoint Cell;
	if (!FindConsoleCell(Cell))
	{
		if (!bDropFailureLogged)
		{
			bDropFailureLogged = true;
			UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no reachable floor cell %.0f to %.0f cells from the miner; console drop retried every tick."),
				Config.ConsoleMinDistanceCells, Config.ConsoleMaxDistanceCells);
		}
		return nullptr;
	}

	UClass* ConsoleClass = Config.ConsoleClass ? Config.ConsoleClass.Get() : ARRConsole::StaticClass();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARRConsole* NewConsole = GetWorld()->SpawnActor<ARRConsole>(ConsoleClass, Grid->GetCellCenter(Cell.X, Cell.Y), FRotator::ZeroRotator, Params);
	if (!NewConsole)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunFlowSubsystem: could not spawn console %s."), *GetNameSafe(ConsoleClass));
		return nullptr;
	}
	Console = NewConsole;
	NewConsole->OnConsoleStateChanged.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleConsoleStateChanged);
	NewConsole->OnConsoleProgressChanged.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleConsoleProgressChanged);
	NewConsole->OnConsoleMarkedRocksChanged.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleConsoleMarkedRocksChanged);
	NewConsole->InitializeConsole(Grid, Miner, Cell);

	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: alert %.2f, console dropped on (%d, %d)."), GetAlert(), Cell.X, Cell.Y);
	PickAlertLine(FRRTextContentLibrary::AlertConsoleDropped);
	OnConsoleDropped.Broadcast(NewConsole);
	UpdateLocatorTarget();
	return NewConsole;
}

ARRBonusChest* URRRunFlowSubsystem::DropBonusChest()
{
	UClass* ChestClass = Config.BonusChestClass ? Config.BonusChestClass.Get() : ARRBonusChest::StaticClass();
	FIntPoint Cell;
	if (!FindObjectiveCell(ChestClass->GetDefaultObject<ARRBonusChest>(), Config.BonusChestMinDistanceCells, Config.BonusChestMaxDistanceCells,
		/*bPreferMarkedRocks*/ false, TEXT("bonus chest"), Cell))
	{
		if (!bBonusChestFailureLogged)
		{
			bBonusChestFailureLogged = true;
			UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no reachable floor cell %.0f to %.0f cells from the miner; bonus chest drop retried every tick."),
				Config.BonusChestMinDistanceCells, Config.BonusChestMaxDistanceCells);
		}
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARRBonusChest* NewChest = GetWorld()->SpawnActor<ARRBonusChest>(ChestClass, Grid->GetCellCenter(Cell.X, Cell.Y), FRotator::ZeroRotator, Params);
	if (!NewChest)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunFlowSubsystem: could not spawn bonus chest %s."), *GetNameSafe(ChestClass));
		return nullptr;
	}
	BonusChest = NewChest;
	NewChest->InitializeConsole(Grid, Miner, Cell);

	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: alert %.2f, bonus chest dropped on (%d, %d)."), GetAlert(), Cell.X, Cell.Y);
	PickAlertLine(FRRTextContentLibrary::AlertBonusChestDropped);
	OnBonusChestDropped.Broadcast(NewChest);
	return NewChest;
}

void URRRunFlowSubsystem::CheckExtraction()
{
	if (bExtractionReady || HasPendingObjective() || GetAlert() < 1.f)
	{
		return;
	}
	bExtractionReady = true;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: alert full, no objective pending: extraction ready."));
	PickAlertLine(FRRTextContentLibrary::AlertExtractionReady);
	OnExtractionReady.Broadcast();
	RefreshAlertBarEvents();
	LandShuttle();
}

bool URRRunFlowSubsystem::FindShuttleCell(FIntPoint& OutCell)
{
	if (!Grid || !Miner)
	{
		return false;
	}
	URRFlowFieldSubsystem* FlowField = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>();
	if (!FlowField)
	{
		return false;
	}
	const FIntPoint MinerCell = Grid->GetCellAtWorld(Miner->GetActorLocation());
	const float MinCells = FMath::Max(0.f, Config.ShuttleMinDistanceCells);
	const float MaxCells = FMath::Max(MinCells, Config.ShuttleMaxDistanceCells);
	const int32 MinSquared = FMath::CeilToInt(FMath::Square(MinCells));
	const int32 MaxSquared = FMath::FloorToInt(FMath::Square(MaxCells));

	const UClass* ShuttleClass = Config.ShuttleClass ? Config.ShuttleClass.Get() : ARRShuttle::StaticClass();
	const FIntPoint Footprint = ShuttleClass->GetDefaultObject<ARRShuttle>()->GetFootprintCells();

	// Distance, in cells, from a squared cell distance to the [Min, Max] band (0 inside it).
	auto BandGap = [MinCells, MaxCells](int32 DistanceSquared)
	{
		const float Distance = FMath::Sqrt(static_cast<float>(DistanceSquared));
		return Distance < MinCells ? MinCells - Distance : (Distance > MaxCells ? Distance - MaxCells : 0.f);
	};

	// Reachable floor cells within the band whose whole footprint is reachable floor. Fallbacks: the fitting spot nearest to the
	// band, then the reachable floor cell nearest to it (footprint ignored).
	TArray<FIntPoint> ReachableCells;
	FlowField->GatherReachableCells(ReachableCells, [](FIntPoint Cell, int32 Steps) { return true; }, /*bFloorOnly*/ true);
	TSet<FIntPoint> ReachableFloor(ReachableCells);
	TArray<FIntPoint> Candidates;
	FIntPoint Nearest(INDEX_NONE, INDEX_NONE);
	float NearestGap = TNumericLimits<float>::Max();
	int32 NearestSquared = -1;
	FIntPoint NearestFitting(INDEX_NONE, INDEX_NONE);
	float NearestFittingGap = TNumericLimits<float>::Max();
	int32 NearestFittingSquared = -1;
	for (const FIntPoint& Cell : ReachableCells)
	{
		const int32 DistanceSquared = FMath::Square(Cell.X - MinerCell.X) + FMath::Square(Cell.Y - MinerCell.Y);
		const float Gap = BandGap(DistanceSquared);
		if (Gap < NearestGap)
		{
			NearestGap = Gap;
			NearestSquared = DistanceSquared;
			Nearest = Cell;
		}
		if (!FootprintFits(ReachableFloor, Cell, Footprint))
		{
			continue;
		}
		if (Gap < NearestFittingGap)
		{
			NearestFittingGap = Gap;
			NearestFittingSquared = DistanceSquared;
			NearestFitting = Cell;
		}
		if (DistanceSquared >= MinSquared && DistanceSquared <= MaxSquared)
		{
			Candidates.Add(Cell);
		}
	}
	if (!Candidates.IsEmpty())
	{
		OutCell = Candidates[RandomStream.RandRange(0, Candidates.Num() - 1)];
		return true;
	}
	if (NearestFittingSquared >= 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no %d x %d cell spot of reachable floor %.0f to %.0f cells from the miner; shuttle lands on the nearest one outside that band (%d, %d), %.1f cells."),
			Footprint.X, Footprint.Y, MinCells, MaxCells, NearestFitting.X, NearestFitting.Y, FMath::Sqrt(static_cast<float>(NearestFittingSquared)));
		OutCell = NearestFitting;
		return true;
	}
	if (NearestSquared < 0)
	{
		return false;
	}
	UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no %d x %d cell spot of reachable floor; shuttle lands on the reachable floor cell nearest to %.0f to %.0f cells (%d, %d), %.1f cells (mesh may overlap blocks)."),
		Footprint.X, Footprint.Y, MinCells, MaxCells, Nearest.X, Nearest.Y, FMath::Sqrt(static_cast<float>(NearestSquared)));
	OutCell = Nearest;
	return true;
}

ARRShuttle* URRRunFlowSubsystem::LandShuttle()
{
	if (Shuttle || bRunEnded)
	{
		return Shuttle;
	}
	FIntPoint Cell;
	if (!FindShuttleCell(Cell))
	{
		if (!bShuttleFailureLogged)
		{
			bShuttleFailureLogged = true;
			UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no reachable floor cell for the shuttle; landing retried every tick."));
		}
		return nullptr;
	}

	UClass* ShuttleClass = Config.ShuttleClass ? Config.ShuttleClass.Get() : ARRShuttle::StaticClass();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARRShuttle* NewShuttle = GetWorld()->SpawnActor<ARRShuttle>(ShuttleClass, Grid->GetCellCenter(Cell.X, Cell.Y), FRotator::ZeroRotator, Params);
	if (!NewShuttle)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunFlowSubsystem: could not spawn shuttle %s."), *GetNameSafe(ShuttleClass));
		return nullptr;
	}
	Shuttle = NewShuttle;
	NewShuttle->OnShuttleCountdownChanged.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleShuttleCountdownChanged);
	NewShuttle->OnShuttleFinished.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleShuttleFinished);

	const float CheatCountdown = GetDefault<URRCheatSettings>()->ShuttleCountdownOverride;
	const float Countdown = CheatCountdown > 0.f ? CheatCountdown : Config.ShuttleCountdownSeconds;
	// One countdown subtitle per countdown start (OnShuttleCountdown fires every frame): shown before the first countdown event.
	PickAlertLine(FRRTextContentLibrary::AlertShuttleCountdown);
	NewShuttle->InitializeShuttle(Grid, Miner, Cell, Countdown, Config.ShuttleBoardRadiusCells * Grid->GetCellSize());
	OnShuttleLanded.Broadcast(NewShuttle);
	UpdateLocatorTarget();
	RefreshAlertBarEvents();
	return NewShuttle;
}

bool URRRunFlowSubsystem::EndRun(bool bSuccess)
{
	if (bRunEnded)
	{
		return false;
	}
	ResolveRunActors();
	if (Miner && Miner->IsDead())
	{
		return false;
	}
	bRunEnded = true;
	bRunSucceeded = bSuccess;

	// The end screen wins over a pending or open level-up offer: close it (it unpauses) before pausing for the end.
	if (URRLevelUpComponent* LevelUp = Miner ? Miner->FindComponentByClass<URRLevelUpComponent>() : nullptr)
	{
		LevelUp->CancelChoices();
	}
	EnterEndPause();

	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: run ended, %s."), bSuccess ? TEXT("miner extracted") : TEXT("shuttle missed, run lost"));
	OnRunEnded.Broadcast(bSuccess);
	UpdateLocatorTarget();
	return true;
}

void URRRunFlowSubsystem::RestartLevel()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(World, true);
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: restarting level %s."), *LevelName);
	UGameplayStatics::OpenLevel(World, FName(*LevelName));
}

void URRRunFlowSubsystem::EnterEndPause()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!Controller)
	{
		return;
	}
	// Standard pause: actors, timers and this subsystem stop (robots, auto-fire); the controller and UMG keep running.
	if (!Controller->IsPaused())
	{
		Controller->SetPause(true);
	}
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Controller->SetInputMode(InputMode);
	Controller->SetShowMouseCursor(true);
}

void URRRunFlowSubsystem::HandleShuttleCountdownChanged(ARRShuttle* InShuttle, float RemainingSeconds)
{
	if (InShuttle == Shuttle)
	{
		OnShuttleCountdown.Broadcast(RemainingSeconds, InShuttle->GetActorLocation());
	}
}

void URRRunFlowSubsystem::HandleShuttleFinished(ARRShuttle* InShuttle, bool bExtracted)
{
	if (InShuttle == Shuttle)
	{
		EndRun(bExtracted);
	}
}

void URRRunFlowSubsystem::BroadcastAlertIfChanged()
{
	const float Alert = GetAlert();
	const bool bPaused = HasPendingObjective();
	if (Alert == LastBroadcastAlert && bPaused == bLastBroadcastPaused)
	{
		return;
	}
	LastBroadcastAlert = Alert;
	bLastBroadcastPaused = bPaused;
	OnAlertChanged.Broadcast(Alert, bPaused);
}

float URRRunFlowSubsystem::GetAlertGainMultiplier() const
{
	const float Multiplier = GetDefault<URRCheatSettings>()->AlertSpeedMultiplier;
	return Multiplier > 0.f ? Multiplier : 1.f;
}

void URRRunFlowSubsystem::HandleConsoleStateChanged(ARRConsole* InConsole, ERRConsoleState NewState)
{
	if (InConsole != Console)
	{
		return;
	}
	if (NewState == ERRConsoleState::Complete && !bConsoleDone)
	{
		bConsoleDone = true;
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: console hacked, alert resumes at %.2f."), GetAlert());
	}
	OnObjectiveChanged.Broadcast(InConsole, NewState, InConsole->GetMarkedRockCount(), InConsole->GetInitialMarkedRockCount());
	BroadcastAlertIfChanged();
	UpdateLocatorTarget();
	RefreshAlertBarEvents();
}

void URRRunFlowSubsystem::HandleConsoleProgressChanged(ARRConsole* InConsole, float Progress, bool bHacking)
{
	if (InConsole == Console)
	{
		OnObjectiveProgressChanged.Broadcast(Progress, bHacking);
	}
}

void URRRunFlowSubsystem::HandleConsoleMarkedRocksChanged(ARRConsole* InConsole, int32 Remaining, int32 Total)
{
	if (InConsole == Console)
	{
		OnObjectiveChanged.Broadcast(InConsole, InConsole->GetConsoleState(), Remaining, Total);
	}
}

void URRRunFlowSubsystem::PickAlertLine(const TCHAR* EventId)
{
	if (URRSubtitleSubsystem* Subtitles = URRSubtitleSubsystem::Get(this))
	{
		Subtitles->ShowAlertLine(FName(EventId));
	}
	else if (URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(this))
	{
		FText Unused;
		TextContent->GetAlertLine(FName(EventId), Unused);
	}
}

void URRRunFlowSubsystem::RegisterMiner(ARRMiner* InMiner)
{
	if (!IsValid(InMiner))
	{
		return;
	}
	if (!IsValid(Miner) || Miner->IsActorBeingDestroyed())
	{
		Miner = InMiner;
	}
	OnMinerRegistered.Broadcast(InMiner);
}

void URRRunFlowSubsystem::RegisterSpawner(ARREnemySpawner* InSpawner)
{
	if (!IsValid(InSpawner) || Spawner.IsValid())
	{
		return;
	}
	Spawner = InSpawner;
	DoneHordePositions.Reset();
	InSpawner->OnHordeIncoming.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleHordeIncoming);
	InSpawner->OnHordeLull.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleHordeLull);
	InSpawner->OnHordeStarted.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleHordeStarted);
	InSpawner->OnHordeFinished.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleHordeFinished);
	OnSpawnerRegistered.Broadcast(InSpawner);
	RefreshAlertBarEvents();
}

const TArray<FRRAlertBarEvent>& URRRunFlowSubsystem::GetAlertBarEvents()
{
	if (!bAlertBarEventsSent)
	{
		RefreshAlertBarEvents();
	}
	return AlertBarEvents;
}

void URRRunFlowSubsystem::RefreshAlertBarEvents()
{
	TArray<FRRAlertBarEvent> Events;
	ComputeAlertBarEvents(this, Spawner.Get(), DoneHordePositions, Events);
	if (bAlertBarEventsSent && !FRRAlertBarEvent::ListsDiffer(Events, AlertBarEvents, Config.AlertEventPositionTolerance))
	{
		return;
	}
	AlertBarEvents = MoveTemp(Events);
	bAlertBarEventsSent = true;
	OnAlertBarEventsChanged.Broadcast(AlertBarEvents);
}

float URRRunFlowSubsystem::EstimateBarPosition(float Alert, float SecondsUntil, float AlertFillSeconds)
{
	const float Current = FMath::Clamp(Alert, 0.f, 1.f);
	return FMath::Clamp(Current + FMath::Max(SecondsUntil, 0.f) / FMath::Max(AlertFillSeconds, 1.f), Current, 1.f);
}

void URRRunFlowSubsystem::ComputeAlertBarEvents(const URRRunFlowSubsystem* RunFlow, const ARREnemySpawner* InSpawner, TMap<int32, float>& InOutDoneHordePositions, TArray<FRRAlertBarEvent>& OutEvents)
{
	OutEvents.Reset();
	static const FRRRunFlowConfig DefaultConfig;
	const FRRRunFlowConfig& RunConfig = RunFlow ? RunFlow->GetConfig() : DefaultConfig;
	const float Alert = RunFlow ? RunFlow->GetAlert() : 0.f;

	FRRAlertBarEvent& ConsoleIcon = OutEvents.AddDefaulted_GetRef();
	ConsoleIcon.Type = ERRAlertBarEventType::Console;
	ConsoleIcon.Position = FMath::Clamp(RunConfig.ConsoleAlertThreshold, 0.f, 1.f);
	const ARRConsole* RunConsole = RunFlow ? RunFlow->GetConsole() : nullptr;
	ConsoleIcon.bDone = (RunConsole && RunConsole->GetConsoleState() == ERRConsoleState::Complete) || (RunFlow && RunFlow->IsExtractionReady());

	// Bonus chest (optional): at its threshold, done once hacked; no icon when the chest is disabled (threshold 0).
	if (RunConfig.BonusChestAlertThreshold > 0.f)
	{
		FRRAlertBarEvent& ChestIcon = OutEvents.AddDefaulted_GetRef();
		ChestIcon.Type = ERRAlertBarEventType::BonusChest;
		ChestIcon.Position = FMath::Clamp(RunConfig.BonusChestAlertThreshold, 0.f, 1.f);
		const ARRBonusChest* Chest = RunFlow ? RunFlow->GetBonusChest() : nullptr;
		ChestIcon.bDone = Chest && Chest->IsComplete();
	}

	if (InSpawner && InSpawner->IsDirectorEnabled())
	{
		const bool bAlertTrigger = InSpawner->GetHordeTrigger() == ERRHordeTrigger::Alert;
		const int32 ActiveHorde = InSpawner->GetActiveHorde();
		for (int32 Index = 0; Index < InSpawner->GetHordeCount(); ++Index)
		{
			FRRAlertBarEvent& HordeIcon = OutEvents.AddDefaulted_GetRef();
			HordeIcon.Type = ERRAlertBarEventType::Horde;
			HordeIcon.Index = Index;
			const ERRHordeState State = InSpawner->GetHordeState(Index);
			HordeIcon.bDone = State == ERRHordeState::Spawning || State == ERRHordeState::Done;
			if (bAlertTrigger)
			{
				// Alert trigger: the lull starts at a fixed alert.
				HordeIcon.Position = FMath::Clamp(InSpawner->GetHordeTriggerAlert(Index), 0.f, 1.f);
			}
			else if (HordeIcon.bDone)
			{
				// Frozen where the fill was when the front started (first seen now when bound after it).
				HordeIcon.Position = InOutDoneHordePositions.FindOrAdd(Index, Alert);
			}
			else
			{
				const float SecondsUntil = Index == ActiveHorde
					? InSpawner->GetSecondsUntilHordeFront()
					: InSpawner->GetHordeTriggerTime(Index) - InSpawner->GetDirector().GetRunTime();
				HordeIcon.Position = EstimateBarPosition(Alert, SecondsUntil, RunConfig.AlertFillSeconds);
			}
		}
	}

	FRRAlertBarEvent& ShuttleIcon = OutEvents.AddDefaulted_GetRef();
	ShuttleIcon.Type = ERRAlertBarEventType::Extraction;
	ShuttleIcon.Position = 1.f;
	ShuttleIcon.bDone = RunFlow && RunFlow->IsExtractionReady();
}

bool URRRunFlowSubsystem::GetLocatorTarget(ERRLocatorTarget& OutTarget, FVector& OutWorldLocation) const
{
	OutTarget = ERRLocatorTarget::None;
	OutWorldLocation = FVector::ZeroVector;
	if (bRunEnded)
	{
		return false;
	}
	if (IsValid(Console) && Console->GetConsoleState() != ERRConsoleState::Complete)
	{
		OutTarget = ERRLocatorTarget::Console;
		OutWorldLocation = Console->GetActorLocation();
		return true;
	}
	if (IsValid(Shuttle))
	{
		OutTarget = ERRLocatorTarget::Shuttle;
		OutWorldLocation = Shuttle->GetActorLocation();
		return true;
	}
	return false;
}

void URRRunFlowSubsystem::UpdateLocatorTarget()
{
	ERRLocatorTarget Target;
	FVector Location;
	GetLocatorTarget(Target, Location);
	if (Target == LastLocatorTarget && Location.Equals(LastLocatorLocation))
	{
		return;
	}
	LastLocatorTarget = Target;
	LastLocatorLocation = Location;
	OnLocatorTargetChanged.Broadcast(Target, Location);
}

void URRRunFlowSubsystem::HandleHordeIncoming(int32 HordeIndex, float SecondsUntil)
{
	OnHordeIncoming.Broadcast(HordeIndex, SecondsUntil);
	RefreshAlertBarEvents();
}

void URRRunFlowSubsystem::HandleHordeLull(int32 HordeIndex)
{
	RefreshAlertBarEvents();
}

void URRRunFlowSubsystem::HandleHordeStarted(int32 HordeIndex, ERRSpawnSide Side)
{
	DoneHordePositions.FindOrAdd(HordeIndex, GetAlert());
	RefreshAlertBarEvents();
}

void URRRunFlowSubsystem::HandleHordeFinished(int32 HordeIndex, int32 Spawned)
{
	RefreshAlertBarEvents();
}
