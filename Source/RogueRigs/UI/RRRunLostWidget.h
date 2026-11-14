// Rogue Rigs - C++ base of the run-lost screen (WBP_RunLost, death-screen gdd, task-484): recap, Retry and Main Menu buttons,
// appearance (shared entrance effect URRScreenEntrance, task-547) with its skip, level desaturation.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/RRRunLostSubsystem.h"
#include "UI/RRScreenEntrance.h"
#include "RRRunLostWidget.generated.h"

class UButton;

/**
 * Run-lost screen (death-screen gdd). Created by ARRPlayerController (RunLostWidgetClass) on URRRunLostSubsystem::OnRunLost, game
 * paused, HUD hidden. Layout and texts live in the Blueprint subclass: implement OnRunLostShown to fill the title, subtitle and
 * recap. The appearance (about 1.4 s, gdd Appearance) is the shared entrance URRScreenEntrance (task-547), started right after
 * OnRunLostShown on the parts named in EntranceParts; a key or click (SkipButton) skips it to the end state, then FinishAppearance
 * gives keyboard and gamepad focus to RetryButton.
 * Optional bound buttons RetryButton and MainMenuButton call Retry and ReturnToMainMenu (URRRunLostSubsystem).
 * SetLevelDesaturation drives the desaturation of the level behind (view target camera post process saturation).
 */
UCLASS(Abstract)
class ROGUERIGS_API URRRunLostWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	URRRunLostWidget(const FObjectInitializer& ObjectInitializer);

	/** Stores Recap, resets the appearance state and calls OnRunLostShown. Called by the controller once the widget is in the viewport. */
	void ShowRunLost(const FRRRunLostRecap& InRecap);

	/** Recap shown (default before ShowRunLost). */
	UFUNCTION(BlueprintPure, Category = "Run Lost")
	const FRRRunLostRecap& GetRecap() const { return Recap; }

	/** Retry button: URRRunLostSubsystem::Retry (new run at level 1 through the loading screen). False when refused. */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	bool Retry();

	/** Main Menu button: URRRunLostSubsystem::ReturnToMainMenu (run reset, L_MainMenu). False when refused. */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	bool ReturnToMainMenu();

	/** End of the appearance animation (played to its end or skipped): input reaches the buttons, focus on RetryButton. */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	void FinishAppearance();

	/** Skip request: during the appearance jumps to its end state, calls OnSkipAppearanceRequested and returns true; false once it finished. */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	bool RequestSkipAppearance();

	/** True once FinishAppearance ran for the shown recap. */
	UFUNCTION(BlueprintPure, Category = "Run Lost")
	bool IsAppearanceFinished() const { return bAppearanceFinished; }

	/**
	 * Desaturation of the level behind the screen, 0 (full colour) to 1 (grey): sets the colour saturation of the owning player's
	 * view target camera post process (the paused level keeps rendering). False without a camera.
	 */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	bool SetLevelDesaturation(float Amount);

	/** Gives keyboard and gamepad focus to RetryButton (or to the widget itself without one). */
	UFUNCTION(BlueprintCallable, Category = "Run Lost")
	void FocusRetry();

	/** Widget to focus in the Game and UI input mode: RetryButton when bound, else this widget. */
	UWidget* GetRetryFocusWidget();

protected:
	/** Makes the widget focusable, binds RetryButton / MainMenuButton clicks and sets up the entrance. */
	virtual void NativeOnInitialized() override;

	/** Advances the entrance (real time, also while paused). */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Puts the level saturation back. */
	virtual void NativeDestruct() override;

	/** Fill the screen from NewRecap and start the appearance animation. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run Lost")
	void OnRunLostShown(const FRRRunLostRecap& NewRecap);

	/** A key or click skipped the appearance (already at its end state, FinishAppearance done). Optional Blueprint notification. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run Lost")
	void OnSkipAppearanceRequested();

	/** RetryButton click: Retry. */
	UFUNCTION()
	void HandleRetryClicked();

	/** MainMenuButton click: ReturnToMainMenu. */
	UFUNCTION()
	void HandleMainMenuClicked();

	/** Optional Retry button of the Blueprint layout (name RetryButton): click calls Retry, focused when the appearance ends. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost", meta = (BindWidgetOptional))
	TObjectPtr<UButton> RetryButton;

	/** Optional Main Menu button of the Blueprint layout (name MainMenuButton): click calls ReturnToMainMenu. */
	UPROPERTY(BlueprintReadOnly, Category = "Run Lost", meta = (BindWidgetOptional))
	TObjectPtr<UButton> MainMenuButton;

	/** Recap of ShowRunLost. */
	UPROPERTY(Transient)
	FRRRunLostRecap Recap;

	/** Widgets the shared entrance drives (title group, RecapBlock, the two buttons, SkipButton). */
	UPROPERTY(EditDefaultsOnly, Category = "Run Lost")
	FRRScreenEntranceParts EntranceParts;

	/** The shared entrance effect of this screen. */
	UPROPERTY(Transient)
	TObjectPtr<URRScreenEntrance> Entrance;

	/** True once FinishAppearance ran. */
	bool bAppearanceFinished = false;
};
