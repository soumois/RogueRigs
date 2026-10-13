// Rogue Rigs - command console objective: marks the rocks around it, then is hacked by the drone while the miner stays in its zone.

#include "Objectives/RRConsole.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Drone/RRDrone.h"
#include "Objectives/RRLandingFootprint.h"
#include "Engine/CollisionProfile.h"
#include "Player/RRMiner.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "RogueRigs.h"

ARRConsole::ARRConsole()
{
	PrimaryActorTick.bCanEverTick = true;

	ConsoleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ConsoleRoot"));
	RootComponent = ConsoleRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(ConsoleRoot);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);

	GroundTelegraph = CreateDefaultSubobject<URRGroundTelegraphComponent>(TEXT("GroundTelegraph"));
	GroundTelegraph->SetupAttachment(ConsoleRoot);
}

void ARRConsole::InitializeConsole(ARRArenaGrid* InGrid, ARRMiner* InMiner, FIntPoint InCell)
{
	Grid = InGrid;
	Miner = InMiner;
	Cell = InCell;
	bInitialized = true;
	if (Grid)
	{
		SetActorLocation(RRLandingFootprint::GetCenter(Grid, Cell, FootprintCells));
	}

	MarkRocks();
	InitialMarkedRockCount = MarkedRocks.Num();
	UE_LOG(LogRogueRigs, Log, TEXT("%s: console on (%d, %d), %d marked rock(s)."), *GetName(), Cell.X, Cell.Y, InitialMarkedRockCount);

	BroadcastMarkedRocks();
	ConsoleState = MarkedRocks.IsEmpty() ? ERRConsoleState::Idle : ERRConsoleState::Locked;
	OnHackStateChanged(ConsoleState);
	OnConsoleStateChanged.Broadcast(this, ConsoleState);
	OnHackProgress(HackProgress, bDroneLinked);
	OnConsoleProgressChanged.Broadcast(this, HackProgress, bDroneLinked);

	if (GroundTelegraph)
	{
		GroundTelegraph->SetRadius(GetZoneRadius());
		RefreshGroundTelegraph();
		GroundTelegraph->Show();
	}
}

float ARRConsole::GetZoneRadius() const
{
	return Grid ? ZoneRadiusCells * Grid->GetCellSize() : 0.f;
}

void ARRConsole::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bInitialized || IsComplete())
	{
		return;
	}

	RefreshMarkedRocks();

	const bool bMinerAlive = IsValid(Miner) && !Miner->IsDead();
	bMinerInZone = bMinerAlive && FVector::DistSquared2D(Miner->GetActorLocation(), GetActorLocation()) <= FMath::Square(GetZoneRadius());

	if (!MarkedRocks.IsEmpty())
	{
		// Locked: nothing to hack yet.
		ReleaseDrone();
		SetProgress(HackProgress, false);
		SetConsoleState(ERRConsoleState::Locked);
		return;
	}

	ARRDrone* Drone = bMinerInZone ? Miner->GetDrone() : nullptr;
	if (Drone && Drone->StartHack(this))
	{
		const float NewProgress = FMath::Min(HackProgress + DeltaSeconds / FMath::Max(HackDuration, UE_KINDA_SMALL_NUMBER), 1.f);
		if (NewProgress >= 1.f)
		{
			CompleteHack();
			return;
		}
		SetProgress(NewProgress, true);
		SetConsoleState(ERRConsoleState::Hacking);
		return;
	}

	ReleaseDrone();
	SetProgress(FMath::Max(HackProgress - DrainPerSecond * DeltaSeconds, 0.f), false);
	SetConsoleState(ERRConsoleState::Idle);
}

void ARRConsole::ForceComplete()
{
#if !UE_BUILD_SHIPPING
	CompleteHack();
#endif
}

void ARRConsole::CompleteHack()
{
	if (IsComplete())
	{
		return;
	}
	if (Grid)
	{
		for (const FIntPoint& Rock : MarkedRocks)
		{
			Grid->SetCellMarked(Rock, false);
		}
	}
	if (!MarkedRocks.IsEmpty())
	{
		MarkedRocks.Reset();
		BroadcastMarkedRocks();
	}
	ReleaseDrone();
	SetProgress(1.f, false);
	SetConsoleState(ERRConsoleState::Complete);
	if (GroundTelegraph)
	{
		GroundTelegraph->Hide();
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: hack complete."), *GetName());
}

void ARRConsole::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GroundTelegraph)
	{
		GroundTelegraph->Hide();
	}
	ReleaseDrone();
	if (IsValid(Grid))
	{
		for (const FIntPoint& Rock : MarkedRocks)
		{
			Grid->SetCellMarked(Rock, false);
		}
	}
	MarkedRocks.Reset();
	Super::EndPlay(EndPlayReason);
}

TArray<FIntPoint> ARRConsole::FindMarkableCells(const ARRArenaGrid* InGrid, FIntPoint Center) const
{
	TArray<FIntPoint> Result;
	if (!InGrid)
	{
		return Result;
	}
	if (MarkedCellTypes.IsEmpty())
	{
		return Result;
	}
	// Zone centre in cell units: the footprint centre, where InitializeConsole places the actor (a cell corner for an even size).
	const FIntPoint ClampedFootprint = RRLandingFootprint::ClampSize(FootprintCells);
	const FIntPoint MinCell = RRLandingFootprint::GetMinCell(Center, ClampedFootprint);
	const double CenterX = MinCell.X + 0.5 * (ClampedFootprint.X - 1);
	const double CenterY = MinCell.Y + 0.5 * (ClampedFootprint.Y - 1);
	const double RadiusSquared = FMath::Square(static_cast<double>(ZoneRadiusCells));
	const int32 Reach = FMath::CeilToInt32(ZoneRadiusCells) + 1;
	const FIntPoint Size = InGrid->GetGridSize();
	for (int32 Y = Center.Y - Reach; Y <= Center.Y + Reach; ++Y)
	{
		for (int32 X = Center.X - Reach; X <= Center.X + Reach; ++X)
		{
			const FIntPoint Candidate(X, Y);
			if (Candidate.X < 0 || Candidate.Y < 0 || Candidate.X >= Size.X || Candidate.Y >= Size.Y)
			{
				continue;
			}
			if (FMath::Square(X - CenterX) + FMath::Square(Y - CenterY) > RadiusSquared)
			{
				continue;
			}
			// Every rock kind inside the zone is marked, ore deposits included (objective-console, owner 2026-10-13).
			if (MarkedCellTypes.Contains(InGrid->GetCellType(Candidate)))
			{
				Result.Add(Candidate);
			}
		}
	}
	return Result;
}

void ARRConsole::MarkRocks()
{
	MarkedRocks = FindMarkableCells(Grid, Cell);
	for (const FIntPoint& Rock : MarkedRocks)
	{
		// Already marked by another objective still counts for this one.
		Grid->SetCellMarked(Rock, true);
	}
}

void ARRConsole::RefreshMarkedRocks()
{
	if (!Grid)
	{
		return;
	}
	const int32 Removed = MarkedRocks.RemoveAll([this](const FIntPoint& Rock) { return !Grid->IsBlockCell(Rock); });
	if (Removed > 0)
	{
		BroadcastMarkedRocks();
	}
}

void ARRConsole::SetConsoleState(ERRConsoleState NewState)
{
	if (ConsoleState == NewState)
	{
		return;
	}
	ConsoleState = NewState;
	UE_LOG(LogRogueRigs, Verbose, TEXT("%s: state %s."), *GetName(), *UEnum::GetValueAsString(NewState));
	OnHackStateChanged(NewState);
	OnConsoleStateChanged.Broadcast(this, NewState);
}

void ARRConsole::SetProgress(float NewProgress, bool bNewHacking)
{
	if (NewProgress == HackProgress && bNewHacking == bDroneLinked)
	{
		return;
	}
	HackProgress = NewProgress;
	bDroneLinked = bNewHacking;
	RefreshGroundTelegraph();
	OnHackProgress(HackProgress, bDroneLinked);
	OnConsoleProgressChanged.Broadcast(this, HackProgress, bDroneLinked);
}

void ARRConsole::BroadcastMarkedRocks()
{
	RefreshGroundTelegraph();
	OnMarkedRocksChanged(MarkedRocks.Num(), InitialMarkedRockCount);
	OnConsoleMarkedRocksChanged.Broadcast(this, MarkedRocks.Num(), InitialMarkedRockCount);
}

void ARRConsole::ReleaseDrone()
{
	if (ARRDrone* Drone = IsValid(Miner) ? Miner->GetDrone() : nullptr)
	{
		Drone->StopHack(this);
	}
}

void ARRConsole::RefreshGroundTelegraph()
{
	if (!GroundTelegraph)
	{
		return;
	}
	GroundTelegraph->SetMode(MarkedRocks.IsEmpty() ? ERRGroundTelegraphMode::ZoneActive : ERRGroundTelegraphMode::ZoneBlocked);
	GroundTelegraph->SetProgress(HackProgress);
}
