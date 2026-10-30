// Rogue Rigs - C++ base of the main menu screen (WBP_MainMenu, task-338-01): New Run, Options (disabled), Exit.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRMainMenuWidget.generated.h"

class UWorld;

/**
 * Main menu screen (main-menu gdd). Created by ARRMainMenuGameMode for the local player. Layout (title, labels, background,
 * Options greyed out) lives in the Blueprint subclass: its New Run button calls StartNewRun, its Exit button QuitGame, and its
 * Options button reads IsOptionsAvailable for its enabled state. Set DesiredFocusWidget to the New Run button in the designer:
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

	/** Options button state: false while the options screen does not exist (main-menu gdd: shown greyed out, does nothing). */
	UFUNCTION(BlueprintPure, Category = "Menu")
	bool IsOptionsAvailable() const { return false; }

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
	/** Makes the widget focusable, so the UI-only input mode can focus it and forward the focus to DesiredFocusWidget. */
	virtual void NativeOnInitialized() override;

	/** True once StartNewRun requested the travel: guards against a double press. */
	bool bNewRunRequested = false;
};
