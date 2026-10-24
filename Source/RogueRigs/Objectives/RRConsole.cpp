// Rogue Rigs - command console objective: marks the rocks around it, then is hacked by the drone while the miner stays in its zone.

#include "Objectives/RRConsole.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Drone/RRDrone.h"
#include "Objectives/RRLandingFootprint.h"
#include "Engine/CollisionProfile.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/RRMiner.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "RogueRigs.h"

namespace RRConsoleVisuals
{
	/** Parameters of M_ConsoleLamp and M_ConsoleScreen (bp-objective-console). */
	static const FName LampColorParam(TEXT("LampColor"));
	static const FName BlinkRateParam(TEXT("BlinkRate"));
	static const FName BlinkAmountParam(TEXT("BlinkAmount"));
	static const FName ScreenColorParam(TEXT("ScreenColor"));
}

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

void ARRConsole::BeginPlay()
{
	Super::BeginPlay();
	ApplyStateVisuals();
}

const FRRConsoleStateVisual& ARRConsole::GetStateVisual(ERRConsoleState State) const
{
	switch (State)
	{
	case ERRConsoleState::Idle:
		return IdleVisual;
	case ERRConsoleState::Hacking:
		return HackingVisual;
	case ERRConsoleState::Complete:
		return CompleteVisual;
	default:
		return LockedVisual;
	}
}

void ARRConsole::SetupStateVisuals()
{
	if (bStateVisualsSetUp || !bDriveStateVisuals)
	{
		return;
	}
	bStateVisualsSetUp = true;

	TInlineComponentArray<UStaticMeshComponent*> MeshComponents(this);
	for (UStaticMeshComponent* Component : MeshComponents)
	{
		if (Component && Component->GetFName() == AntennaHeadName)
		{
			AntennaHeadComp = Component;
			break;
		}
	}

	// A slot the mesh lacks is skipped (INDEX_NONE): a console without the C2 mesh (headless tests) simply shows nothing.
	auto MakeMID = [](UStaticMeshComponent* Component, FName SlotName) -> UMaterialInstanceDynamic*
	{
		if (!Component || SlotName.IsNone())
		{
			return nullptr;
		}
		const int32 Index = Component->GetMaterialIndex(SlotName);
		return Index != INDEX_NONE && Component->GetMaterial(Index) ? Component->CreateDynamicMaterialInstance(Index) : nullptr;
	};
	OtherLampMID = MakeMID(Mesh, OtherLampSlotName);
	ScreenMID = MakeMID(Mesh, ScreenSlotName);
	AntennaLampMID = MakeMID(AntennaHeadComp, AntennaLampSlotName);
	UE_LOG(LogRogueRigs, Verbose, TEXT("%s: state visuals head %s, antenna lamp %s, other lamp %s, screen %s."), *GetName(),
		AntennaHeadComp ? TEXT("yes") : TEXT("no"), AntennaLampMID ? TEXT("yes") : TEXT("no"), OtherLampMID ? TEXT("yes") : TEXT("no"), ScreenMID ? TEXT("yes") : TEXT("no"));
}

void ARRConsole::ApplyStateVisuals()
{
	if (!bDriveStateVisuals)
	{
		return;
	}
	SetupStateVisuals();

	const FRRConsoleStateVisual& Visual = GetStateVisual(ConsoleState);
	if (AntennaLampMID)
	{
		AntennaLampMID->SetVectorParameterValue(RRConsoleVisuals::LampColorParam, Visual.LampColor);
		AntennaLampMID->SetScalarParameterValue(RRConsoleVisuals::BlinkRateParam, Visual.AntennaBlinkRate);
		AntennaLampMID->SetScalarParameterValue(RRConsoleVisuals::BlinkAmountParam, Visual.AntennaBlinkRate > 0.f ? AntennaBlinkAmount : 0.f);
	}
	if (OtherLampMID)
	{
		OtherLampMID->SetVectorParameterValue(RRConsoleVisuals::LampColorParam, Visual.LampColor);
		OtherLampMID->SetScalarParameterValue(RRConsoleVisuals::BlinkRateParam, 0.f);
		OtherLampMID->SetScalarParameterValue(RRConsoleVisuals::BlinkAmountParam, 0.f);
	}
	if (ScreenMID)
	{
		ScreenMID->SetVectorParameterValue(RRConsoleVisuals::ScreenColorParam, Visual.ScreenColor);
	}
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
	ApplyStateVisuals();
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

	if (AntennaHeadComp)
	{
		const float YawSpeed = GetStateVisual(ConsoleState).HeadYawSpeed;
		if (YawSpeed > 0.f)
		{
			AntennaHeadComp->AddLocalRotation(FRotator(0.f, YawSpeed * DeltaSeconds, 0.f));
		}
	}

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
	ApplyStateVisuals();
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
