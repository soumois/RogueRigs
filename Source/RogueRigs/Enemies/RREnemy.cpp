// Rogue Rigs - robot base: own ASC with health, movement and enemy sets, CSV stats, flow-field movement, single death with shrink and XP drop, floating damage numbers.

#include "Enemies/RREnemy.h"

#include "AbilitySystemComponent.h"
#include "AI/Navigation/AvoidanceManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Debug/RRFloatingDamageText.h"
#include "Enemies/RRRobotMovementComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RREnemyAttributeSet.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Projectiles/RRCollisionChannels.h"
#include "RogueRigs.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RREnemy)

const TCHAR* ARREnemy::EnemyStatsPath = TEXT("RogueRigs/Data/Tables/EnemyStats.csv");

ARREnemy::ARREnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<URRRobotMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// Robots run without a controller: CharacterMovement simulates on its own (see BeginPlay).
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Attribute sets created as default subobjects are registered by UAbilitySystemComponent::InitializeComponent.
	AbilitySystemComponent = CreateDefaultSubobject<URRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HealthSet = CreateDefaultSubobject<URRHealthAttributeSet>(TEXT("HealthSet"));
	MovementSet = CreateDefaultSubobject<URRMovementAttributeSet>(TEXT("MovementSet"));
	EnemySet = CreateDefaultSubobject<URREnemyAttributeSet>(TEXT("EnemySet"));
	EnemyMovement = CreateDefaultSubobject<URREnemyMovementComponent>(TEXT("EnemyMovement"));

	// Movement defaults (editable on the component). MaxWalkSpeed is driven by the MoveSpeed attribute.
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 720.f, 0.f);
	Movement->bRunPhysicsWithNoController = true;
	Movement->bCanWalkOffLedges = false;
	Movement->bUseRVOAvoidance = true;
	// RVO is the only robot-robot separation (bIgnoreOtherRobots): consider neighbors within 250 uu (engine default 500).
	Movement->AvoidanceConsiderationRadius = 250.f;

	// Shots (Projectile channel) overlap the capsule: the projectile applies its spec to the ASC, never blocks (sys-projectiles).
	GetCapsuleComponent()->SetCollisionResponseToChannel(RRCollisionChannels::Projectile, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	FloatingTextClass = ARRFloatingDamageText::StaticClass();
}

UAbilitySystemComponent* ARREnemy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARREnemy::SetIgnoreOtherRobots(bool bIgnore)
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	const FMaskFilter Mask = bIgnore ? RRCollisionChannels::RobotMaskFilter : 0;
	Capsule->SetMaskFilterOnBodyInstance(Mask);
	Capsule->SetMoveIgnoreMask(Mask);
}

bool ARREnemy::IsIgnoringOtherRobots() const
{
	return (GetCapsuleComponent()->GetMoveIgnoreMask() & RRCollisionChannels::RobotMaskFilter) != 0;
}

bool ARREnemy::LoadStatsRow(FName RowName, FRREnemyStatsRow& OutRow)
{
	return FRRBaseStatsLoader::FindRow(EnemyStatsPath, RowName, OutRow, TEXT("ARREnemy::LoadStatsRow"));
}

void ARREnemy::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	MoveSpeedChangedHandle = AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(URRMovementAttributeSet::GetMoveSpeedAttribute())
		.AddUObject(this, &ARREnemy::HandleMoveSpeedChanged);

	if (LoadStatsRow(StatsRowName, Stats))
	{
		ApplyStats();
	}
	else
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: stats row '%s' could not be read."), *GetName(), *StatsRowName.ToString());
	}
	GetCharacterMovement()->MaxWalkSpeed = MovementSet->GetMoveSpeed();

	// Crowd: robots pass through each other, RVO separates them; the avoidance manager is world-wide (robots are its only users).
	SetIgnoreOtherRobots(bIgnoreOtherRobots);
	if (UAvoidanceManager* AvoidanceManager = GetWorld()->GetAvoidanceManager())
	{
		if (AvoidanceRadiusExpansion > 0.f)
		{
			AvoidanceManager->ArtificialRadiusExpansion = AvoidanceRadiusExpansion;
		}
		if (AvoidanceLockTimeAfterAvoid >= 0.f)
		{
			AvoidanceManager->LockTimeAfterAvoid = AvoidanceLockTimeAfterAvoid;
		}
	}

	if (bLightAttachedParts)
	{
		ApplyLightAttachedParts();
	}
	MovementLODTimer = FMath::FRand() * MovementLODCheckInterval;

	// An unpossessed character stays in MOVE_None: start walking (falling until it lands).
	GetCharacterMovement()->SetDefaultMovementMode();

	DeathHandle = HealthSet->OnDeath.AddUObject(this, &ARREnemy::HandleDeath);
	DamageReceivedHandle = HealthSet->OnDamageReceived.AddUObject(this, &ARREnemy::HandleDamageReceived);
}

void ARREnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MoveSpeedChangedHandle.IsValid())
	{
		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(URRMovementAttributeSet::GetMoveSpeedAttribute())
			.Remove(MoveSpeedChangedHandle);
		MoveSpeedChangedHandle.Reset();
	}
	if (HealthSet)
	{
		HealthSet->OnDeath.Remove(DeathHandle);
		HealthSet->OnDamageReceived.Remove(DamageReceivedHandle);
	}
	DeathHandle.Reset();
	DamageReceivedHandle.Reset();

	Super::EndPlay(EndPlayReason);
}

void ARREnemy::ApplyStats()
{
	// MaxHealth first: Health is clamped against the current MaxHealth.
	AbilitySystemComponent->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), Stats.MaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), Stats.MaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(URRMovementAttributeSet::GetMoveSpeedAttribute(), Stats.MoveSpeed);
}

void ARREnemy::HandleMoveSpeedChanged(const FOnAttributeChangeData& ChangeData)
{
	GetCharacterMovement()->MaxWalkSpeed = ChangeData.NewValue;
}

bool ARREnemy::IsDead() const
{
	return AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(RRGameplayTags::State_Dead);
}

float ARREnemy::GetKnockbackResistance() const
{
	return FMath::Clamp(Stats.KnockbackResistance, 0.f, 1.f);
}

void ARREnemy::ApplyKnockback(FVector Velocity)
{
	if (IsDead())
	{
		return;
	}
	const FVector Launch = FVector(Velocity.X, Velocity.Y, 0.f) * (1.f - GetKnockbackResistance());
	if (Launch.IsNearlyZero())
	{
		return;
	}
	// Applied by CharacterMovement on its next update (falling for a frame, then ground friction brakes it).
	LaunchCharacter(Launch, /*bXYOverride*/ true, /*bZOverride*/ false);
}

ARRMiner* ARREnemy::FindTarget()
{
	ARRMiner* Target = CachedTarget.Get();
	if (Target && !Target->IsDead())
	{
		return Target;
	}
	Target = RRWorld::FindLivingMiner(GetWorld());
	CachedTarget = Target;
	return Target;
}

void ARREnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDying)
	{
		UpdateDeath(DeltaSeconds);
		return;
	}
	UpdateMovementLOD(DeltaSeconds);
	UpdateBehavior(DeltaSeconds);
}

void ARREnemy::ApplyLightAttachedParts()
{
	// The inherited character mesh is empty on the robots (parts are static meshes): no tick, no collision, no render, and fully
	// absolute so USceneComponent::UpdateChildTransforms skips it on every move.
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (CharacterMesh && !CharacterMesh->GetSkinnedAsset() && CharacterMesh->GetAttachChildren().Num() == 0)
	{
		CharacterMesh->SetComponentTickEnabled(false);
		CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CharacterMesh->SetGenerateOverlapEvents(false);
		CharacterMesh->SetVisibility(false);
		CharacterMesh->SetUsingAbsoluteLocation(true);
		CharacterMesh->SetUsingAbsoluteRotation(true);
		CharacterMesh->SetUsingAbsoluteScale(true);
	}

	// Visual parts without collision never need overlap updates when the capsule moves.
	TInlineComponentArray<UPrimitiveComponent*> Primitives(this);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive != GetCapsuleComponent() && Primitive->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
		{
			Primitive->SetGenerateOverlapEvents(false);
		}
	}
}

void ARREnemy::UpdateMovementLOD(float DeltaSeconds)
{
	MovementLODTimer -= DeltaSeconds;
	if (MovementLODTimer > 0.f)
	{
		return;
	}
	MovementLODTimer += MovementLODCheckInterval;
	if (MovementLODTimer <= 0.f)
	{
		MovementLODTimer = MovementLODCheckInterval;
	}

	bool bFar = false;
	if (FarMovementDistance > 0.f && GetWorld()->GetGameViewport() && GetCharacterMovement()->IsMovingOnGround())
	{
		const ARRMiner* Target = FindTarget();
		bFar = Target
			&& FVector::DistSquared2D(Target->GetActorLocation(), GetActorLocation()) > FMath::Square(FarMovementDistance)
			&& !WasRecentlyRendered(OffScreenGraceSeconds);
	}
	SetFarMovement(bFar);
}

void ARREnemy::SetFarMovement(bool bFar)
{
	if (bFar == bFarMovementActive)
	{
		return;
	}
	bFarMovementActive = bFar;
	const float Interval = bFar ? FarMovementTickInterval : 0.f;
	GetCharacterMovement()->SetComponentTickInterval(Interval);
	EnemyMovement->SetComponentTickInterval(Interval);
}

void ARREnemy::HandleDeath(AActor* DeathInstigator)
{
	if (bDying)
	{
		return;
	}
	bDying = true;
	DeathElapsed = 0.f;
	ScaleAtDeath = GetActorScale3D();

	// Feet = bottom of the capsule; the pickups add their own hover height.
	DropLocation = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	SetMovementActive(false);
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	SetMeshesVisible(true);

	UE_LOG(LogRogueRigs, Log, TEXT("%s died (instigator: %s)."), *GetName(), *GetNameSafe(DeathInstigator));

	// Kills feed the alert bar (a kamikaze's own fuse does not count: the run flow ignores self-instigated deaths).
	if (URRRunFlowSubsystem* RunFlow = GetWorld() ? GetWorld()->GetSubsystem<URRRunFlowSubsystem>() : nullptr)
	{
		RunFlow->NotifyEnemyKilled(this, DeathInstigator);
	}
}

void ARREnemy::UpdateDeath(float DeltaSeconds)
{
	if (bDeathFinished)
	{
		return;
	}
	DeathElapsed += DeltaSeconds;
	if (DeathElapsed >= DeathShrinkDuration)
	{
		bDeathFinished = true;
		DropXP();
		Destroy();
		return;
	}
	// Never scale to exactly 0 (degenerate transforms); the robot is destroyed at the end of the shrink.
	const float Alpha = FMath::Max(1.f - DeathElapsed / DeathShrinkDuration, UE_KINDA_SMALL_NUMBER);
	SetActorScale3D(ScaleAtDeath * Alpha);
}

void ARREnemy::DropXP()
{
	if (Stats.XPValue <= 0)
	{
		return;
	}
	if (!XPPickupClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: XPPickupClass is not set; %d XP not dropped."), *GetName(), Stats.XPValue);
		return;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int32 Index = 0; Index < Stats.XPValue; ++Index)
	{
		if (!GetWorld()->SpawnActor<ARRPickup>(XPPickupClass, DropLocation, FRotator::ZeroRotator, SpawnParameters))
		{
			UE_LOG(LogRogueRigs, Error, TEXT("%s: XP pickup of class %s could not be spawned."), *GetName(), *XPPickupClass->GetName());
		}
	}
}

void ARREnemy::HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft)
{
	if (bShowDamageNumbers)
	{
		SpawnDamageText(Damage, bIsCritical);
	}
}

void ARREnemy::SpawnDamageText(float Damage, bool bIsCritical)
{
	UWorld* World = GetWorld();
	if (!World || !FloatingTextClass)
	{
		return;
	}

	// Capsule top (scaled) + height, jittered horizontally only.
	const FVector Jitter(FMath::FRandRange(-TextSpawnJitter, TextSpawnJitter), FMath::FRandRange(-TextSpawnJitter, TextSpawnJitter), 0.f);
	const float CapsuleTop = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FTransform SpawnTransform(GetActorLocation() + FVector(0.f, 0.f, CapsuleTop + TextSpawnHeight) + Jitter);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ARRFloatingDamageText* Text = World->SpawnActor<ARRFloatingDamageText>(FloatingTextClass, SpawnTransform, SpawnParams))
	{
		Text->InitDamageText(Damage, bIsCritical ? CriticalTextColor : NormalTextColor, bIsCritical ? CriticalTextSize : NormalTextSize);
	}
}

void ARREnemy::SetMeshesVisible(bool bVisible)
{
	TInlineComponentArray<UMeshComponent*> Meshes(this);
	for (UMeshComponent* MeshComponent : Meshes)
	{
		MeshComponent->SetVisibility(bVisible);
	}
}

void ARREnemy::SetMovementActive(bool bActive)
{
	EnemyMovement->SetMovementEnabled(bActive);
	if (!bActive)
	{
		ConsumeMovementInputVector();
		GetCharacterMovement()->StopMovementImmediately();
	}
}

bool ARREnemy::IsTargetWithin(const ARRMiner* Target, float Range) const
{
	return Target && !Target->IsDead()
		&& FVector::DistSquared2D(GetActorLocation(), Target->GetActorLocation()) <= FMath::Square(Range);
}

void ARREnemy::MarkStateStart()
{
	StateStartTime = GetWorld()->GetTimeSeconds();
}

double ARREnemy::GetTimeInState() const
{
	return GetWorld()->GetTimeSeconds() - StateStartTime;
}

bool ARREnemy::HasStateLasted(double Duration) const
{
	return GetTimeInState() + TimeTolerance >= Duration;
}

bool ARREnemy::ApplyDamageSpecTo(UAbilitySystemComponent* TargetASC, TSubclassOf<UGameplayEffect> EffectClass, float Damage)
{
	if (!EffectClass || !TargetASC)
	{
		return false;
	}
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, AbilitySystemComponent->MakeEffectContext());
	if (!Spec.IsValid())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: damage spec of class %s could not be made."), *GetName(), *EffectClass->GetName());
		return false;
	}
	Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Damage);
	// The target's i-frames or death refuse the spec (target tag requirements of the damage effect).
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, TargetASC);
	return true;
}

void ARREnemy::UpdateTelegraphBlink(float Rate)
{
	if (Rate <= 0.f)
	{
		return;
	}
	const double Phase = FMath::Frac(GetTimeInState() * Rate);
	SetMeshesVisible(Phase >= 0.5);
}
