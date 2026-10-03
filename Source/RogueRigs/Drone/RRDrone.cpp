// Rogue Rigs - the helper drone: separate actor spawned by the miner; orbits it, cuts blocks with a laser (dig) and links to an objective (hack).

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
		if (!IsValid(OwnerMiner) || !IsValid(DigGrid) || !DigGrid->IsBlockCell(DigCell))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: cannot enter Dig without a miner and a block target (use RequestDig); staying in %s."),
				*GetName(), *UEnum::GetValueAsString(State));
			return false;
		}
		if (State != ERRDroneState::Dig)
		{
			State = ERRDroneState::Dig;
			DigGrid->SetCellShaking(DigCell, true);
		}
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

bool ARRDrone::RequestDig(ARRArenaGrid* Grid, FIntPoint Cell, FIntPoint MinerSide)
{
	// The drone cannot dig and hack at the same time: the objective releases it first.
	if (State == ERRDroneState::Hack || !IsValid(OwnerMiner) || !IsValid(Grid) || !Grid->IsBlockCell(Cell))
	{
		return false;
	}

	// Same target: only the side may change.
	if (State == ERRDroneState::Dig && DigGrid == Grid && DigCell == Cell)
	{
		DigSide = MinerSide;
		return true;
	}

	// New target: release the previous block (its damage stays), then start on this one.
	if (State == ERRDroneState::Dig)
	{
		StopDig();
		State = ERRDroneState::Orbit;
	}

	DigGrid = Grid;
	DigCell = Cell;
	DigSide = MinerSide;
	return SetState(ERRDroneState::Dig);
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
	// Cancel rules: target gone (dug out or blown up), or the miner walked away.
	if (!IsValid(OwnerMiner) || !IsValid(DigGrid) || !DigGrid->IsBlockCell(DigCell))
	{
		SetState(ERRDroneState::Orbit);
		return;
	}

	const FVector BlockCenter = DigGrid->GetCellCenter(DigCell.X, DigCell.Y);
	const float CancelDistance = DigCancelDistanceCells * DigGrid->GetCellSize();
	if (FVector::DistSquared2D(OwnerMiner->GetActorLocation(), BlockCenter) > FMath::Square(CancelDistance))
	{
		SetState(ERRDroneState::Orbit);
		return;
	}

	SetActorLocation(FMath::VInterpTo(GetActorLocation(), ComputeDigHoverPoint(), DeltaSeconds, FollowInterpSpeed));

	// Laser first: the face center is still valid before the hit that may destroy the block.
	UpdateLaser(GetActorLocation(), DigGrid->GetCellFaceCenter(DigCell, DigSide));

	if (DigGrid->ApplyTerrainDamage(DigCell, DeltaSeconds * GetMiningSpeed(), ERRTerrainDamageSource::Drone))
	{
		SetState(ERRDroneState::Orbit);
	}
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

	// Outward normal of the cut face, in world space: from the block center to the face center.
	const FVector FaceCenter = DigGrid->GetCellFaceCenter(DigCell, DigSide);
	const FVector BlockCenter = DigGrid->GetCellCenter(DigCell.X, DigCell.Y);
	const FVector FaceNormal = (FaceCenter - BlockCenter).GetSafeNormal2D();

	const FVector HoverPoint = FaceCenter + FaceNormal * DigHoverDistance;
	return FVector(HoverPoint.X, HoverPoint.Y, FloorZ + HoverHeight);
}

void ARRDrone::StopDig()
{
	if (IsValid(DigGrid))
	{
		DigGrid->SetCellShaking(DigCell, false);
	}

	LaserMesh->SetVisibility(false);
	DigGrid = nullptr;
	DigCell = FIntPoint(-1, -1);
	DigSide = FIntPoint(0, 0);

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
	UpdateLaser(GetActorLocation(), TargetLocation + HackLinkOffset);
}

void ARRDrone::EndHack()
{
	LaserMesh->SetVisibility(false);
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

void ARRDrone::UpdateLaser(const FVector& Start, const FVector& End)
{
	const FVector Beam = End - Start;
	const double Length = Beam.Size();
	if (Length <= UE_KINDA_SMALL_NUMBER)
	{
		LaserMesh->SetVisibility(false);
		return;
	}

	// The cylinder is centered on its pivot and runs along local Z.
	const double WidthScale = LaserDiameter / FMath::Max(LaserMeshBaseDiameter, UE_KINDA_SMALL_NUMBER);
	const double LengthScale = Length / FMath::Max(LaserMeshBaseHeight, UE_KINDA_SMALL_NUMBER);
	const FRotator Rotation = FRotationMatrix::MakeFromZ(Beam / Length).Rotator();

	LaserMesh->SetWorldTransform(FTransform(Rotation, Start + Beam * 0.5, FVector(WidthScale, WidthScale, LengthScale)));
	LaserMesh->SetVisibility(true);
}
