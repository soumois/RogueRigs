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
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Progression/RRProgressionComponent.h"
#include "Projectiles/RRCollisionChannels.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRPickup)

namespace RRPickupPhysics
{
	/** task-229-02: runtime switch of the landing hand-off (stress comparisons); read at each landing. */
	static TAutoConsoleVariable<int32> CVarPickupPhysics(
		TEXT("rr.Pickup.Physics"),
		1,
		TEXT("1: pickups with bPhysicsOnLanding become Chaos rigid bodies when they land. 0: kinematic bounce and rest point (pre task-229 motion; the body stays kinematic)."),
		ECVF_Default);

	/** Time step of the finite difference that gives the arc's velocity at touch-down (one 60 Hz frame). */
	constexpr float LandingVelocityStep = 1.f / 60.f;
}

ARRPickup::ARRPickup()
{
	// Moved by URRPickupSubsystem; the actor tick is only the fallback when the subsystem is missing.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	// task-229-02: physics-only body against the floor and walls (kinematic until landing); Blueprints keep this profile.
	PickupMesh->SetMobility(EComponentMobility::Movable);
	PickupMesh->SetCollisionProfileName(RRCollisionChannels::PickupProfile);
	PickupMesh->SetGenerateOverlapEvents(false);
	PickupMesh->SetNotifyRigidBodyCollision(false);
	PickupMesh->SetCanEverAffectNavigation(false);
	PickupMesh->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(PickupMesh);

	HealEffectClass = URRGE_Heal::StaticClass();
}

void ARRPickup::BeginPlay()
{
	Super::BeginPlay();

	// No component of a pickup may block or carry a character (a Blueprint mesh such as the hidden XP trail would otherwise
	// stop the miner or let it climb onto the rock). PickupMesh is the only body: physics only, floor and walls (task-229-02).
	TInlineComponentArray<UPrimitiveComponent*> Primitives(this);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		Primitive->CanCharacterStepUpOn = ECB_No;
		if (Primitive != PickupMesh)
		{
			Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	ConfigurePhysicsBody();

	StartLife();
}

void ARRPickup::ConfigurePhysicsBody()
{
	if (!bPhysicsOnLanding || PhysicsCollisionProfile.IsNone())
	{
		PickupMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		return;
	}
	PickupMesh->SetCollisionProfileName(PhysicsCollisionProfile);
	PickupMesh->SetGenerateOverlapEvents(false);
	PickupMesh->SetNotifyRigidBodyCollision(false);
	PickupMesh->SetLinearDamping(PhysicsLinearDamping);
	PickupMesh->SetAngularDamping(PhysicsAngularDamping);
	PickupMesh->BodyInstance.SetMaxDepenetrationVelocity(PhysicsMaxDepenetrationVelocity);

	URRPickupSubsystem* Pickups = GetWorld() ? GetWorld()->GetSubsystem<URRPickupSubsystem>() : nullptr;
	UPhysicalMaterial* Material = Pickups ? Pickups->FindOrAddPhysicalMaterial(GetClass()) : MakePhysicalMaterial(this);
	PickupMesh->SetPhysMaterialOverride(Material);
}

UPhysicalMaterial* ARRPickup::MakePhysicalMaterial(UObject* Outer) const
{
	// The class is found by path: UPhysicalMaterial::StaticClass would need a link dependency on PhysicsCore (not in RogueRigs.Build.cs).
	UClass* MaterialClass = FindObject<UClass>(nullptr, TEXT("/Script/PhysicsCore.PhysicalMaterial"));
	UPhysicalMaterial* Material = MaterialClass ? static_cast<UPhysicalMaterial*>(NewObject<UObject>(Outer, MaterialClass, NAME_None, RF_Transient)) : nullptr;
	if (!Material)
	{
		return nullptr;
	}
	Material->Friction = PhysicsFriction;
	Material->Restitution = PhysicsRestitution;
	Material->SleepLinearVelocityThreshold = PhysicsSleepLinearThreshold;
	Material->SleepAngularVelocityThreshold = PhysicsSleepAngularThreshold;
	Material->SleepCounterThreshold = PhysicsSleepCounterThreshold;
	return Material;
}

bool ARRPickup::IsSimulatingPhysics() const
{
	return PickupMesh && PickupMesh->IsSimulatingPhysics();
}

bool ARRPickup::CanSimulateOnLanding() const
{
	if (!bPhysicsOnLanding || RRPickupPhysics::CVarPickupPhysics.GetValueOnGameThread() == 0 || !PickupMesh)
	{
		return false;
	}
	const FBodyInstance* Body = PickupMesh->GetBodyInstance();
	return Body && Body->IsValidBodyInstance() && PickupMesh->IsPhysicsCollisionEnabled();
}

void ARRPickup::StartPhysics(const FVector& LinearVelocity, bool bWithSpin)
{
	PickupMesh->SetSimulatePhysics(true);
	PickupMesh->SetPhysicsLinearVelocity(LinearVelocity);
	PickupMesh->SetPhysicsAngularVelocityInRadians(bWithSpin ? SpinAxis * FMath::DegreesToRadians(PopSpinDegPerSec) : FVector::ZeroVector);
}

void ARRPickup::StopPhysics()
{
	if (!IsSimulatingPhysics())
	{
		return;
	}
	PickupMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	PickupMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	PickupMesh->SetSimulatePhysics(false);
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
	// task-229-02: flight spin axis of this life (kinematic until landing; the pool released the body kinematic).
	SpinAxis = FMath::VRand();
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
	// task-229-02: a landed body goes back kinematic, velocities zeroed; the pool resets the transform on the next acquire.
	StopPhysics();

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
		// task-195-01: mid-air magnet once PopAttractDelay has elapsed since spawn (PopElapsed is the life time in Pop). A latched
		// pickup drops the rest of its pop and flies straight to the miner; a refused heal keeps popping, then rests.
		if (bAttractDuringPop && PopElapsed >= PopAttractDelay && TryLatchMagnet(Miner, PickupRadiusSquared, DeltaSeconds))
		{
			return;
		}
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
		// task-229-02 fall guard: a body that fell through a hole or the floor goes back to its rest point, kinematic.
		if (PhysicsFallLimit > 0.f && GetActorLocation().Z < PopEnd.Z - PhysicsFallLimit && IsSimulatingPhysics())
		{
			StopPhysics();
			SetActorLocation(PopEnd, false, nullptr, ETeleportType::TeleportPhysics);
		}
		// Rest on the floor for AttractDelay after the pop so the drop reads before it flies away; with bAttractDuringPop the
		// wait also ends once PopAttractDelay has elapsed since spawn (task-195-01).
		if (RestElapsed < AttractDelay && !(bAttractDuringPop && GetLifeElapsed() >= PopAttractDelay))
		{
			RestElapsed += DeltaSeconds;
			return;
		}
		TryLatchMagnet(Miner, PickupRadiusSquared, DeltaSeconds);
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

bool ARRPickup::TryLatchMagnet(ARRMiner* Miner, float PickupRadiusSquared, float DeltaSeconds)
{
	if (!Miner || FVector::DistSquared2D(GetActorLocation(), Miner->GetActorLocation()) > PickupRadiusSquared)
	{
		return false;
	}
	// heal-ore: a heal pickup is left alone while the miner is at full health; re-checked every frame, so it latches on the
	// first frame the miner is both damaged and within radius (no event binding, nothing to reset).
	if (IsRefusedByMiner(Miner))
	{
		return false;
	}
	// task-229-02: the kinematic fly-in takes over from physics.
	StopPhysics();
	TargetMiner = Miner;
	Phase = ERRPickupPhase::Magnet;
	SetGlowFadeTarget(0.f);
	MagnetSpeed = MagnetStartSpeed;
	OnMagnetStart();
	FlyToMiner(Miner, DeltaSeconds);
	return true;
}

bool ARRPickup::UpdatePop(float DeltaSeconds)
{
	PopElapsed += DeltaSeconds;

	// task-229-02: with physics the pop ends at the touch-down of the main arc (the kinematic bounce is left to physics): the
	// body starts from the arc's velocity there (finite difference over one 60 Hz frame), scaled per axis, plus the spin.
	const float ArcEndTime = (1.f - FMath::Clamp(PopBounceFraction, 0.f, 0.9f)) * FMath::Max(PopDuration, 0.f);
	if (PopElapsed >= ArcEndTime && CanSimulateOnLanding())
	{
		const FVector Landing = EvaluatePop(ArcEndTime);
		const float Step = FMath::Min(RRPickupPhysics::LandingVelocityStep, ArcEndTime);
		FVector Velocity = Step > 0.f ? (Landing - EvaluatePop(ArcEndTime - Step)) / Step : FVector::ZeroVector;
		Velocity.X *= LandingHorizontalSpeedScale;
		Velocity.Y *= LandingHorizontalSpeedScale;
		Velocity.Z *= LandingVerticalSpeedScale;
		ApplyPopStep(Landing, DeltaSeconds);
		StartPhysics(Velocity, /*bWithSpin*/ true);
		return true;
	}

	ApplyPopStep(EvaluatePop(PopElapsed), DeltaSeconds);
	return PopDuration <= 0.f || PopElapsed >= PopDuration;
}

FVector ARRPickup::EvaluatePop(float Elapsed) const
{
	const float Alpha = PopDuration > 0.f ? FMath::Clamp(Elapsed / PopDuration, 0.f, 1.f) : 1.f;

	const float BounceFraction = FMath::Clamp(PopBounceFraction, 0.f, 0.9f);
	const float ArcEnd = 1.f - BounceFraction;

	// Vertical: main arc from the spawn Z to the rest Z, then a small bounce on the rest Z. The arc is the symmetric parabola
	// of parameter S in [0, 1]; task-195-01: PopFallTimeRatio remaps the arc time T to S piecewise linearly so that S = 0.5
	// (the apex) is reached at T = 1 / (1 + ratio): rise over that part, fall over the rest (ratio 1 = S equals T, unchanged).
	// The horizontal ease-out runs on the same remapped time, so the spatial path is the symmetric one and only the speed
	// along it changes (fast rise, slow floaty descent).
	float HorizontalAlpha = Alpha;
	float Z = PopEnd.Z;
	if (Alpha < ArcEnd)
	{
		const float T = Alpha / ArcEnd;
		float S = T;
		const float Ratio = FMath::Max(PopFallTimeRatio, 0.1f);
		if (Ratio != 1.f)
		{
			const float ApexTime = 1.f / (1.f + Ratio);
			S = T < ApexTime ? 0.5f * T / ApexTime : 0.5f + 0.5f * (T - ApexTime) / (1.f - ApexTime);
			HorizontalAlpha = S * ArcEnd;
		}
		Z = FMath::Lerp(PopStart.Z, PopEnd.Z, S) + PopArcHeight * 4.f * S * (1.f - S);
	}
	else
	{
		const float T = BounceFraction > 0.f ? (Alpha - ArcEnd) / BounceFraction : 1.f;
		Z = PopEnd.Z + PopBounceHeight * 4.f * T * (1.f - T);
	}

	// Horizontal: ease-out over the whole pop (fast ejection, slows through the bounce).
	const float Horizontal = 1.f - FMath::Square(1.f - HorizontalAlpha);
	FVector Location = PopStart + (PopEnd - PopStart) * FVector(Horizontal, Horizontal, 0.f);
	Location.Z = Z;
	return Location;
}

void ARRPickup::ApplyPopStep(const FVector& Location, float DeltaSeconds)
{
	// Teleport: the kinematic body follows without sweep nor kinematic target (it touches nothing in flight).
	if (PopSpinDegPerSec > 0.f)
	{
		const FQuat Spin(SpinAxis, FMath::DegreesToRadians(PopSpinDegPerSec) * DeltaSeconds);
		SetActorLocationAndRotation(Location, Spin * GetActorQuat(), false, nullptr, ETeleportType::TeleportPhysics);
		return;
	}
	SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
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
	// RestElapsed already passed AttractDelay (or the life time passed PopAttractDelay for a mid-air latch): attracted again on the first frame the miner is damaged and in radius.
	// task-229-02: with physics it falls where it is and settles; without, it snaps to its rest height.
	if (CanSimulateOnLanding())
	{
		StartPhysics(FVector::ZeroVector, /*bWithSpin*/ false);
		return;
	}
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
		SetActorLocation(Target, false, nullptr, ETeleportType::TeleportPhysics);
		Collect(Miner);
		return;
	}

	SetActorLocation(GetActorLocation() + ToTarget / Distance * Step, false, nullptr, ETeleportType::TeleportPhysics);
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
