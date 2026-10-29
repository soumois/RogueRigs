// Rogue Rigs - run flow of a level: alert bar from time and kills, console drop and pause, extraction shuttle and run end.

#include "Objectives/RRRunFlowSubsystem.h"

#include "Run/RRRunSubsystem.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "RunLog/RRRunLogSubsystem.h"

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
#include "Objectives/RROreBankSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "RogueRigs.h"
#include "Text/RRSubtitleSubsystem.h"
#include "Text/RRTextContentSubsystem.h"
#include "Weapons/RRWeapon.h"
#include "World/RRWorld.h"

void URRRunFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RandomStream.Initialize(FMath::Rand());
}

void URRRunFlowSubsystem::Deinitialize()
{
	StopWaitingForArrivalFade();
	Super::Deinitialize();
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
	TickArrival();
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

bool URRRunFlowSubsystem::FindObjectiveCell(const ARRConsole* Defaults, float MinCells, float MaxCells, bool bApplyMarkedRange, const TCHAR* What, FIntPoint& OutCell)
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

	// Marked-block range: keep spots whose zone holds [MinMarkedCells, MaxMarkedCells] markable blocks; none = the nearest to the range.
	if (bApplyMarkedRange && Defaults->UsesMarkedCellRange())
	{
		const int32 MinMarked = Defaults->GetMinMarkedCells();
		const int32 MaxMarked = FMath::Max(MinMarked, Defaults->GetMaxMarkedCells());
		TArray<FIntPoint> InRange;
		TArray<FIntPoint> Nearest;
		int32 NearestGap = MAX_int32;
		int32 NearestCount = 0;
		for (const FIntPoint& Cell : Candidates)
		{
			const int32 Count = Defaults->FindMarkableCells(Grid, Cell).Num();
			const int32 Gap = Count < MinMarked ? MinMarked - Count : (Count > MaxMarked ? Count - MaxMarked : 0);
			if (Gap == 0)
			{
				InRange.Add(Cell);
			}
			else if (InRange.IsEmpty())
			{
				if (Gap < NearestGap)
				{
					NearestGap = Gap;
					NearestCount = Count;
					Nearest.Reset();
				}
				if (Gap == NearestGap)
				{
					Nearest.Add(Cell);
				}
			}
		}
		if (!InRange.IsEmpty())
		{
			Candidates = MoveTemp(InRange);
		}
		else
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("URRRunFlowSubsystem: no %s spot holds %d to %d markable blocks; landing on the nearest (%d blocks)."),
				What, MinMarked, MaxMarked, NearestCount);
			Candidates = MoveTemp(Nearest);
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
		/*bApplyMarkedRange*/ false, TEXT("bonus chest"), Cell))
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
	// task-372-01: the blocks under the landing footprint are destroyed before the extractor is spawned (FindShuttleCell picks
	// an open footprint when one exists, so this usually finds nothing; the fallback cell may overlap blocks).
	const FIntPoint Size = RRLandingFootprint::ClampSize(ShuttleClass->GetDefaultObject<ARRShuttle>()->GetFootprintCells());
	const URRRunSubsystem* Run = URRRunSubsystem::Get(this);
	Grid->ClearExtractorFootprint(RRLandingFootprint::GetMinCell(Cell, Size), Size,
		FString::Printf(TEXT("extraction landing on (%d, %d), run level %d"), Cell.X, Cell.Y, Run ? Run->GetLevelIndex() : 0));

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

ARRShuttle* URRRunFlowSubsystem::GetArrivalShuttle() const
{
	return IsValid(ArrivalShuttle) ? ArrivalShuttle.Get() : nullptr;
}

void URRRunFlowSubsystem::TickArrival()
{
	if (!bArrivalPlaced)
	{
		ResolveConfig();
		if (!ResolveRunActors())
		{
			return;
		}
		PlaceArrivalShuttle();
	}
	// No fade awaited: the level plays (this subsystem does not tick while paused), the arrival takes off now.
	if (bArrivalPlaced && !bArrivalWaitsForFade && !bArrivalTakeoffStarted)
	{
		StartArrivalTakeoff(TEXT("level start"));
	}
}

ARRShuttle* URRRunFlowSubsystem::PlaceArrivalShuttle()
{
	if (bArrivalPlaced)
	{
		return GetArrivalShuttle();
	}
	ResolveConfig();
	if (!ResolveRunActors())
	{
		return nullptr;
	}
	bArrivalPlaced = true;

	UClass* ShuttleClass = Config.ShuttleClass ? Config.ShuttleClass.Get() : ARRShuttle::StaticClass();
	const FIntPoint Size = RRLandingFootprint::ClampSize(ShuttleClass->GetDefaultObject<ARRShuttle>()->GetFootprintCells());
	const FIntPoint StartCell = Grid->GetCellAtWorld(Miner->GetActorLocation());

	// Screen up (camera yaw) in grid space, snapped to the nearest grid axis.
	const FVector Up = Grid->GetActorTransform().InverseTransformVectorNoScale(Miner->GetScreenUpDirection());
	const FIntPoint Step = FMath::Abs(Up.X) >= FMath::Abs(Up.Y) ? FIntPoint(Up.X >= 0.f ? 1 : -1, 0) : FIntPoint(0, Up.Y >= 0.f ? 1 : -1);

	// Footprint: its nearest row ArrivalDistanceCells from the start cell along Step, centred on the start cell across it.
	const int32 Distance = FMath::Max(1, Config.ArrivalDistanceCells);
	auto MinAlong = [Distance](int32 Start, int32 Dir, int32 Extent)
	{
		return Dir > 0 ? Start + Distance : (Dir < 0 ? Start - Distance - (Extent - 1) : Start - (Extent - 1) / 2);
	};
	const FIntPoint MinCell(MinAlong(StartCell.X, Step.X, Size.X), MinAlong(StartCell.Y, Step.Y, Size.Y));
	const FIntPoint Anchor = MinCell + FIntPoint((Size.X - 1) / 2, (Size.Y - 1) / 2);

	TArray<FIntPoint> FootprintCells;
	RRLandingFootprint::GetCells(Anchor, Size, FootprintCells);
	int32 BlockCells = 0;
	for (const FIntPoint& Cell : FootprintCells)
	{
		BlockCells += Grid->IsBlockCell(Cell) ? 1 : 0;
	}
	// task-372-01: the blocks under the footprint are destroyed before the extractor is spawned (drops at rest, no barrel blast).
	const URRRunSubsystem* Run = URRRunSubsystem::Get(this);
	Grid->ClearExtractorFootprint(MinCell, Size, FString::Printf(TEXT("arrival, run level %d"), Run ? Run->GetLevelIndex() : 0));

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARRShuttle* NewArrival = GetWorld()->SpawnActor<ARRShuttle>(ShuttleClass, RRLandingFootprint::GetCenter(Grid, Anchor, Size), FRotator::ZeroRotator, Params);
	if (!NewArrival)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("URRRunFlowSubsystem: could not spawn the arrival extractor %s."), *GetNameSafe(ShuttleClass));
		return nullptr;
	}
	ArrivalShuttle = NewArrival;
	NewArrival->InitializeArrival(Grid, Anchor);

	// A level transition running or fading in: the takeoff waits for the end of the fade in; otherwise it starts when the level plays.
	URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	bArrivalWaitsForFade = Transition && (Transition->GetPhase() != ERRRunTransitionPhase::None || Transition->GetScreenFade() == ERRTransitionFade::FadingIn);
	if (bArrivalWaitsForFade)
	{
		ArrivalFadeSource = Transition;
		Transition->OnScreenFadeFinished.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleScreenFadeFinished);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: arrival extractor %s placed on (%d, %d), footprint %d x %d from (%d, %d), start cell (%d, %d), screen up (%d, %d), %d block cells under it, frame %llu; takeoff %s."),
		*NewArrival->GetName(), Anchor.X, Anchor.Y, Size.X, Size.Y, MinCell.X, MinCell.Y, StartCell.X, StartCell.Y, Step.X, Step.Y, BlockCells, GFrameCounter,
		bArrivalWaitsForFade ? TEXT("waits for the fade in") : TEXT("when the level plays"));
	return NewArrival;
}

void URRRunFlowSubsystem::StartArrivalTakeoff(const TCHAR* Trigger)
{
	StopWaitingForArrivalFade();
	if (bArrivalTakeoffStarted)
	{
		return;
	}
	bArrivalTakeoffStarted = true;
	ARRShuttle* Arrival = GetArrivalShuttle();
	if (!Arrival)
	{
		return;
	}
	// Controls: the controller ignores gameplay input only while a level is loading (ARRPlayerController::IsGameplayInputHeld).
	const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	const UWorld* World = GetWorld();
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: arrival extractor %s takeoff started on (%d, %d) (trigger: %s), frame %llu; world %s, controls %s, miner %s."),
		*Arrival->GetName(), Arrival->GetCell().X, Arrival->GetCell().Y, Trigger, GFrameCounter, World && World->IsPaused() ? TEXT("paused") : TEXT("playing"),
		Transition && Transition->IsLevelLoading() ? TEXT("held") : TEXT("free"), Miner ? *Miner->GetActorLocation().ToCompactString() : TEXT("none"));
	Arrival->StartTakeoff();
}

void URRRunFlowSubsystem::StopWaitingForArrivalFade()
{
	bArrivalWaitsForFade = false;
	if (URRRunTransitionSubsystem* Transition = ArrivalFadeSource.Get())
	{
		Transition->OnScreenFadeFinished.RemoveDynamic(this, &URRRunFlowSubsystem::HandleScreenFadeFinished);
	}
	ArrivalFadeSource.Reset();
}

void URRRunFlowSubsystem::HandleScreenFadeFinished(ERRTransitionFade Fade)
{
	if (Fade == ERRTransitionFade::FadingIn && bArrivalWaitsForFade)
	{
		StartArrivalTakeoff(TEXT("fade in finished"));
	}
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
	// Run log (sys-run-log): the level entry is captured before the ore moves and before the intermission advances the run level.
	if (URRRunLogSubsystem* RunLog = URRRunLogSubsystem::Get(this))
	{
		RunLog->HandleLevelEnded(GetWorld(), bSuccess ? ERRRunLevelOutcome::Extracted : ERRRunLevelOutcome::MissedShuttle);
	}
	// Ore before the end event, so an end screen bound to OnRunEnded reads the final counts.
	if (bSuccess)
	{
		SecureCarriedOre();
	}
	else
	{
		LoseCarriedOre(TEXT("shuttle missed"));
	}

	// The end screen wins over a pending or open level-up offer: close it (it unpauses) before pausing for the end.
	if (URRLevelUpComponent* LevelUp = Miner ? Miner->FindComponentByClass<URRLevelUpComponent>() : nullptr)
	{
		LevelUp->CancelChoices();
	}
	EnterEndPause();

	// Success: the miner and his drone disappear and the extractor takes off behind the end screen (it ticks while paused, task-370-01).
	if (bSuccess)
	{
		if (Miner)
		{
			Miner->HideForExtraction();
		}
		if (IsValid(Shuttle))
		{
			Shuttle->StartTakeoff();
		}
	}

	// A success during a run (task-206-03, task-317-01) opens the stats screen instead of the success end screen; on the last level
	// the stats screen's button calls ShowRunWonEndScreen.
	URRRunTransitionSubsystem* Transition = bSuccess ? URRRunTransitionSubsystem::Get(this) : nullptr;
	if (Transition && Transition->HandleLevelExtracted(Miner))
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: level ended, miner extracted: stats screen (no end screen yet)%s."), Transition->IsRunWon() ? TEXT(", run won") : TEXT(""));
		UpdateLocatorTarget();
		return true;
	}

	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: run ended, %s."), bSuccess ? (Transition && Transition->IsRunWon() ? TEXT("miner extracted, run won") : TEXT("miner extracted"))
		: TEXT("shuttle missed, run lost"));
	OnRunEnded.Broadcast(bSuccess);
	UpdateLocatorTarget();
	return true;
}

bool URRRunFlowSubsystem::ShowRunWonEndScreen()
{
	if (!bRunEnded || !bRunSucceeded)
	{
		return false;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: run ended, miner extracted, run won (after the stats screen)."));
	OnRunEnded.Broadcast(true);
	return true;
}

void URRRunFlowSubsystem::RestartLevel()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	// Run lost or won (task-206-03): the restart begins a new run at level 1.
	if (URRRunSubsystem* Run = URRRunSubsystem::Get(this))
	{
		Run->ResetRun();
	}
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(World, true);
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: restarting level %s."), *LevelName);
	UGameplayStatics::OpenLevel(World, FName(*LevelName));
}

void URRRunFlowSubsystem::ResetForNextLevel()
{
	for (AActor* Objective : TArray<AActor*>{ Console.Get(), BonusChest.Get(), Shuttle.Get(), ArrivalShuttle.Get() })
	{
		if (IsValid(Objective))
		{
			Objective->Destroy();
		}
	}
	Console = nullptr;
	BonusChest = nullptr;
	Shuttle = nullptr;
	// The next level places its own arrival (URRRunTransitionSubsystem::PlaceMinerAtStart).
	StopWaitingForArrivalFade();
	ArrivalShuttle = nullptr;
	bArrivalPlaced = false;
	bArrivalTakeoffStarted = false;
	AlertSeconds = 0.f;
	KillCount = 0;
	LastBroadcastAlert = -1.f;
	bLastBroadcastPaused = false;
	bConsoleTriggered = false;
	bConsoleDone = false;
	bExtractionReady = false;
	bBonusChestTriggered = false;
	bBonusChestFailureLogged = false;
	bRunEnded = false;
	bRunSucceeded = false;
	bShuttleFailureLogged = false;
	bDropFailureLogged = false;
	CarriedOre = 0;
	bCarriedOreClosed = false;
	DoneHordePositions.Reset();
	AlertBarEvents.Reset();
	bAlertBarEventsSent = false;
	LastLocatorTarget = ERRLocatorTarget::None;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: reset for the next level in the same world (objectives removed, alert 0)."));
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
	InMiner->OnDeath.AddUniqueDynamic(this, &URRRunFlowSubsystem::HandleMinerDeath);
	OnMinerRegistered.Broadcast(InMiner);
}

int32 URRRunFlowSubsystem::GetSecuredOre() const
{
	const URROreBankSubsystem* Bank = URROreBankSubsystem::Get(this);
	return Bank ? Bank->GetSecuredOre() : LocalSecuredOre;
}

bool URRRunFlowSubsystem::AddCarriedOre(int32 Amount)
{
	if (Amount <= 0 || bCarriedOreClosed)
	{
		UE_LOG(LogRogueRigs, Verbose, TEXT("URRRunFlowSubsystem: %d ore refused (%s)."), Amount, bCarriedOreClosed ? TEXT("run ended or miner dead") : TEXT("not positive"));
		return false;
	}
	CarriedOre += Amount;
	UE_LOG(LogRogueRigs, Verbose, TEXT("URRRunFlowSubsystem: +%d ore, carried %d."), Amount, CarriedOre);
	OnOreChanged.Broadcast(CarriedOre, GetSecuredOre());
	return true;
}

bool URRRunFlowSubsystem::SpendOre(int32 Amount)
{
	if (Amount <= 0 || !CanSpendOre(Amount))
	{
		UE_LOG(LogRogueRigs, Verbose, TEXT("URRRunFlowSubsystem: spending %d ore refused (spendable %d)."), Amount, GetSpendableOre());
		return false;
	}
	const int32 FromCarried = FMath::Min(Amount, CarriedOre);
	const int32 FromSecured = Amount - FromCarried;
	CarriedOre -= FromCarried;
	int32 Secured = GetSecuredOre();
	if (FromSecured > 0)
	{
		if (URROreBankSubsystem* Bank = URROreBankSubsystem::Get(this))
		{
			Secured = Bank->RemoveSecuredOre(FromSecured);
		}
		else
		{
			LocalSecuredOre -= FromSecured;
			Secured = LocalSecuredOre;
		}
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: %d ore spent (%d carried, %d secured), carried %d, secured %d."), Amount, FromCarried, FromSecured, CarriedOre, Secured);
	OnOreChanged.Broadcast(CarriedOre, Secured);
	return true;
}

void URRRunFlowSubsystem::SecureCarriedOre()
{
	bCarriedOreClosed = true;
	const int32 Amount = CarriedOre;
	if (Amount <= 0)
	{
		return;
	}
	CarriedOre = 0;
	int32 Secured = 0;
	if (URROreBankSubsystem* Bank = URROreBankSubsystem::Get(this))
	{
		Secured = Bank->AddSecuredOre(Amount);
	}
	else
	{
		LocalSecuredOre += Amount;
		Secured = LocalSecuredOre;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: extraction secured %d ore, secured %d."), Amount, Secured);
	OnOreChanged.Broadcast(CarriedOre, Secured);
}

void URRRunFlowSubsystem::LoseCarriedOre(const TCHAR* Reason)
{
	bCarriedOreClosed = true;
	const int32 Lost = CarriedOre;
	if (Lost <= 0)
	{
		return;
	}
	CarriedOre = 0;
	UE_LOG(LogRogueRigs, Log, TEXT("URRRunFlowSubsystem: %s, %d carried ore lost."), Reason, Lost);
	OnOreChanged.Broadcast(CarriedOre, GetSecuredOre());
}

void URRRunFlowSubsystem::HandleMinerDeath(AActor* DeathInstigator)
{
	LoseCarriedOre(TEXT("miner died"));
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
