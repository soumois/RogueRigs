// Rogue Rigs - options runtime (options-screen gdd, task-500-03): volumes applied to the sound classes, window mode and
// resolution with the keep-or-revert countdown, pause on focus loss.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "GenericPlatform/GenericWindowDefinition.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RROptionsSubsystem.generated.h"

class USoundClass;
class USoundMix;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRROnDisplayConfirmChanged, bool, bPending);

/**
 * Game instance subsystem behind the options screen (the values live in URRGameUserSettings).
 * - Sound: a transient sound mix holds one class override per volume group (URROptionsSettings sound classes: Master = Master,
 *   Music = Master x Music, Effects = Master x Effects, children not touched), pushed on the audio device at start, after each
 *   map load and after each settings change.
 * - Display: SetWindowMode / SetResolution apply at once and open a keep-or-revert window of URROptionsSettings::
 *   DisplayConfirmSeconds (real time, runs while paused); the file keeps the confirmed values until KeepDisplay; RevertDisplay
 *   or the end of the countdown restores them. OnDisplayConfirmChanged tells the widget to show or hide its dialog.
 * - Focus loss: when the application is deactivated and the PauseOnFocusLoss toggle is on, the first local ARRPlayerController
 *   opens its pause menu (as the pause key would; refused where it would be refused).
 */
UCLASS()
class ROGUERIGS_API URROptionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** The subsystem of WorldContextObject's game instance, or null. */
	static URROptionsSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Applies a window mode at once and starts (or restarts) the keep-or-revert countdown. Same mode: nothing. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetWindowMode(EWindowMode::Type Mode);

	/** Applies a resolution at once and starts (or restarts) the keep-or-revert countdown. Same resolution: nothing. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetResolution(FIntPoint Resolution);

	/** Graphics tab Reset to defaults: the other Graphics values at once, window mode and resolution through the countdown. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void ResetGraphicsToDefaults();

	/** Debug and capture (rr.Options.Open ... dialog): opens the keep-or-revert countdown on the current display values (reapplied unchanged). */
	void StartDisplayConfirmOnCurrent() { BeginDisplayChange(); ApplyDisplayChange(); }

	/** Keep: the pending window mode and resolution become the saved ones. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void KeepDisplay();

	/** Revert (button, Escape, end of countdown): restores and saves the confirmed window mode and resolution. */
	UFUNCTION(BlueprintCallable, Category = "Options")
	void RevertDisplay();

	/** True while a display change waits for Keep or Revert. */
	UFUNCTION(BlueprintPure, Category = "Options")
	bool IsDisplayConfirmPending() const { return bPending; }

	/** Seconds left on the keep-or-revert countdown (0 when none). */
	UFUNCTION(BlueprintPure, Category = "Options")
	float GetDisplayConfirmSecondsLeft() const { return bPending ? FMath::Max(SecondsLeft, 0.f) : 0.f; }

	/** Resolutions offered by the Resolution selector: the screen's fullscreen list, plus the current one, ascending. */
	UFUNCTION(BlueprintPure, Category = "Options")
	TArray<FIntPoint> GetResolutionOptions() const;

	/** Pushes the volumes of the settings on the audio device (also called on every settings change and map load). */
	void ApplySoundVolumes();

	/** Broadcast when the keep-or-revert dialog must show (true) or hide (false). */
	UPROPERTY(BlueprintAssignable, Category = "Options")
	FRROnDisplayConfirmChanged OnDisplayConfirmChanged;

private:
	/** Shared start of a window mode or resolution change: confirms the current state first when nothing is pending. */
	void BeginDisplayChange();
	void ApplyDisplayChange();
	void EndPending(bool bKeep);
	bool TickCountdown(float DeltaTime);
	void HandleMapLoaded(UWorld* World);
	void HandleActivationChanged(bool bActive);

	/** Transient mix holding the three class overrides. */
	UPROPERTY(Transient)
	TObjectPtr<USoundMix> VolumeMix;

	/** The three sound classes, held so they are never collected while the audio device keeps pointers to them in the mix. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<USoundClass>> HeldSoundClasses;

	bool bPending = false;
	float SecondsLeft = 0.f;
	bool bWarnedMissingClass = false;

	FTSTicker::FDelegateHandle TickerHandle;
	FDelegateHandle SettingsChangedHandle;
	FDelegateHandle MapLoadedHandle;
	FDelegateHandle ActivationHandle;
};
