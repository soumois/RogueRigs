// Rogue Rigs - player settings saved in GameUserSettings.ini: options screen values (sound, gameplay toggles) on top of the
// engine's display and quality settings (options-screen gdd, task-500-03).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "RRGameUserSettings.generated.h"

/** Quality preset of the options screen: the engine scalability levels 0..3, or Custom when the five sub-settings differ. */
UENUM(BlueprintType)
enum class ERRQualityPreset : uint8
{
	Low = 0,
	Medium = 1,
	High = 2,
	Epic = 3,
	Custom = 4,
};

/** One of the five quality sub-settings of the options screen (engine scalability groups). */
UENUM(BlueprintType)
enum class ERRQualitySetting : uint8
{
	Shadows,
	Effects,
	AntiAliasing,
	PostProcessing,
	Textures,
};

/** The three volume groups of the options screen (sound classes of URROptionsSettings). */
UENUM(BlueprintType)
enum class ERRVolumeGroup : uint8
{
	Master,
	Music,
	Effects,
};

/** The four gameplay toggles of the options screen. */
UENUM(BlueprintType)
enum class ERRGameplayToggle : uint8
{
	DamageNumbers,
	AimIndicator,
	ScreenShake,
	PauseOnFocusLoss,
};

/** Tab of the options screen (Reset to defaults resets the current one). */
UENUM(BlueprintType)
enum class ERROptionsTab : uint8
{
	Sound,
	Graphics,
	Gameplay,
};

DECLARE_MULTICAST_DELEGATE(FRROnSettingsChanged);

/**
 * The project's game user settings class (Config/DefaultEngine.ini: [/Script/Engine.Engine] GameUserSettingsClassName).
 * Holds the options screen values that are not engine ones (volumes, gameplay toggles); display and quality use the engine
 * fields. Every Set*AndApply / SetVolume / SetGameplayToggle / SetQuality* applies the value at once and saves the file
 * (GameUserSettings.ini of the user's Saved/Config; per Windows user in a packaged game), except the window mode and resolution,
 * which go through URROptionsSubsystem (keep-or-revert countdown). While a display change waits for Keep, SaveSettings writes
 * the last confirmed display values. The engine loads the file and applies it at launch; a first launch has no file and keeps
 * the engine values.
 */
UCLASS(Config = GameUserSettings, ConfigDoNotCheckDefaults)
class ROGUERIGS_API URRGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	/** The engine's game user settings as this class; null when the engine uses another class or has none. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	static URRGameUserSettings* GetRRGameUserSettings();

	/** Broadcast after any change made through this class or URROptionsSubsystem (the subsystem applies the volumes from it). */
	FRROnSettingsChanged OnSettingsChanged;

	// ---- Gameplay toggles ----

	/** Shows or hides floating damage, heal and ore numbers at once, without saving (tests); the options screen uses SetGameplayToggle. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetShowDamageNumbers(bool bShow) { bShowDamageNumbers = bShow; }

	/** True when floating damage numbers are shown. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	bool GetShowDamageNumbers() const { return bShowDamageNumbers; }

	/** Sets one gameplay toggle (its system reads it live) and saves. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetGameplayToggle(ERRGameplayToggle Toggle, bool bOn);

	/** Value of one gameplay toggle. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	bool GetGameplayToggle(ERRGameplayToggle Toggle) const;

	/** Toggle value from the engine's settings; true (default on) when they are not URRGameUserSettings. */
	static bool IsGameplayToggleOn(ERRGameplayToggle Toggle);

	// ---- Sound ----

	/** Sets a volume group (0..1, snapped to URROptionsSettings::VolumeStep), applied through URROptionsSubsystem, saved. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetVolume(ERRVolumeGroup Group, float Volume);

	/** Volume of a group, 0..1. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	float GetVolume(ERRVolumeGroup Group) const;

	// ---- Graphics (window mode and resolution: URROptionsSubsystem) ----

	/** VSync on or off, applied and saved. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetVSyncAndApply(bool bEnable);

	/** Frame rate limit in frames per second (0 = Unlimited), applied and saved. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetFrameRateLimitAndApply(float Limit);

	/** Resolution scale in percent (clamped to URROptionsSettings Min/MaxResolutionScale, snapped to its step), applied and saved. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetResolutionScalePercent(float Percent);

	/** Current resolution scale in percent (engine screen percentage). */
	UFUNCTION(BlueprintPure, Category = "Settings")
	float GetResolutionScalePercent() const;

	/** Preset: Low..Epic sets every scalability group (the five sub-settings included); Custom changes nothing. Applied and saved. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetQualityPreset(ERRQualityPreset Preset);

	/** Low..Epic when the five sub-settings share one level, else Custom. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	ERRQualityPreset GetQualityPreset() const;

	/** Sets one sub-setting (0 Low .. 3 Epic), applied and saved; the preset then reads Custom unless all five match. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetQualitySetting(ERRQualitySetting Setting, int32 Level);

	/** Level 0..3 of one sub-setting. */
	UFUNCTION(BlueprintPure, Category = "Settings")
	int32 GetQualitySetting(ERRQualitySetting Setting) const;

	// ---- Defaults and saving ----

	/**
	 * Restores the defaults of one tab, applies and saves. Sound: 100 %. Gameplay: every toggle on. Graphics: VSync off,
	 * Unlimited, 100 %, Epic; window mode and resolution go back to the engine defaults through URROptionsSubsystem (countdown).
	 */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetTabToDefaults(ERROptionsTab Tab);

	/** Engine defaults plus the project options (volumes 100 %, every gameplay toggle on). */
	virtual void SetToDefaults() override;

	/** Writes the file; while a display change is pending, with the last confirmed window mode and resolution. */
	virtual void SaveSettings() override;

	/** Marks a window mode or resolution change as waiting for Keep (URROptionsSubsystem); SaveSettings then keeps the confirmed values. */
	void SetDisplayChangePending(bool bPending) { bDisplayChangePending = bPending; }

	/** True while a display change waits for Keep or Revert. */
	bool IsDisplayChangePending() const { return bDisplayChangePending; }

	/** Confirmed (saved) resolution. */
	FIntPoint GetConfirmedResolution() const { return FIntPoint(LastUserConfirmedResolutionSizeX, LastUserConfirmedResolutionSizeY); }

	/** Default window mode and resolution of the engine (Graphics reset). */
	static EWindowMode::Type GetEngineDefaultWindowMode() { return GetDefaultWindowMode(); }
	static FIntPoint GetEngineDefaultResolution() { return GetDefaultResolution(); }

	/** One line of every options value, for the log and the debug commands. */
	FString Describe() const;

	/** Saves and broadcasts OnSettingsChanged. */
	void CommitChange();

protected:
	/** Player toggle of the floating damage, heal and ore numbers (damage-numbers). */
	UPROPERTY(Config)
	bool bShowDamageNumbers = true;

	/** Player toggle of the aim laser (aim-indicator). */
	UPROPERTY(Config)
	bool bShowAimIndicator = true;

	/** Player toggle of every camera shake (URRCameraShakeLibrary). */
	UPROPERTY(Config)
	bool bScreenShake = true;

	/** Player toggle: the pause menu opens when the game window loses focus during play. */
	UPROPERTY(Config)
	bool bPauseOnFocusLoss = true;

	/** Volume groups, 0..1. */
	UPROPERTY(Config)
	float MasterVolume = 1.f;

	UPROPERTY(Config)
	float MusicVolume = 1.f;

	UPROPERTY(Config)
	float EffectsVolume = 1.f;

	/** True while a window mode or resolution change waits for Keep (not saved). */
	bool bDisplayChangePending = false;
};
