// Rogue Rigs - pickup actor dropped by ore and heal ore blocks: pops out, then flies to the miner within its pickup radius.

#include "Pickups/RRPickup.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRPickup)

ARRPickup::ARRPickup()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	PickupMesh->SetGenerateOverlapEvents(false);
	PickupMesh->SetCanEverAffectNavigation(false);
	PickupMesh->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(PickupMesh);
}

void ARRPickup::BeginPlay()
{
	Super::BeginPlay();

	PopStart = GetActorLocation();
	PopEnd = PopStart + FVector(FMath::FRandRange(-PopMaxOffset, PopMaxOffset), FMath::FRandRange(-PopMaxOffset, PopMaxOffset), HoverHeight);
	PopElapsed = 0.f;
	RestElapsed = 0.f;
	bPopDone = false;
}

void ARRPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bCollected)
	{
		return;
	}

	if (!bPopDone)
	{
		bPopDone = UpdatePop(DeltaSeconds);
		if (!bPopDone)
		{
			return;
		}
	}

	if (!bAttracted)
	{
		// Rest on the floor for AttractDelay after the pop so the drop reads before it flies away.
		if (RestElapsed < AttractDelay)
		{
			RestElapsed += DeltaSeconds;
			return;
		}
	}

	ARRMiner* Miner = FindMiner();
	if (!Miner)
	{
		return;
	}

	if (!bAttracted)
	{
		const float Radius = Miner->GetPickupRadius();
		bAttracted = FVector::DistSquared2D(GetActorLocation(), Miner->GetActorLocation()) <= FMath::Square(Radius);
	}

	if (bAttracted)
	{
		FlyToMiner(Miner, DeltaSeconds);
	}
}

bool ARRPickup::UpdatePop(float DeltaSeconds)
{
	PopElapsed += DeltaSeconds;
	const float Alpha = PopDuration > 0.f ? FMath::Clamp(PopElapsed / PopDuration, 0.f, 1.f) : 1.f;

	// Linear horizontal travel plus a parabolic bump (0 at both ends, PopArcHeight at mid-pop).
	FVector Location = FMath::Lerp(PopStart, PopEnd, Alpha);
	Location.Z += PopArcHeight * 4.f * Alpha * (1.f - Alpha);
	SetActorLocation(Location);

	return Alpha >= 1.f;
}

ARRMiner* ARRPickup::FindMiner()
{
	if (!TargetMiner)
	{
		TargetMiner = Cast<ARRMiner>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
	return TargetMiner;
}

void ARRPickup::FlyToMiner(ARRMiner* Miner, float DeltaSeconds)
{
	const FVector Target = Miner->GetActorLocation();
	const FVector ToTarget = Target - GetActorLocation();
	const float Distance = ToTarget.Size();
	const float Step = FlySpeed * DeltaSeconds;

	if (Distance <= CollectDistance || Distance <= Step)
	{
		SetActorLocation(Target);
		Collect(Miner);
		return;
	}

	SetActorLocation(GetActorLocation() + ToTarget / Distance * Step);
	if (FVector::Dist(GetActorLocation(), Target) <= CollectDistance)
	{
		Collect(Miner);
	}
}

void ARRPickup::Collect(ARRMiner* Miner)
{
	if (bCollected)
	{
		return;
	}
	bCollected = true;

	UE_LOG(LogRogueRigs, Log, TEXT("ARRPickup: %s collected by %s."), *UEnum::GetValueAsString(PickupType), *GetNameSafe(Miner));

	if (Miner)
	{
		Miner->NotifyPickupCollected(PickupType);
	}
	Destroy();
}
