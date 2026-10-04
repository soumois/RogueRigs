// Rogue Rigs - kamikaze robot: hovering charger that arms near the miner, explodes after a short fuse or when killed (C++ state machine Chase, Armed, Dead).

#include "Enemies/RRKamikaze.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Explosions/RRExplosionSubsystem.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRGameplayTags.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRKamikaze)

ARRKamikaze::ARRKamikaze(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Kamikaze");
	ArrivalRangeRatio = 0.5f;

	// 1.1 x the miner's capsule (34 x 88 -> 37.4 x 96.8, enemy-kamikaze size), on the ground: the flow field and avoidance are
	// unchanged. Contact with the miner is at 71.4 uu between centers, inside the arm range (CSV AttackRange 150) and the arrival distance (75). Editable in the Blueprint.
	GetCapsuleComponent()->InitCapsuleSize(37.4f, 96.8f);

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(GetCapsuleComponent());

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(VisualRoot);
	BodyMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BodyMesh->SetGenerateOverlapEvents(false);
	BodyMesh->SetCanEverAffectNavigation(false);

	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	CoreMesh->SetupAttachment(VisualRoot);
	CoreMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	CoreMesh->SetGenerateOverlapEvents(false);
	CoreMesh->SetCanEverAffectNavigation(false);
	CoreMesh->CastShadow = false;
}

void ARRKamikaze::BeginPlay()
{
	Super::BeginPlay();

	CoreBaseScale = CoreMesh->GetRelativeScale3D();
	VisualRootBaseLocation = VisualRoot->GetRelativeLocation();
	EnemyMovement->SetArrivalDistance(GetArmRange() * ArrivalRangeRatio);
	EnterState(ERRKamikazeState::Chase);
	UpdateHover();
	UpdateCore(0.f);
}

void ARRKamikaze::EnterState(ERRKamikazeState NewState)
{
	State = NewState;
	MarkStateStart();

	if (NewState == ERRKamikazeState::Dead)
	{
		// The base death stops the movement.
		return;
	}
	SetMovementActive(NewState == ERRKamikazeState::Chase);
}

void ARRKamikaze::UpdateBehavior(float DeltaSeconds)
{
	const double Now = GetWorld()->GetTimeSeconds();

	switch (State)
	{
	case ERRKamikazeState::Chase:
		if (IsTargetInArmRange(FindTarget()))
		{
			EnterState(ERRKamikazeState::Armed);
			ArmTime = Now;
		}
		break;

	case ERRKamikazeState::Armed:
		// Committed: the fuse runs to its end even if the miner steps away.
		if (HasStateLasted(GetFuseTime()))
		{
			Detonate();
			return;
		}
		break;

	case ERRKamikazeState::Dead:
		return;
	}

	UpdateHover();
	UpdateCore(DeltaSeconds);
}

void ARRKamikaze::Detonate()
{
	QueueOwnExplosion();
	bDetonated = true;

	// Dies by its own fuse: State.Dead directly (its explosion is then refused by its ASC and is never a kill), no XP.
	if (!AbilitySystemComponent->HasMatchingGameplayTag(RRGameplayTags::State_Dead))
	{
		AbilitySystemComponent->AddLooseGameplayTag(RRGameplayTags::State_Dead);
	}
	HandleDeath(this);
}

void ARRKamikaze::QueueOwnExplosion()
{
	if (bExplosionRequested)
	{
		return;
	}
	bExplosionRequested = true;
	ExplosionQueuedTime = GetWorld()->GetTimeSeconds();

	URRExplosionSubsystem* Explosions = GetWorld()->GetSubsystem<URRExplosionSubsystem>();
	if (!Explosions || !Explosions->QueueExplosion(GetActorLocation(), ExplosionRowName, this, this))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: explosion '%s' could not be queued."), *GetName(), *ExplosionRowName.ToString());
	}
}

void ARRKamikaze::HandleDeath(AActor* DeathInstigator)
{
	if (bDying)
	{
		return;
	}
	EnterState(ERRKamikazeState::Dead);
	// Killed (shots, another explosion): same explosion as the fuse; no-op after the fuse.
	QueueOwnExplosion();
	Super::HandleDeath(DeathInstigator);
}

void ARRKamikaze::DropXP()
{
	if (bDetonated)
	{
		return;
	}
	Super::DropXP();
}

void ARRKamikaze::UpdateHover()
{
	const double Time = GetWorld()->GetTimeSeconds();
	const float Bob = HoverBobAmplitude * FMath::Sin(UE_TWO_PI * HoverBobRate * Time);
	const float Z = -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + HoverHeight + Bob;
	VisualRoot->SetRelativeLocation(FVector(VisualRootBaseLocation.X, VisualRootBaseLocation.Y, Z));
}

void ARRKamikaze::UpdateCore(float DeltaSeconds)
{
	const bool bArmed = State == ERRKamikazeState::Armed;
	CorePulsePhase = FMath::Frac(CorePulsePhase + DeltaSeconds * (bArmed ? ArmedPulseRate : CorePulseRate));

	float Growth = 1.f;
	if (bArmed)
	{
		const float FuseTime = GetFuseTime();
		const double Alpha = FuseTime > 0.f ? FMath::Clamp(GetTimeInState() / FuseTime, 0.0, 1.0) : 1.0;
		Growth = FMath::Lerp(1.f, ArmedCoreScale, static_cast<float>(Alpha));
	}
	CoreScaleFactor = Growth * (1.f + CorePulseAmplitude * FMath::Sin(UE_TWO_PI * CorePulsePhase));
	CoreMesh->SetRelativeScale3D(CoreBaseScale * CoreScaleFactor);
}
