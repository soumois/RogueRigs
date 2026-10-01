// Rogue Rigs - the helper drone: separate actor spawned by the miner; orbits it (dig and hack states declared, not implemented).

#include "Drone/RRDrone.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRPlayerAttributeSet.h"
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
}

void ARRDrone::SetOwnerMiner(ARRMiner* NewOwnerMiner)
{
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
	if (NewState != ERRDroneState::Orbit)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: drone state %s is not implemented yet; staying in %s."),
			*GetName(), *UEnum::GetValueAsString(NewState), *UEnum::GetValueAsString(State));
		return false;
	}

	State = NewState;
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
	case ERRDroneState::Hack:
		// Not implemented yet (SetState refuses them).
		break;
	}
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

FVector ARRDrone::ComputeOrbitPoint() const
{
	const FVector MinerLocation = OwnerMiner->GetActorLocation();
	const double FloorZ = MinerLocation.Z - OwnerMiner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	float Sin = 0.f;
	float Cos = 0.f;
	FMath::SinCos(&Sin, &Cos, OrbitAngle);

	return FVector(MinerLocation.X + Cos * OrbitRadius, MinerLocation.Y + Sin * OrbitRadius, FloorZ + HoverHeight);
}
