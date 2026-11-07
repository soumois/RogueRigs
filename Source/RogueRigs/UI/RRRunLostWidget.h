// Rogue Rigs - C++ base of the run-lost screen (WBP_RunLost, death-screen gdd, task-484): recap, Retry and Main Menu buttons,
// appearance skip and level desaturation helpers for the Blueprint animation.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Run/RRRunLostSubsystem.h"
#include "RRRunLostWidget.generated.h"

class UButton;
class UCameraComponent;

/**
 * Run-lost screen (death-screen gdd). Created by ARRPlayerController (RunLostWidgetClass) on URRRunLostSubsystem::OnRunLost, game
 * paused, HUD hidden. Layout, texts and the appearance animation (about 1.4 s, gdd Appearance) live in the Blueprint subclass:
 * implement OnRunLostShown to fill the title, subtitle and recap and start the animation; call FinishAppearance when it ends.
 * Skip: the Blueprint's OnPreviewKeyDown and OnPreviewMouseButtonDown overrides call RequestSkipAppearance and return Handled when
 * it is true (the input is consumed, so the buttons cannot fire during the animation); it calls OnSkipAppearanceRequested (jump to
 * the end state, then call FinishAppearance). The module has no SlateCore dependency, so the input hooks stay in Blueprint.
 * FinishAppearance gives keyboard and gamepad focus to RetryButton.
 * Optional bound buttons RetryButton and MainMenuButton call Retry and ReturnToMainMenu (URRRunLostSubsystem).
 * SetLevelDesaturation drives the desaturation of the level behind (view target camera post process saturation).
 */
UCLASS(Abstract)
class ROGUERIGS_API URRRunLostWidget : public UUserWidget
{
	GENERATED_BODY()

public:
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

	/** Skip request from a key or click: during the appearance calls OnSkipAppearanceRequested and returns true (consume the input); false once it finished. */
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
	/** Makes the widget focusable and binds RetryButton / MainMenuButton clicks. */
	virtual void NativeOnInitialized() override;

	/** Fill the screen from NewRecap and start the appearance animation. Implement in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Run Lost")
	void OnRunLostShown(const FRRRunLostRecap& NewRecap);

	/** A key or click came during the appearance: jump to the end state, then call FinishAppearance. Implement in Blueprint. */
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

	/** True once FinishAppearance ran. */
	bool bAppearanceFinished = false;

	/** Camera whose saturation SetLevelDesaturation changes, and its values before the first change. */
	TWeakObjectPtr<UCameraComponent> SavedCamera;
	FVector4 SavedSaturation = FVector4(1.f, 1.f, 1.f, 1.f);
	bool bSavedOverride = false;
	bool bSaturationSaved = false;
};
