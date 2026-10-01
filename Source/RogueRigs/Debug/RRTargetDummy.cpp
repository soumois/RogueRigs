// Rogue Rigs - target dummy: immobile, never-dying test target with an ASC and health, hit flash and debug damage numbers.

#include "Debug/RRTargetDummy.h"

#include "Components/StaticMeshComponent.h"
#include "Debug/RRFloatingDamageText.h"
#include "Engine/World.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "GAS/RRHealthAttributeSet.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Projectiles/RRCollisionChannels.h"
#include "RogueRigs.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRTargetDummy)

ARRTargetDummy::ARRTargetDummy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	// Pawn object type: the projectile sphere overlaps Pawn. Explicit Overlap on Projectile so the pair never blocks.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_Pawn);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(RRCollisionChannels::Projectile, ECR_Overlap);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetSimulatePhysics(false);
	Mesh->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(Mesh);

	// Attribute sets created as default subobjects of the owner are registered by UAbilitySystemComponent::InitializeComponent.
	AbilitySystemComponent = CreateDefaultSubobject<URRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	HealthSet = CreateDefaultSubobject<URRHealthAttributeSet>(TEXT("HealthSet"));

	FloatingTextClass = ARRFloatingDamageText::StaticClass();
}

UAbilitySystemComponent* ARRTargetDummy::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARRTargetDummy::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	// MaxHealth first: Health is clamped against the current MaxHealth (sys-gas-core).
	AbilitySystemComponent->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), MaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), MaxHealth);

	DamageReceivedHandle = HealthSet->OnDamageReceived.AddUObject(this, &ARRTargetDummy::HandleDamageReceived);

	FlashMaterial = Mesh->CreateDynamicMaterialInstance(0);
	if (FlashMaterial)
	{
		if (!FlashMaterial->GetVectorParameterValue(FHashedMaterialParameterInfo(ColorParameterName), BaseColor))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("%s: material has no vector parameter '%s'; the hit flash will not show."), *GetName(), *ColorParameterName.ToString());
		}
	}
	else
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: mesh has no material in slot 0; the hit flash is disabled."), *GetName());
	}
}

void ARRTargetDummy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HealthSet)
	{
		HealthSet->OnDamageReceived.Remove(DamageReceivedHandle);
	}
	DamageReceivedHandle.Reset();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FlashTimerHandle);
		World->GetTimerManager().ClearTimer(RefillTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ARRTargetDummy::HandleDamageReceived(float Damage, bool bIsCritical, float HealthLeft)
{
	UE_LOG(LogRogueRigs, Log, TEXT("%s: hit for %.1f%s, health left %.1f / %.1f"),
		*GetName(), Damage, bIsCritical ? TEXT(" (critical)") : TEXT(""), HealthLeft, HealthSet->GetMaxHealth());

	StartFlash();
	SpawnFloatingText(Damage, bIsCritical);

	FTimerManager& TimerManager = GetWorldTimerManager();
	if (HealthLeft <= 0.f || RefillDelay <= 0.f)
	{
		TimerManager.ClearTimer(RefillTimerHandle);
		RefillHealth();
	}
	else
	{
		// Restarting the timer on every hit makes the delay count from the last hit.
		TimerManager.SetTimer(RefillTimerHandle, this, &ARRTargetDummy::RefillHealth, RefillDelay, false);
	}
}

void ARRTargetDummy::StartFlash()
{
	if (!FlashMaterial)
	{
		return;
	}
	FlashMaterial->SetVectorParameterValue(ColorParameterName, FlashColor);
	GetWorldTimerManager().SetTimer(FlashTimerHandle, this, &ARRTargetDummy::EndFlash, FMath::Max(FlashDuration, 0.01f), false);
}

void ARRTargetDummy::EndFlash()
{
	if (FlashMaterial)
	{
		FlashMaterial->SetVectorParameterValue(ColorParameterName, BaseColor);
	}
}

void ARRTargetDummy::SpawnFloatingText(float Damage, bool bIsCritical)
{
	UWorld* World = GetWorld();
	if (!World || !FloatingTextClass)
	{
		return;
	}

	const FVector Jitter(FMath::FRandRange(-TextSpawnJitter, TextSpawnJitter), FMath::FRandRange(-TextSpawnJitter, TextSpawnJitter), 0.f);
	const FTransform SpawnTransform(GetActorLocation() + TextSpawnOffset + Jitter);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ARRFloatingDamageText* Text = World->SpawnActor<ARRFloatingDamageText>(FloatingTextClass, SpawnTransform, SpawnParams))
	{
		Text->InitDamageText(Damage, bIsCritical ? CriticalTextColor : NormalTextColor, bIsCritical ? CriticalTextSize : NormalTextSize);
	}
}

void ARRTargetDummy::RefillHealth()
{
	const float CurrentMaxHealth = AbilitySystemComponent->GetNumericAttribute(URRHealthAttributeSet::GetMaxHealthAttribute());
	AbilitySystemComponent->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), CurrentMaxHealth);
	UE_LOG(LogRogueRigs, Log, TEXT("%s: health refilled to %.1f"), *GetName(), CurrentMaxHealth);
}
