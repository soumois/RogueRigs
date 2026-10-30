// Rogue Rigs - game mode of the main menu map (L_MainMenu, task-338-01): no pawn, menu player controller, menu widget class.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RRMainMenuGameMode.generated.h"

class APlayerController;
class URRLoadingScreenWidget;
class URRMainMenuWidget;

/**
 * Game mode of the main menu (main-menu gdd). No pawn is spawned; PlayerControllerClass is ARRMainMenuPlayerController (mouse
 * cursor, UI-only input). The menu screen is MainMenuWidgetClass (the WBP subclass of URRMainMenuWidget), set in the Blueprint
 * subclass of this game mode; the menu controller asks CreateMainMenuWidget for it at its BeginPlay, and CreateLoadingScreenWidget
 * (LoadingScreenWidgetClass) when New Run starts the loading (task-379-01). Arena maps keep
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

	/**
	 * Creates LoadingScreenWidgetClass owned by PlayerController and adds it to the viewport above the menu (LoadingScreenWidgetZOrder),
	 * on the New Run loading start (task-379-01). Returns the widget, or null (logged) without a class or a local player.
	 */
	URRLoadingScreenWidget* CreateLoadingScreenWidget(APlayerController* PlayerController);

	/** Loading screen class shown over the menu after New Run (WBP_Loading). */
	UFUNCTION(BlueprintPure, Category = "Menu")
	TSubclassOf<URRLoadingScreenWidget> GetLoadingScreenWidgetClass() const { return LoadingScreenWidgetClass; }

protected:
	/** Menu screen created for the local player: the Blueprint subclass of URRMainMenuWidget (WBP_MainMenu). None = no menu, warning. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	TSubclassOf<URRMainMenuWidget> MainMenuWidgetClass;

	/** Z-order of the menu widget in the viewport. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	int32 MainMenuWidgetZOrder = 0;

	/**
	 * Loading screen shown over the menu from New Run until the travel (task-379-01): the Blueprint subclass of
	 * URRLoadingScreenWidget (WBP_Loading). None = no screen (logged); the travel then starts after
	 * URRRunSettings::LoadingPaintWaitMaxFrames frames without a paint.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	TSubclassOf<URRLoadingScreenWidget> LoadingScreenWidgetClass;

	/** Z-order of the New Run loading screen in the viewport; above MainMenuWidgetZOrder. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	int32 LoadingScreenWidgetZOrder = 10;
};
