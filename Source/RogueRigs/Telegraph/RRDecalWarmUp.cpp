// Rogue Rigs - decal material warm-up (sys-ground-telegraph): draws each ground decal material once at level start, out of sight, so its first gameplay frame is not the engine default decal.

#include "Telegraph/RRDecalWarmUp.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Misc/App.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRDecalWarmUp)

ARRDecalWarmUp::ARRDecalWarmUp()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	// A pause during the warm-up must not keep the decals drawn.
	PrimaryActorTick.bTickEvenWhenPaused = true;
	SetCanBeDamaged(false);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ARRDecalWarmUp::WarmUp(UWorld* World, UMaterialInterface* Material)
{
	if (!World || !World->IsGameWorld() || !FApp::CanEverRender() || !Material)
	{
		return;
	}
	const UMaterial* BaseMaterial = Material->GetMaterial();
	if (!BaseMaterial || BaseMaterial->MaterialDomain != MD_DeferredDecal)
	{
		return;
	}
	ARRDecalWarmUp* WarmUpActor = nullptr;
	for (TActorIterator<ARRDecalWarmUp> It(World); It; ++It)
	{
		if (IsValid(*It))
		{
			WarmUpActor = *It;
			break;
		}
	}
	if (!WarmUpActor)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.ObjectFlags |= RF_Transient;
		WarmUpActor = World->SpawnActor<ARRDecalWarmUp>(ARRDecalWarmUp::StaticClass(), FTransform::Identity, Params);
	}
	if (WarmUpActor)
	{
		WarmUpActor->AddMaterial(Material);
	}
}

void ARRDecalWarmUp::AddMaterial(UMaterialInterface* Material)
{
	if (!Material || Materials.Contains(Material))
	{
		return;
	}
	Materials.Add(Material);

	// Hidden until placed in front of the camera: a decal drawn at an arbitrary spot could project the fallback on the floor.
	UDecalComponent* Decal = NewObject<UDecalComponent>(this, NAME_None, RF_Transient);
	Decal->SetCanEverAffectNavigation(false);
	Decal->FadeScreenSize = 0.f;
	Decal->DecalSize = FVector(BoxHalfSize);
	Decal->SetVisibleFlag(false);
	Decal->SetupAttachment(RootComponent);
	Decal->SetDecalMaterial(Material);
	Decal->RegisterComponent();
	Decals.Add(Decal);

	RemainingSeconds = WarmUpSeconds;
	SetActorTickEnabled(true);
}

bool ARRDecalWarmUp::PlaceInFrontOfCamera()
{
	const UWorld* World = GetWorld();
	const APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	const APlayerCameraManager* CameraManager = PlayerController ? PlayerController->PlayerCameraManager.Get() : nullptr;
	// Time stamp 0 = the camera has not been updated yet (its location is not the view's).
	if (!CameraManager || CameraManager->GetCameraCacheTime() <= 0.f)
	{
		return false;
	}
	const FVector Location = CameraManager->GetCameraLocation() + CameraManager->GetCameraRotation().Vector() * CameraDistance;
	SetActorLocation(Location);
	return true;
}

void ARRDecalWarmUp::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Decals.IsEmpty())
	{
		SetActorTickEnabled(false);
		return;
	}
	if (!PlaceInFrontOfCamera())
	{
		return;
	}
	for (UDecalComponent* Decal : Decals)
	{
		if (Decal && !Decal->IsVisible())
		{
			Decal->SetVisibility(true);
		}
	}
	RemainingSeconds -= DeltaSeconds;
	if (RemainingSeconds <= 0.f)
	{
		FinishWarmUp();
	}
}

void ARRDecalWarmUp::FinishWarmUp()
{
	for (UDecalComponent* Decal : Decals)
	{
		if (Decal)
		{
			Decal->DestroyComponent();
		}
	}
	Decals.Reset();
	SetActorTickEnabled(false);
}
