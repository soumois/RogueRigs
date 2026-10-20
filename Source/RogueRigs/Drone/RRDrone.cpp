// Rogue Rigs - the helper drone: separate actor spawned by the miner; escorts it (Orbit state), cuts an arc of blocks with a laser (dig) and links to an objective (hack).

#include "Drone/RRDrone.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Grid/RRCellType.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "RogueRigs.h"

ARRDrone::ARRDrone()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCanEverAffectNavigation(false);
	RootComponent = Mesh;

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
	EscortMoveBlend = 0.f;
	EscortFigureEightPhase = 0.f;
	EscortBobPhase = 0.f;
	EscortVelocity = FVector::ZeroVector;

	if (OwnerMiner)
	{
		// Follow the miner's position of this frame, not the previous one.
		AddTickPrerequisiteActor(OwnerMiner);
		// Until the miner first moves, the escort heading is its facing: the drone starts at its shoulder.
		EscortHeadingYaw = OwnerMiner->GetActorRotation().Yaw;
		EscortSlotIndex = SelectEscortSlot();
		EscortSlotHoldTime = 0.f;
		SetActorLocation(ComputeOrbitPoint());
	}
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

	// The impact effect plays only in Dig (UpdateDigImpact): it fades after a dig and never shows in Hack.
	if (State == ERRDroneState::Orbit)
	{
		FadeDigImpact(DeltaSeconds);
	}
	else if (State == ERRDroneState::Hack)
	{
		StopDigImpact();
	}
}

void ARRDrone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopDigImpact();
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

	// Heading: turns toward the miner's horizontal velocity at a limited rate (the slot swings around the miner, never
	// across it); kept while the miner is at rest.
	const FVector MinerVelocity = OwnerMiner->GetVelocity();
	const double MinerSpeed = MinerVelocity.Size2D();
	if (MinerSpeed > EscortHeadingMinSpeed)
	{
		const float TargetYaw = FMath::RadiansToDegrees(FMath::Atan2(MinerVelocity.Y, MinerVelocity.X));
		EscortHeadingYaw = FRotator::NormalizeAxis(FMath::FixedTurn(EscortHeadingYaw, TargetYaw, EscortHeadingTurnRate * DeltaSeconds));
	}

	// Move blend: the miner's speed fraction of its max speed, smoothed, so the slot slides from the shoulder to behind.
	const UCharacterMovementComponent* MinerMovement = OwnerMiner->GetCharacterMovement();
	const double MaxSpeed = MinerMovement ? MinerMovement->GetMaxSpeed() : 0.0;
	const float SpeedFraction = MaxSpeed > UE_KINDA_SMALL_NUMBER ? static_cast<float>(FMath::Clamp(MinerSpeed / MaxSpeed, 0.0, 1.0)) : 0.f;
	EscortMoveBlend = FMath::FInterpTo(EscortMoveBlend, SpeedFraction, DeltaSeconds, EscortBlendSpeed);

	EscortFigureEightPhase = FMath::Fmod(EscortFigureEightPhase + UE_TWO_PI * DeltaSeconds / FMath::Max(EscortFigureEightPeriod, 0.1f), UE_TWO_PI);
	EscortBobPhase = FMath::Fmod(EscortBobPhase + UE_TWO_PI * DeltaSeconds / FMath::Max(EscortBobPeriod, 0.1f), UE_TWO_PI);

	// Walls (task-248-01): the slot is the first clear candidate, held a minimum time.
	UpdateEscortSlot(DeltaSeconds);

	const FVector Slot = ComputeOrbitPoint();
	FVector Location = GetActorLocation();
	if (FVector::DistSquared(Location, Slot) > FMath::Square(EscortSnapDistance))
	{
		// Miner teleported (cheat, run transition): no spring flight across the arena.
		EscortVelocity = FVector::ZeroVector;
		SetActorLocation(Slot);
		return;
	}

	// Damped spring toward the slot; damping acts on the velocity relative to part of the miner's velocity (feed-forward),
	// so the drone lags a little behind a moving slot and overshoots a little when the miner stops. Semi-implicit Euler,
	// sub-stepped for stability at low frame rates.
	const double Omega = UE_TWO_PI * FMath::Max(EscortSpringFrequency, 0.01f);
	const FVector TargetVelocity = MinerVelocity * FMath::Clamp(EscortVelocityFeedForward, 0.f, 1.f);
	constexpr double MaxSubstepSeconds = 1.0 / 120.0;
	const int32 Substeps = FMath::Max(1, FMath::CeilToInt32(DeltaSeconds / MaxSubstepSeconds));
	const double Step = DeltaSeconds / Substeps;
	const FVector Start = Location;
	for (int32 Index = 0; Index < Substeps; ++Index)
	{
		const FVector Acceleration = (Slot - Location) * (Omega * Omega) + (TargetVelocity - EscortVelocity) * (2.0 * EscortDampingRatio * Omega);
		EscortVelocity += Acceleration * Step;
		Location += EscortVelocity * Step;
	}
	// Overshoot or a slot change across a corner: never into a block cell.
	SetActorLocation(ConstrainToClearCells(GetTerrainGrid(), Start, Location, &EscortVelocity));
}

const ARRArenaGrid* ARRDrone::GetTerrainGrid() const
{
	if (IsValid(DigGrid))
	{
		return DigGrid;
	}
	return IsValid(OwnerMiner) ? OwnerMiner->GetArenaGrid() : nullptr;
}

float ARRDrone::GetEscortCandidateYaw(int32 SlotIndex) const
{
	const float SideSign = bEscortOnRightSide ? 1.f : -1.f;
	const float IdealYaw = EscortHeadingYaw + SideSign * FMath::Lerp(EscortRestAngleDegrees, EscortMoveAngleDegrees, EscortMoveBlend);
	switch (SlotIndex)
	{
	case 1:
		return IdealYaw + SideSign * EscortAlternateAngleDegrees;
	case 2:
		return IdealYaw - SideSign * EscortAlternateAngleDegrees;
	case 3:
		return IdealYaw + SideSign * EscortWideAngleDegrees;
	case 4:
		return IdealYaw - SideSign * EscortWideAngleDegrees;
	case 5:
		return EscortHeadingYaw + 180.f;
	default:
		return IdealYaw;
	}
}

bool ARRDrone::IsEscortSlotClear(const ARRArenaGrid* Grid, int32 SlotIndex) const
{
	if (!Grid || SlotIndex == EscortFallbackSlotIndex)
	{
		return true;
	}

	float Sin = 0.f;
	float Cos = 0.f;
	FMath::SinCos(&Sin, &Cos, FMath::DegreesToRadians(GetEscortCandidateYaw(SlotIndex)));
	const FVector Radial(Cos, Sin, 0.f);
	const FVector Tangential(-Sin, Cos, 0.f);
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const FVector Slot = MinerLocation + Radial * EscortDistance;

	// The miner-to-slot segment, a margin past the slot, then the figure-eight span across it (it fades out while moving).
	if (!Grid->HasLineOfSight(MinerLocation, Slot + Radial * DroneWallClearance))
	{
		return false;
	}
	const float TangentialReach = EscortFigureEightTangential * (1.f - EscortMoveBlend) + DroneWallClearance;
	return Grid->HasLineOfSight(Slot, Slot + Tangential * TangentialReach) && Grid->HasLineOfSight(Slot, Slot - Tangential * TangentialReach);
}

int32 ARRDrone::SelectEscortSlot() const
{
	const ARRArenaGrid* Grid = GetTerrainGrid();
	for (int32 SlotIndex = 0; SlotIndex < EscortFallbackSlotIndex; ++SlotIndex)
	{
		if (IsEscortSlotClear(Grid, SlotIndex))
		{
			return SlotIndex;
		}
	}
	return EscortFallbackSlotIndex;
}

void ARRDrone::UpdateEscortSlot(float DeltaSeconds)
{
	EscortSlotHoldTime += DeltaSeconds;
	const int32 BestSlot = SelectEscortSlot();
	if (BestSlot == EscortSlotIndex)
	{
		return;
	}
	// A blocked slot is left at once; a better one waits for the minimum hold, so a corner does not make the drone hesitate.
	const bool bCurrentBlocked = !IsEscortSlotClear(GetTerrainGrid(), EscortSlotIndex);
	if (bCurrentBlocked || EscortSlotHoldTime >= EscortSlotChangeDelay)
	{
		EscortSlotIndex = BestSlot;
		EscortSlotHoldTime = 0.f;
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
		DigGrid->ApplyTerrainDamage(Cell, Damage, ERRTerrainDamageSource::Drone);
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

	// No clear candidate (task-248-01): just above the miner, a bit higher, without figure-eight.
	if (EscortSlotIndex == EscortFallbackSlotIndex)
	{
		return FVector(MinerLocation.X, MinerLocation.Y, FloorZ + HoverHeight + EscortFallbackExtraHeight + EscortBobAmplitude * FMath::Sin(EscortBobPhase));
	}

	// Slot direction: the current candidate (0 = ideal: from the heading, toward the escort side, at the angle blended from the
	// shoulder at rest to behind when moving).
	const float SlotYaw = GetEscortCandidateYaw(EscortSlotIndex);
	float Sin = 0.f;
	float Cos = 0.f;
	FMath::SinCos(&Sin, &Cos, FMath::DegreesToRadians(SlotYaw));
	const FVector Radial(Cos, Sin, 0.f);
	const FVector Tangential(-Sin, Cos, 0.f);

	// Rest figure-eight (Lissajous 1:2): long axis tangential, short axis radial, so the distance to the miner varies little.
	const float RestWeight = 1.f - EscortMoveBlend;
	const FVector FigureEight = (Tangential * (EscortFigureEightTangential * FMath::Sin(EscortFigureEightPhase))
		+ Radial * (EscortFigureEightRadial * FMath::Sin(2.f * EscortFigureEightPhase))) * RestWeight;

	const FVector Horizontal = Radial * EscortDistance + FigureEight;
	return FVector(MinerLocation.X + Horizontal.X, MinerLocation.Y + Horizontal.Y, FloorZ + HoverHeight + EscortBobAmplitude * FMath::Sin(EscortBobPhase));
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
	const FVector TargetLocation = HackTarget->GetActorLocation();
	const FVector TowardMiner = (MinerLocation - TargetLocation).GetSafeNormal2D();
	const FVector HoverPoint = TargetLocation + TowardMiner * HackHoverDistance;

	// The hover point must be clear of rock (task-248-01), else it is pulled back toward the miner.
	const ARRArenaGrid* Grid = GetTerrainGrid();
	const FVector ClearHover = ResolveClearHoverPoint(Grid, FVector(HoverPoint.X, HoverPoint.Y, FloorZ + HoverHeight));
	const FVector From = GetActorLocation();
	SetActorLocation(ConstrainToClearCells(Grid, From, FMath::VInterpTo(From, ClearHover, DeltaSeconds, FollowInterpSpeed)));
	UpdateLaser(LaserMesh, GetActorLocation(), TargetLocation + HackLinkOffset);
}

void ARRDrone::EndHack()
{
	LaserMesh->SetVisibility(false);
	HackTarget = nullptr;
	ResumeOrbit();
}

void ARRDrone::ResumeOrbit()
{
	// The spring starts from where the drone is, at rest: it flies back to the slot smoothly.
	EscortVelocity = FVector::ZeroVector;
	// The slot is chosen afresh after a dig or hack (task-248-01).
	if (IsValid(OwnerMiner))
	{
		EscortSlotIndex = SelectEscortSlot();
		EscortSlotHoldTime = 0.f;
	}
}

void ARRDrone::UpdateLaser(UStaticMeshComponent* Beam, const FVector& Start, const FVector& End)
{
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
	UpdateLaser(LaserMesh, GetActorLocation(), GetDigImpactPoint());
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
