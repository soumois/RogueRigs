// Rogue Rigs - extraction shuttle: lands far from the miner, counts down, ends the run with a win (miner on board) or a loss (countdown over).

#include "Objectives/RRShuttle.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Grid/RRArenaGrid.h"
#include "NiagaraComponent.h"
#include "Objectives/RRLandingFootprint.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

ARRShuttle::ARRShuttle()
{
	PrimaryActorTick.bCanEverTick = true;

	ShuttleRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ShuttleRoot"));
	RootComponent = ShuttleRoot;

	// task-377 Q2: 1608 x 1608 x 400 uu centred on the actor, bottom on the floor top (the actor origin); edited in the Blueprint.
	BoardZone = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardZone"));
	BoardZone->SetupAttachment(ShuttleRoot);
	BoardZone->InitBoxExtent(FVector(804.f, 804.f, 200.f));
	BoardZone->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	BoardZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoardZone->SetCollisionObjectType(ECC_WorldDynamic);
	BoardZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoardZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoardZone->SetGenerateOverlapEvents(true);
	BoardZone->SetCanEverAffectNavigation(false);
	BoardZone->CanCharacterStepUpOn = ECB_No;
	BoardZone->SetHiddenInGame(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(ShuttleRoot);
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);

	RotorFL = CreateRotor(TEXT("RotorFL"));
	RotorFR = CreateRotor(TEXT("RotorFR"));
	RotorRL = CreateRotor(TEXT("RotorRL"));
	RotorRR = CreateRotor(TEXT("RotorRR"));
}

UStaticMeshComponent* ARRShuttle::CreateRotor(FName Name)
{
	UStaticMeshComponent* Rotor = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	Rotor->SetupAttachment(Mesh);
	Rotor->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Rotor->SetGenerateOverlapEvents(false);
	Rotor->SetCanEverAffectNavigation(false);
	return Rotor;
}

TArray<UStaticMeshComponent*> ARRShuttle::GetRotors() const
{
	return { RotorFL.Get(), RotorFR.Get(), RotorRL.Get(), RotorRR.Get() };
}

void ARRShuttle::InitializeShuttle(ARRArenaGrid* InGrid, ARRMiner* InMiner, FIntPoint InCell, float InCountdownSeconds)
{
	Grid = InGrid;
	Miner = InMiner;
	Cell = InCell;
	CountdownSeconds = FMath::Max(InCountdownSeconds, 0.f);
	RemainingSeconds = CountdownSeconds;
	// Placed before bInitialized: an overlap raised by this move is ignored, the check below covers a miner already inside.
	if (Grid)
	{
		SetActorLocation(RRLandingFootprint::GetCenter(Grid, Cell, FootprintCells));
	}
	bInitialized = true;
	UE_LOG(LogRogueRigs, Log, TEXT("%s: shuttle landed on (%d, %d), countdown %.1f s, board zone extent %s."), *GetName(), Cell.X, Cell.Y, CountdownSeconds,
		BoardZone ? *BoardZone->GetScaledBoxExtent().ToCompactString() : TEXT("none"));
	OnLanded();
	BroadcastCountdown();
	// A miner already inside the zone raises no begin-overlap event.
	if (BoardZone && IsValid(Miner) && BoardZone->IsOverlappingComponent(Miner->GetCapsuleComponent()))
	{
		TryBoard();
	}
}

void ARRShuttle::InitializeArrival(ARRArenaGrid* InGrid, FIntPoint InCell)
{
	Grid = InGrid;
	Miner = nullptr;
	Cell = InCell;
	CountdownSeconds = 0.f;
	RemainingSeconds = 0.f;
	// bInitialized stays false: no board and no countdown for the arrival; the zone is switched off too (task-377 Q5).
	bArrival = true;
	if (BoardZone)
	{
		BoardZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (Grid)
	{
		SetActorLocation(RRLandingFootprint::GetCenter(Grid, Cell, FootprintCells));
	}
}

void ARRShuttle::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (BoardZone)
	{
		BoardZone->OnComponentBeginOverlap.AddUniqueDynamic(this, &ARRShuttle::HandleBoardZoneBeginOverlap);
	}
}

void ARRShuttle::HandleBoardZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(Miner) && OtherActor == Miner && OtherComp == Miner->GetCapsuleComponent())
	{
		TryBoard();
	}
}

bool ARRShuttle::TryBoard()
{
	if (!bInitialized || bFinished || bTakingOff)
	{
		return false;
	}
	// A dead miner: the game over owns the end of the run.
	if (!IsValid(Miner) || Miner->IsDead())
	{
		return false;
	}
	Finish(true);
	return true;
}

void ARRShuttle::BeginPlay()
{
	Super::BeginPlay();
	RotorRevPerSecond = bTakingOff ? FlightRevPerSecond : GroundRevPerSecond;
	OnRotorSpeedChanged(GetRotorSpeedAlpha());
	OnHeightAboveFloorChanged(HeightAboveFloor);
}

float ARRShuttle::GetRotorSpeedAlpha() const
{
	return FlightRevPerSecond > 0.f ? FMath::Clamp(RotorRevPerSecond / FlightRevPerSecond, 0.f, 1.f) : 0.f;
}

void ARRShuttle::TickRotors(float DeltaSeconds)
{
	// Ramp: linear, from the ground to the flight speed in RampSeconds once the takeoff started.
	const float Target = bTakingOff ? FlightRevPerSecond : GroundRevPerSecond;
	if (RotorRevPerSecond != Target)
	{
		const float Span = FMath::Abs(FlightRevPerSecond - GroundRevPerSecond);
		const float Step = (RampSeconds > 0.f && Span > 0.f) ? Span / RampSeconds * DeltaSeconds : TNumericLimits<float>::Max();
		RotorRevPerSecond = FMath::Abs(Target - RotorRevPerSecond) <= Step ? Target : RotorRevPerSecond + FMath::Sign(Target - RotorRevPerSecond) * Step;
		if (RotorRevPerSecond == Target)
		{
			UE_LOG(LogRogueRigs, Log, TEXT("%s: rotors at %.2f rev/s (alpha %.2f)."), *GetName(), RotorRevPerSecond, GetRotorSpeedAlpha());
		}
		OnRotorSpeedChanged(GetRotorSpeedAlpha());
	}

	// Positive yaw about the rotor local Z = clockwise seen from above (Z up, X forward, Y right).
	const float DegreesThisFrame = RotorRevPerSecond * 360.f * DeltaSeconds;
	if (DegreesThisFrame == 0.f)
	{
		return;
	}
	const bool bClockwise[4] = { bRotorFLClockwise, bRotorFRClockwise, bRotorRLClockwise, bRotorRRClockwise };
	const TArray<UStaticMeshComponent*> Rotors = GetRotors();
	for (int32 Index = 0; Index < Rotors.Num(); ++Index)
	{
		if (Rotors[Index])
		{
			Rotors[Index]->AddLocalRotation(FRotator(0.f, bClockwise[Index] ? DegreesThisFrame : -DegreesThisFrame, 0.f));
		}
	}
}

void ARRShuttle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Always first: the rotors turn on the ground, during the takeoff and, once taking off, while the game is paused.
	TickRotors(DeltaSeconds);

	if (bTakingOff)
	{
		TickTakeoff(DeltaSeconds);
		return;
	}
	if (!bInitialized || bFinished)
	{
		return;
	}
	// A dead miner: the game over owns the end of the run, the countdown freezes.
	if (!IsValid(Miner) || Miner->IsDead())
	{
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
	// Finished is final: the zone no longer boards, and its box stays out of the takeoff view bounds (GetActorBounds counts colliding components).
	if (BoardZone)
	{
		BoardZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
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

void ARRShuttle::StartTakeoff()
{
	if (bTakingOff)
	{
		return;
	}
	bTakingOff = true;
	if (BoardZone)
	{
		BoardZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	TakeoffElapsed = 0.f;
	ForwardSpeed = 0.f;
	TakeoffStartLocation = GetActorLocation();
	TakeoffStartRotation = GetActorRotation();
	// The run end pauses the world: the takeoff must keep ticking behind the end screen.
	SetActorTickEnabled(true);
	SetTickableWhenPaused(true);
	// Niagara's batched tick stops with the world: solo, each rotor effect ticks on its own component, so it plays behind the end screen too (task-443).
	TInlineComponentArray<UNiagaraComponent*> Effects(this);
	for (UNiagaraComponent* Effect : Effects)
	{
		Effect->SetTickableWhenPaused(true);
		Effect->SetForceSolo(true);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: takeoff started (spin-up %.2f s, rise %.0f uu in %.2f s, pitch %.0f deg, acceleration %.0f uu/s2), world paused: %s."),
		*GetName(), SpinUpSeconds, RiseHeight, RiseSeconds, PitchDegrees, ForwardAcceleration, GetWorld() && GetWorld()->IsPaused() ? TEXT("yes") : TEXT("no"));
	OnTakeoffStarted();
}

void ARRShuttle::TickTakeoff(float DeltaSeconds)
{
	TakeoffElapsed += DeltaSeconds;
	const float RiseEnd = SpinUpSeconds + RiseSeconds;
	const float RiseAlpha = FMath::Clamp((TakeoffElapsed - SpinUpSeconds) / RiseSeconds, 0.f, 1.f);
	const float TiltAlpha = FMath::Clamp((TakeoffElapsed - RiseEnd) / TiltSeconds, 0.f, 1.f);

	// Rise first (smooth start and stop), then the nose tilts down and the shuttle accelerates along the horizontal heading of its nose,
	// climbing a little: following the tilted nose would take it into the floor (task-528).
	const FRotator Rotation(TakeoffStartRotation.Pitch - PitchDegrees * FMath::InterpEaseInOut(0.f, 1.f, TiltAlpha, 2.f), TakeoffStartRotation.Yaw, TakeoffStartRotation.Roll);
	FVector Location = TakeoffStartLocation + FVector::UpVector * RiseHeight * FMath::InterpEaseInOut(0.f, 1.f, RiseAlpha, 2.f);
	if (TakeoffElapsed > RiseEnd)
	{
		// Top of the rise on the first forward frame, then the position keeps integrating the speed along the current nose.
		Location = (TakeoffElapsed - DeltaSeconds <= RiseEnd) ? TakeoffStartLocation + FVector::UpVector * RiseHeight : GetActorLocation();
		ForwardSpeed += ForwardAcceleration * DeltaSeconds * TiltAlpha;
		const FVector Heading = FRotator(0.f, Rotation.Yaw, 0.f).Vector();
		Location += (Heading + FVector::UpVector * FMath::Max(ClimbRatio, 0.f)) * ForwardSpeed * DeltaSeconds;
	}
	SetActorLocationAndRotation(Location, Rotation);

	const float Height = FMath::Max(HeightAboveFloor, static_cast<float>(Location.Z - TakeoffStartLocation.Z));
	if (Height != HeightAboveFloor)
	{
		HeightAboveFloor = Height;
		OnHeightAboveFloorChanged(HeightAboveFloor);
	}

	if (TakeoffElapsed >= RiseEnd + TiltSeconds && IsOutOfPlayerView())
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: takeoff: out of the player view after %.2f s, removed."), *GetName(), TakeoffElapsed);
		FinishTakeoff();
	}
	else if (TakeoffElapsed >= TakeoffMaxSeconds)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: takeoff: safety time %.1f s reached (no viewport or still in view), removed."), *GetName(), TakeoffMaxSeconds);
		FinishTakeoff();
	}
}

bool ARRShuttle::IsOutOfPlayerView() const
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	const ULocalPlayer* LocalPlayer = Controller ? Controller->GetLocalPlayer() : nullptr;
	if (!LocalPlayer || !LocalPlayer->ViewportClient)
	{
		return false;
	}
	FVector2D ViewportSize = FVector2D::ZeroVector;
	LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
	if (ViewportSize.X <= 1.f || ViewportSize.Y <= 1.f)
	{
		return false;
	}
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	for (int32 Corner = 0; Corner < 8; ++Corner)
	{
		const FVector Point = Origin + FVector((Corner & 1) ? Extent.X : -Extent.X, (Corner & 2) ? Extent.Y : -Extent.Y, (Corner & 4) ? Extent.Z : -Extent.Z);
		FVector2D Screen;
		if (Controller->ProjectWorldLocationToScreen(Point, Screen, false)
			&& Screen.X >= 0.f && Screen.X <= ViewportSize.X && Screen.Y >= 0.f && Screen.Y <= ViewportSize.Y)
		{
			return false;
		}
	}
	return true;
}

void ARRShuttle::FinishTakeoff()
{
	if (!bTakingOff)
	{
		return;
	}
	bTakingOff = false;
	OnTakeoffFinished();
	OnShuttleTakeoffFinished.Broadcast(this);
	Destroy();
}
