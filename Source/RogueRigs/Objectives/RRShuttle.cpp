// Rogue Rigs - extraction shuttle: lands far from the miner, counts down, ends the run with a win (miner on board) or a loss (countdown over).

#include "Objectives/RRShuttle.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Grid/RRArenaGrid.h"
#include "Objectives/RRLandingFootprint.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

ARRShuttle::ARRShuttle()
{
	PrimaryActorTick.bCanEverTick = true;

	ShuttleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ShuttleRoot"));
	RootComponent = ShuttleRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(ShuttleRoot);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);
}

void ARRShuttle::InitializeShuttle(ARRArenaGrid* InGrid, ARRMiner* InMiner, FIntPoint InCell, float InCountdownSeconds, float InBoardRadius)
{
	Grid = InGrid;
	Miner = InMiner;
	Cell = InCell;
	CountdownSeconds = FMath::Max(InCountdownSeconds, 0.f);
	RemainingSeconds = CountdownSeconds;
	BoardRadius = FMath::Max(InBoardRadius, 0.f);
	bInitialized = true;
	if (Grid)
	{
		SetActorLocation(RRLandingFootprint::GetCenter(Grid, Cell, FootprintCells));
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: shuttle landed on (%d, %d), countdown %.1f s."), *GetName(), Cell.X, Cell.Y, CountdownSeconds);
	OnLanded();
	BroadcastCountdown();
}

void ARRShuttle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bInitialized || bFinished)
	{
		return;
	}
	// A dead miner: the game over owns the end of the run, the countdown freezes.
	if (!IsValid(Miner) || Miner->IsDead())
	{
		return;
	}
	if (FVector::DistSquared2D(Miner->GetActorLocation(), GetActorLocation()) <= FMath::Square(BoardRadius))
	{
		Finish(true);
		return;
	}
	RemainingSeconds = FMath::Max(RemainingSeconds - DeltaSeconds, 0.f);
	BroadcastCountdown();
	if (RemainingSeconds <= 0.f)
	{
		Finish(false);
	}
}

void ARRShuttle::Finish(bool bInExtracted)
{
	if (bFinished)
	{
		return;
	}
	bFinished = true;
	bExtracted = bInExtracted;
	UE_LOG(LogRogueRigs, Log, TEXT("%s: %s (%.1f s left)."), *GetName(), bExtracted ? TEXT("miner extracted") : TEXT("countdown over, shuttle left"), RemainingSeconds);
	if (bExtracted)
	{
		OnExtracted();
	}
	else
	{
		OnMissed();
	}
	OnShuttleFinished.Broadcast(this, bExtracted);
}

void ARRShuttle::BroadcastCountdown()
{
	OnCountdown(RemainingSeconds);
	OnShuttleCountdownChanged.Broadcast(this, RemainingSeconds);
}
