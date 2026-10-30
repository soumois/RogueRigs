// Rogue Rigs - player controller of the main menu map (task-338-01): mouse cursor shown, UI-only input focused on the menu;
// New Run loading screen over the menu (task-379-01).

#include "Menu/RRMainMenuPlayerController.h"

#include "Engine/World.h"
#include "Menu/RRMainMenuGameMode.h"
#include "Menu/RRMainMenuWidget.h"
#include "RogueRigs.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "UI/RRLoadingScreenWidget.h"

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

	if (URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this))
	{
		BoundTransition = Transition;
		Transition->OnLoadingStarted.AddUniqueDynamic(this, &ARRMainMenuPlayerController::HandleLoadingStarted);
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

void ARRMainMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (URRRunTransitionSubsystem* Transition = BoundTransition.Get())
	{
		Transition->OnLoadingStarted.RemoveDynamic(this, &ARRMainMenuPlayerController::HandleLoadingStarted);
	}
	BoundTransition.Reset();
	Super::EndPlay(EndPlayReason);
}

void ARRMainMenuPlayerController::HandleLoadingStarted(int32 LevelIndex, const FString& LayoutName)
{
	// The menu stays drawn under the loading screen but takes no click or key any more (no second New Run).
	if (MainMenuWidget)
	{
		MainMenuWidget->SetIsEnabled(false);
		MainMenuWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	ARRMainMenuGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ARRMainMenuGameMode>() : nullptr;
	if (!LoadingScreenWidget && GameMode)
	{
		LoadingScreenWidget = GameMode->CreateLoadingScreenWidget(this);
	}
	if (!LoadingScreenWidget)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: New Run loading of level %d without a loading screen (%s)."), *GetName(), LevelIndex,
			GameMode ? TEXT("LoadingScreenWidgetClass not set or not created") : TEXT("no main menu game mode"));
		return;
	}
	LoadingScreenWidget->ShowLoading(LevelIndex, LayoutName);

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(LoadingScreenWidget->TakeWidget());
	SetInputMode(InputMode);
	UE_LOG(LogRogueRigs, Log, TEXT("%s: loading screen shown over the main menu for level %d (layout %s, progress %s)."), *GetName(), LevelIndex, *LayoutName,
		LoadingScreenWidget->IsLoadingProgressMeasured() ? TEXT("measured") : TEXT("not measured"));
}
