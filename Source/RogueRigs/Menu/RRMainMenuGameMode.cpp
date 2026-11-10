// Rogue Rigs - game mode of the main menu map (L_MainMenu, task-338-01): no pawn, menu player controller, menu widget class.

#include "Menu/RRMainMenuGameMode.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Menu/RRMainMenuPlayerController.h"
#include "Menu/RRMainMenuWidget.h"
#include "RogueRigs.h"
#include "Engine/World.h"
#include "Run/RRRunSettings.h"
#include "Run/RRRunSubsystem.h"
#include "UI/RRLoadingScreenWidget.h"

ARRMainMenuGameMode::ARRMainMenuGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ARRMainMenuPlayerController::StaticClass();
}

void ARRMainMenuGameMode::StartPlay()
{
	Super::StartPlay();
	URRRunSubsystem* Run = URRRunSubsystem::Get(this);
	if (!Run || !GetWorld() || !GetWorld()->IsGameWorld())
	{
		return;
	}
	if (Run->HasNewRunLayout())
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: main menu open, next run already prepared (run seed %d, level 1 layout %s)."), *GetName(), Run->GetRunSeed(),
			Run->IsLayoutPending(1) ? TEXT("pending") : (Run->IsLayoutReady(1) ? TEXT("ready") : TEXT("failed")));
		return;
	}
	if (Run->IsRunInProgress())
	{
		// A run left without ResetRun (should not happen: Main Menu resets it); New Run resets and prepares then.
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: main menu open with a run in progress; level 1 is prepared at New Run."), *GetName());
		return;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("%s: main menu open, preparing the level 1 terrain of the next run in the background."), *GetName());
	Run->PrepareNewRunLayout();
}

URRMainMenuWidget* ARRMainMenuGameMode::CreateMainMenuWidget(APlayerController* PlayerController)
{
	if (!PlayerController || !PlayerController->IsLocalPlayerController() || !PlayerController->GetLocalPlayer())
	{
		return nullptr;
	}
	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: MainMenuWidgetClass is not set, no main menu shown (rr.Menu.NewRun and rr.Menu.Exit still work)."), *GetName());
		return nullptr;
	}
	URRMainMenuWidget* Widget = CreateWidget<URRMainMenuWidget>(PlayerController, MainMenuWidgetClass);
	if (Widget)
	{
		Widget->AddToViewport(MainMenuWidgetZOrder);
		UE_LOG(LogRogueRigs, Log, TEXT("%s: main menu shown (%s)."), *GetName(), *GetNameSafe(MainMenuWidgetClass.Get()));
	}
	return Widget;
}

URRLoadingScreenWidget* ARRMainMenuGameMode::CreateLoadingScreenWidget(APlayerController* PlayerController)
{
	if (!PlayerController || !PlayerController->IsLocalPlayerController() || !PlayerController->GetLocalPlayer())
	{
		return nullptr;
	}
	if (!LoadingScreenWidgetClass)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("%s: LoadingScreenWidgetClass is not set, New Run loads without a loading screen (the travel starts after %d frames without a paint)."),
			*GetName(), GetDefault<URRRunSettings>()->LoadingPaintWaitMaxFrames);
		return nullptr;
	}
	URRLoadingScreenWidget* Widget = CreateWidget<URRLoadingScreenWidget>(PlayerController, LoadingScreenWidgetClass);
	if (!Widget)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("%s: could not create the loading screen widget %s."), *GetName(), *GetNameSafe(LoadingScreenWidgetClass.Get()));
		return nullptr;
	}
	Widget->AddToViewport(LoadingScreenWidgetZOrder);
	return Widget;
}
