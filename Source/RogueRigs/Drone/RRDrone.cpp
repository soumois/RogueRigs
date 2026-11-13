// Rogue Rigs - the helper drone: separate actor spawned by the miner; escorts it (Orbit state), cuts an arc of blocks with a laser (dig) and links to an objective (hack).

#include "Drone/RRDrone.h"

#include "Audio/RRAudioSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Enemies/RREnemy.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Objectives/RRShuttle.h"
#include "Grid/RRCellType.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Pool/RRNiagaraPoolSubsystem.h"
#include "Misc/App.h"
#include "UObject/ConstructorHelpers.h"
#include "RogueRigs.h"
#include "Telegraph/RRGroundTelegraphComponent.h"
#include "World/RRBlobShadowComponent.h"
#include "World/RRShadows.h"

ARRDrone::ARRDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);
	RootComponent = Mesh;

	// Animated parts (task-253-01): relative to the base, offsets read from the mesh bounds in Unreal axes (bp-drone-materials).
	LaserPivot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserPivot"));
	LaserPivot->SetupAttachment(Mesh);
	LaserPivot->SetRelativeLocation(FVector(1.606, 0.0, -17.248));
	LaserPivot->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LaserPivot->SetGenerateOverlapEvents(false);
	LaserPivot->SetCanEverAffectNavigation(false);
	LaserPivot->SetCastShadow(false);

	AntennaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AntennaMesh"));
	AntennaMesh->SetupAttachment(Mesh);
	AntennaMesh->SetRelativeLocation(FVector(-4.257, -0.837, 28.752));
	AntennaMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	AntennaMesh->SetGenerateOverlapEvents(false);
	AntennaMesh->SetCanEverAffectNavigation(false);
	AntennaMesh->SetCastShadow(false);

	// The laser is placed in world space every Dig tick, so it ignores the drone's transform.
	LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
	LaserMesh->SetupAttachment(Mesh);
	LaserMesh->SetUsingAbsoluteLocation(true);
	LaserMesh->SetUsingAbsoluteRotation(true);
	LaserMesh->SetUsingAbsoluteScale(true);
	LaserMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	LaserMesh->SetGenerateOverlapEvents(false);
	LaserMesh->SetCanEverAffectNavigation(false);
	LaserMesh->SetCastShadow(false);
	LaserMesh->SetVisibility(false);

	// One owned impact effect, moved every Dig tick (never spawned per frame); world-space placement like the laser.
	DigImpactComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DigImpactComponent"));
	DigImpactComponent->SetupAttachment(Mesh);
	DigImpactComponent->SetUsingAbsoluteLocation(true);
	DigImpactComponent->SetUsingAbsoluteRotation(true);
	DigImpactComponent->SetUsingAbsoluteScale(true);
	DigImpactComponent->bAutoActivate = false;
	DigImpactComponent->SetAutoDestroy(false);
	DigImpactComponent->SetCanEverAffectNavigation(false);

	// Constructor reference, so BP_Drone needs no edit; a Blueprint may override DigImpactSystem.
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DigImpactFinder(TEXT("/Game/RogueRigs/FX/Dig/NS_DigImpact.NS_DigImpact"));
	if (DigImpactFinder.Succeeded())
	{
		DigImpactSystem = DigImpactFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DigDebrisFinder(TEXT("/Game/RogueRigs/FX/Dig/NS_DigDebris.NS_DigDebris"));
	if (DigDebrisFinder.Succeeded())
	{
		DigDebrisSystem = DigDebrisFinder.Object;
	}

	// Thrusters (task-254-02): one owned component per side pod, relative to the base so they follow its yaw and tilt.
	ThrusterLeft = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterLeft"));
	ThrusterRight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterRight"));
	for (UNiagaraComponent* Thruster : { ThrusterLeft.Get(), ThrusterRight.Get() })
	{
		Thruster->SetupAttachment(Mesh);
		Thruster->bAutoActivate = false;
		Thruster->SetAutoDestroy(false);
		Thruster->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		Thruster->SetGenerateOverlapEvents(false);
		Thruster->SetCanEverAffectNavigation(false);
	}
	ThrusterLeft->SetRelativeTransform(ThrusterLeftOffset);
	ThrusterRight->SetRelativeTransform(ThrusterRightOffset);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ThrusterFinder(TEXT("/Game/RogueRigs/FX/Drone/NS_DroneThruster.NS_DroneThruster"));
	if (ThrusterFinder.Succeeded())
	{
		ThrusterSystem = ThrusterFinder.Object;
	}

	// Blob shadow (task-264-02): on the floor, not at the hover height, so the whole transform is world space (UpdateBlobShadow).
	BlobShadow = CreateDefaultSubobject<URRBlobShadowComponent>(TEXT("BlobShadow"));
	BlobShadow->SetupAttachment(Mesh);
	BlobShadow->SetUsingAbsoluteLocation(true);
	BlobShadow->SetUsingAbsoluteScale(true);
}

void ARRDrone::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// task-263-01: only the grid rocks cast shadows (from the player light); no mesh of this actor or its Blueprint does.
	RRShadows::DisableCastShadows(this);
	// task-312-01: ground telegraph decals (hack zone) stay on the ground and never tint the drone.
	URRGroundTelegraphComponent::DisableDecalsOn(this);
	BlobShadow->SetFootprintDiameter(BlobShadowFootprint);
}

void ARRDrone::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ThrusterLeft->SetRelativeTransform(ThrusterLeftOffset);
	ThrusterRight->SetRelativeTransform(ThrusterRightOffset);
}

void ARRDrone::SetOwnerMiner(ARRMiner* NewOwnerMiner)
{
	if (State == ERRDroneState::Dig)
	{
		StopDig();
		State = ERRDroneState::Orbit;
	}
	else if (State == ERRDroneState::Hack)
	{
		EndHack();
	}

	if (OwnerMiner)
	{
		RemoveTickPrerequisiteActor(OwnerMiner);
	}

	OwnerMiner = NewOwnerMiner;
	EscortBobPhase = 0.f;
	EscortBobSecondPhase = 0.f;
	PatrolVelocity = FVector::ZeroVector;
	EscortAlert = 0.f;
	bEnemyNearMiner = false;
	InterestActor.Reset();
	bHasInterestLocation = false;
	InterestRefreshTimer = 0.f;
	TiltPitch = 0.f;
	TiltRoll = 0.f;
	SmoothedMinerAcceleration = FVector::ZeroVector;
	IdleGesture = ERRDroneIdleGesture::None;
	IdleGestureTime = 0.f;
	// Own stream: the gestures never draw from the global FMath stream that tests seed (Ore.HealVein).
	GestureRandom.Initialize(static_cast<int32>(GetUniqueID()));
	// Patrol picks use their own stream too (another sequence than the gestures), so they never depend on the gestures.
	PatrolRandom.Initialize(static_cast<int32>(GetUniqueID()) * 7919 + 1);
	ResetIdleGestureCountdown();
	// The snap onto the patrol point below is not movement: the thrusters measure from the next tick.
	bHasThrusterLocation = false;
	ThrusterSpeed = 0.f;

	if (OwnerMiner)
	{
		// Follow the miner's position of this frame, not the previous one.
		AddTickPrerequisiteActor(OwnerMiner);
		PreviousMinerVelocity = OwnerMiner->GetVelocity();
		// The look starts along the miner's facing; the position never depends on it (task-251-01).
		LookYaw = OwnerMiner->GetActorRotation().Yaw;
		SetActorRotation(FRotator(0.f, LookYaw, 0.f));
		RestartPatrol();

		// The first broken block finds its components ready and the system compiled (task-438-01).
		URRNiagaraPoolSubsystem* FXPool = GetWorld() ? GetWorld()->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
		if (FXPool && DigDebrisSystem)
		{
			FXPool->Prewarm(DigDebrisSystem, DigDebrisPoolSize, DigDebrisPoolSize);
		}
	}
}

void ARRDrone::RestartPatrol()
{
	PatrolVelocity = FVector::ZeroVector;
	// Picked from the miner's own location: the drone is snapped onto the point right after.
	SetActorLocation(FVector(OwnerMiner->GetActorLocation().X, OwnerMiner->GetActorLocation().Y, GetActorLocation().Z));
	PickPatrolPoint();
	SetActorLocation(ComputeOrbitPoint());
}

bool ARRDrone::SetState(ERRDroneState NewState)
{
	switch (NewState)
	{
	case ERRDroneState::Orbit:
		if (State == ERRDroneState::Dig)
		{
			StopDig();
		}
		else if (State == ERRDroneState::Hack)
		{
			EndHack();
		}
		State = ERRDroneState::Orbit;
		return true;

	case ERRDroneState::Dig:
		if (State == ERRDroneState::Hack)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: cannot dig while hacking %s."), *GetName(), *GetNameSafe(HackTarget));
			return false;
		}
		if (!IsValid(OwnerMiner) || !IsValid(DigGrid) || !DigCells.ContainsByPredicate([this](const FIntPoint& Cell) { return DigGrid->IsBlockCell(Cell); }))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: cannot enter Dig without a miner and a block in the arc (use RequestArcDig); staying in %s."),
				*GetName(), *UEnum::GetValueAsString(State));
			return false;
		}
		State = ERRDroneState::Dig;
		return true;

	case ERRDroneState::Hack:
	default:
		if (State == NewState)
		{
			return true;
		}
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: state %s is entered through StartHack; staying in %s."),
			*GetName(), *UEnum::GetValueAsString(NewState), *UEnum::GetValueAsString(State));
		return false;
	}
}

bool ARRDrone::StartHack(AActor* Target)
{
	if (!IsValid(OwnerMiner) || !IsValid(Target))
	{
		return false;
	}
	if (State == ERRDroneState::Hack)
	{
		// One target at a time.
		return HackTarget == Target;
	}
	if (State == ERRDroneState::Dig)
	{
		StopDig();
	}
	HackTarget = Target;
	State = ERRDroneState::Hack;
	UE_LOG(LogRogueRigs, Verbose, TEXT("%s: hacking %s."), *GetName(), *Target->GetName());
	return true;
}

void ARRDrone::StopHack(AActor* Target)
{
	if (State == ERRDroneState::Hack && HackTarget == Target)
	{
		EndHack();
		State = ERRDroneState::Orbit;
	}
}

bool ARRDrone::RequestArcDig(ARRArenaGrid* Grid, FVector ArcDirection)
{
	return RequestArcDig(Grid, ArcDirection, TArray<FIntPoint>());
}

bool ARRDrone::RequestArcDig(ARRArenaGrid* Grid, FVector ArcDirection, const TArray<FIntPoint>& TouchingCells)
{
	// The drone cannot dig and hack at the same time: the objective releases it first.
	if (State == ERRDroneState::Hack || !IsValid(OwnerMiner) || !IsValid(Grid))
	{
		return false;
	}

	const FVector Direction = FVector(ArcDirection.X, ArcDirection.Y, 0.0).GetSafeNormal();
	TArray<FIntPoint> NewCells;
	// Empty arc while touching rock (the cone misses every touching block): dig the touching block nearest the arc
	// direction instead, never nothing (task-065-01).
	if (!Direction.IsZero() && SelectArcCells(Grid, OwnerMiner->GetActorLocation(), Direction, NewCells) == 0)
	{
		SelectContactFallbackCells(Grid, OwnerMiner->GetActorLocation(), Direction, TouchingCells, NewCells);
	}
	if (Direction.IsZero() || NewCells.Num() == 0)
	{
		ReleaseDig();
		return false;
	}

	// Another grid: release everything first.
	if (State == ERRDroneState::Dig && DigGrid != Grid)
	{
		StopDig();
		State = ERRDroneState::Orbit;
	}

	// The whole veins of the new arc shake (heal-ore, owner 2026-10-07 fifth test); blocks that left the arc stop (damage
	// kept), blocks that entered it start. Iterate copies: listeners may react.
	const TArray<FIntPoint> PreviousCells = DigCells;
	DigGrid = Grid;
	DigCells = NewCells;
	DigArcDirection = Direction;
	State = ERRDroneState::Dig;
	RefreshDigShake();
	for (const FIntPoint& Cell : PreviousCells)
	{
		if (!NewCells.Contains(Cell))
		{
			StopDigBlock(Cell);
		}
	}
	for (const FIntPoint& Cell : NewCells)
	{
		if (!PreviousCells.Contains(Cell))
		{
			OnDigBlockStarted.Broadcast(Cell);
		}
	}
	return true;
}

void ARRDrone::ReleaseDig()
{
	if (State == ERRDroneState::Dig)
	{
		StopDig();
		State = ERRDroneState::Orbit;
	}
}

int32 ARRDrone::SelectArcCells(const ARRArenaGrid* Grid, const FVector& MinerLocation, const FVector& ArcDirection, TArray<FIntPoint>& OutCells) const
{
	OutCells.Reset();
	const FVector Direction = FVector(ArcDirection.X, ArcDirection.Y, 0.0).GetSafeNormal();
	if (!Grid || Direction.IsZero() || MaxArcBlocks <= 0)
	{
		return 0;
	}

	const double CellSize = Grid->GetCellSize();
	const double MinerRadius = IsValid(OwnerMiner) ? OwnerMiner->GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0;
	const FVector Origin(MinerLocation.X, MinerLocation.Y, 0.0);
	const FVector Front = Origin + Direction * MinerRadius;
	const double Reach = ArcReachCells * CellSize;
	const double MinCosine = FMath::Cos(FMath::DegreesToRadians(0.5 * FMath::Clamp(ArcAngleDegrees, 0.f, 360.f)));

	struct FCandidate
	{
		FIntPoint Cell;
		double DistanceSquared;
		double Cosine;
	};
	TArray<FCandidate> Candidates;

	// Every cell whose center can be within the reach of the front point.
	const FIntPoint FrontCell = Grid->GetCellAtWorld(FVector(Front.X, Front.Y, Grid->GetActorLocation().Z));
	const int32 Span = FMath::CeilToInt32(Reach / FMath::Max(CellSize, 1.0)) + 1;
	for (int32 Y = FrontCell.Y - Span; Y <= FrontCell.Y + Span; ++Y)
	{
		for (int32 X = FrontCell.X - Span; X <= FrontCell.X + Span; ++X)
		{
			const FIntPoint Cell(X, Y);
			if (!Grid->IsBlockCell(Cell))
			{
				continue;
			}
			const FVector Center = Grid->GetCellCenter(X, Y);
			const FVector Center2D(Center.X, Center.Y, 0.0);
			const double DistanceSquared = FVector::DistSquared(Center2D, Front);
			if (DistanceSquared > FMath::Square(Reach))
			{
				continue;
			}
			const FVector FromMiner = (Center2D - Origin).GetSafeNormal();
			const double Cosine = FromMiner.IsZero() ? 1.0 : FVector::DotProduct(FromMiner, Direction);
			if (Cosine < MinCosine - UE_KINDA_SMALL_NUMBER)
			{
				continue;
			}
			Candidates.Add({ Cell, DistanceSquared, Cosine });
		}
	}

	Candidates.Sort([](const FCandidate& Left, const FCandidate& Right)
	{
		if (!FMath::IsNearlyEqual(Left.DistanceSquared, Right.DistanceSquared, 1e-3))
		{
			return Left.DistanceSquared < Right.DistanceSquared;
		}
		if (!FMath::IsNearlyEqual(Left.Cosine, Right.Cosine, 1e-9))
		{
			return Left.Cosine > Right.Cosine;
		}
		return Left.Cell.Y != Right.Cell.Y ? Left.Cell.Y < Right.Cell.Y : Left.Cell.X < Right.Cell.X;
	});

	const int32 Count = FMath::Min(Candidates.Num(), MaxArcBlocks);
	for (int32 Index = 0; Index < Count; ++Index)
	{
		OutCells.Add(Candidates[Index].Cell);
	}
	return Count;
}

int32 ARRDrone::SelectContactFallbackCells(const ARRArenaGrid* Grid, const FVector& MinerLocation, const FVector& ArcDirection, const TArray<FIntPoint>& TouchingCells, TArray<FIntPoint>& OutCells) const
{
	OutCells.Reset();
	const FVector Direction = FVector(ArcDirection.X, ArcDirection.Y, 0.0).GetSafeNormal();
	if (!Grid || Direction.IsZero() || MaxArcBlocks <= 0)
	{
		return 0;
	}

	const FVector Origin(MinerLocation.X, MinerLocation.Y, 0.0);
	TArray<TPair<FIntPoint, double>> Candidates;
	double BestCosine = -2.0;
	for (const FIntPoint& Cell : TouchingCells)
	{
		if (!Grid->IsBlockCell(Cell) || Candidates.ContainsByPredicate([&Cell](const TPair<FIntPoint, double>& Entry) { return Entry.Key == Cell; }))
		{
			continue;
		}
		const FVector Center = Grid->GetCellCenter(Cell.X, Cell.Y);
		const FVector FromMiner = (FVector(Center.X, Center.Y, 0.0) - Origin).GetSafeNormal();
		const double Cosine = FromMiner.IsZero() ? 1.0 : FVector::DotProduct(FromMiner, Direction);
		Candidates.Add({ Cell, Cosine });
		BestCosine = FMath::Max(BestCosine, Cosine);
	}

	// Ties within ContactFallbackTieCosine are kept together, so a symmetric corner digs both of its blocks.
	constexpr double ContactFallbackTieCosine = 0.01;
	for (const TPair<FIntPoint, double>& Entry : Candidates)
	{
		if (Entry.Value >= BestCosine - ContactFallbackTieCosine)
		{
			OutCells.Add(Entry.Key);
		}
	}
	OutCells.Sort([](const FIntPoint& Left, const FIntPoint& Right)
	{
		return Left.Y != Right.Y ? Left.Y < Right.Y : Left.X < Right.X;
	});
	if (OutCells.Num() > MaxArcBlocks)
	{
		OutCells.SetNum(MaxArcBlocks);
	}
	return OutCells.Num();
}

FVector ARRDrone::GetDigTargetPoint(FIntPoint Cell) const
{
	if (!IsValid(DigGrid) || !IsValid(OwnerMiner))
	{
		return FVector::ZeroVector;
	}

	return DigGrid->GetCellFaceCenter(Cell, GetDigFaceSide(Cell));
}

FIntPoint ARRDrone::GetDigFaceSide(FIntPoint Cell) const
{
	// Face toward the miner, on the dominant axis of the miner's offset from the block center.
	const FVector ToMiner = OwnerMiner->GetActorLocation() - DigGrid->GetCellCenter(Cell.X, Cell.Y);
	FIntPoint Side(0, 0);
	if (FMath::Abs(ToMiner.X) >= FMath::Abs(ToMiner.Y))
	{
		Side.X = ToMiner.X >= 0.0 ? 1 : -1;
	}
	else
	{
		Side.Y = ToMiner.Y >= 0.0 ? 1 : -1;
	}
	return Side;
}

FVector ARRDrone::GetDigImpactPoint() const
{
	FVector Point;
	FIntPoint FaceSide;
	ComputeDigImpact(Point, FaceSide);
	return Point;
}

bool ARRDrone::ComputeDigImpact(FVector& OutPoint, FIntPoint& OutFaceSide) const
{
	OutPoint = FVector::ZeroVector;
	OutFaceSide = FIntPoint(0, 0);
	if (State != ERRDroneState::Dig || DigCells.Num() == 0 || !IsValid(DigGrid) || !IsValid(OwnerMiner))
	{
		return false;
	}

	// Mean of the per-block target points, and the target point nearest the miner (the wall face in front of it).
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	FVector Mean = FVector::ZeroVector;
	FVector Nearest = FVector::ZeroVector;
	FIntPoint NearestSide(0, 0);
	double NearestDistanceSquared = TNumericLimits<double>::Max();
	for (const FIntPoint& Cell : DigCells)
	{
		const FVector Target = GetDigTargetPoint(Cell);
		Mean += Target;
		const double DistanceSquared = FVector::DistSquared2D(Target, MinerLocation);
		if (DistanceSquared < NearestDistanceSquared)
		{
			NearestDistanceSquared = DistanceSquared;
			Nearest = Target;
			NearestSide = GetDigFaceSide(Cell);
		}
	}
	Mean /= DigCells.Num();

	// Project on the plane of that face: second-row target points sit one cell deeper, the beam must stop on the wall face.
	if (NearestSide.X != 0)
	{
		Mean.X = Nearest.X;
	}
	else
	{
		Mean.Y = Nearest.Y;
	}
	OutPoint = Mean;
	OutFaceSide = NearestSide;
	return true;
}

float ARRDrone::GetMiningSpeed() const
{
	const URRAbilitySystemComponent* AbilitySystem = OwnerMiner ? OwnerMiner->GetRRAbilitySystemComponent() : nullptr;
	if (!AbilitySystem || !AbilitySystem->HasAttributeSetForAttribute(URRPlayerAttributeSet::GetMiningSpeedAttribute()))
	{
		return 1.f;
	}

	return AbilitySystem->GetNumericAttribute(URRPlayerAttributeSet::GetMiningSpeedAttribute());
}

void ARRDrone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Alive (task-249-01): interest, alert and gesture timing first, so the Orbit slot height uses them this frame.
	const bool bHasMiner = IsValid(OwnerMiner) && DeltaSeconds > 0.f;
	if (bHasMiner)
	{
		UpdateLiveliness(DeltaSeconds);
	}

	switch (State)
	{
	case ERRDroneState::Orbit:
		TickOrbit(DeltaSeconds);
		break;
	case ERRDroneState::Dig:
		TickDig(DeltaSeconds);
		break;
	case ERRDroneState::Hack:
		TickHack(DeltaSeconds);
		break;
	}

	// Laser mining sound (task-517): one loop held while the drone digs (only the player's drone digs), faded in and out by its row.
	if (!DigSoundEvent.IsNone())
	{
		URRAudioSubsystem::SetLoop(this, DigSoundEvent, State == ERRDroneState::Dig);
	}

	// The impact effect plays only in Dig (UpdateDigImpact): it fades after a dig and never shows in Hack.
	if (State == ERRDroneState::Orbit)
	{
		FadeDigImpact(DeltaSeconds);
	}
	else if (State == ERRDroneState::Hack)
	{
		StopDigImpact();
	}

	// Rotation last: the look direction uses this frame's drone location (visual only, the meshes never collide).
	if (bHasMiner && IsValid(OwnerMiner))
	{
		UpdateAttitude(DeltaSeconds);
	}

	// Parts after the base rotation (task-253-01), so the laser pitch and the beam start use this frame's attitude.
	if (DeltaSeconds > 0.f)
	{
		UpdateParts(DeltaSeconds);
	}

	// Thrusters last (task-254-02): the speed uses this frame's final location, in every state.
	UpdateThrusters(DeltaSeconds);

	// Shadow after the final location of this frame.
	UpdateBlobShadow();
}

void ARRDrone::UpdateBlobShadow()
{
	if (!IsValid(OwnerMiner))
	{
		return;
	}
	const FVector Location = GetActorLocation();
	const double FloorZ = OwnerMiner->GetActorLocation().Z - OwnerMiner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// Same size at every height (owner 2026-11-06 Q3, task-472-01).
	BlobShadow->PlaceOnFloor(Location, FloorZ);
}

void ARRDrone::UpdateThrusters(float DeltaSeconds)
{
	const FVector Location = GetActorLocation();
	if (!bHasThrusterLocation)
	{
		ThrusterPreviousLocation = Location;
		bHasThrusterLocation = true;
		ThrusterSpeed = 0.f;
	}
	else if (DeltaSeconds > 0.f)
	{
		const float RawSpeed = static_cast<float>(FVector::Dist(Location, ThrusterPreviousLocation) / DeltaSeconds);
		const float Alpha = ThrusterSpeedSmoothing > UE_KINDA_SMALL_NUMBER
			? 1.f - FMath::Exp(-DeltaSeconds / ThrusterSpeedSmoothing)
			: 1.f;
		ThrusterSpeed += (RawSpeed - ThrusterSpeed) * Alpha;
		ThrusterPreviousLocation = Location;
	}

	// Niagara never runs without rendering (-nullrhi, commandlets): skip the activation attempts there.
	if (!ThrusterSystem || !FApp::CanEverRender())
	{
		return;
	}

	static const FName SpeedParameter(TEXT("User.Speed"));
	for (UNiagaraComponent* Thruster : { ThrusterLeft.Get(), ThrusterRight.Get() })
	{
		if (!IsValid(Thruster))
		{
			continue;
		}
		if (Thruster->GetAsset() != ThrusterSystem)
		{
			Thruster->SetAsset(ThrusterSystem);
		}
		Thruster->SetVariableFloat(SpeedParameter, ThrusterSpeed);
		if (!Thruster->IsActive())
		{
			Thruster->Activate(/*bReset*/ true);
		}
	}
}

FVector ARRDrone::GetLaserEmitterLocation() const
{
	return LaserPivot->GetComponentTransform().TransformPosition(LaserEmitterLocal);
}

bool ARRDrone::GetLaserAimPoint(FVector& OutPoint) const
{
	OutPoint = FVector::ZeroVector;
	if (State == ERRDroneState::Dig)
	{
		OutPoint = GetDigImpactPoint();
		return !OutPoint.IsZero();
	}
	if (State == ERRDroneState::Hack && IsValid(HackTarget))
	{
		OutPoint = GetHackLinkPoint();
		return true;
	}
	return false;
}

bool ARRDrone::GetHackTargetSocket(FVector& OutLocation, FVector& OutFacing) const
{
	const ARRConsole* Console = State == ERRDroneState::Hack ? Cast<ARRConsole>(HackTarget) : nullptr;
	if (IsValid(Console))
	{
		return Console->GetHackDataSocket(OutLocation, OutFacing);
	}
	OutLocation = FVector::ZeroVector;
	OutFacing = FVector::ZeroVector;
	return false;
}

FVector ARRDrone::GetHackLinkPoint() const
{
	FVector Socket;
	FVector Facing;
	return GetHackTargetSocket(Socket, Facing) ? Socket : HackTarget->GetActorLocation() + HackLinkOffset;
}

float ARRDrone::ComputeLaserPitchToward(const FVector& Point) const
{
	// In the base frame (yaw and tilt removed), from the pivot: pitch P turns the arm's down axis to (sin P, 0, -cos P).
	const FVector Local = GetActorTransform().InverseTransformPosition(Point) - LaserPivot->GetRelativeLocation();
	const double AimDegrees = FMath::RadiansToDegrees(FMath::Atan2(Local.X, -Local.Z));
	// The emitter tip sits slightly off the arm's axis: aim the pivot-to-emitter line, not the axis.
	const double EmitterDegrees = FMath::RadiansToDegrees(FMath::Atan2(LaserEmitterLocal.X, -LaserEmitterLocal.Z));
	return static_cast<float>(AimDegrees - EmitterDegrees);
}

void ARRDrone::UpdateParts(float DeltaSeconds)
{
	// Antenna: continuous spin, whatever the state (visual only).
	AntennaYaw = FMath::Fmod(AntennaYaw + AntennaSpinSpeed * DeltaSeconds, 360.f);
	AntennaYaw = AntennaYaw < 0.f ? AntennaYaw + 360.f : AntennaYaw;
	AntennaMesh->SetRelativeRotation(FRotator(0.f, AntennaYaw, 0.f));

	// Laser arm: toward the aim point in Dig and Hack, straight down in Orbit; clamped, at a limited rate.
	FVector AimPoint;
	float TargetPitch = 0.f;
	if (GetLaserAimPoint(AimPoint))
	{
		TargetPitch = ComputeLaserPitchToward(AimPoint);
	}
	TargetPitch = FMath::Clamp(TargetPitch, LaserPitchMinDegrees, FMath::Max(LaserPitchMinDegrees, LaserPitchMaxDegrees));
	const float MaxStep = FMath::Max(0.f, LaserPitchTurnRate) * DeltaSeconds;
	LaserPitch += FMath::Clamp(TargetPitch - LaserPitch, -MaxStep, MaxStep);
	LaserPivot->SetRelativeRotation(FRotator(LaserPitch, 0.f, 0.f));

	// The beam was placed in the state tick from the emitter of the previous attitude: start it at the moved emitter.
	if (State != ERRDroneState::Orbit && LaserMesh->IsVisible())
	{
		UpdateLaser(LaserMesh, GetLaserEmitterLocation(), LaserBeamEnd);
	}
}

void ARRDrone::ResetIdleGestureCountdown()
{
	const float MinInterval = FMath::Max(0.1f, IdleGestureMinInterval);
	IdleGestureCountdown = GestureRandom.FRandRange(MinInterval, FMath::Max(MinInterval, IdleGestureMaxInterval));
}

void ARRDrone::UpdateLiveliness(float DeltaSeconds)
{
	// Lookups at a low rate only (TActorIterator on enemies, a few hundred grid cells): never a scan per tick.
	InterestRefreshTimer -= DeltaSeconds;
	if (InterestRefreshTimer <= 0.f)
	{
		InterestRefreshTimer = FMath::Max(0.02f, InterestRefreshInterval);
		RefreshInterest();
	}

	// Slightly higher with an enemy near the miner, lower when calm; smoothed so the height never jumps.
	EscortAlert = FMath::FInterpTo(EscortAlert, bEnemyNearMiner ? 1.f : 0.f, DeltaSeconds, EscortAlertBlendSpeed);

	// Idle gestures: only in Orbit with the miner at rest; any movement cancels the gesture and restarts the countdown.
	const bool bMinerAtRest = OwnerMiner->GetVelocity().Size2D() <= MinerRestSpeed;
	if (State != ERRDroneState::Orbit || !bMinerAtRest)
	{
		IdleGesture = ERRDroneIdleGesture::None;
		IdleGestureTime = 0.f;
		ResetIdleGestureCountdown();
		return;
	}
	if (IdleGesture != ERRDroneIdleGesture::None)
	{
		IdleGestureTime += DeltaSeconds;
		const float Duration = IdleGesture == ERRDroneIdleGesture::ScanSweep ? ScanSweepDuration
			: IdleGesture == ERRDroneIdleGesture::Nod ? NodDuration : DipDuration;
		if (IdleGestureTime >= Duration)
		{
			IdleGesture = ERRDroneIdleGesture::None;
			IdleGestureTime = 0.f;
		}
		return;
	}
	IdleGestureCountdown -= DeltaSeconds;
	if (IdleGestureCountdown <= 0.f)
	{
		IdleGesture = static_cast<ERRDroneIdleGesture>(GestureRandom.RandRange(1, 3));
		IdleGestureTime = 0.f;
		// The next interval counts from the start of this gesture.
		ResetIdleGestureCountdown();
	}
}

void ARRDrone::RefreshInterest()
{
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	InterestActor.Reset();
	bHasInterestLocation = false;
	bEnemyNearMiner = false;

	// Nearest living enemy (pooled idle robots keep State.Dead, so IsDead skips them).
	const UWorld* World = GetWorld();
	if (World)
	{
		double BestDistanceSquared = FMath::Square(static_cast<double>(InterestEnemyRadius));
		for (TActorIterator<ARREnemy> It(World); It; ++It)
		{
			ARREnemy* Enemy = *It;
			if (!IsValid(Enemy) || Enemy->IsActorBeingDestroyed() || Enemy->IsDead())
			{
				continue;
			}
			const double DistanceSquared = FVector::DistSquared2D(Enemy->GetActorLocation(), MinerLocation);
			if (DistanceSquared <= BestDistanceSquared)
			{
				BestDistanceSquared = DistanceSquared;
				InterestActor = Enemy;
			}
		}
		if (InterestActor.IsValid())
		{
			bEnemyNearMiner = BestDistanceSquared <= FMath::Square(static_cast<double>(EscortAlertRadius));
			return;
		}
	}

	// No enemy: nearest standing deposit block or objective within InterestSiteRadius.
	double BestSiteDistanceSquared = FMath::Square(static_cast<double>(InterestSiteRadius));
	if (const ARRArenaGrid* Grid = OwnerMiner->GetArenaGrid())
	{
		const double CellSize = FMath::Max(Grid->GetCellSize(), 1.0);
		const FIntPoint MinerCell = Grid->GetCellAtWorld(MinerLocation);
		const int32 Span = FMath::CeilToInt32(InterestSiteRadius / CellSize);
		for (int32 Y = MinerCell.Y - Span; Y <= MinerCell.Y + Span; ++Y)
		{
			for (int32 X = MinerCell.X - Span; X <= MinerCell.X + Span; ++X)
			{
				if (Grid->GetDepositId(FIntPoint(X, Y)) == INDEX_NONE)
				{
					continue;
				}
				const FVector Center = Grid->GetCellCenter(X, Y);
				const double DistanceSquared = FVector::DistSquared2D(Center, MinerLocation);
				if (DistanceSquared <= BestSiteDistanceSquared)
				{
					BestSiteDistanceSquared = DistanceSquared;
					InterestLocation = Center;
					bHasInterestLocation = true;
				}
			}
		}
	}
	const URRRunFlowSubsystem* RunFlow = World ? World->GetSubsystem<URRRunFlowSubsystem>() : nullptr;
	if (RunFlow)
	{
		AActor* Objective = nullptr;
		if (IsValid(RunFlow->GetConsole()) && RunFlow->GetConsole()->GetConsoleState() != ERRConsoleState::Complete)
		{
			Objective = RunFlow->GetConsole();
		}
		else if (IsValid(RunFlow->GetShuttle()))
		{
			Objective = RunFlow->GetShuttle();
		}
		if (Objective && FVector::DistSquared2D(Objective->GetActorLocation(), MinerLocation) <= BestSiteDistanceSquared)
		{
			InterestActor = Objective;
			bHasInterestLocation = false;
		}
	}
}

float ARRDrone::GetIdleGestureEnvelope() const
{
	if (IdleGesture == ERRDroneIdleGesture::None)
	{
		return 0.f;
	}
	const float Duration = IdleGesture == ERRDroneIdleGesture::ScanSweep ? ScanSweepDuration
		: IdleGesture == ERRDroneIdleGesture::Nod ? NodDuration : DipDuration;
	const float Progress = FMath::Clamp(IdleGestureTime / FMath::Max(Duration, 0.1f), 0.f, 1.f);
	return FMath::Sin(UE_PI * Progress);
}

float ARRDrone::GetEscortHeight() const
{
	const float Centre = FMath::Lerp(EscortCalmHeight, EscortAlertHeight, EscortAlert);
	const float Bob = EscortBobAmplitude * FMath::Sin(EscortBobPhase) + EscortBobSecondAmplitude * FMath::Sin(EscortBobSecondPhase);
	const float Dip = IdleGesture == ERRDroneIdleGesture::Dip ? DipDepth * GetIdleGestureEnvelope() : 0.f;
	return FMath::Clamp(Centre + Bob - Dip, EscortMinHeight, FMath::Max(EscortMinHeight, EscortMaxHeight));
}

FVector ARRDrone::GetLookTarget() const
{
	if (State == ERRDroneState::Dig)
	{
		const FVector Impact = GetDigImpactPoint();
		if (!Impact.IsZero())
		{
			return Impact;
		}
	}
	else if (State == ERRDroneState::Hack && IsValid(HackTarget))
	{
		// Faces the data socket when the target has one (task-313-01), else the target.
		FVector Socket;
		FVector Facing;
		return GetHackTargetSocket(Socket, Facing) ? Socket : HackTarget->GetActorLocation();
	}
	else if (IdleGesture != ERRDroneIdleGesture::Nod)
	{
		if (const AActor* Interest = InterestActor.Get())
		{
			return Interest->GetActorLocation();
		}
		if (bHasInterestLocation)
		{
			return InterestLocation;
		}
	}
	return OwnerMiner->GetActorLocation();
}

void ARRDrone::UpdateAttitude(float DeltaSeconds)
{
	// Yaw: toward the point of interest with delay; the scan sweep swings around it.
	const FVector ToTarget = GetLookTarget() - GetActorLocation();
	float TargetYaw = LookYaw;
	if (ToTarget.SizeSquared2D() > 1.0)
	{
		TargetYaw = FMath::RadiansToDegrees(FMath::Atan2(ToTarget.Y, ToTarget.X));
	}
	if (IdleGesture == ERRDroneIdleGesture::ScanSweep)
	{
		const float Progress = FMath::Clamp(IdleGestureTime / FMath::Max(ScanSweepDuration, 0.1f), 0.f, 1.f);
		TargetYaw += ScanSweepDegrees * FMath::Sin(UE_TWO_PI * Progress);
	}
	// task-253-01: the base never turns instantly. Dig and Hack turn at the constant BaseYawTurnRate; the other interests keep the
	// task-249-01 delay, capped by the same rate (a switch of interest no longer whips the base around).
	const float YawDelta = FMath::FindDeltaAngleDegrees(LookYaw, TargetYaw);
	const float MaxYawStep = FMath::Max(0.f, BaseYawTurnRate) * DeltaSeconds;
	const bool bWorkTarget = State == ERRDroneState::Dig || State == ERRDroneState::Hack;
	const float YawStep = bWorkTarget ? YawDelta : FMath::FInterpTo(0.f, YawDelta, DeltaSeconds, LookYawInterpSpeed);
	LookYaw = FRotator::NormalizeAxis(LookYaw + FMath::Clamp(YawStep, -MaxYawStep, MaxYawStep));

	// Miner acceleration on XY, smoothed; a teleport spike is clamped first.
	const FVector MinerVelocity = OwnerMiner->GetVelocity();
	constexpr double MaxMeasuredAcceleration = 5000.0;
	const FVector RawAcceleration = ((MinerVelocity - PreviousMinerVelocity) / DeltaSeconds).GetClampedToMaxSize2D(MaxMeasuredAcceleration);
	PreviousMinerVelocity = MinerVelocity;
	SmoothedMinerAcceleration = FMath::VInterpTo(SmoothedMinerAcceleration, FVector(RawAcceleration.X, RawAcceleration.Y, 0.0), DeltaSeconds, AccelerationSmoothingSpeed);

	// In the drone's frame: forward acceleration dips the nose (negative pitch), side acceleration banks toward it (into the turn).
	float Sin = 0.f;
	float Cos = 0.f;
	FMath::SinCos(&Sin, &Cos, FMath::DegreesToRadians(LookYaw));
	const double Forward = SmoothedMinerAcceleration.X * Cos + SmoothedMinerAcceleration.Y * Sin;
	const double Right = -SmoothedMinerAcceleration.X * Sin + SmoothedMinerAcceleration.Y * Cos;
	const float MaxTilt = FMath::Max(0.f, MaxTiltDegrees);
	float TargetPitch = FMath::Clamp(static_cast<float>(-Forward * TiltDegreesPerAcceleration), -MaxTilt, MaxTilt);
	const float TargetRoll = FMath::Clamp(static_cast<float>(Right * TiltDegreesPerAcceleration), -MaxTilt, MaxTilt);
	if (IdleGesture == ERRDroneIdleGesture::Nod)
	{
		TargetPitch = FMath::Clamp(TargetPitch - NodPitchDegrees * GetIdleGestureEnvelope(), -MaxTilt, MaxTilt);
	}
	TiltPitch = FMath::FInterpTo(TiltPitch, TargetPitch, DeltaSeconds, TiltInterpSpeed);
	TiltRoll = FMath::FInterpTo(TiltRoll, TargetRoll, DeltaSeconds, TiltInterpSpeed);

	SetActorRotation(FRotator(TiltPitch, LookYaw, TiltRoll));
}

void ARRDrone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopDigImpact();
	if (!DigSoundEvent.IsNone())
	{
		URRAudioSubsystem::SetLoop(this, DigSoundEvent, false);
	}
	if (State == ERRDroneState::Dig)
	{
		StopDig();
		State = ERRDroneState::Orbit;
	}
	else if (State == ERRDroneState::Hack)
	{
		EndHack();
		State = ERRDroneState::Orbit;
	}

	Super::EndPlay(EndPlayReason);
}

void ARRDrone::TickOrbit(float DeltaSeconds)
{
	if (!IsValid(OwnerMiner))
	{
		return;
	}

	if (DeltaSeconds <= 0.f)
	{
		return;
	}

	EscortBobPhase = FMath::Fmod(EscortBobPhase + UE_TWO_PI * DeltaSeconds / FMath::Max(EscortBobPeriod, 0.1f), UE_TWO_PI);
	EscortBobSecondPhase = FMath::Fmod(EscortBobSecondPhase + UE_TWO_PI * DeltaSeconds / FMath::Max(EscortBobSecondPeriod, 0.1f), UE_TWO_PI);

	// Patrol point (task-251-01): reached, timed out or no longer valid = a new one. Never from the miner's facing or movement.
	UpdatePatrolPoint(DeltaSeconds);

	const FVector Point = ComputeOrbitPoint();
	const FVector Start = GetActorLocation();
	if (FVector::DistSquared2D(Start, Point) > FMath::Square(EscortSnapDistance))
	{
		// Miner teleported (cheat, run transition): no flight across the arena.
		PatrolVelocity = FVector::ZeroVector;
		SetActorLocation(Point);
		return;
	}

	// No clear line from the drone to its point (the miner turned a corner): head back toward the miner first.
	const ARRArenaGrid* Grid = GetTerrainGrid();
	FVector Target = Point;
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	if (Grid && !Grid->HasLineOfSight(Start, Point) && Grid->HasLineOfSight(Start, MinerLocation))
	{
		Target = FVector(MinerLocation.X, MinerLocation.Y, Point.Z);
	}

	// Lag follow: the desired speed grows with the distance (slow near the point, fast far from it); the velocity reaches it
	// exponentially, so a moving miner leaves the drone behind until it catches up. Frame-rate independent.
	const FVector2D ToTarget(Target.X - Start.X, Target.Y - Start.Y);
	const double Distance = ToTarget.Size();
	const FVector2D Direction = Distance > UE_KINDA_SMALL_NUMBER ? ToTarget / Distance : FVector2D::ZeroVector;
	const FVector2D Desired = Direction * GetPatrolSpeed(Distance);
	const double Alpha = 1.0 - FMath::Exp(-FMath::Max(PatrolVelocityResponse, 0.01f) * DeltaSeconds);
	PatrolVelocity.X += (Desired.X - PatrolVelocity.X) * Alpha;
	PatrolVelocity.Y += (Desired.Y - PatrolVelocity.Y) * Alpha;
	PatrolVelocity.Z = 0.0;

	FVector End = Start + PatrolVelocity * DeltaSeconds;
	// Never past the point: a step that would cross it lands on it (no jitter around the point).
	const double Along = Direction.X * (End.X - Start.X) + Direction.Y * (End.Y - Start.Y);
	if (Distance > UE_KINDA_SMALL_NUMBER && Along >= Distance)
	{
		End.X = Target.X;
		End.Y = Target.Y;
		PatrolVelocity = FVector::ZeroVector;
	}
	End.Z = FMath::FInterpTo(Start.Z, Target.Z, DeltaSeconds, PatrolHeightInterpSpeed);

	// Last guard (task-248-01): never into a block cell.
	SetActorLocation(ConstrainToClearCells(Grid, Start, End, &PatrolVelocity));
}

float ARRDrone::GetPatrolSpeed(double Distance) const
{
	const double Ramp = FMath::Max(1.0, static_cast<double>(PatrolSpeedRampDistance));
	const double Blend = FMath::SmoothStep(0.0, Ramp, Distance);
	const double Speed = FMath::Lerp(static_cast<double>(PatrolNearSpeed), static_cast<double>(PatrolFarSpeed), Blend);
	// Arrival: below Speed / PatrolArrivalRate uu the drone settles exponentially instead of arriving at full near speed.
	return static_cast<float>(PatrolArrivalRate > 0.f ? FMath::Min(Speed, Distance * PatrolArrivalRate) : Speed);
}

const ARRArenaGrid* ARRDrone::GetTerrainGrid() const
{
	if (IsValid(DigGrid))
	{
		return DigGrid;
	}
	return IsValid(OwnerMiner) ? OwnerMiner->GetArenaGrid() : nullptr;
}

bool ARRDrone::IsPatrolOffsetClear(const ARRArenaGrid* Grid, const FVector2D& Offset) const
{
	if (!Grid)
	{
		return true;
	}
	const double Length = Offset.Size();
	if (Length <= UE_KINDA_SMALL_NUMBER)
	{
		return !Grid->IsBlockCell(Grid->GetCellAtWorld(OwnerMiner->GetActorLocation()));
	}
	// The miner-to-point segment and a margin past the point (task-248-01 rule): no block on the way, none at the point.
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const FVector2D Reach = Offset * ((Length + DroneWallClearance) / Length);
	return Grid->HasLineOfSight(MinerLocation, MinerLocation + FVector(Reach.X, Reach.Y, 0.0));
}

void ARRDrone::PickPatrolPoint()
{
	const ARRArenaGrid* Grid = GetTerrainGrid();
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const FVector2D DroneOffset(GetActorLocation().X - MinerLocation.X, GetActorLocation().Y - MinerLocation.Y);
	const double MinRadius = FMath::Max(0.f, FMath::Min(PatrolMinRadius, PatrolMaxRadius));
	const double MaxRadius = FMath::Max(PatrolMinRadius, PatrolMaxRadius);

	// Random candidates in the zone; the miner's facing, aim and movement play no part (task-251-01).
	bool bFound = false;
	FVector2D Best = FVector2D::ZeroVector;
	double BestScore = TNumericLimits<double>::Lowest();
	for (int32 Attempt = 0; Attempt < FMath::Max(1, PatrolPickAttempts); ++Attempt)
	{
		const double Angle = PatrolRandom.FRandRange(0.f, UE_TWO_PI);
		const double Radius = PatrolRandom.FRandRange(static_cast<float>(MinRadius), static_cast<float>(MaxRadius));
		const FVector2D Candidate(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius);
		if (!IsPatrolOffsetClear(Grid, Candidate))
		{
			continue;
		}
		// The straight flight to the candidate should stay in the zone (never across the miner), and be a real hop.
		const double PathDistance = FMath::ClosestPointOnSegment2D(FVector2D::ZeroVector, DroneOffset, Candidate).Size();
		const bool bHop = FVector2D::Distance(DroneOffset, Candidate) >= PatrolMinHopDistance;
		if (bHop && PathDistance >= MinRadius - UE_KINDA_SMALL_NUMBER)
		{
			Best = Candidate;
			bFound = true;
			break;
		}
		const double Score = PathDistance - (bHop ? 0.0 : 1.0e6);
		if (!bFound || Score > BestScore)
		{
			Best = Candidate;
			BestScore = Score;
			bFound = true;
		}
	}

	bPatrolFallback = !bFound;
	PatrolOffset = bFound ? Best : FVector2D::ZeroVector;
	if (bPatrolFallback)
	{
		PatrolTimer = FMath::Max(0.02f, PatrolFallbackRetryInterval);
	}
	else
	{
		const float MinInterval = FMath::Max(0.1f, PatrolMinInterval);
		PatrolTimer = static_cast<float>(PatrolRandom.FRandRange(MinInterval, FMath::Max(MinInterval, PatrolMaxInterval)));
	}
}

void ARRDrone::UpdatePatrolPoint(float DeltaSeconds)
{
	PatrolTimer -= DeltaSeconds;
	bool bRepick = PatrolTimer <= 0.f;
	if (!bRepick && !bPatrolFallback)
	{
		const FVector Point = ComputeOrbitPoint();
		bRepick = FVector::DistSquared2D(GetActorLocation(), Point) <= FMath::Square(PatrolReachDistance)
			|| !IsPatrolOffsetClear(GetTerrainGrid(), PatrolOffset);
	}
	if (bRepick)
	{
		PickPatrolPoint();
	}
}

FVector ARRDrone::ResolveClearHoverPoint(const ARRArenaGrid* Grid, const FVector& Desired) const
{
	if (!Grid)
	{
		return Desired;
	}
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const FVector Offset(Desired.X - MinerLocation.X, Desired.Y - MinerLocation.Y, 0.0);
	const double Distance = Offset.Size();
	if (Distance <= UE_KINDA_SMALL_NUMBER)
	{
		return Desired;
	}
	const FVector Direction = Offset / Distance;
	const double CheckedDistance = Distance + DroneWallClearance;
	const double Clear = Grid->GetClearDistance(MinerLocation, Direction, static_cast<float>(CheckedDistance));
	if (Clear >= CheckedDistance - UE_KINDA_SMALL_NUMBER)
	{
		return Desired;
	}
	// Back toward the miner, a margin before the first block.
	const double Kept = FMath::Clamp(Clear - DroneWallClearance, 0.0, Distance);
	return FVector(MinerLocation.X + Direction.X * Kept, MinerLocation.Y + Direction.Y * Kept, Desired.Z);
}

FVector ARRDrone::ConstrainToClearCells(const ARRArenaGrid* Grid, const FVector& From, const FVector& To, FVector* InOutVelocity) const
{
	if (!Grid || !Grid->IsBlockCell(Grid->GetCellAtWorld(To)) || Grid->IsBlockCell(Grid->GetCellAtWorld(From)))
	{
		return To;
	}
	const FVector AlongX(To.X, From.Y, To.Z);
	if (!Grid->IsBlockCell(Grid->GetCellAtWorld(AlongX)))
	{
		if (InOutVelocity)
		{
			InOutVelocity->Y = 0.0;
		}
		return AlongX;
	}
	const FVector AlongY(From.X, To.Y, To.Z);
	if (!Grid->IsBlockCell(Grid->GetCellAtWorld(AlongY)))
	{
		if (InOutVelocity)
		{
			InOutVelocity->X = 0.0;
		}
		return AlongY;
	}
	if (InOutVelocity)
	{
		InOutVelocity->X = 0.0;
		InOutVelocity->Y = 0.0;
	}
	return FVector(From.X, From.Y, To.Z);
}

void ARRDrone::TickDig(float DeltaSeconds)
{
	// Cancel rule: miner or grid gone. Leaving the wall is handled by the miner (ReleaseDig).
	if (!IsValid(OwnerMiner) || !IsValid(DigGrid))
	{
		SetState(ERRDroneState::Orbit);
		return;
	}

	// Blocks blown up since the last tick leave the arc.
	for (int32 Index = DigCells.Num() - 1; Index >= 0; --Index)
	{
		if (!DigGrid->IsBlockCell(DigCells[Index]))
		{
			const FIntPoint Cell = DigCells[Index];
			DigCells.RemoveAt(Index);
			StopDigBlock(Cell);
		}
	}

	const FVector From = GetActorLocation();
	SetActorLocation(ConstrainToClearCells(DigGrid, From, FMath::VInterpTo(From, ComputeDigHoverPoint(), DeltaSeconds, FollowInterpSpeed)));

	// Laser first: the face centers are still valid before the hit that may destroy the blocks.
	UpdateDigLaser();
	UpdateDigImpact(DeltaSeconds);

	// Every block of the arc is cut in parallel at the full rate. A block of a deposit (Duskite 2 x 2, heal vein) makes the whole
	// deposit one target (ore-blocks, owner 2026-10-06 fourth test and 2026-10-10): each deposit block takes the damage once
	// per tick, however many of its blocks are in the arc, so the deposit breaks whole after one block dig time. Dig holds even when the arc empties:
	// the miner selects the next blocks (or releases the dig) on its next tick.
	const float Damage = DeltaSeconds * GetMiningSpeed();
	TArray<FIntPoint> Targets;
	CollectDigTargets(Targets);
	for (const FIntPoint& Cell : Targets)
	{
		// Read before the hit: a broken cell is Floor. A barrel brought to 0 only lights its fuse, and its explosion does the rest.
		const ERRCellType Type = DigGrid->GetCellType(Cell);
		if (DigGrid->ApplyTerrainDamage(Cell, Damage, ERRTerrainDamageSource::Drone) && Type != ERRCellType::Barrel)
		{
			PlayDigDebris(Cell, Type);
		}
	}
	for (int32 Index = 0; Index < DigCells.Num();)
	{
		const FIntPoint Cell = DigCells[Index];
		if (!DigGrid->IsBlockCell(Cell))
		{
			DigCells.RemoveAt(Index);
			StopDigBlock(Cell);
		}
		else
		{
			++Index;
		}
	}

	// Vein blocks destroyed this tick (or blown up out of the arc) stop shaking; the rest of the veins keep shaking.
	RefreshDigShake();
}

void ARRDrone::CollectDigTargets(TArray<FIntPoint>& OutTargets) const
{
	OutTargets.Reset();
	TArray<FIntPoint> Vein;
	for (const FIntPoint& Cell : DigCells)
	{
		if (OutTargets.Contains(Cell))
		{
			continue;
		}
		DigGrid->GetDepositCells(Cell, Vein);
		for (const FIntPoint& VeinCell : Vein)
		{
			OutTargets.AddUnique(VeinCell);
		}
	}
}

void ARRDrone::RefreshDigShake()
{
	TArray<FIntPoint> Targets;
	if (IsValid(DigGrid))
	{
		CollectDigTargets(Targets);
	}
	for (const FIntPoint& Cell : ShakeCells)
	{
		if (!Targets.Contains(Cell) && IsValid(DigGrid))
		{
			DigGrid->SetCellShaking(Cell, false);
		}
	}
	for (const FIntPoint& Cell : Targets)
	{
		if (!ShakeCells.Contains(Cell))
		{
			DigGrid->SetCellShaking(Cell, true);
		}
	}
	ShakeCells = MoveTemp(Targets);
}

FVector ARRDrone::ComputeOrbitPoint() const
{
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const double FloorZ = MinerLocation.Z - OwnerMiner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// No valid patrol candidate (task-248-01): just above the miner, a bit higher.
	if (bPatrolFallback)
	{
		return FVector(MinerLocation.X, MinerLocation.Y, FloorZ + GetEscortHeight() + EscortFallbackExtraHeight);
	}

	// The point moves with the miner (task-251-01). Height (task-249-01): calm-to-alert centre, two bobs and the dip gesture,
	// inside the 130-190 band.
	return FVector(MinerLocation.X + PatrolOffset.X, MinerLocation.Y + PatrolOffset.Y, FloorZ + GetEscortHeight());
}

FVector ARRDrone::ComputeDigHoverPoint() const
{
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const double FloorZ = MinerLocation.Z - OwnerMiner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// Free side of the wall face (task-248-01): from the impact point along the face's outward normal; keep the current XY
	// while the arc is empty.
	FVector Hover = GetActorLocation();
	FVector ImpactPoint;
	FIntPoint FaceSide;
	if (ComputeDigImpact(ImpactPoint, FaceSide))
	{
		Hover = ImpactPoint + FVector(FaceSide.X, FaceSide.Y, 0.0) * DigHoverFaceDistance;
	}
	return ResolveClearHoverPoint(DigGrid, FVector(Hover.X, Hover.Y, FloorZ + DigHoverHeight));
}

void ARRDrone::StopDigBlock(FIntPoint Cell)
{
	const bool bDestroyed = !IsValid(DigGrid) || !DigGrid->IsBlockCell(Cell);
	OnDigBlockStopped.Broadcast(Cell, bDestroyed);
}

void ARRDrone::StopDig()
{
	TArray<FIntPoint> StoppedCells = MoveTemp(DigCells);
	DigCells.Reset();
	RefreshDigShake();
	for (const FIntPoint& Cell : StoppedCells)
	{
		StopDigBlock(Cell);
	}

	LaserMesh->SetVisibility(false);
	DigGrid = nullptr;

	ResumeOrbit();
}

void ARRDrone::TickHack(float DeltaSeconds)
{
	if (!IsValid(OwnerMiner) || !IsValid(HackTarget))
	{
		EndHack();
		State = ERRDroneState::Orbit;
		return;
	}

	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const double FloorZ = MinerLocation.Z - OwnerMiner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector HoverPoint;
	FVector Socket;
	FVector SocketFacing;
	if (GetHackTargetSocket(Socket, SocketFacing))
	{
		// Console data socket (task-313-01, owner decision 13): in front of it along its facing, at its height.
		HoverPoint = Socket + SocketFacing * HackSocketStandoff + FVector(0.0, 0.0, HackSocketHeightOffset);
	}
	else
	{
		const FVector TargetLocation = HackTarget->GetActorLocation();
		const FVector TowardMiner = (MinerLocation - TargetLocation).GetSafeNormal2D();
		HoverPoint = TargetLocation + TowardMiner * HackHoverDistance;
		HoverPoint.Z = FloorZ + HoverHeight;
	}

	// The hover point must be clear of rock (task-248-01), else it is pulled back toward the miner.
	const ARRArenaGrid* Grid = GetTerrainGrid();
	const FVector ClearHover = ResolveClearHoverPoint(Grid, HoverPoint);
	const FVector From = GetActorLocation();
	SetActorLocation(ConstrainToClearCells(Grid, From, FMath::VInterpTo(From, ClearHover, DeltaSeconds, FollowInterpSpeed)));
	UpdateLaser(LaserMesh, GetLaserEmitterLocation(), GetHackLinkPoint());
}

void ARRDrone::EndHack()
{
	LaserMesh->SetVisibility(false);
	HackTarget = nullptr;
	ResumeOrbit();
}

void ARRDrone::ResumeOrbit()
{
	// The follow starts from where the drone is, at rest: it flies back to the zone smoothly.
	PatrolVelocity = FVector::ZeroVector;
	// A fresh patrol point after a dig or hack (task-251-01), valid on the current terrain (task-248-01).
	if (IsValid(OwnerMiner))
	{
		PickPatrolPoint();
	}
}

void ARRDrone::UpdateLaser(UStaticMeshComponent* Beam, const FVector& Start, const FVector& End)
{
	LaserBeamEnd = End;
	const FVector Delta = End - Start;
	const double Length = Delta.Size();
	if (Length <= UE_KINDA_SMALL_NUMBER)
	{
		Beam->SetVisibility(false);
		return;
	}

	// The cylinder is centered on its pivot and runs along local Z.
	const double WidthScale = LaserDiameter / FMath::Max(LaserMeshBaseDiameter, UE_KINDA_SMALL_NUMBER);
	const double LengthScale = Length / FMath::Max(LaserMeshBaseHeight, UE_KINDA_SMALL_NUMBER);
	const FRotator Rotation = FRotationMatrix::MakeFromZ(Delta / Length).Rotator();

	Beam->SetWorldTransform(FTransform(Rotation, Start + Delta * 0.5, FVector(WidthScale, WidthScale, LengthScale)));
	Beam->SetVisibility(true);
}

void ARRDrone::UpdateDigLaser()
{
	// One beam whatever the number of dug blocks (drone gdd, owner 2026-10-19), aimed at the middle of their faces.
	if (DigCells.Num() == 0)
	{
		LaserMesh->SetVisibility(false);
		return;
	}
	UpdateLaser(LaserMesh, GetLaserEmitterLocation(), GetDigImpactPoint());
}

void ARRDrone::UpdateDigImpact(float DeltaSeconds)
{
	FVector ImpactPoint;
	FIntPoint FaceSide;
	if (!ComputeDigImpact(ImpactPoint, FaceSide))
	{
		FadeDigImpact(DeltaSeconds);
		return;
	}
	if (!DigImpactSystem)
	{
		return;
	}

	if (DigImpactComponent->GetAsset() != DigImpactSystem)
	{
		DigImpactComponent->SetAsset(DigImpactSystem);
	}
	DigImpactComponent->SetWorldLocation(ImpactPoint);

	// Outward normal of the impact face: the grid step from the block toward the miner (unrotated grid, world X / Y = grid X / Y).
	static const FName WallNormalParameter(TEXT("User.WallNormal"));
	static const FName ChipColorParameter(TEXT("User.ChipColor"));
	static const FName IntensityParameter(TEXT("User.Intensity"));
	DigImpactIntensity = 1.f;
	DigImpactComponent->SetVariableVec3(WallNormalParameter, FVector(FaceSide.X, FaceSide.Y, 0.0));
	DigImpactComponent->SetVariableLinearColor(ChipColorParameter, GetDigChipColor());
	DigImpactComponent->SetVariableFloat(IntensityParameter, DigImpactIntensity);
	if (!DigImpactComponent->IsActive())
	{
		DigImpactComponent->Activate(/*bReset*/ true);
	}
}

void ARRDrone::FadeDigImpact(float DeltaSeconds)
{
	if (!DigImpactComponent->IsActive())
	{
		DigImpactIntensity = 0.f;
		return;
	}

	static const FName IntensityParameter(TEXT("User.Intensity"));
	DigImpactIntensity = DigImpactFadeTime > UE_KINDA_SMALL_NUMBER
		? FMath::Max(0.f, DigImpactIntensity - DeltaSeconds / DigImpactFadeTime)
		: 0.f;
	DigImpactComponent->SetVariableFloat(IntensityParameter, DigImpactIntensity);
	if (DigImpactIntensity <= 0.f)
	{
		// Spawning stops; particles already alive finish their short lifetime.
		DigImpactComponent->Deactivate();
	}
}

void ARRDrone::StopDigImpact()
{
	DigImpactIntensity = 0.f;
	if (IsValid(DigImpactComponent) && DigImpactComponent->IsActive())
	{
		static const FName IntensityParameter(TEXT("User.Intensity"));
		DigImpactComponent->SetVariableFloat(IntensityParameter, 0.f);
		DigImpactComponent->Deactivate();
	}
}

void ARRDrone::PlayDigDebris(FIntPoint Cell, ERRCellType Type)
{
	URRNiagaraPoolSubsystem* FXPool = GetWorld() ? GetWorld()->GetSubsystem<URRNiagaraPoolSubsystem>() : nullptr;
	if (!FXPool || !DigDebrisSystem || !IsValid(DigGrid) || !IsValid(OwnerMiner))
	{
		return;
	}
	const FVector Location = DigGrid->GetCellCenter(Cell.X, Cell.Y) + FVector(0.0, 0.0, DigDebrisSpawnHeight);
	const FIntPoint FaceSide = GetDigFaceSide(Cell);
	const FLinearColor Color = GetChipColorForType(Type);
	FXPool->Spawn(DigDebrisSystem, Location, FRotator::ZeroRotator, DigDebrisPoolSize, [&FaceSide, &Color](UNiagaraComponent& Component)
	{
		static const FName ChipColorParameter(TEXT("User.ChipColor"));
		static const FName ThrowDirectionParameter(TEXT("User.ThrowDirection"));
		Component.SetVariableLinearColor(ChipColorParameter, Color);
		Component.SetVariableVec3(ThrowDirectionParameter, FVector(FaceSide.X, FaceSide.Y, 0.0));
	});
}

FLinearColor ARRDrone::GetDigChipColor() const
{
	// Majority type of the dug cells; ties keep the type met first (DigCells is sorted nearest the miner's front first).
	TArray<TPair<ERRCellType, int32>, TInlineAllocator<4>> Counts;
	for (const FIntPoint& Cell : DigCells)
	{
		const ERRCellType Type = DigGrid->GetCellType(Cell);
		TPair<ERRCellType, int32>* Entry = Counts.FindByPredicate([Type](const TPair<ERRCellType, int32>& Pair) { return Pair.Key == Type; });
		if (Entry)
		{
			++Entry->Value;
		}
		else
		{
			Counts.Add({ Type, 1 });
		}
	}

	ERRCellType BestType = ERRCellType::Rock;
	int32 BestCount = 0;
	for (const TPair<ERRCellType, int32>& Pair : Counts)
	{
		if (Pair.Value > BestCount)
		{
			BestType = Pair.Key;
			BestCount = Pair.Value;
		}
	}
	return GetChipColorForType(BestType);
}

FLinearColor ARRDrone::GetChipColorForType(ERRCellType Type) const
{
	switch (Type)
	{
	case ERRCellType::Ore:
	case ERRCellType::SpecialOre:
		return OreChipColor;
	case ERRCellType::HealOre:
		return HealOreChipColor;
	case ERRCellType::Barrel:
		return BarrelChipColor;
	default:
		return RockChipColor;
	}
}
