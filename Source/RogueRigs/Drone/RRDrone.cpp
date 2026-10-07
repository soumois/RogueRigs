// Rogue Rigs - the helper drone: separate actor spawned by the miner; orbits it, cuts an arc of blocks with a laser (dig) and links to an objective (hack).

#include "Drone/RRDrone.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
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
	OrbitAngle = 0.f;

	if (OwnerMiner)
	{
		// Follow the miner's position of this frame, not the previous one.
		AddTickPrerequisiteActor(OwnerMiner);
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
	// The drone cannot dig and hack at the same time: the objective releases it first.
	if (State == ERRDroneState::Hack || !IsValid(OwnerMiner) || !IsValid(Grid))
	{
		return false;
	}

	const FVector Direction = FVector(ArcDirection.X, ArcDirection.Y, 0.0).GetSafeNormal();
	TArray<FIntPoint> NewCells;
	if (Direction.IsZero() || SelectArcCells(Grid, OwnerMiner->GetActorLocation(), Direction, NewCells) == 0)
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

FVector ARRDrone::GetDigTargetPoint(FIntPoint Cell) const
{
	if (!IsValid(DigGrid) || !IsValid(OwnerMiner))
	{
		return FVector::ZeroVector;
	}

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
	return DigGrid->GetCellFaceCenter(Cell, Side);
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
}

void ARRDrone::BeginPlay()
{
	Super::BeginPlay();
	EnsureLaserBeams(MaxArcBlocks);
}

void ARRDrone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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

	const float SafePeriod = FMath::Max(OrbitPeriod, UE_KINDA_SMALL_NUMBER);
	OrbitAngle = FMath::Fmod(OrbitAngle + UE_TWO_PI * DeltaSeconds / SafePeriod, UE_TWO_PI);

	// VInterpTo snaps to the target when the speed is 0.
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), ComputeOrbitPoint(), DeltaSeconds, FollowInterpSpeed));
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

	SetActorLocation(FMath::VInterpTo(GetActorLocation(), ComputeDigHoverPoint(), DeltaSeconds, FollowInterpSpeed));

	// Lasers first: the face centers are still valid before the hit that may destroy the blocks.
	UpdateDigLasers();

	// Every block of the arc is cut in parallel at the full rate. A block of an ore or heal vein makes the whole vein one
	// target (ore-blocks, owner 2026-10-06 fourth test): each vein block takes the damage once per tick, however many of
	// its blocks are in the arc, so the vein breaks whole after one block dig time. Dig holds even when the arc empties:
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
		DigGrid->GetVeinCells(Cell, Vein);
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

	float Sin = 0.f;
	float Cos = 0.f;
	FMath::SinCos(&Sin, &Cos, OrbitAngle);

	return FVector(MinerLocation.X + Cos * OrbitRadius, MinerLocation.Y + Sin * OrbitRadius, FloorZ + HoverHeight);
}

FVector ARRDrone::ComputeDigHoverPoint() const
{
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const double FloorZ = MinerLocation.Z - OwnerMiner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// Arc centre: mean of the dug cell centers; keep the current XY while the arc is empty.
	FVector Centre = GetActorLocation();
	if (DigCells.Num() > 0)
	{
		Centre = FVector::ZeroVector;
		for (const FIntPoint& Cell : DigCells)
		{
			Centre += DigGrid->GetCellCenter(Cell.X, Cell.Y);
		}
		Centre /= DigCells.Num();
	}
	return FVector(Centre.X, Centre.Y, FloorZ + HoverHeight);
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

	HideLaserBeams();
	DigGrid = nullptr;

	ResumeOrbitFromCurrentAngle();
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

	SetActorLocation(FMath::VInterpTo(GetActorLocation(), FVector(HoverPoint.X, HoverPoint.Y, FloorZ + HoverHeight), DeltaSeconds, FollowInterpSpeed));
	UpdateLaser(LaserMesh, GetActorLocation(), TargetLocation + HackLinkOffset);
}

void ARRDrone::EndHack()
{
	HideLaserBeams();
	HackTarget = nullptr;
	ResumeOrbitFromCurrentAngle();
}

void ARRDrone::ResumeOrbitFromCurrentAngle()
{
	// Resume the orbit from where the drone is, so it does not cut across the miner.
	if (IsValid(OwnerMiner))
	{
		const FVector FromMiner = GetActorLocation() - OwnerMiner->GetActorLocation();
		if (FromMiner.SizeSquared2D() > UE_KINDA_SMALL_NUMBER)
		{
			OrbitAngle = FMath::Fmod(FMath::Atan2(FromMiner.Y, FromMiner.X) + UE_TWO_PI, UE_TWO_PI);
		}
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

void ARRDrone::EnsureLaserBeams(int32 Count)
{
	if (LaserBeams.Num() == 0)
	{
		LaserBeams.Add(LaserMesh);
	}

	while (LaserBeams.Num() < Count)
	{
		// Copy of LaserMesh: same mesh and materials (set on LaserMesh in Blueprint), same world-space setup.
		UStaticMeshComponent* Beam = NewObject<UStaticMeshComponent>(this, MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), TEXT("LaserBeam")));
		Beam->SetupAttachment(Mesh);
		Beam->SetUsingAbsoluteLocation(true);
		Beam->SetUsingAbsoluteRotation(true);
		Beam->SetUsingAbsoluteScale(true);
		Beam->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		Beam->SetGenerateOverlapEvents(false);
		Beam->SetCanEverAffectNavigation(false);
		Beam->SetCastShadow(false);
		Beam->SetVisibility(false);
		Beam->SetStaticMesh(LaserMesh->GetStaticMesh());
		for (int32 MaterialIndex = 0; MaterialIndex < LaserMesh->GetNumMaterials(); ++MaterialIndex)
		{
			Beam->SetMaterial(MaterialIndex, LaserMesh->GetMaterial(MaterialIndex));
		}
		Beam->RegisterComponent();
		LaserBeams.Add(Beam);
	}
}

void ARRDrone::UpdateDigLasers()
{
	EnsureLaserBeams(DigCells.Num());
	const FVector Start = GetActorLocation();
	for (int32 Index = 0; Index < LaserBeams.Num(); ++Index)
	{
		if (Index < DigCells.Num())
		{
			UpdateLaser(LaserBeams[Index], Start, GetDigTargetPoint(DigCells[Index]));
		}
		else
		{
			LaserBeams[Index]->SetVisibility(false);
		}
	}
}

void ARRDrone::HideLaserBeams()
{
	LaserMesh->SetVisibility(false);
	for (UStaticMeshComponent* Beam : LaserBeams)
	{
		if (Beam)
		{
			Beam->SetVisibility(false);
		}
	}
}
