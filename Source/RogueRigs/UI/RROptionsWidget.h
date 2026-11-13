// Rogue Rigs - C++ base of the options screen (WBP_Options, task-500-04): Sound, Graphics and Gameplay tabs bound to
// URRGameUserSettings / URROptionsSubsystem, Reset to defaults, Back and Escape, keep-or-revert display dialog.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Settings/RRGameUserSettings.h"
#include "RROptionsWidget.generated.h"

class UButton;
class UCheckBox;
class UProgressBar;
class USlider;
class UTextBlock;
class UTexture2D;
class UWidgetSwitcher;
class URROptionsWidget;

/** Forwards one control event of WBP_Options with its key (internal to URROptionsWidget). */
UCLASS(Transient)
class ROGUERIGS_API URROptionsControlProxy : public UObject
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<URROptionsWidget> Screen;
	FString Key;
	int32 Direction = 0;

	UFUNCTION() void HandleClicked();
	UFUNCTION() void HandleValueChanged(float Value);
	UFUNCTION() void HandleCheckChanged(bool bIsChecked);
};

/**
 * Options screen (options-screen gdd). Opened by OpenOptions from the main menu (Options button) or the pause menu (Options
 * button): the menu it came from is collapsed under it and comes back, with focus on its Options button, on Back or Escape;
 * the pause stays on. Layout (title, labels, frames, sprites, anchors) lives in the Blueprint subclass WBP_Options; this class
 * finds its controls by name (all optional, missing ones are skipped) and keeps them in sync with the settings:
 * - tabs: SoundTabButton, GraphicsTabButton, GameplayTabButton (labels <Tab>TabText), TabSwitcher (pages Sound, Graphics,
 *   Gameplay in that order); the active tab button gets URROptionsSettings::ActiveTabTexture, the others InactiveTabTexture;
 * - sliders <Key>Slider (USlider 0..1), <Key>Fill (UProgressBar), <Key>Value (text) for Master, Music, Effects, ResolutionScale;
 * - arrow selectors <Key>Prev, <Key>Next (buttons), <Key>Value (text) for WindowMode, Resolution, FrameLimit, Preset, Shadows,
 *   EffectsQuality, AntiAliasing, PostProcessing, Textures;
 * - toggles <Key>Toggle (UCheckBox, checked = On), <Key>Knob (moved left or right in its overlay or horizontal slot),
 *   <Key>State (text ON/OFF) for VSync, DamageNumbers, AimIndicator, ScreenShake, PauseOnFocusLoss;
 * - ResetButton (resets the current tab), BackButton;
 * - ConfirmDialog (shown while a display change waits), ConfirmCountdownText ("Reverting in 15 s"), KeepButton, RevertButton.
 * Every change applies at once (no Apply button) and is saved by the settings.
 */
UCLASS(Abstract)
class ROGUERIGS_API URROptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Opens the options screen over FromMenu (main or pause menu widget) for its owning player, on Tab: class
	 * URROptionsSettings::OptionsWidgetClass, Z order FromMenu's + 1 (OptionsZOrder), FromMenu collapsed, keyboard focus on the
	 * screen. Returns the open screen (the already open one when called twice), null without a class or a player.
	 */
	static URROptionsWidget* OpenOptions(UUserWidget* FromMenu, ERROptionsTab Tab = ERROptionsTab::Sound);

	/** The options screen open in World, or null. */
	static URROptionsWidget* FindOpen(const UWorld* World);

	/** True when URROptionsSettings::OptionsWidgetClass loads (menus enable their Options button from it). */
	static bool IsOptionsAvailable();

	/** Shows a tab (button look, page). */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void ShowTab(ERROptionsTab Tab);

	/** Current tab. */
	UFUNCTION(BlueprintPure, Category = "Options")
	ERROptionsTab GetCurrentTab() const { return CurrentTab; }

	/** Back button: reverts a pending display change, closes the screen and gives the menu it came from back with focus. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void Back();

	/** Escape / gamepad back: Revert while the dialog shows, else Back. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void HandleEscape();

	/** Reset to defaults of the current tab. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void ResetCurrentTab();

	/** Reads every value from the settings into the controls (called on open and after each change). */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void RefreshFromSettings();

	/** Shows or hides the keep-or-revert dialog (bound to URROptionsSubsystem::OnDisplayConfirmChanged). */
	UFUNCTION()
	void HandleDisplayConfirmChanged(bool bPending);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	/** Called after the tab changes (layout extras in Blueprint, optional). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Options")
	void OnTabShown(ERROptionsTab Tab);

private:
	template <typename T> T* Find(const FString& Name) const { return Cast<T>(GetWidgetFromName(FName(*Name))); }

	void BindControls();
	void BindSlider(const FString& Key);
	void BindSelector(const FString& Key);
	void BindToggle(const FString& Key);
	void SetSlider(const FString& Key, float Normalized, const FText& ValueText);
	void SetSelectorText(const FString& Key, const FText& ValueText);
	void SetToggle(const FString& Key, bool bOn);
	void StyleTabButton(UButton* Button, UTextBlock* Label, bool bActive);
	void UpdateCountdownText();

	/** Selector step (-1 Prev, +1 Next) on Key. */
	friend class URROptionsControlProxy;
	void StepSelector(const FString& Key, int32 Direction);
	void OnSliderChanged(const FString& Key, float Value);
	void OnToggleChanged(const FString& Key, bool bOn);

	/**
	 * Effects slider preview (task-520): plays URROptionsSettings::EffectsPreviewEvent when the Effects volume reaches a step other
	 * than the last one played, at most once per EffectsPreviewInterval (real time); a step reached inside the interval plays from
	 * NativeTick once it ends. bFromTick: the deferred play.
	 */
	void UpdateEffectsPreview(bool bFromTick);

	UFUNCTION() void HandleSoundTab();
	UFUNCTION() void HandleGraphicsTab();
	UFUNCTION() void HandleGameplayTab();
	UFUNCTION() void HandleKeep();
	UFUNCTION() void HandleRevert();
	UFUNCTION() void HandleBackClicked();
	UFUNCTION() void HandleResetClicked();

	/** Menu widget hidden under the screen, its visibility before, and the widget to focus on return. */
	TWeakObjectPtr<UUserWidget> FromMenu;
	ESlateVisibility FromMenuVisibility = ESlateVisibility::Visible;

	ERROptionsTab CurrentTab = ERROptionsTab::Sound;
	bool bClosing = false;

	/** True while RefreshFromSettings writes the controls: their change events are ignored (no write back). */
	bool bRefreshing = false;

	/** Effects preview state: step last played (set on open, so a move inside it plays nothing), its real time, a play waiting. */
	int32 EffectsPreviewStep = INDEX_NONE;
	double EffectsPreviewSeconds = -1.0e9;
	bool bEffectsPreviewPending = false;

	FDelegateHandle SettingsChangedHandle;

	/** One proxy per bound control: carries the control key to the shared handlers (UMG delegates have no payload). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URROptionsControlProxy>> Proxies;

	static TWeakObjectPtr<URROptionsWidget> OpenScreen;
};
