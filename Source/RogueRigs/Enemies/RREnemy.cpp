// Rogue Rigs - robot base: own ASC with health, movement and enemy sets, CSV stats, flow-field movement, single death with shrink and XP drop, floating damage numbers.

#include "Enemies/RREnemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "Debug/RRFloatingDamageText.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FlowField/RREnemyMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RREnemyAttributeSet.h"
#include "GAS/RRGameplayTags.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Projectiles/RRCollisionChannels.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RREnemy)

const TCHAR* ARREnemy::EnemyStatsPath = TEXT("RogueRigs/Data/Tables/EnemyStats.csv");

ARREnemy::ARREnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

	// Shots (Projectile channel) overlap the capsule: the projectile applies its spec to the ASC, never blocks (sys-projectiles).
	GetCapsuleComponent()->SetCollisionResponseToChannel(RRCollisionChannels::Projectile, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	FloatingTextClass = ARRFloatingDamageText::StaticClass();
}

UAbilitySystemComponent* ARREnemy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ARREnemy::LoadStatsRow(FName RowName, FRREnemyStatsRow& OutRow)
{
	const UDataTable* Table = FRRBaseStatsLoader::LoadTableFromCSV(EnemyStatsPath, FRREnemyStatsRow::StaticStruct());
	if (!Table)
	{
		return false;
	}
	const FRREnemyStatsRow* Row = Table->FindRow<FRREnemyStatsRow>(RowName, TEXT("ARREnemy::LoadStatsRow"), false);
	if (!Row)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("ARREnemy: row '%s' not found in %s"), *RowName.ToString(), EnemyStatsPath);
		return false;
	}
	OutRow = *Row;
	return true;
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
	CachedTarget.Reset();
	for (TActorIterator<ARRMiner> It(GetWorld()); It; ++It)
	{
		if (IsValid(*It) && !It->IsDead())
		{
			CachedTarget = *It;
			return *It;
		}
	}
	return nullptr;
}

void ARREnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDying)
	{
		UpdateDeath(DeltaSeconds);
		return;
	}
	UpdateBehavior(DeltaSeconds);
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

	EnemyMovement->SetMovementEnabled(false);
	ConsumeMovementInputVector();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	SetMeshesVisible(true);

	UE_LOG(LogRogueRigs, Log, TEXT("%s died (instigator: %s)."), *GetName(), *GetNameSafe(DeathInstigator));
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
