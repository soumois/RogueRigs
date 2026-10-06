// Rogue Rigs - pickup actor dropped by ore and heal ore blocks and by dying robots: pops out in an arc, rests, then is magnetized into the miner within its pickup radius.

#include "Pickups/RRPickup.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRPickup)

ARRPickup::ARRPickup()
{
	// Moved by URRPickupSubsystem; the actor tick is only the fallback when the subsystem is missing.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

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

	// Motion and collection are computed, never physical: no component of a pickup may block or carry a character
	// (a Blueprint mesh such as the hidden XP trail would otherwise stop the miner or let it climb onto the rock).
	TInlineComponentArray<UPrimitiveComponent*> Primitives(this);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Primitive->CanCharacterStepUpOn = ECB_No;
	}

	PopStart = GetActorLocation();
	const float Yaw = bHasPopYaw ? FMath::DegreesToRadians(PopYawDegrees) : FMath::FRandRange(0.f, UE_TWO_PI);
	const float MinDistance = FMath::Min(PopMinDistance, PopMaxDistance);
	const float Distance = FMath::FRandRange(MinDistance, FMath::Max(PopMinDistance, PopMaxDistance));
	PopEnd = PopStart + FVector(FMath::Cos(Yaw) * Distance, FMath::Sin(Yaw) * Distance, HoverHeight);
	PopElapsed = 0.f;
	RestElapsed = 0.f;
	MagnetSpeed = 0.f;
	Phase = ERRPickupPhase::Pop;

	URRPickupSubsystem* Pickups = GetWorld() ? GetWorld()->GetSubsystem<URRPickupSubsystem>() : nullptr;
	if (Pickups)
	{
		Pickups->RegisterPickup(this);
	}
	else
	{
		SetActorTickEnabled(true);
	}
}

void ARRPickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URRPickupSubsystem* Pickups = GetWorld() ? GetWorld()->GetSubsystem<URRPickupSubsystem>() : nullptr)
	{
		Pickups->UnregisterPickup(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ARRPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ARRMiner* Miner = FindMiner();
	const float Radius = Miner ? Miner->GetPickupRadius() : 0.f;
	UpdateMotion(DeltaSeconds, Miner, FMath::Square(Radius));
}

void ARRPickup::UpdateMotion(float DeltaSeconds, ARRMiner* Miner, float PickupRadiusSquared)
{
	switch (Phase)
	{
	case ERRPickupPhase::Pop:
		if (!UpdatePop(DeltaSeconds))
		{
			return;
		}
		Phase = ERRPickupPhase::Rest;
		// The rest delay counts from the end of the pop.
		return;

	case ERRPickupPhase::Rest:
		// Rest on the floor for AttractDelay after the pop so the drop reads before it flies away.
		if (RestElapsed < AttractDelay)
		{
			RestElapsed += DeltaSeconds;
			return;
		}
		if (!Miner || FVector::DistSquared2D(GetActorLocation(), Miner->GetActorLocation()) > PickupRadiusSquared)
		{
			return;
		}
		TargetMiner = Miner;
		Phase = ERRPickupPhase::Magnet;
		MagnetSpeed = MagnetStartSpeed;
		OnMagnetStart();
		FlyToMiner(Miner, DeltaSeconds);
		return;

	case ERRPickupPhase::Magnet:
		// Latched: keeps flying to the miner it latched onto, even out of radius.
		if (IsValid(TargetMiner))
		{
			FlyToMiner(TargetMiner, DeltaSeconds);
		}
		return;

	case ERRPickupPhase::Collected:
		return;
	}
}

bool ARRPickup::UpdatePop(float DeltaSeconds)
{
	PopElapsed += DeltaSeconds;
	const float Alpha = PopDuration > 0.f ? FMath::Clamp(PopElapsed / PopDuration, 0.f, 1.f) : 1.f;

	// Horizontal: ease-out over the whole pop (fast ejection, slows through the bounce).
	const float Horizontal = 1.f - FMath::Square(1.f - Alpha);
	FVector Location = PopStart + (PopEnd - PopStart) * FVector(Horizontal, Horizontal, 0.f);

	// Vertical: main arc from the spawn Z to the rest Z, then a small bounce on the rest Z.
	const float BounceFraction = FMath::Clamp(PopBounceFraction, 0.f, 0.9f);
	const float ArcEnd = 1.f - BounceFraction;
	if (Alpha < ArcEnd)
	{
		const float T = Alpha / ArcEnd;
		Location.Z = FMath::Lerp(PopStart.Z, PopEnd.Z, T) + PopArcHeight * 4.f * T * (1.f - T);
	}
	else
	{
		const float T = BounceFraction > 0.f ? (Alpha - ArcEnd) / BounceFraction : 1.f;
		Location.Z = PopEnd.Z + PopBounceHeight * 4.f * T * (1.f - T);
	}
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
	MagnetSpeed = FMath::Min(MagnetSpeed + MagnetAcceleration * DeltaSeconds, MagnetMaxSpeed);

	const FVector Target = Miner->GetActorLocation();
	const FVector ToTarget = Target - GetActorLocation();
	const float Distance = ToTarget.Size();
	const float Step = MagnetSpeed * DeltaSeconds;

	if (Distance <= CollectDistance || Distance <= Step)
	{
		SetActorLocation(Target);
		Collect(Miner);
		return;
	}

	SetActorLocation(GetActorLocation() + ToTarget / Distance * Step);
	if (Distance - Step <= CollectDistance)
	{
		Collect(Miner);
	}
}

void ARRPickup::Collect(ARRMiner* Miner)
{
	if (Phase == ERRPickupPhase::Collected)
	{
		return;
	}
	Phase = ERRPickupPhase::Collected;

	const FVector Location = GetActorLocation();
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRPickup: %s collected by %s."), *UEnum::GetValueAsString(PickupType), *GetNameSafe(Miner));

	if (Miner)
	{
		if (PickupType == ERRPickupType::XP && Miner->GetProgression())
		{
			Miner->GetProgression()->AddXP(XPAmount);
		}
		Miner->NotifyPickupCollected(PickupType, Location);
	}
	OnCollected(Location);
	Destroy();
}
