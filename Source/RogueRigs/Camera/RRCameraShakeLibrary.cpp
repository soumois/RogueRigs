// Copyright Rogue Rigs. Static calls to play a camera shake on the local player's camera (task-310-01, located call task-416-01).

#include "Camera/RRCameraShakeLibrary.h"
#include "Camera/RRCameraShake.h"
#include "Camera/RRCameraShakeModifier.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UCameraShakeBase* URRCameraShakeLibrary::PlayCameraShake(const UObject* WorldContextObject, float Scale, TSubclassOf<UCameraShakeBase> ShakeClass)
{
	if (Scale <= 0.f)
	{
		return nullptr;
	}

	return StartShake(GetLocalCameraManager(WorldContextObject), Scale, ShakeClass);
}

UCameraShakeBase* URRCameraShakeLibrary::PlayCameraShakeAtLocation(const UObject* WorldContextObject, FVector Epicenter, float MaxDistance, float Scale, TSubclassOf<UCameraShakeBase> ShakeClass)
{
	if (Scale <= 0.f || MaxDistance <= 0.f)
	{
		return nullptr;
	}

	APlayerCameraManager* CameraManager = GetLocalCameraManager(WorldContextObject);
	if (!CameraManager)
	{
		return nullptr;
	}

	// Distance from the miner: the camera view target, or the controlled pawn as a fallback.
	const AActor* Listener = CameraManager->GetViewTarget();
	if (!Listener && CameraManager->GetOwningPlayerController())
	{
		Listener = CameraManager->GetOwningPlayerController()->GetPawn();
	}
	if (!Listener)
	{
		return nullptr;
	}

	const float FalloffScale = GetDistanceFalloffScale(FVector::Dist2D(Epicenter, Listener->GetActorLocation()), MaxDistance);
	if (FalloffScale <= 0.f)
	{
		return nullptr;
	}

	return StartShake(CameraManager, Scale * FalloffScale, ShakeClass);
}

float URRCameraShakeLibrary::GetDistanceFalloffScale(float Distance2D, float MaxDistance)
{
	if (MaxDistance <= 0.f || Distance2D > MaxDistance)
	{
		return 0.f;
	}

	const float EdgeScale = GetDefault<URRCameraShakeModifier>()->GetEdgeFalloffScale();
	return FMath::Lerp(1.f, EdgeScale, FMath::Clamp(Distance2D / MaxDistance, 0.f, 1.f));
}

APlayerCameraManager* URRCameraShakeLibrary::GetLocalCameraManager(const UObject* WorldContextObject)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return nullptr;
	}

	return PlayerController->PlayerCameraManager;
}

URRCameraShakeModifier* URRCameraShakeLibrary::GetOrAddShakeModifier(APlayerCameraManager* CameraManager)
{
	if (!CameraManager)
	{
		return nullptr;
	}

	if (URRCameraShakeModifier* Existing = Cast<URRCameraShakeModifier>(CameraManager->FindCameraModifierByClass(URRCameraShakeModifier::StaticClass())))
	{
		return Existing;
	}

	return Cast<URRCameraShakeModifier>(CameraManager->AddNewCameraModifier(URRCameraShakeModifier::StaticClass()));
}

UCameraShakeBase* URRCameraShakeLibrary::StartShake(APlayerCameraManager* CameraManager, float Scale, TSubclassOf<UCameraShakeBase> ShakeClass)
{
	URRCameraShakeModifier* ShakeModifier = GetOrAddShakeModifier(CameraManager);
	if (!ShakeModifier)
	{
		return nullptr;
	}

	const TSubclassOf<UCameraShakeBase> ClassToPlay = ShakeClass ? ShakeClass : TSubclassOf<UCameraShakeBase>(URRCameraShake::StaticClass());
	return ShakeModifier->AddCameraShake(ClassToPlay, FAddCameraShakeParams(Scale, ECameraShakePlaySpace::CameraLocal));
}
