// Rogue Rigs - player settings saved in GameUserSettings.ini: Gameplay toggles, Sound and Graphics options (options-menu).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Containers/Ticker.h"
#include "RRGameUserSettings.generated.h"

class UWorld;

/** Volume group of a Sound slider (options-menu, Sound). */
UENUM(BlueprintType)
enum class ERRVolumeGroup : uint8
{
	Master,
	Music,
	Effects
};

/** Quality detail of the Graphics tab set by the preset and changeable alone (options-menu, Graphics). */
UENUM(BlueprintType)
enum class ERRGraphicsDetail : uint8
{
	Shadows,
	Effects,
	AntiAliasing,
	PostProcessing,
	Textures
};

/** On/off option of the Gameplay tab (options-menu, Gameplay). */
UENUM(BlueprintType)
enum class ERRGameplayOption : uint8
{
	DamageNumbers,
	AimIndicator,
	ScreenShake,
	PauseOnFocusLoss
};

/** Fired when the display confirmation starts (true) or ends by Keep, Revert or the end of the countdown (false). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRRDisplayConfirmEvent, bool, bPending);

/**
 * The project's game user settings class (Config/DefaultEngine.ini: [/Script/Engine.Engine] GameUserSettingsClassName).
 * Holds the player options; every setter applies at once and saves into the user's GameUserSettings.ini (Saved/Config, per
 * Windows user). The engine reloads the file and applies it at launch; the volumes are pushed again after each map load.
 * A window mode or resolution change starts a display confirmation (URROptionsSettings::DisplayConfirmSeconds): until
 * ConfirmDisplayChange the previous mode and resolution are what is saved, and the end of the countdown reverts them.
 * Data (asset paths, steps, choices): URROptionsSettings. Debug command: rr.Settings (RRSettingsDebugCommands.cpp).
 */
UCLASS(Config = GameUserSettings, ConfigDoNotCheckDefaults)
class ROGUERIGS_API URRGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	/** The engine's game user settings as this class; null when the engine uses another class or has none. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static URRGameUserSettings* GetRRGameUserSettings();

	/** Shows or hides floating damage numbers (takes effect at once; call SaveSettings to keep it). */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetShowDamageNumbers(bool bShow) { bShowDamageNumbers = bShow; }

	/** True when floating damage numbers are shown. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	bool GetShowDamageNumbers() const { return bShowDamageNumbers; }

	// ---- Gameplay ----------------------------------------------------------------------------------------------------

	/** Value of a Gameplay option (all default On). */
	UFUNCTION(BlueprintPure, Category = "Settings|Gameplay")
	bool GetGameplayOption(ERRGameplayOption Option) const;

	/** Sets a Gameplay option and saves; the game systems read it at their next use (damage number, laser refresh, shake,
	 * focus loss). */
	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetGameplayOption(ERRGameplayOption Option, bool bOn);

	/** Every Gameplay option back On, saved (Reset to defaults of the Gameplay tab). */
	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void ResetGameplayToDefaults();

	/** Aim laser drawn (aim-indicator). */
	bool GetShowAimIndicator() const { return bShowAimIndicator; }

	/** Camera shakes played (URRCameraShakeLibrary). */
	bool GetScreenShakeOn() const { return bScreenShake; }

	/** The run pauses (pause menu) when the game window loses focus. */
	bool GetPauseOnFocusLoss() const { return bPauseOnFocusLoss; }

	/** Gameplay setting of the running game: the engine's settings as this class, true (default On) without one. */
	static bool IsGameplayOptionOn(ERRGameplayOption Option);

	/** Engine defaults plus the project options (gameplay options on, volumes 100 %, resolution scale 100 %). */
	virtual void SetToDefaults() override;

	/** Also registers the map-load hook that pushes the volumes again. */
	virtual void LoadSettings(bool bForceReload = false) override;

	/** Saves; while a display change waits for confirmation, the previous window mode and resolution are written instead. */
	virtual void SaveSettings() override;

	virtual void BeginDestroy() override;

	// ---- Sound -------------------------------------------------------------------------------------------------------

	/** Slider value of a volume group, 0..1. */
	UFUNCTION(BlueprintPure, Category = "Settings|Sound")
	float GetVolume(ERRVolumeGroup Group) const;

	/** Sets a volume (clamped 0..1, snapped to URROptionsSettings::VolumeStep), applies it to its sound class and saves. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Sound")
	void SetVolume(ERRVolumeGroup Group, float Volume);

	/** Volumes back to 100 %, applied and saved (Reset to defaults of the Sound tab). */
	UFUNCTION(BlueprintCallable, Category = "Settings|Sound")
	void ResetSoundToDefaults();

	/** Pushes the volume mix with the three slider volumes on the main audio device (no-op without one or without the assets). */
	void ApplyVolumes();

	// ---- Graphics: display (confirmed) -------------------------------------------------------------------------------

	/** Takes the window mode and size the game window really has (a -windowed or -ResX launch differs from the file); called
	 * before each display change and when the options screen opens. No-op without a game window. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SyncDisplayFromWindow();

	/** Window mode to show: Fullscreen, WindowedFullscreen or Windowed. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	TEnumAsByte<EWindowMode::Type> GetWindowMode() const { return GetFullscreenMode(); }

	/** Changes the window mode (resolution adjusted to fit), applies it and starts the display confirmation. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void RequestWindowMode(TEnumAsByte<EWindowMode::Type> NewMode);

	/** Next (Direction > 0) or previous window mode in the order Fullscreen, Windowed fullscreen, Windowed (wraps). */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void StepWindowMode(int32 Direction);

	/** Resolutions of the selector for the current mode: monitor fullscreen modes, or the convenient windowed sizes. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	TArray<FIntPoint> GetResolutionOptions() const;

	/** False in Windowed fullscreen (the desktop resolution is used). */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	bool IsResolutionSelectable() const;

	/** Changes the resolution, applies it and starts the display confirmation. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void RequestResolution(FIntPoint NewResolution);

	/** Next or previous entry of GetResolutionOptions (clamped); no-op when the resolution is not selectable. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void StepResolution(int32 Direction);

	/** True while a window mode or resolution change waits for Keep. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	bool IsDisplayConfirmPending() const { return bDisplayConfirmPending; }

	/** Whole seconds left before the pending display change reverts (0 when none). */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetDisplayConfirmSecondsLeft() const;

	/** Keep: the pending display change becomes the saved one. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ConfirmDisplayChange();

	/** Revert (button, Escape or end of countdown): the previous window mode and resolution come back. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void RevertDisplayChange();

	/** Display confirmation started (true) or ended (false). */
	UPROPERTY(BlueprintAssignable, Category = "Settings|Graphics")
	FRRDisplayConfirmEvent OnDisplayConfirmChanged;

	// ---- Graphics: applied at once -----------------------------------------------------------------------------------

	/** Turns VSync on or off, applies and saves. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetVSyncOn(bool bOn);

	/** Frame rate limit (0 = Unlimited), applies and saves. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetFrameRateLimitValue(int32 Limit);

	/** Current frame rate limit, 0 = Unlimited. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetFrameRateLimitValue() const { return FMath::RoundToInt(GetFrameRateLimit()); }

	/** Next or previous entry of URROptionsSettings::FrameRateLimitOptions (wraps). */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void StepFrameRateLimit(int32 Direction);

	/** Preset 0 Low .. 3 Epic, or -1 (Custom) when the five details differ. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetQualityPreset() const;

	/** Sets every quality group to Level (0..3), keeps the resolution scale, applies and saves. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetQualityPreset(int32 Level);

	/** Next or previous preset (Low..Epic, wraps; from Custom it starts at the rounded mean of the details). */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void StepQualityPreset(int32 Direction);

	/** Level 0..3 of one detail. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	int32 GetGraphicsDetail(ERRGraphicsDetail Detail) const;

	/** Sets one detail (0..3), applies and saves; the preset then reads Custom when the details differ. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetGraphicsDetail(ERRGraphicsDetail Detail, int32 Level);

	/** Next or previous level of one detail (wraps). */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void StepGraphicsDetail(ERRGraphicsDetail Detail, int32 Direction);

	/** Resolution scale in percent (50..100). */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	float GetResolutionScalePercent() const;

	/** Sets the resolution scale (clamped, snapped to the step), applies and saves. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetResolutionScalePercent(float Percent);

	/** Graphics back to the first-launch values (Windowed fullscreen at the desktop size, VSync off, Unlimited, Epic,
	 * 100 %); a window mode or resolution change goes through the display confirmation. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ResetGraphicsToDefaults();

	// ---- Texts drawn by the widget -----------------------------------------------------------------------------------

	/** "Fullscreen", "Windowed fullscreen", "Windowed". */
	UFUNCTION(BlueprintPure, Category = "Settings|Text")
	static FText GetWindowModeText(TEnumAsByte<EWindowMode::Type> Mode);

	/** "1920 x 1080". */
	UFUNCTION(BlueprintPure, Category = "Settings|Text")
	static FText GetResolutionText(FIntPoint Resolution);

	/** "60" or "Unlimited". */
	UFUNCTION(BlueprintPure, Category = "Settings|Text")
	static FText GetFrameRateLimitText(int32 Limit);

	/** "Low", "Medium", "High", "Epic", or "Custom" for -1. */
	UFUNCTION(BlueprintPure, Category = "Settings|Text")
	static FText GetQualityLevelText(int32 Level);

	/** "On" / "Off". */
	UFUNCTION(BlueprintPure, Category = "Settings|Text")
	static FText GetOnOffText(bool bOn);

	/** "85%" for 0.85 (volumes) or 85 (with bAlreadyPercent). */
	UFUNCTION(BlueprintPure, Category = "Settings|Text")
	static FText GetPercentText(float Value, bool bAlreadyPercent);

	/** Shown resolution: the desktop size in Windowed fullscreen, else the stored resolution. */
	UFUNCTION(BlueprintPure, Category = "Settings|Graphics")
	FIntPoint GetShownResolution() const;

	/** One line of every stored value and of what the engine reports (rr.Settings dump). */
	FString DescribeSettings() const;

protected:
	/** Player toggle of the floating damage numbers (damage-numbers). */
	UPROPERTY(Config)
	bool bShowDamageNumbers = true;

	/** Player toggle of the aim laser (aim-indicator). */
	UPROPERTY(Config)
	bool bShowAimIndicator = true;

	/** Player toggle of every camera shake. */
	UPROPERTY(Config)
	bool bScreenShake = true;

	/** Player toggle of the pause on focus loss (in a run). */
	UPROPERTY(Config)
	bool bPauseOnFocusLoss = true;

	/** Master slider, 0..1. */
	UPROPERTY(Config)
	float MasterVolume = 1.f;

	/** Music slider, 0..1. */
	UPROPERTY(Config)
	float MusicVolume = 1.f;

	/** Effects slider, 0..1. */
	UPROPERTY(Config)
	float EffectsVolume = 1.f;

private:
	/** Stores the current mode and resolution as the revert target (first change only) and starts the countdown. */
	void BeginDisplayChange();
	/** Applies the stored mode and resolution to the window. */
	void ApplyDisplay();
	/** Ends the confirmation and fires OnDisplayConfirmChanged(false). */
	void EndDisplayConfirm();
	/** Countdown ticker. */
	bool TickDisplayConfirm(float DeltaTime);
	/** Applies scalability, VSync and frame limit, then saves. */
	void ApplyAndSaveNonDisplay();
	/** Map-load hook: ApplyVolumes. */
	void HandlePostLoadMap(UWorld* World);

	bool bDisplayConfirmPending = false;
	TEnumAsByte<EWindowMode::Type> PreviousWindowMode = EWindowMode::WindowedFullscreen;
	FIntPoint PreviousResolution = FIntPoint::ZeroValue;
	double DisplayConfirmEndTime = 0.0;
	FTSTicker::FDelegateHandle DisplayConfirmTicker;
	FDelegateHandle PostLoadMapHandle;
};
