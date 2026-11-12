// Rogue Rigs - C++ base of the main menu screen (WBP_MainMenu, task-338-01): New Run, Options (task-505-03), Exit.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRMainMenuWidget.generated.h"

class UWorld;
class URROptionsWidget;

/**
 * Main menu screen (main-menu gdd). Created by ARRMainMenuGameMode for the local player. Layout (title, labels, background,
 * Options greyed out) lives in the Blueprint subclass: its New Run button calls StartNewRun, its Exit button QuitGame, and its
 * Options button (named OptionsButton) is bound here: enabled when IsOptionsAvailable, it opens OptionsWidgetClass over the menu. Set DesiredFocusWidget to the New Run button in the designer:
 * the widget is focusable and forwards its focus there, so Enter starts a run. The debug commands rr.Menu.NewRun and
 * rr.Menu.Exit call the same static functions (StartNewRunInWorld, QuitGameInWorld) without a widget.
 */
UCLASS(Abstract)
class ROGUERIGS_API URRMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * New Run button: StartNewRunInWorld (run reset, loading screen, then URRRunSettings::NewRunMapName, L_ArenaM1); the arena
	 * miner then begins level 1. A second call while the travel is pending is ignored. False when no travel was requested.
	 */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	bool StartNewRun();

	/** Exit button: quits the game (UKismetSystemLibrary::QuitGame). */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void QuitGame();

	URRMainMenuWidget(const FObjectInitializer& ObjectInitializer);

	/** Options button state: true when OptionsWidgetClass is set (options-menu, task-505). */
	UFUNCTION(BlueprintPure, Category = "Menu")
	bool IsOptionsAvailable() const { return !OptionsWidgetClass.IsNull(); }

	/** Options button: opens the options screen over the menu (Z order OptionsZOrder); Back or Escape closes it and puts the
	 * keyboard focus back on OptionsButton. False when no class is set or the screen is already open. */
	UFUNCTION(BlueprintCallable, Category = "Menu")
	bool OpenOptions();

	/** The open options screen, or null. */
	UFUNCTION(BlueprintPure, Category = "Menu")
	URROptionsWidget* GetOptionsWidget() const { return OptionsWidget; }

	/** Options screen class (WBP_Options). */
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSoftClassPtr<URROptionsWidget> OptionsWidgetClass;

	/** Viewport Z order of the options screen (the menu is 0). */
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	int32 OptionsZOrder = 10;

	/**
	 * Shared New Run action (widget button and rr.Menu.NewRun): URRRunTransitionSubsystem::StartNewRun on
	 * URRRunSettings::NewRunMapName (ResetRun, loading phase for level 1 with the loading screen, paint wait, OpenLevel; the arena
	 * is held until level 1 is ready, then fades in, task-379-01). False (warning logged) without a world, with an empty map
	 * name, or while a transition already runs (second press).
	 */
	static bool StartNewRunInWorld(UWorld* World);

	/** Shared Exit action (widget button and rr.Menu.Exit): quits the game through player 0's controller of World. */
	static void QuitGameInWorld(UWorld* World);

protected:
	/** Makes the widget focusable, so the UI-only input mode can focus it and forward the focus to DesiredFocusWidget; gives New Run
	 * and Exit the hover growth of ui-button-hover. */
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleOptionsClicked();

	UFUNCTION()
	void HandleOptionsClosed();

	UFUNCTION()
	void HandleOptionsHovered();

	UPROPERTY(Transient)
	TObjectPtr<URROptionsWidget> OptionsWidget;

	/** True once StartNewRun requested the travel: guards against a double press. */
	bool bNewRunRequested = false;
};
