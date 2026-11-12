// Rogue Rigs - options runtime (options-screen gdd, task-500-03): volumes applied to the sound classes, window mode and
// resolution with the keep-or-revert countdown, pause on focus loss.

#include "Settings/RROptionsSubsystem.h"

#include "AudioDevice.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/RRPlayerController.h"
#include "RogueRigs.h"
#include "Settings/RRGameUserSettings.h"
#include "Settings/RROptionsSettings.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "UObject/UObjectGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RROptionsSubsystem)

URROptionsSubsystem* URROptionsSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject && GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URROptionsSubsystem>() : nullptr;
}

void URROptionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	VolumeMix = NewObject<USoundMix>(this, TEXT("RROptionsVolumeMix"), RF_Transient);
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		SettingsChangedHandle = Settings->OnSettingsChanged.AddUObject(this, &URROptionsSubsystem::ApplySoundVolumes);
		UE_LOG(LogRogueRigs, Log, TEXT("Options: loaded settings: %s."), *Settings->Describe());
	}
	MapLoadedHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URROptionsSubsystem::HandleMapLoaded);
	if (FSlateApplication::IsInitialized())
	{
		ActivationHandle = FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &URROptionsSubsystem::HandleActivationChanged);
	}
	ApplySoundVolumes();
}

void URROptionsSubsystem::Deinitialize()
{
	if (bPending)
	{
		// Leaving the game during the countdown: the file already holds the confirmed values; restore them in memory too.
		EndPending(false);
	}
	FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Settings->OnSettingsChanged.Remove(SettingsChangedHandle);
	}
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(MapLoadedHandle);
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().Remove(ActivationHandle);
	}
	Super::Deinitialize();
}

// ---- Sound ----

void URROptionsSubsystem::ApplySoundVolumes()
{
	const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings || !VolumeMix)
	{
		return;
	}
	const UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	FAudioDeviceHandle Device = World ? World->GetAudioDevice() : FAudioDeviceHandle();
	if (!Device.IsValid() && GEngine)
	{
		Device = GEngine->GetMainAudioDevice();
	}
	if (!Device.IsValid())
	{
		return;
	}

	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	const float Master = Settings->GetVolume(ERRVolumeGroup::Master);
	const TPair<const TSoftObjectPtr<USoundClass>*, float> Groups[] = {
		{&Options->MasterSoundClass, Master},
		{&Options->MusicSoundClass, Master * Settings->GetVolume(ERRVolumeGroup::Music)},
		{&Options->EffectsSoundClass, Master * Settings->GetVolume(ERRVolumeGroup::Effects)},
	};
	bool bAnyClass = false;
	for (const TPair<const TSoftObjectPtr<USoundClass>*, float>& Group : Groups)
	{
		if (USoundClass* SoundClass = Group.Key->LoadSynchronous())
		{
			// Children not touched: each group carries its own product with Master (Music and Effects are children of Master).
			HeldSoundClasses.AddUnique(SoundClass);
			Device->SetSoundMixClassOverride(VolumeMix, SoundClass, Group.Value, 1.f, 0.f, false);
			bAnyClass = true;
		}
		else if (!bWarnedMissingClass)
		{
			bWarnedMissingClass = true;
			UE_LOG(LogRogueRigs, Warning, TEXT("Options: sound class %s not found (URROptionsSettings); its volume is not applied."), *Group.Key->ToString());
		}
	}
	if (bAnyClass)
	{
		Device->PushSoundMixModifier(VolumeMix);
	}
}

void URROptionsSubsystem::HandleMapLoaded(UWorld* World)
{
	if (World && World->GetGameInstance() == GetGameInstance())
	{
		ApplySoundVolumes();
	}
}

// ---- Focus loss ----

void URROptionsSubsystem::HandleActivationChanged(bool bActive)
{
	if (bActive || !URRGameUserSettings::IsGameplayToggleOn(ERRGameplayToggle::PauseOnFocusLoss))
	{
		return;
	}
	ARRPlayerController* Controller = GetGameInstance() ? Cast<ARRPlayerController>(GetGameInstance()->GetFirstLocalPlayerController()) : nullptr;
	if (Controller && Controller->CanOpenPauseMenu())
	{
		Controller->OpenPauseMenu(TEXT("window focus lost"));
	}
}

// ---- Display ----

void URROptionsSubsystem::SetWindowMode(EWindowMode::Type Mode)
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings || Settings->GetFullscreenMode() == Mode)
	{
		return;
	}
	BeginDisplayChange();
	Settings->SetFullscreenMode(Mode);
	ApplyDisplayChange();
}

void URROptionsSubsystem::SetResolution(FIntPoint Resolution)
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings || Resolution.X <= 0 || Resolution.Y <= 0 || Settings->GetScreenResolution() == Resolution)
	{
		return;
	}
	BeginDisplayChange();
	Settings->SetScreenResolution(Resolution);
	ApplyDisplayChange();
}

void URROptionsSubsystem::ResetGraphicsToDefaults()
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings)
	{
		return;
	}
	Settings->ResetTabToDefaults(ERROptionsTab::Graphics);
	const EWindowMode::Type Mode = URRGameUserSettings::GetEngineDefaultWindowMode();
	const FIntPoint Resolution = URRGameUserSettings::GetEngineDefaultResolution();
	if (Settings->GetFullscreenMode() != Mode || Settings->GetScreenResolution() != Resolution)
	{
		BeginDisplayChange();
		Settings->SetFullscreenMode(Mode);
		Settings->SetScreenResolution(Resolution);
		ApplyDisplayChange();
	}
}

void URROptionsSubsystem::BeginDisplayChange()
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (Settings && !bPending)
	{
		// The state on screen before the first change is the one Revert restores.
		Settings->ConfirmVideoMode();
	}
}

void URROptionsSubsystem::ApplyDisplayChange()
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings)
	{
		return;
	}
	const bool bWasPending = bPending;
	bPending = true;
	SecondsLeft = FMath::Max(GetDefault<URROptionsSettings>()->DisplayConfirmSeconds, 1.f);
	Settings->SetDisplayChangePending(true);
	Settings->ApplyResolutionSettings(false);
	Settings->CommitChange();
	if (!TickerHandle.IsValid())
	{
		TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &URROptionsSubsystem::TickCountdown));
	}
	const FIntPoint Resolution = Settings->GetScreenResolution();
	UE_LOG(LogRogueRigs, Log, TEXT("Options: display change applied (window mode %d, %dx%d), keep or revert within %.0f s."),
		static_cast<int32>(Settings->GetFullscreenMode()), Resolution.X, Resolution.Y, SecondsLeft);
	if (!bWasPending)
	{
		OnDisplayConfirmChanged.Broadcast(true);
	}
}

void URROptionsSubsystem::KeepDisplay()
{
	if (bPending)
	{
		EndPending(true);
	}
}

void URROptionsSubsystem::RevertDisplay()
{
	if (bPending)
	{
		EndPending(false);
	}
}

void URROptionsSubsystem::EndPending(bool bKeep)
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
	TickerHandle.Reset();
	bPending = false;
	SecondsLeft = 0.f;
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (Settings)
	{
		Settings->SetDisplayChangePending(false);
		if (bKeep)
		{
			Settings->ConfirmVideoMode();
		}
		else
		{
			Settings->RevertVideoMode();
			Settings->ApplyResolutionSettings(false);
		}
		Settings->CommitChange();
		const FIntPoint Resolution = Settings->GetScreenResolution();
		UE_LOG(LogRogueRigs, Log, TEXT("Options: display %s (window mode %d, %dx%d)."), bKeep ? TEXT("kept") : TEXT("reverted"),
			static_cast<int32>(Settings->GetFullscreenMode()), Resolution.X, Resolution.Y);
	}
	OnDisplayConfirmChanged.Broadcast(false);
}

bool URROptionsSubsystem::TickCountdown(float DeltaTime)
{
	if (!bPending)
	{
		TickerHandle.Reset();
		return false;
	}
	SecondsLeft -= DeltaTime;
	if (SecondsLeft <= 0.f)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("Options: keep-or-revert countdown ended."));
		// EndPending removes this ticker; returning false is then harmless.
		EndPending(false);
		return false;
	}
	return true;
}

TArray<FIntPoint> URROptionsSubsystem::GetResolutionOptions() const
{
	TArray<FIntPoint> Resolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
	if (const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Resolutions.AddUnique(Settings->GetScreenResolution());
	}
	Resolutions.Sort([](const FIntPoint& A, const FIntPoint& B) { return A.X != B.X ? A.X < B.X : A.Y < B.Y; });
	return Resolutions;
}
