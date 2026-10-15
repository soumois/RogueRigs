// Rogue Rigs - pickup actor dropped by ore and heal ore blocks and by dying robots: pops out in an arc, rests, then is magnetized into the miner within its pickup radius.

#include "Pickups/RRPickup.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRGE_Heal.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
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

	HealEffectClass = URRGE_Heal::StaticClass();
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

	StartLife();
}

void ARRPickup::StartLife()
{
	if (bLifeStarted)
	{
		return;
	}
	bLifeStarted = true;

	// task-192-01: mesh variant and rest orientation of this life, before the pop moves the actor.
	ApplyLifeLook();

	PopStart = GetActorLocation();
	const float Yaw = bHasPopYaw ? FMath::DegreesToRadians(PopYawDegrees) : FMath::FRandRange(0.f, UE_TWO_PI);
	const float MinDistance = FMath::Min(PopMinDistance, PopMaxDistance);
	const float Distance = FMath::FRandRange(MinDistance, FMath::Max(PopMinDistance, PopMaxDistance));
	PopEnd = PopStart + FVector(FMath::Cos(Yaw) * Distance, FMath::Sin(Yaw) * Distance, HoverHeight);
	PopElapsed = 0.f;
	RestElapsed = 0.f;
	MagnetSpeed = 0.f;
	Phase = ERRPickupPhase::Pop;
	TargetMiner = nullptr;
	// Glow off in the air; written explicitly so a fresh mesh never shows the material default.
	ResetGlowFade();

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

void ARRPickup::ApplyLifeLook()
{
	if (MeshVariants.Num() > 0)
	{
		TArray<UStaticMesh*, TInlineAllocator<8>> ValidMeshes;
		for (UStaticMesh* Mesh : MeshVariants)
		{
			if (Mesh)
			{
				ValidMeshes.Add(Mesh);
			}
		}
		if (ValidMeshes.Num() > 0)
		{
			// SetStaticMesh keeps OverrideMaterials and is a no-op when the same mesh is drawn again.
			PickupMesh->SetStaticMesh(ValidMeshes[FMath::RandRange(0, ValidMeshes.Num() - 1)]);
		}
	}

	switch (RestOrientation)
	{
	case ERRPickupRestOrientation::None:
		return;

	case ERRPickupRestOrientation::YawOnly:
		SetActorRotation(FRotator(0.f, FMath::FRandRange(-180.f, 180.f), 0.f));
		return;

	case ERRPickupRestOrientation::Full:
	{
		// Uniform random rotation (Shoemake): no bias toward any pole, unlike three independent uniform angles.
		const float U1 = FMath::FRand();
		const float U2 = FMath::FRandRange(0.f, UE_TWO_PI);
		const float U3 = FMath::FRandRange(0.f, UE_TWO_PI);
		const float A = FMath::Sqrt(1.f - U1);
		const float B = FMath::Sqrt(U1);
		SetActorRotation(FQuat(A * FMath::Sin(U2), A * FMath::Cos(U2), B * FMath::Sin(U3), B * FMath::Cos(U3)).GetNormalized());
		return;
	}
	}
}

void ARRPickup::OnPoolAcquired_Implementation()
{
	// Fresh pool actors started their life in BeginPlay; reused ones start it here, after the caller set their values.
	StartLife();
	TInlineComponentArray<UFXSystemComponent*> Effects(this);
	for (UFXSystemComponent* Effect : Effects)
	{
		if (Effect->bAutoActivate && !Effect->IsActive())
		{
			Effect->Activate(true);
		}
	}
}

void ARRPickup::OnPoolReleased_Implementation()
{
	if (URRPickupSubsystem* Pickups = GetWorld() ? GetWorld()->GetSubsystem<URRPickupSubsystem>() : nullptr)
	{
		Pickups->UnregisterPickup(this);
	}
	SetActorTickEnabled(false);
	bLifeStarted = false;

	// Trail (BP_PickupXP TrailFX, activated by OnMagnetStart): particles killed now so a reused cube never shows an old ribbon.
	TInlineComponentArray<UFXSystemComponent*> Effects(this);
	for (UFXSystemComponent* Effect : Effects)
	{
		Effect->DeactivateImmediate();
	}

	// Motion state and magnet latch.
	Phase = ERRPickupPhase::Pop;
	TargetMiner = nullptr;
	PopElapsed = 0.f;
	RestElapsed = 0.f;
	MagnetSpeed = 0.f;
	ResetGlowFade();

	// Per-drop values back to the class defaults: the next caller sets its own before OnPoolAcquired (a heal request with
	// HealAmount 0 keeps the class default, as a fresh spawn would).
	const ARRPickup* Defaults = GetDefault<ARRPickup>(GetClass());
	XPAmount = Defaults->XPAmount;
	OreValue = Defaults->OreValue;
	HealAmount = Defaults->HealAmount;
	PopYawDegrees = 0.f;
	bHasPopYaw = false;
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
	// task-148-01: glow fade toward the target set by the last phase change; nothing to do once reached (or bGlowFade off).
	if (bGlowFade && GlowFade != GlowFadeTarget)
	{
		UpdateGlowFade(DeltaSeconds);
	}

	switch (Phase)
	{
	case ERRPickupPhase::Pop:
		if (!UpdatePop(DeltaSeconds))
		{
			return;
		}
		Phase = ERRPickupPhase::Rest;
		// Landed: the glow disc fades in.
		SetGlowFadeTarget(1.f);
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
		// heal-ore: a heal pickup waits on the floor while the miner is at full health; re-checked every frame, so it latches
		// on the first frame the miner is both damaged and within radius (no event binding, nothing to reset).
		if (IsRefusedByMiner(Miner))
		{
			return;
		}
		TargetMiner = Miner;
		Phase = ERRPickupPhase::Magnet;
		SetGlowFadeTarget(0.f);
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

bool ARRPickup::IsRefusedByMiner(const ARRMiner* Miner) const
{
	if (PickupType != ERRPickupType::Heal || !Miner)
	{
		return false;
	}
	const URRHealthAttributeSet* Health = Miner->GetHealthSet();
	return Health && Health->GetHealth() >= Health->GetMaxHealth();
}

void ARRPickup::ReturnToRest()
{
	Phase = ERRPickupPhase::Rest;
	SetGlowFadeTarget(1.f);
	MagnetSpeed = 0.f;
	// RestElapsed already passed AttractDelay: attracted again on the first frame the miner is damaged and in radius.
	FVector Location = GetActorLocation();
	Location.Z = PopEnd.Z;
	SetActorLocation(Location);
}

void ARRPickup::UpdateGlowFade(float DeltaSeconds)
{
	const float Step = GlowFadeDuration > 0.f ? DeltaSeconds / GlowFadeDuration : 1.f;
	GlowFade = FMath::Clamp(GlowFade + FMath::Clamp(GlowFadeTarget - GlowFade, -Step, Step), 0.f, 1.f);
	if (FMath::Abs(GlowFadeTarget - GlowFade) <= UE_KINDA_SMALL_NUMBER)
	{
		GlowFade = GlowFadeTarget;
	}
	PickupMesh->SetCustomPrimitiveDataFloat(GlowFadeDataIndex, GlowFade);
}

void ARRPickup::ResetGlowFade()
{
	if (!bGlowFade)
	{
		return;
	}
	GlowFade = 0.f;
	GlowFadeTarget = 0.f;
	PickupMesh->SetCustomPrimitiveDataFloat(GlowFadeDataIndex, 0.f);
}

void ARRPickup::FlyToMiner(ARRMiner* Miner, float DeltaSeconds)
{
	// A heal pickup in flight when the miner reaches full health (another heal collected first) is not consumed.
	if (IsRefusedByMiner(Miner))
	{
		ReturnToRest();
		return;
	}

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
	SetGlowFadeTarget(0.f);

	const FVector Location = GetActorLocation();
	UE_LOG(LogRogueRigs, Verbose, TEXT("ARRPickup: %s collected by %s."), *UEnum::GetValueAsString(PickupType), *GetNameSafe(Miner));

	if (Miner)
	{
		if (PickupType == ERRPickupType::XP && Miner->GetProgression())
		{
			Miner->GetProgression()->AddXP(XPAmount);
		}
		if (PickupType == ERRPickupType::Heal)
		{
			ApplyHeal(Miner);
		}
		if (PickupType == ERRPickupType::Ore && OreValue > 0)
		{
			// Ore counter (ore-economy): the run flow owns the carried count.
			if (URRRunFlowSubsystem* RunFlow = GetWorld() ? GetWorld()->GetSubsystem<URRRunFlowSubsystem>() : nullptr)
			{
				RunFlow->AddCarriedOre(OreValue);
			}
		}
		Miner->NotifyPickupCollected(PickupType, Location);
	}
	OnCollected(Location);
	// task-092-01: pool pickups go back to the pool (OnPoolReleased unregisters them, safe inside the subsystem loop).
	URRActorPoolSubsystem* Pool = GetWorld() ? GetWorld()->GetSubsystem<URRActorPoolSubsystem>() : nullptr;
	if (Pool && Pool->IsPoolMember(this) && !Pool->IsIdle(this))
	{
		Pool->Release(this);
		return;
	}
	Destroy();
}

void ARRPickup::ApplyHeal(ARRMiner* Miner) const
{
	URRAbilitySystemComponent* ASC = Miner ? Miner->GetRRAbilitySystemComponent() : nullptr;
	if (!ASC || !HealEffectClass || HealAmount <= 0.f || Miner->IsDead())
	{
		return;
	}
	const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(HealEffectClass, 1.f, ASC->MakeEffectContext());
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Heal, HealAmount);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}
