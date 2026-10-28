// Rogue Rigs - player controller of the main menu map (task-338-01): mouse cursor shown, UI-only input focused on the menu.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RRMainMenuPlayerController.generated.h"

class URRMainMenuWidget;

/**
 * Controller of the main menu (main-menu gdd): shows the mouse cursor and, for the local player, asks ARRMainMenuGameMode to
 * create the menu widget at BeginPlay, then sets a UI-only input mode focused on it (the widget forwards the focus to its
 * DesiredFocusWidget, the New Run button, so Enter starts a run). No gameplay input.
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

protected:
	/** Local player only: creates the menu widget through the game mode and sets the UI-only input mode. */
	virtual void BeginPlay() override;

	/** Menu widget created at BeginPlay. */
	UPROPERTY(Transient)
	TObjectPtr<URRMainMenuWidget> MainMenuWidget;
};
