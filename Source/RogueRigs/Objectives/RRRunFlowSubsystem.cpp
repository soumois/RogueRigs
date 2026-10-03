// Rogue Rigs - run flow of a level: alert bar from time and kills, console drop and pause, extraction shuttle and run end.

#include "Objectives/RRRunFlowSubsystem.h"

#include "Debug/RRCheatSettings.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FlowField/RRFlowFieldSubsystem.h"
#include "Grid/RRArenaGrid.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "RogueRigs.h"

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

void URRRunFlowSubsystem::SetAlert(float NewAlert)
{
	ResolveConfig();
	AlertSeconds = FMath::Clamp(NewAlert, 0.f, 1.f) * FMath::Max(Config.AlertFillSeconds, 1.f);
	BroadcastAlertIfChanged();
}

ARRConsole* URRRunFlowSubsystem::SkipToConsole()
{
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
}

bool URRRunFlowSubsystem::CompleteConsoleHack()
{
	if (!Console || Console->IsComplete())
	{
		return false;
	}
	Console->ForceComplete();
	return true;
}

bool URRRunFlowSubsystem::SkipToExtraction()
{
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
}

bool URRRunFlowSubsystem::ResolveRunActors()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	if (!IsValid(Grid))
	{
		Grid = nullptr;
		for (TActorIterator<ARRArenaGrid> It(World); It; ++It)
		{
			Grid = *It;
			break;
		}
	}
	if (!IsValid(Miner) || Miner->IsActorBeingDestroyed())
	{
		Miner = Cast<ARRMiner>(UGameplayStatics::GetPlayerPawn(World, 0));
		if (!Miner)
		{
			for (TActorIterator<ARRMiner> It(World); It; ++It)
			{
				if (IsValid(*It) && !It->IsActorBeingDestroyed() && !It->IsDead())
				{
					Miner = *It;
					break;
				}
			}
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
	if (!Grid || !Miner)
	{
		return false;
	}
	URRFlowFieldSubsystem* FlowField = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>();
	const FIntPoint MinerCell = Grid->GetCellAtWorld(Miner->GetActorLocation());
	const FIntPoint Size = Grid->GetGridSize();
	const float MinSquared = FMath::Square(Config.ConsoleMinDistanceCells);
	const float MaxSquared = FMath::Square(Config.ConsoleMaxDistanceCells);

	TArray<FIntPoint> Candidates;
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (Grid->GetCellType(Cell) != ERRCellType::Floor)
			{
				continue;
			}
			const float DistanceSquared = static_cast<float>(FMath::Square(X - MinerCell.X) + FMath::Square(Y - MinerCell.Y));
			if (DistanceSquared < MinSquared || DistanceSquared > MaxSquared)
			{
				continue;
			}
			if (FlowField && FlowField->GetCellDistance(Cell) < 0)
			{
				continue;
			}
			Candidates.Add(Cell);
		}
	}
	if (Candidates.IsEmpty())
	{
		return false;
	}

	// Prefer cells with rocks to mark, so the console has something to clear.
	if (Config.bPreferCellsWithMarkedRocks)
	{
		const UClass* ConsoleClass = Config.ConsoleClass ? Config.ConsoleClass.Get() : ARRConsole::StaticClass();
		const ARRConsole* ConsoleDefaults = ConsoleClass->GetDefaultObject<ARRConsole>();
		TArray<FIntPoint> WithRocks = Candidates.FilterByPredicate([this, ConsoleDefaults](const FIntPoint& Cell)
		{
			return !ConsoleDefaults->FindMarkableCells(Grid, Cell).IsEmpty();
		});
		if (!WithRocks.IsEmpty())
		{
			Candidates = MoveTemp(WithRocks);
		}
	}
	OutCell = Candidates[RandomStream.RandRange(0, Candidates.Num() - 1)];
	return true;
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
	OnConsoleDropped.Broadcast(NewConsole);
	return NewConsole;
}

void URRRunFlowSubsystem::CheckExtraction()
{
	if (bExtractionReady || HasPendingObjective() || GetAlert() < 1.f)
	{
		return;
	}
	bExtractionReady = true;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: alert full, no objective pending: extraction ready."));
	OnExtractionReady.Broadcast();
	LandShuttle();
}

bool URRRunFlowSubsystem::FindShuttleCell(FIntPoint& OutCell)
{
	if (!Grid || !Miner)
	{
		return false;
	}
	URRFlowFieldSubsystem* FlowField = GetWorld()->GetSubsystem<URRFlowFieldSubsystem>();
	const FIntPoint MinerCell = Grid->GetCellAtWorld(Miner->GetActorLocation());
	const FIntPoint Size = Grid->GetGridSize();
	const int32 MinSquared = FMath::CeilToInt(FMath::Square(Config.ShuttleMinDistanceCells));

	TArray<FIntPoint> Candidates;
	FIntPoint Farthest(INDEX_NONE, INDEX_NONE);
	int32 FarthestSquared = -1;
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (Grid->GetCellType(Cell) != ERRCellType::Floor || (FlowField && FlowField->GetCellDistance(Cell) < 0))
			{
				continue;
			}
			const int32 DistanceSquared = FMath::Square(X - MinerCell.X) + FMath::Square(Y - MinerCell.Y);
			if (DistanceSquared >= MinSquared)
			{
				Candidates.Add(Cell);
			}
			if (DistanceSquared > FarthestSquared)
			{
				FarthestSquared = DistanceSquared;
				Farthest = Cell;
			}
		}
	}
	if (!Candidates.IsEmpty())
	{
		OutCell = Candidates[RandomStream.RandRange(0, Candidates.Num() - 1)];
		return true;
	}
	if (FarthestSquared < 0)
	{
		return false;
	}
	UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no reachable floor cell %.0f cells from the miner; shuttle lands on the farthest one (%d, %d), %.1f cells."),
		Config.ShuttleMinDistanceCells, Farthest.X, Farthest.Y, FMath::Sqrt(static_cast<float>(FarthestSquared)));
	OutCell = Farthest;
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
	NewShuttle->InitializeShuttle(Grid, Miner, Cell, Countdown, Config.ShuttleBoardRadiusCells * Grid->GetCellSize());
	OnShuttleLanded.Broadcast(NewShuttle);
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
