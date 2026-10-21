// Rogue Rigs - player light mask writer (task-272-07): pushes the miner position and the mask tunables into MPC_PlayerLight each tick.

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
	static const FName MaskEnabled(TEXT("MaskEnabled"));
	static const FName MaskRadius(TEXT("MaskRadius"));
	static const FName MaskExponent(TEXT("MaskExponent"));
	static const FName NearGain(TEXT("NearGain"));
	static const FName FarGain(TEXT("FarGain"));
	static const FName AddIntensity(TEXT("AddIntensity"));
	static const FName NearColour(TEXT("NearColour"));
	static const FName FarColour(TEXT("FarColour"));
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

	// The engine skips the render update for unchanged values.
	Instance->SetVectorParameterValue(RRPlayerLightMask::PlayerPosition, FLinearColor(FVector(Owner->GetActorLocation())));
	Instance->SetScalarParameterValue(RRPlayerLightMask::MaskEnabled, bMaskEnabled ? 1.f : 0.f);
	Instance->SetScalarParameterValue(RRPlayerLightMask::MaskRadius, MaskRadius);
	Instance->SetScalarParameterValue(RRPlayerLightMask::MaskExponent, MaskExponent);
	Instance->SetScalarParameterValue(RRPlayerLightMask::NearGain, NearGain);
	Instance->SetScalarParameterValue(RRPlayerLightMask::FarGain, FarGain);
	Instance->SetScalarParameterValue(RRPlayerLightMask::AddIntensity, AddIntensity);
	Instance->SetVectorParameterValue(RRPlayerLightMask::NearColour, NearColour);
	Instance->SetVectorParameterValue(RRPlayerLightMask::FarColour, FarColour);
}
