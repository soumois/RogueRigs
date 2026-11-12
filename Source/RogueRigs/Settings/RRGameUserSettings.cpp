// Rogue Rigs - player settings saved in GameUserSettings.ini: options screen values (sound, gameplay toggles) on top of the
// engine's display and quality settings (options-screen gdd, task-500-03).

#include "Settings/RRGameUserSettings.h"

#include "Engine/Engine.h"
#include "RogueRigs.h"
#include "Settings/RROptionsSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGameUserSettings)

URRGameUserSettings* URRGameUserSettings::GetRRGameUserSettings()
{
	return GEngine ? Cast<URRGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void URRGameUserSettings::CommitChange()
{
	SaveSettings();
	OnSettingsChanged.Broadcast();
}

// ---- Gameplay toggles ----

void URRGameUserSettings::SetGameplayToggle(ERRGameplayToggle Toggle, bool bOn)
{
	switch (Toggle)
	{
	case ERRGameplayToggle::DamageNumbers: bShowDamageNumbers = bOn; break;
	case ERRGameplayToggle::AimIndicator: bShowAimIndicator = bOn; break;
	case ERRGameplayToggle::ScreenShake: bScreenShake = bOn; break;
	case ERRGameplayToggle::PauseOnFocusLoss: bPauseOnFocusLoss = bOn; break;
	}
	CommitChange();
}

bool URRGameUserSettings::GetGameplayToggle(ERRGameplayToggle Toggle) const
{
	switch (Toggle)
	{
	case ERRGameplayToggle::DamageNumbers: return bShowDamageNumbers;
	case ERRGameplayToggle::AimIndicator: return bShowAimIndicator;
	case ERRGameplayToggle::ScreenShake: return bScreenShake;
	case ERRGameplayToggle::PauseOnFocusLoss: return bPauseOnFocusLoss;
	}
	return true;
}

bool URRGameUserSettings::IsGameplayToggleOn(ERRGameplayToggle Toggle)
{
	const URRGameUserSettings* Settings = GetRRGameUserSettings();
	return Settings == nullptr || Settings->GetGameplayToggle(Toggle);
}

// ---- Sound ----

void URRGameUserSettings::SetVolume(ERRVolumeGroup Group, float Volume)
{
	const float Step = FMath::Max(GetDefault<URROptionsSettings>()->VolumeStep, 0.01f);
	const float Snapped = FMath::Clamp(FMath::RoundToFloat(Volume / Step) * Step, 0.f, 1.f);
	switch (Group)
	{
	case ERRVolumeGroup::Master: MasterVolume = Snapped; break;
	case ERRVolumeGroup::Music: MusicVolume = Snapped; break;
	case ERRVolumeGroup::Effects: EffectsVolume = Snapped; break;
	}
	CommitChange();
}

float URRGameUserSettings::GetVolume(ERRVolumeGroup Group) const
{
	switch (Group)
	{
	case ERRVolumeGroup::Master: return MasterVolume;
	case ERRVolumeGroup::Music: return MusicVolume;
	case ERRVolumeGroup::Effects: return EffectsVolume;
	}
	return 1.f;
}

// ---- Graphics ----

void URRGameUserSettings::SetVSyncAndApply(bool bEnable)
{
	SetVSyncEnabled(bEnable);
	ApplyNonResolutionSettings();
	CommitChange();
}

void URRGameUserSettings::SetFrameRateLimitAndApply(float Limit)
{
	SetFrameRateLimit(FMath::Max(Limit, 0.f));
	ApplyNonResolutionSettings();
	CommitChange();
}

void URRGameUserSettings::SetResolutionScalePercent(float Percent)
{
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	const float Step = FMath::Max(Options->ResolutionScaleStep, 1.f);
	const float Snapped = FMath::Clamp(FMath::RoundToFloat(Percent / Step) * Step, Options->MinResolutionScalePercent, Options->MaxResolutionScalePercent);
	SetResolutionScaleValueEx(Snapped);
	ApplyNonResolutionSettings();
	CommitChange();
}

float URRGameUserSettings::GetResolutionScalePercent() const
{
	float Normalized = 0.f, Value = 100.f, MinValue = 0.f, MaxValue = 100.f;
	GetResolutionScaleInformationEx(Normalized, Value, MinValue, MaxValue);
	// 0 = the engine default screen percentage (r.ScreenPercentage.Default, 100 % on desktop): shown as the top of the range.
	return Value > 0.f ? Value : GetDefault<URROptionsSettings>()->MaxResolutionScalePercent;
}

void URRGameUserSettings::SetQualityPreset(ERRQualityPreset Preset)
{
	if (Preset == ERRQualityPreset::Custom)
	{
		return;
	}
	// The engine preset sets every scalability group; the resolution scale is a separate option and is kept.
	const float ScalePercent = GetResolutionScalePercent();
	SetOverallScalabilityLevel(static_cast<int32>(Preset));
	SetResolutionScaleValueEx(ScalePercent);
	ApplyNonResolutionSettings();
	CommitChange();
}

ERRQualityPreset URRGameUserSettings::GetQualityPreset() const
{
	const int32 First = GetQualitySetting(ERRQualitySetting::Shadows);
	for (const ERRQualitySetting Setting : {ERRQualitySetting::Effects, ERRQualitySetting::AntiAliasing, ERRQualitySetting::PostProcessing, ERRQualitySetting::Textures})
	{
		if (GetQualitySetting(Setting) != First)
		{
			return ERRQualityPreset::Custom;
		}
	}
	return First >= 0 && First <= 3 ? static_cast<ERRQualityPreset>(First) : ERRQualityPreset::Custom;
}

void URRGameUserSettings::SetQualitySetting(ERRQualitySetting Setting, int32 Level)
{
	const int32 Clamped = FMath::Clamp(Level, 0, 3);
	switch (Setting)
	{
	case ERRQualitySetting::Shadows: SetShadowQuality(Clamped); break;
	case ERRQualitySetting::Effects: SetVisualEffectQuality(Clamped); break;
	case ERRQualitySetting::AntiAliasing: SetAntiAliasingQuality(Clamped); break;
	case ERRQualitySetting::PostProcessing: SetPostProcessingQuality(Clamped); break;
	case ERRQualitySetting::Textures: SetTextureQuality(Clamped); break;
	}
	ApplyNonResolutionSettings();
	CommitChange();
}

int32 URRGameUserSettings::GetQualitySetting(ERRQualitySetting Setting) const
{
	switch (Setting)
	{
	case ERRQualitySetting::Shadows: return ScalabilityQuality.ShadowQuality;
	case ERRQualitySetting::Effects: return ScalabilityQuality.EffectsQuality;
	case ERRQualitySetting::AntiAliasing: return ScalabilityQuality.AntiAliasingQuality;
	case ERRQualitySetting::PostProcessing: return ScalabilityQuality.PostProcessQuality;
	case ERRQualitySetting::Textures: return ScalabilityQuality.TextureQuality;
	}
	return 3;
}

// ---- Defaults and saving ----

void URRGameUserSettings::ResetTabToDefaults(ERROptionsTab Tab)
{
	switch (Tab)
	{
	case ERROptionsTab::Sound:
		MasterVolume = MusicVolume = EffectsVolume = 1.f;
		break;
	case ERROptionsTab::Gameplay:
		bShowDamageNumbers = bShowAimIndicator = bScreenShake = bPauseOnFocusLoss = true;
		break;
	case ERROptionsTab::Graphics:
		// Window mode and resolution: URROptionsSubsystem::ResetGraphicsToDefaults (keep-or-revert countdown) calls this first.
		SetVSyncEnabled(false);
		SetFrameRateLimit(0.f);
		SetOverallScalabilityLevel(3);
		SetResolutionScaleValueEx(100.f);
		ApplyNonResolutionSettings();
		break;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Options: %s tab reset to defaults."), *UEnum::GetDisplayValueAsText(Tab).ToString());
	CommitChange();
}

void URRGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	bShowDamageNumbers = bShowAimIndicator = bScreenShake = bPauseOnFocusLoss = true;
	MasterVolume = MusicVolume = EffectsVolume = 1.f;
}

void URRGameUserSettings::SaveSettings()
{
	if (!bDisplayChangePending)
	{
		Super::SaveSettings();
		return;
	}
	// A display change waits for Keep: the file keeps the confirmed window mode and resolution (options-screen, Saving).
	const int32 PendingMode = FullscreenMode;
	const uint32 PendingX = ResolutionSizeX;
	const uint32 PendingY = ResolutionSizeY;
	FullscreenMode = LastConfirmedFullscreenMode;
	ResolutionSizeX = LastUserConfirmedResolutionSizeX;
	ResolutionSizeY = LastUserConfirmedResolutionSizeY;
	Super::SaveSettings();
	FullscreenMode = PendingMode;
	ResolutionSizeX = PendingX;
	ResolutionSizeY = PendingY;
}

FString URRGameUserSettings::Describe() const
{
	const FIntPoint Resolution = GetScreenResolution();
	return FString::Printf(
		TEXT("Master %.0f%% Music %.0f%% Effects %.0f%% | WindowMode %d Resolution %dx%d (confirmed %d %dx%d%s) VSync %d FrameLimit %.0f Scale %.0f%% Preset %s Shadows %d Effects %d AA %d Post %d Textures %d | DamageNumbers %d AimIndicator %d ScreenShake %d PauseOnFocusLoss %d"),
		MasterVolume * 100.f, MusicVolume * 100.f, EffectsVolume * 100.f,
		static_cast<int32>(GetFullscreenMode()), Resolution.X, Resolution.Y,
		LastConfirmedFullscreenMode, LastUserConfirmedResolutionSizeX, LastUserConfirmedResolutionSizeY, bDisplayChangePending ? TEXT(", pending") : TEXT(""),
		IsVSyncEnabled() ? 1 : 0, GetFrameRateLimit(), GetResolutionScalePercent(),
		*UEnum::GetDisplayValueAsText(GetQualityPreset()).ToString(),
		GetQualitySetting(ERRQualitySetting::Shadows), GetQualitySetting(ERRQualitySetting::Effects), GetQualitySetting(ERRQualitySetting::AntiAliasing),
		GetQualitySetting(ERRQualitySetting::PostProcessing), GetQualitySetting(ERRQualitySetting::Textures),
		bShowDamageNumbers ? 1 : 0, bShowAimIndicator ? 1 : 0, bScreenShake ? 1 : 0, bPauseOnFocusLoss ? 1 : 0);
}
