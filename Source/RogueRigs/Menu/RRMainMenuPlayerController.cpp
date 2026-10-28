// Rogue Rigs - player controller of the main menu map (task-338-01): mouse cursor shown, UI-only input focused on the menu.

#include "Menu/RRMainMenuPlayerController.h"

#include "Engine/World.h"
#include "Menu/RRMainMenuGameMode.h"
#include "Menu/RRMainMenuWidget.h"

ARRMainMenuPlayerController::ARRMainMenuPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
}

void ARRMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalPlayerController() || !GetLocalPlayer())
	{
		return;
	}

	if (ARRMainMenuGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ARRMainMenuGameMode>() : nullptr)
	{
		MainMenuWidget = GameMode->CreateMainMenuWidget(this);
	}

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	if (MainMenuWidget)
	{
		InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
	}
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}
