// Rogue Rigs - robot base: own ASC with health, movement and enemy sets, CSV stats, flow-field movement, single death with shrink and XP drop, damage numbers sent to the damage number subsystem.

#include "Enemies/RREnemy.h"

#include "AbilitySystemComponent.h"
#include "AI/Navigation/AvoidanceManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Damage/RRDamageNumberSubsystem.h"
#include "Enemies/RRRobotMovementComponent.h"
#include "Enemies/RRRobotSeparationSubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/LatentActionManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RREnemyAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Pool/RRActorPoolSubsystem.h"
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
	// RVO off (task-034-01): robots gave way to each other; the light separation (UpdateSeparation) keeps them from stacking.
	Movement->bUseRVOAvoidance = false;
	Movement->AvoidanceConsiderationRadius = 250.f;

	// Shots (Projectile channel) overlap the body-size hit capsule only: the projectile applies its spec to the ASC, never
	// blocks (sys-projectiles). The root capsule (terrain size) ignores them.
	GetCapsuleComponent()->SetCollisionResponseToChannel(RRCollisionChannels::Projectile, ECR_Ignore);
	HitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitCapsule"));
	HitCapsule->SetupAttachment(GetCapsuleComponent());
	HitCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCapsule->SetCollisionObjectType(ECC_Pawn);
	HitCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitCapsule->SetCollisionResponseToChannel(RRCollisionChannels::Projectile, ECR_Overlap);
	HitCapsule->SetGenerateOverlapEvents(true);
	HitCapsule->SetCanEverAffectNavigation(false);
	HitCapsule->CanCharacterStepUpOn = ECB_No;
	HitCapsule->SetHiddenInGame(true);

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

void ARREnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	BodyRadius = Capsule->GetScaledCapsuleRadius();
	HitCapsule->SetCapsuleSize(Capsule->GetUnscaledCapsuleRadius(), Capsule->GetUnscaledCapsuleHalfHeight());
	if (TerrainCapsuleRadius > 0.f && TerrainCapsuleRadius < Capsule->GetUnscaledCapsuleRadius())
	{
		Capsule->SetCapsuleRadius(TerrainCapsuleRadius);
	}
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
	// Light movement (rr.Enemy.LightMovement): the root capsule's overlaps have no listener (shots overlap HitCapsule; pickups,
	// explosions and weapons query), so its moves skip the overlap update.
	if (URRRobotMovementComponent::IsLightMovementEnabled())
	{
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	}
	MovementLODTimer = FMath::FRand() * MovementLODCheckInterval;

	// An unpossessed character stays in MOVE_None: start walking (falling until it lands).
	GetCharacterMovement()->SetDefaultMovementMode();

	DeathHandle = HealthSet->OnDeath.AddUObject(this, &ARREnemy::HandleDeath);
	DamageReceivedHandle = HealthSet->OnDamageReceived.AddUObject(this, &ARREnemy::HandleDamageReceived);

	if (URRRobotSeparationSubsystem* Separation = GetWorld()->GetSubsystem<URRRobotSeparationSubsystem>())
	{
		Separation->RegisterRobot(this);
	}
}

void ARREnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URRRobotSeparationSubsystem* Separation = GetWorld() ? GetWorld()->GetSubsystem<URRRobotSeparationSubsystem>() : nullptr)
	{
		Separation->UnregisterRobot(this);
	}
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
	AbilitySystemComponent->SetNumericAttributeBase(URRHealthAttributeSet::GetShieldAttribute(), Stats.Shield);
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
	FVector Launch = FVector(Velocity.X, Velocity.Y, 0.f) * (1.f - GetKnockbackResistance());
	if (Launch.IsNearlyZero())
	{
		return;
	}
	// LaunchCharacter overrides the pending launch: keep the XY of hits already received this frame (one push per pellet).
	if (const UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Launch += FVector(Movement->PendingLaunchVelocity.X, Movement->PendingLaunchVelocity.Y, 0.f);
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
	UpdateSeparation();
}

void ARREnemy::UpdateSeparation()
{
	if (!bUseSeparation || SeparationWeight <= 0.f || bDying || IsDead() || !EnemyMovement->IsMovementEnabled())
	{
		return;
	}
	URRRobotSeparationSubsystem* Separation = GetWorld()->GetSubsystem<URRRobotSeparationSubsystem>();
	const FVector Push = Separation ? Separation->GetSeparation(this) : FVector::ZeroVector;
	if (!Push.IsNearlyZero())
	{
		AddMovementInput(Push, SeparationWeight);
	}
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

bool ARREnemy::RelocateTo(const FVector& Location)
{
	if (bDying || IsDead())
	{
		return false;
	}
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	ConsumeMovementInputVector();
	Movement->StopMovementImmediately();
	Movement->PendingLaunchVelocity = FVector::ZeroVector;
	SetActorLocation(Location, /*bSweep*/ false, nullptr, ETeleportType::ResetPhysics);
	SetFarMovement(false);
	MovementLODTimer = 0.f;
	SetMeshesVisible(true);
	ResetStateMachine();
	return true;
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
	if (!bDeathFinished)
	{
		DeathElapsed += DeltaSeconds;
		if (DeathElapsed < DeathShrinkDuration)
		{
			// Never scale to exactly 0 (degenerate transforms); the robot leaves the game at the end of the shrink.
			const float Alpha = FMath::Max(1.f - DeathElapsed / DeathShrinkDuration, UE_KINDA_SMALL_NUMBER);
			SetActorScale3D(ScaleAtDeath * Alpha);
			return;
		}
		bDeathFinished = true;
		DropXP();
	}
	// Pooled robots go back to the pool (a kamikaze waits for its own blast to leave the queue); others are destroyed.
	URRActorPoolSubsystem* Pool = GetWorld() ? GetWorld()->GetSubsystem<URRActorPoolSubsystem>() : nullptr;
	if (Pool && Pool->IsPoolMember(this))
	{
		if (CanReleaseToPool())
		{
			Pool->Release(this);
		}
		return;
	}
	Destroy();
}

void ARREnemy::OnPoolAcquired_Implementation()
{
	++PoolLifeCount;
	if (!bVisualPartsCaptured)
	{
		// First delivery: BeginPlay set everything up; remember the look of the parts for the next lives.
		CaptureVisualParts();
		bVisualPartsCaptured = true;
	}
	if (bPoolReleasedOnce)
	{
		ResetForNewLife();
	}
}

void ARREnemy::OnPoolReleased_Implementation()
{
	bPoolReleasedOnce = true;
	if (URRRobotSeparationSubsystem* Separation = GetWorld() ? GetWorld()->GetSubsystem<URRRobotSeparationSubsystem>() : nullptr)
	{
		Separation->UnregisterRobot(this);
	}
	SetMovementActive(false);
	SetFarMovement(false);
	if (UWorld* World = GetWorld())
	{
		// Blueprint Delay nodes of this life (shield shell pop, slam ring) must not fire on the next one.
		World->GetLatentActionManager().RemoveActionsForObject(this);
	}
}

void ARREnemy::ResetForNewLife()
{
	// GAS: no effect of the previous life (status effects, slows), alive again, CSV stats and attributes back.
	AbilitySystemComponent->RemoveActiveEffects(FGameplayEffectQuery());
	AbilitySystemComponent->SetLooseGameplayTagCount(RRGameplayTags::State_Dead, 0);
	AbilitySystemComponent->SetNumericAttributeBase(URREnemyAttributeSet::GetAttackPowerAttribute(), GetDefault<URREnemyAttributeSet>()->GetAttackPower());
	AbilitySystemComponent->SetNumericAttributeBase(URRHealthAttributeSet::GetIncomingDamageAttribute(), 0.f);
	ApplyStats();
	GetCharacterMovement()->MaxWalkSpeed = MovementSet->GetMoveSpeed();

	// Death state.
	bDying = false;
	bDeathFinished = false;
	DeathElapsed = 0.f;
	ScaleAtDeath = FVector::OneVector;
	DropLocation = FVector::ZeroVector;

	// Movement: the pool already placed the robot (transform, scale 1, collision on); clear the motion of the last life.
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	ConsumeMovementInputVector();
	Movement->StopMovementImmediately();
	Movement->PendingLaunchVelocity = FVector::ZeroVector;
	if (URRRobotMovementComponent* RobotMovement = Cast<URRRobotMovementComponent>(Movement))
	{
		RobotMovement->ResetLightMovementState();
	}
	Movement->SetDefaultMovementMode();
	EnemyMovement->ResetMovementState();
	SetMovementActive(true);
	SetFarMovement(false);
	MovementLODTimer = FMath::FRand() * MovementLODCheckInterval;
	CachedTarget.Reset();
	MarkStateStart();

	RestoreVisualParts();

	if (URRRobotSeparationSubsystem* Separation = GetWorld()->GetSubsystem<URRRobotSeparationSubsystem>())
	{
		Separation->RegisterRobot(this);
	}
}

void ARREnemy::CaptureVisualParts()
{
	VisualParts.Reset();
	TInlineComponentArray<USceneComponent*> Components(this);
	for (USceneComponent* Component : Components)
	{
		// The root follows the actor transform; absolute parts (the hidden character mesh) are not part of the look.
		if (Component == GetRootComponent() || Component->IsUsingAbsoluteLocation())
		{
			continue;
		}
		FVisualPartState& Part = VisualParts.AddDefaulted_GetRef();
		Part.Component = Component;
		Part.RelativeTransform = Component->GetRelativeTransform();
		Part.bVisible = Component->IsVisible();
		Part.bHiddenInGame = Component->bHiddenInGame;
		if (const UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
		{
			for (int32 Slot = 0; Slot < Primitive->GetNumMaterials(); ++Slot)
			{
				Part.Materials.Add(Primitive->GetMaterial(Slot));
			}
		}
		if (const ULightComponent* Light = Cast<ULightComponent>(Component))
		{
			Part.LightIntensity = Light->Intensity;
		}
	}
}

void ARREnemy::RestoreVisualParts()
{
	for (const FVisualPartState& Part : VisualParts)
	{
		USceneComponent* Component = Part.Component.Get();
		if (!Component)
		{
			continue;
		}
		if (!Component->GetRelativeTransform().Equals(Part.RelativeTransform))
		{
			Component->SetRelativeTransform(Part.RelativeTransform);
		}
		if (Component->IsVisible() != Part.bVisible)
		{
			Component->SetVisibility(Part.bVisible);
		}
		if (Component->bHiddenInGame != Part.bHiddenInGame)
		{
			Component->SetHiddenInGame(Part.bHiddenInGame);
		}
		if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
		{
			for (int32 Slot = 0; Slot < Part.Materials.Num() && Slot < Primitive->GetNumMaterials(); ++Slot)
			{
				UMaterialInterface* Material = Part.Materials[Slot].Get();
				if (Material && Primitive->GetMaterial(Slot) != Material)
				{
					Primitive->SetMaterial(Slot, Material);
				}
			}
		}
		if (ULightComponent* Light = Cast<ULightComponent>(Component); Light && Part.LightIntensity >= 0.f && Light->Intensity != Part.LightIntensity)
		{
			Light->SetIntensity(Part.LightIntensity);
		}
	}
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
	// The robot XP is split evenly over its cubes (values add up to Stats.XPValue), ejected in evenly spread directions
	// from a random start angle, each jittered by up to a quarter of the spacing.
	const int32 CubeCount = XPCubeCount > 0 ? XPCubeCount : Stats.XPValue;
	const float CubeXP = static_cast<float>(Stats.XPValue) / static_cast<float>(CubeCount);
	const float Spacing = 360.f / static_cast<float>(CubeCount);
	const float StartYaw = FMath::FRandRange(0.f, 360.f);
	const FTransform SpawnTransform(FRotator::ZeroRotator, DropLocation);
	for (int32 Index = 0; Index < CubeCount; ++Index)
	{
		ARRPickup* Cube = GetWorld()->SpawnActorDeferred<ARRPickup>(XPPickupClass, SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Cube)
		{
			UE_LOG(LogRogueRigs, Error, TEXT("%s: XP pickup of class %s could not be spawned."), *GetName(), *XPPickupClass->GetName());
			continue;
		}
		Cube->SetXPAmount(CubeXP);
		Cube->SetPopYaw(StartYaw + Spacing * (static_cast<float>(Index) + FMath::FRandRange(-0.25f, 0.25f)));
		Cube->FinishSpawning(SpawnTransform);
	}
}

void ARREnemy::HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft)
{
	// Capsule top (scaled) + height; one number per hit, starting at a random point of this robot's spawn disc.
	if (URRDamageNumberSubsystem* Numbers = GetWorld()->GetSubsystem<URRDamageNumberSubsystem>())
	{
		const float CapsuleTop = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		Numbers->AddHit(this, Damage, bIsCritical, GetActorLocation() + FVector(0.f, 0.f, CapsuleTop + DamageNumberHeight), DamageNumberSpawnRadius);
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
	const FGameplayEffectSpecHandle Spec = MakeDamageSpec(EffectClass, Damage);
	if (!Spec.IsValid())
	{
		return false;
	}
	// The target's i-frames or death refuse the spec (target tag requirements of the damage effect).
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, TargetASC);
	return true;
}

FGameplayEffectSpecHandle ARREnemy::MakeDamageSpec(TSubclassOf<UGameplayEffect> EffectClass, float Damage) const
{
	if (!EffectClass)
	{
		return FGameplayEffectSpecHandle();
	}
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, AbilitySystemComponent->MakeEffectContext());
	if (!Spec.IsValid())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: damage spec of class %s could not be made."), *GetName(), *EffectClass->GetName());
		return Spec;
	}
	Spec.Data->SetSetByCallerMagnitude(RRGameplayTags::Data_Damage, Damage);
	return Spec;
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
