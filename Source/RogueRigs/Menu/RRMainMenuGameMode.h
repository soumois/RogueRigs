// Rogue Rigs - game mode of the main menu map (L_MainMenu, task-338-01): no pawn, menu player controller, menu widget class.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RRMainMenuGameMode.generated.h"

class APlayerController;
class URRMainMenuWidget;

/**
 * Game mode of the main menu (main-menu gdd). No pawn is spawned; PlayerControllerClass is ARRMainMenuPlayerController (mouse
 * cursor, UI-only input). The menu screen is MainMenuWidgetClass (the WBP subclass of URRMainMenuWidget), set in the Blueprint
 * subclass of this game mode; the menu controller asks CreateMainMenuWidget for it at its BeginPlay. Arena maps keep
 * ARRGameMode: nothing here runs outside the menu map.
 */
UCLASS()
class ROGUERIGS_API ARRMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Sets DefaultPawnClass to none and PlayerControllerClass to ARRMainMenuPlayerController. */
	ARRMainMenuGameMode();

	/**
	 * Creates MainMenuWidgetClass owned by PlayerController and adds it to the viewport (called once by the local menu
	 * controller at BeginPlay). Returns the widget, or null (warning logged) without a class or a local player.
	 */
	URRMainMenuWidget* CreateMainMenuWidget(APlayerController* PlayerController);

	/** Menu widget class created for the local player (WBP_MainMenu). */
	UFUNCTION(BlueprintPure, Category = "Menu")
	TSubclassOf<URRMainMenuWidget> GetMainMenuWidgetClass() const { return MainMenuWidgetClass; }

protected:
	/** Menu screen created for the local player: the Blueprint subclass of URRMainMenuWidget (WBP_MainMenu). None = no menu, warning. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	TSubclassOf<URRMainMenuWidget> MainMenuWidgetClass;

	/** Z-order of the menu widget in the viewport. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	int32 MainMenuWidgetZOrder = 0;
};
