// Rogue Rigs - player controller of the main menu map (task-338-01): mouse cursor shown, UI-only input focused on the menu;
// New Run loading screen over the menu (task-379-01).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RRMainMenuPlayerController.generated.h"

class URRLoadingScreenWidget;
class URRMainMenuWidget;
class URRRunTransitionSubsystem;

/**
 * Controller of the main menu (main-menu gdd): shows the mouse cursor and, for the local player, asks ARRMainMenuGameMode to
 * create the menu widget at BeginPlay, then sets a UI-only input mode focused on it (the widget forwards the focus to its
 * DesiredFocusWidget, the New Run button, so Enter starts a run). No gameplay input. On
 * URRRunTransitionSubsystem::OnLoadingStarted (New Run, task-379-01) it shows the game mode's loading screen above the menu and
 * the menu stops taking input; the widget lives until the travel destroys the menu world.
 */
UCLASS()
class ROGUERIGS_API ARRMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Shows the mouse cursor and enables click events. */
	ARRMainMenuPlayerController();

	/** Menu widget created at BeginPlay (null without a widget class or for a non-local controller). */
	UFUNCTION(BlueprintPure, Category = "Menu")
	URRMainMenuWidget* GetMainMenuWidget() const { return MainMenuWidget; }

	/** Loading screen shown after New Run (null before, or without ARRMainMenuGameMode::LoadingScreenWidgetClass). */
	UFUNCTION(BlueprintPure, Category = "Menu")
	URRLoadingScreenWidget* GetLoadingScreenWidget() const { return LoadingScreenWidget; }

protected:
	/** Local player only: creates the menu widget through the game mode, sets the UI-only input mode, binds the loading start. */
	virtual void BeginPlay() override;

	/** Unbinds the transition subsystem. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** OnLoadingStarted callback (New Run): loading screen over the menu, menu input off. */
	UFUNCTION()
	void HandleLoadingStarted(int32 LevelIndex, const FString& LayoutName);

	/** Menu widget created at BeginPlay. */
	UPROPERTY(Transient)
	TObjectPtr<URRMainMenuWidget> MainMenuWidget;

	/** Loading screen created on OnLoadingStarted. */
	UPROPERTY(Transient)
	TObjectPtr<URRLoadingScreenWidget> LoadingScreenWidget;

	/** Transition subsystem bound at BeginPlay. */
	TWeakObjectPtr<URRRunTransitionSubsystem> BoundTransition;
};
