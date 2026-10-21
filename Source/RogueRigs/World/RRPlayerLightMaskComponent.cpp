// Rogue Rigs - player light mask writer (task-272-07, task-272-13): pushes the miner position into MPC_PlayerLight each tick; the tunables only for console sweeps.

#include "World/RRPlayerLightMaskComponent.h"

#include "RogueRigs.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "UObject/ConstructorHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRPlayerLightMaskComponent)

namespace RRPlayerLightMask
{
	// Parameter names of MPC_PlayerLight (mpp-player-light-mask).
	static const FName PlayerPosition(TEXT("PlayerPosition"));
	static const FName Enable(TEXT("Enable"));
	static const FName CenterBrightness(TEXT("CenterBrightness"));
	static const FName FarBrightness(TEXT("FarBrightness"));
	static const FName Radius(TEXT("Radius"));
	static const FName Exponent(TEXT("Exponent"));
	static const FName TintStrength(TEXT("TintStrength"));
	static const FName WarmTint(TEXT("WarmTint"));
	static const FName FarTint(TEXT("FarTint"));
}

URRPlayerLightMaskComponent::URRPlayerLightMaskComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	// After movement and camera updates: the position written matches the frame being rendered.
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> CollectionFinder(
		TEXT("/Game/RogueRigs/Materials/PostProcess/MPC_PlayerLight.MPC_PlayerLight"));
	if (CollectionFinder.Succeeded())
	{
		ParameterCollection = CollectionFinder.Object;
	}
}

void URRPlayerLightMaskComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsNetMode(NM_DedicatedServer))
	{
		SetComponentTickEnabled(false);
		return;
	}
	if (!ParameterCollection)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: no ParameterCollection (MPC_PlayerLight), player light mask disabled."), *GetPathName());
		SetComponentTickEnabled(false);
		return;
	}
	PushParameters();
}

void URRPlayerLightMaskComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PushParameters();
}

UMaterialParameterCollectionInstance* URRPlayerLightMaskComponent::ResolveCollectionInstance()
{
	UWorld* World = GetWorld();
	if (!World || !ParameterCollection)
	{
		return nullptr;
	}
	if (CachedWorld.Get() != World || !CachedInstance.IsValid())
	{
		CachedWorld = World;
		CachedInstance = World->GetParameterCollectionInstance(ParameterCollection);
	}
	return CachedInstance.Get();
}

void URRPlayerLightMaskComponent::PushParameters()
{
	UMaterialParameterCollectionInstance* Instance = ResolveCollectionInstance();
	const AActor* Owner = GetOwner();
	if (!Instance || !Owner)
	{
		return;
	}

	// The engine skips the render update for unchanged values and ignores names missing from the collection.
	Instance->SetVectorParameterValue(RRPlayerLightMask::PlayerPosition, FLinearColor(FVector(Owner->GetActorLocation())));

	// MPC_PlayerLight is the owner's tuning place: the tunables are pushed only for console sweeps.
	if (!bDriveTunables)
	{
		return;
	}
	Instance->SetScalarParameterValue(RRPlayerLightMask::Enable, Enable);
	Instance->SetScalarParameterValue(RRPlayerLightMask::CenterBrightness, CenterBrightness);
	Instance->SetScalarParameterValue(RRPlayerLightMask::FarBrightness, FarBrightness);
	Instance->SetScalarParameterValue(RRPlayerLightMask::Radius, Radius);
	Instance->SetScalarParameterValue(RRPlayerLightMask::Exponent, Exponent);
	Instance->SetScalarParameterValue(RRPlayerLightMask::TintStrength, TintStrength);
	Instance->SetVectorParameterValue(RRPlayerLightMask::WarmTint, WarmTint);
	Instance->SetVectorParameterValue(RRPlayerLightMask::FarTint, FarTint);
}
