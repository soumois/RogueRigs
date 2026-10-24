// Copyright Rogue Rigs. One static call to play a camera shake on the local player's camera (task-310-01).

#include "Camera/RRCameraShakeLibrary.h"
#include "Camera/RRCameraShake.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UCameraShakeBase* URRCameraShakeLibrary::PlayCameraShake(const UObject* WorldContextObject, float Scale, TSubclassOf<UCameraShakeBase> ShakeClass)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World || Scale <= 0.f)
	{
		return nullptr;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->IsLocalController() || !PlayerController->PlayerCameraManager)
	{
		return nullptr;
	}

	const TSubclassOf<UCameraShakeBase> ClassToPlay = ShakeClass ? ShakeClass : TSubclassOf<UCameraShakeBase>(URRCameraShake::StaticClass());
	return PlayerController->PlayerCameraManager->StartCameraShake(ClassToPlay, Scale);
}
