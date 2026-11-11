// Rogue Rigs - player settings saved in GameUserSettings.ini: damage number toggle, Sound and Graphics options (options-menu).

#include "Settings/RRGameUserSettings.h"

#include "AudioDevice.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "UnrealEngine.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RogueRigs.h"
#include "Scalability.h"
#include "Settings/RROptionsSettings.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/SWindow.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRGameUserSettings)

namespace RRSettingsPrivate
{
	/** Selector order of the window modes. */
	const EWindowMode::Type WindowModeOrder[] = {EWindowMode::Fullscreen, EWindowMode::WindowedFullscreen, EWindowMode::Windowed};

	int32 Wrap(int32 Index, int32 Count)
	{
		return Count > 0 ? ((Index % Count) + Count) % Count : 0;
	}

	int32 ReadCVarInt(const TCHAR* Name)
	{
		const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name);
		return CVar ? CVar->GetInt() : -1;
	}

	float ReadCVarFloat(const TCHAR* Name)
	{
		const IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name);
		return CVar ? CVar->GetFloat() : -1.f;
	}

	/** Device id the volume mix was pushed on (pushed once per audio device; mixes survive map loads). */
	Audio::FDeviceId PushedMixDevice = INDEX_NONE;
}

URRGameUserSettings* URRGameUserSettings::GetRRGameUserSettings()
{
	return GEngine ? Cast<URRGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void URRGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	bShowDamageNumbers = true;
	MasterVolume = MusicVolume = EffectsVolume = 1.f;
}

void URRGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	if (!HasAnyFlags(RF_ClassDefaultObject) && !PostLoadMapHandle.IsValid())
	{
		PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &URRGameUserSettings::HandlePostLoadMap);
	}
}

void URRGameUserSettings::BeginDestroy()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	PostLoadMapHandle.Reset();
	if (DisplayConfirmTicker.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(DisplayConfirmTicker);
		DisplayConfirmTicker.Reset();
	}
	Super::BeginDestroy();
}

void URRGameUserSettings::SaveSettings()
{
	if (!bDisplayConfirmPending)
	{
		Super::SaveSettings();
		return;
	}
	// An unconfirmed mode must never be the one found at the next launch: write the previous one, keep the current in memory.
	const EWindowMode::Type CurrentMode = GetFullscreenMode();
	const FIntPoint CurrentResolution = GetScreenResolution();
	FullscreenMode = PreviousWindowMode;
	ResolutionSizeX = PreviousResolution.X;
	ResolutionSizeY = PreviousResolution.Y;
	Super::SaveSettings();
	FullscreenMode = CurrentMode;
	ResolutionSizeX = CurrentResolution.X;
	ResolutionSizeY = CurrentResolution.Y;
}

void URRGameUserSettings::HandlePostLoadMap(UWorld* World)
{
	ApplyVolumes();
}

// ---- Sound ------------------------------------------------------------------------------------------------------------

float URRGameUserSettings::GetVolume(ERRVolumeGroup Group) const
{
	switch (Group)
	{
	case ERRVolumeGroup::Music: return MusicVolume;
	case ERRVolumeGroup::Effects: return EffectsVolume;
	default: return MasterVolume;
	}
}

void URRGameUserSettings::SetVolume(ERRVolumeGroup Group, float Volume)
{
	const float Step = FMath::Max(GetDefault<URROptionsSettings>()->VolumeStep, KINDA_SMALL_NUMBER);
	const float Snapped = FMath::Clamp(FMath::RoundToFloat(Volume / Step) * Step, 0.f, 1.f);
	switch (Group)
	{
	case ERRVolumeGroup::Music: MusicVolume = Snapped; break;
	case ERRVolumeGroup::Effects: EffectsVolume = Snapped; break;
	default: MasterVolume = Snapped; break;
	}
	ApplyVolumes();
	SaveSettings();
}

void URRGameUserSettings::ResetSoundToDefaults()
{
	MasterVolume = MusicVolume = EffectsVolume = 1.f;
	ApplyVolumes();
	SaveSettings();
}

void URRGameUserSettings::ApplyVolumes()
{
	FAudioDevice* Device = GEngine ? GEngine->GetMainAudioDeviceRaw() : nullptr;
	if (!Device)
	{
		return;
	}
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	USoundMix* Mix = Cast<USoundMix>(Options->VolumeSoundMix.TryLoad());
	USoundClass* Master = Cast<USoundClass>(Options->MasterSoundClass.TryLoad());
	USoundClass* Music = Cast<USoundClass>(Options->MusicSoundClass.TryLoad());
	USoundClass* Effects = Cast<USoundClass>(Options->EffectsSoundClass.TryLoad());
	if (!Mix || !Master || !Music || !Effects)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Settings volumes: not applied, a volume asset is missing (URROptionsSettings)."));
		return;
	}
	// Master scales its children (Music, Effects are children of SC_Master): effective Music = Master x Music.
	Device->SetSoundMixClassOverride(Mix, Master, MasterVolume, 1.f, 0.f, true);
	Device->SetSoundMixClassOverride(Mix, Music, MusicVolume, 1.f, 0.f, false);
	Device->SetSoundMixClassOverride(Mix, Effects, EffectsVolume, 1.f, 0.f, false);
	if (RRSettingsPrivate::PushedMixDevice != Device->DeviceID)
	{
		Device->PushSoundMixModifier(Mix);
		RRSettingsPrivate::PushedMixDevice = Device->DeviceID;
	}
}

// ---- Graphics: display ------------------------------------------------------------------------------------------------

void URRGameUserSettings::SyncDisplayFromWindow()
{
	if (bDisplayConfirmPending || !GEngine || !GEngine->GameViewport || GSystemResolution.ResX <= 0 || GSystemResolution.ResY <= 0)
	{
		return;
	}
	FullscreenMode = GSystemResolution.WindowMode;
	if (FullscreenMode != EWindowMode::WindowedFullscreen)
	{
		ResolutionSizeX = GSystemResolution.ResX;
		ResolutionSizeY = GSystemResolution.ResY;
	}
}

void URRGameUserSettings::RequestWindowMode(TEnumAsByte<EWindowMode::Type> NewMode)
{
	SyncDisplayFromWindow();
	if (NewMode == GetFullscreenMode())
	{
		return;
	}
	BeginDisplayChange();
	SetFullscreenMode(NewMode);
	const FIntPoint Desktop = GetDesktopResolution();
	if (NewMode == EWindowMode::WindowedFullscreen)
	{
		SetScreenResolution(Desktop);
	}
	else
	{
		// Keep the resolution when the new mode offers it, else take the largest it offers.
		const TArray<FIntPoint> Options = GetResolutionOptions();
		if (Options.Num() > 0 && !Options.Contains(GetScreenResolution()))
		{
			SetScreenResolution(Options.Last());
		}
	}
	ApplyDisplay();
}

void URRGameUserSettings::StepWindowMode(int32 Direction)
{
	using namespace RRSettingsPrivate;
	int32 Index = 1;
	for (int32 i = 0; i < UE_ARRAY_COUNT(WindowModeOrder); ++i)
	{
		if (WindowModeOrder[i] == GetFullscreenMode())
		{
			Index = i;
		}
	}
	RequestWindowMode(WindowModeOrder[Wrap(Index + (Direction >= 0 ? 1 : -1), UE_ARRAY_COUNT(WindowModeOrder))]);
}

TArray<FIntPoint> URRGameUserSettings::GetResolutionOptions() const
{
	TArray<FIntPoint> Result;
	if (GetFullscreenMode() == EWindowMode::Fullscreen)
	{
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(Result);
	}
	else
	{
		UKismetSystemLibrary::GetConvenientWindowedResolutions(Result);
	}
	Result.Sort([](const FIntPoint& A, const FIntPoint& B) { return A.X != B.X ? A.X < B.X : A.Y < B.Y; });
	return Result;
}

bool URRGameUserSettings::IsResolutionSelectable() const
{
	return GetFullscreenMode() != EWindowMode::WindowedFullscreen;
}

void URRGameUserSettings::RequestResolution(FIntPoint NewResolution)
{
	SyncDisplayFromWindow();
	if (NewResolution == GetScreenResolution() || NewResolution.X <= 0 || NewResolution.Y <= 0)
	{
		return;
	}
	BeginDisplayChange();
	SetScreenResolution(NewResolution);
	ApplyDisplay();
}

void URRGameUserSettings::StepResolution(int32 Direction)
{
	if (!IsResolutionSelectable())
	{
		return;
	}
	const TArray<FIntPoint> Options = GetResolutionOptions();
	if (Options.Num() == 0)
	{
		return;
	}
	// Index of the current size, or of the nearest one by pixel count.
	const FIntPoint Current = GetScreenResolution();
	int32 Index = Options.IndexOfByKey(Current);
	if (Index == INDEX_NONE)
	{
		Index = 0;
		int64 BestDelta = TNumericLimits<int64>::Max();
		for (int32 i = 0; i < Options.Num(); ++i)
		{
			const int64 Delta = FMath::Abs(int64(Options[i].X) * Options[i].Y - int64(Current.X) * Current.Y);
			if (Delta < BestDelta)
			{
				BestDelta = Delta;
				Index = i;
			}
		}
	}
	RequestResolution(Options[FMath::Clamp(Index + (Direction >= 0 ? 1 : -1), 0, Options.Num() - 1)]);
}

FIntPoint URRGameUserSettings::GetShownResolution() const
{
	return GetFullscreenMode() == EWindowMode::WindowedFullscreen ? GetDesktopResolution() : GetScreenResolution();
}

void URRGameUserSettings::BeginDisplayChange()
{
	if (!bDisplayConfirmPending)
	{
		PreviousWindowMode = GetFullscreenMode();
		PreviousResolution = GetScreenResolution();
	}
	bDisplayConfirmPending = true;
	DisplayConfirmEndTime = FPlatformTime::Seconds() + FMath::Max(1.f, GetDefault<URROptionsSettings>()->DisplayConfirmSeconds);
	if (!DisplayConfirmTicker.IsValid())
	{
		DisplayConfirmTicker = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateUObject(this, &URRGameUserSettings::TickDisplayConfirm));
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Settings display: change pending confirmation (revert target %s %dx%d)."),
		*GetWindowModeText(PreviousWindowMode).ToString(), PreviousResolution.X, PreviousResolution.Y);
	OnDisplayConfirmChanged.Broadcast(true);
}

void URRGameUserSettings::ApplyDisplay()
{
	ApplyResolutionSettings(false);
	SaveSettings();
}

int32 URRGameUserSettings::GetDisplayConfirmSecondsLeft() const
{
	return bDisplayConfirmPending ? FMath::Max(0, FMath::CeilToInt(DisplayConfirmEndTime - FPlatformTime::Seconds())) : 0;
}

bool URRGameUserSettings::TickDisplayConfirm(float DeltaTime)
{
	if (!bDisplayConfirmPending)
	{
		DisplayConfirmTicker.Reset();
		return false;
	}
	if (FPlatformTime::Seconds() >= DisplayConfirmEndTime)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("Settings display: countdown ended, reverting."));
		DisplayConfirmTicker.Reset();
		RevertDisplayChange();
		return false;
	}
	return true;
}

void URRGameUserSettings::ConfirmDisplayChange()
{
	if (!bDisplayConfirmPending)
	{
		return;
	}
	ConfirmVideoMode();
	UE_LOG(LogRogueRigs, Log, TEXT("Settings display: kept %s %dx%d."), *GetWindowModeText(GetFullscreenMode()).ToString(),
		GetScreenResolution().X, GetScreenResolution().Y);
	EndDisplayConfirm();
}

void URRGameUserSettings::RevertDisplayChange()
{
	if (!bDisplayConfirmPending)
	{
		return;
	}
	SetFullscreenMode(PreviousWindowMode);
	SetScreenResolution(PreviousResolution);
	ApplyResolutionSettings(false);
	UE_LOG(LogRogueRigs, Log, TEXT("Settings display: reverted to %s %dx%d."), *GetWindowModeText(GetFullscreenMode()).ToString(),
		GetScreenResolution().X, GetScreenResolution().Y);
	EndDisplayConfirm();
}

void URRGameUserSettings::EndDisplayConfirm()
{
	bDisplayConfirmPending = false;
	if (DisplayConfirmTicker.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(DisplayConfirmTicker);
		DisplayConfirmTicker.Reset();
	}
	SaveSettings();
	OnDisplayConfirmChanged.Broadcast(false);
}

// ---- Graphics: applied at once ----------------------------------------------------------------------------------------

void URRGameUserSettings::ApplyAndSaveNonDisplay()
{
	ApplyNonResolutionSettings();
	SaveSettings();
}

void URRGameUserSettings::SetVSyncOn(bool bOn)
{
	SetVSyncEnabled(bOn);
	ApplyAndSaveNonDisplay();
}

void URRGameUserSettings::SetFrameRateLimitValue(int32 Limit)
{
	SetFrameRateLimit(float(FMath::Max(0, Limit)));
	ApplyAndSaveNonDisplay();
}

void URRGameUserSettings::StepFrameRateLimit(int32 Direction)
{
	const TArray<int32>& Options = GetDefault<URROptionsSettings>()->FrameRateLimitOptions;
	if (Options.Num() == 0)
	{
		return;
	}
	int32 Index = Options.IndexOfByKey(GetFrameRateLimitValue());
	Index = Index == INDEX_NONE ? Options.Num() - 1 : RRSettingsPrivate::Wrap(Index + (Direction >= 0 ? 1 : -1), Options.Num());
	SetFrameRateLimitValue(Options[Index]);
}

int32 URRGameUserSettings::GetGraphicsDetail(ERRGraphicsDetail Detail) const
{
	switch (Detail)
	{
	case ERRGraphicsDetail::Effects: return ScalabilityQuality.EffectsQuality;
	case ERRGraphicsDetail::AntiAliasing: return ScalabilityQuality.AntiAliasingQuality;
	case ERRGraphicsDetail::PostProcessing: return ScalabilityQuality.PostProcessQuality;
	case ERRGraphicsDetail::Textures: return ScalabilityQuality.TextureQuality;
	default: return ScalabilityQuality.ShadowQuality;
	}
}

void URRGameUserSettings::SetGraphicsDetail(ERRGraphicsDetail Detail, int32 Level)
{
	const int32 Clamped = FMath::Clamp(Level, 0, 3);
	switch (Detail)
	{
	case ERRGraphicsDetail::Effects: ScalabilityQuality.EffectsQuality = Clamped; break;
	case ERRGraphicsDetail::AntiAliasing: ScalabilityQuality.AntiAliasingQuality = Clamped; break;
	case ERRGraphicsDetail::PostProcessing: ScalabilityQuality.PostProcessQuality = Clamped; break;
	case ERRGraphicsDetail::Textures: ScalabilityQuality.TextureQuality = Clamped; break;
	default: ScalabilityQuality.ShadowQuality = Clamped; break;
	}
	ApplyAndSaveNonDisplay();
}

void URRGameUserSettings::StepGraphicsDetail(ERRGraphicsDetail Detail, int32 Direction)
{
	SetGraphicsDetail(Detail, RRSettingsPrivate::Wrap(GetGraphicsDetail(Detail) + (Direction >= 0 ? 1 : -1), 4));
}

int32 URRGameUserSettings::GetQualityPreset() const
{
	const int32 Level = GetGraphicsDetail(ERRGraphicsDetail::Shadows);
	for (const ERRGraphicsDetail Detail : {ERRGraphicsDetail::Effects, ERRGraphicsDetail::AntiAliasing,
		ERRGraphicsDetail::PostProcessing, ERRGraphicsDetail::Textures})
	{
		if (GetGraphicsDetail(Detail) != Level)
		{
			return -1;
		}
	}
	return Level;
}

void URRGameUserSettings::SetQualityPreset(int32 Level)
{
	const float ResolutionQuality = ScalabilityQuality.ResolutionQuality;
	SetOverallScalabilityLevel(FMath::Clamp(Level, 0, 3));
	ScalabilityQuality.ResolutionQuality = ResolutionQuality; // the preset does not move the resolution scale slider
	ApplyAndSaveNonDisplay();
}

void URRGameUserSettings::StepQualityPreset(int32 Direction)
{
	int32 Current = GetQualityPreset();
	if (Current < 0)
	{
		int32 Sum = 0;
		for (const ERRGraphicsDetail Detail : {ERRGraphicsDetail::Shadows, ERRGraphicsDetail::Effects,
			ERRGraphicsDetail::AntiAliasing, ERRGraphicsDetail::PostProcessing, ERRGraphicsDetail::Textures})
		{
			Sum += GetGraphicsDetail(Detail);
		}
		SetQualityPreset(FMath::RoundToInt(Sum / 5.f));
		return;
	}
	SetQualityPreset(RRSettingsPrivate::Wrap(Current + (Direction >= 0 ? 1 : -1), 4));
}

float URRGameUserSettings::GetResolutionScalePercent() const
{
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	// 0 (or less) is the engine's "default screen percentage", shown as the full 100 %.
	const float Stored = ScalabilityQuality.ResolutionQuality <= 0.f ? 100.f : ScalabilityQuality.ResolutionQuality;
	return FMath::Clamp(Stored, Options->MinResolutionScalePercent, Options->MaxResolutionScalePercent);
}

void URRGameUserSettings::SetResolutionScalePercent(float Percent)
{
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	const float Step = FMath::Max(Options->ResolutionScaleStepPercent, 1.f);
	const float Snapped = FMath::Clamp(FMath::RoundToFloat(Percent / Step) * Step, Options->MinResolutionScalePercent,
		Options->MaxResolutionScalePercent);
	SetResolutionScaleValueEx(Snapped);
	ApplyAndSaveNonDisplay();
}

void URRGameUserSettings::ResetGraphicsToDefaults()
{
	SetVSyncEnabled(false);
	SetFrameRateLimit(0.f);
	SetOverallScalabilityLevel(3);
	SetResolutionScaleValueEx(100.f);
	ApplyNonResolutionSettings();
	SyncDisplayFromWindow();
	const FIntPoint Desktop = GetDesktopResolution();
	if (GetFullscreenMode() != EWindowMode::WindowedFullscreen || GetScreenResolution() != Desktop)
	{
		BeginDisplayChange();
		SetFullscreenMode(EWindowMode::WindowedFullscreen);
		SetScreenResolution(Desktop);
		ApplyDisplay();
	}
	else
	{
		SaveSettings();
	}
}

// ---- Texts ------------------------------------------------------------------------------------------------------------

FText URRGameUserSettings::GetWindowModeText(TEnumAsByte<EWindowMode::Type> Mode)
{
	switch (Mode)
	{
	case EWindowMode::Fullscreen: return FText::FromString(TEXT("Fullscreen"));
	case EWindowMode::Windowed: return FText::FromString(TEXT("Windowed"));
	default: return FText::FromString(TEXT("Windowed fullscreen"));
	}
}

FText URRGameUserSettings::GetResolutionText(FIntPoint Resolution)
{
	return FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
}

FText URRGameUserSettings::GetFrameRateLimitText(int32 Limit)
{
	return Limit <= 0 ? FText::FromString(TEXT("Unlimited")) : FText::AsNumber(Limit, &FNumberFormattingOptions::DefaultNoGrouping());
}

FText URRGameUserSettings::GetQualityLevelText(int32 Level)
{
	switch (Level)
	{
	case 0: return FText::FromString(TEXT("Low"));
	case 1: return FText::FromString(TEXT("Medium"));
	case 2: return FText::FromString(TEXT("High"));
	case 3: return FText::FromString(TEXT("Epic"));
	default: return FText::FromString(TEXT("Custom"));
	}
}

FText URRGameUserSettings::GetOnOffText(bool bOn)
{
	return FText::FromString(bOn ? TEXT("On") : TEXT("Off"));
}

FText URRGameUserSettings::GetPercentText(float Value, bool bAlreadyPercent)
{
	return FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(bAlreadyPercent ? Value : Value * 100.f)));
}

FString URRGameUserSettings::DescribeSettings() const
{
	using namespace RRSettingsPrivate;
	const Scalability::FQualityLevels Engine = Scalability::GetQualityLevels();
	FString WindowRead = TEXT("no viewport");
	if (GEngine && GEngine->GameViewport)
	{
		const TSharedPtr<SWindow> Window = GEngine->GameViewport->GetWindow();
		const FVector2D Size = Window.IsValid() ? Window->GetSizeInScreen() : FVector2D::ZeroVector;
		WindowRead = FString::Printf(TEXT("%s %dx%d"), Window.IsValid() ? *GetWindowModeText(Window->GetWindowMode()).ToString() : TEXT("?"),
			FMath::RoundToInt(Size.X), FMath::RoundToInt(Size.Y));
	}
	return FString::Printf(
		TEXT("stored: master=%.2f music=%.2f effects=%.2f mode=%s res=%dx%d vsync=%d fps=%d preset=%s shadows=%d effects=%d aa=%d pp=%d textures=%d scale=%.0f pending=%d | ")
		TEXT("engine: window=%s r.VSync=%d t.MaxFPS=%.0f sg.Shadow=%d sg.Effects=%d sg.AA=%d sg.PP=%d sg.Texture=%d sg.Resolution=%.0f r.ScreenPercentage=%.0f"),
		MasterVolume, MusicVolume, EffectsVolume, *GetWindowModeText(GetFullscreenMode()).ToString(), GetScreenResolution().X,
		GetScreenResolution().Y, IsVSyncEnabled() ? 1 : 0, GetFrameRateLimitValue(), *GetQualityLevelText(GetQualityPreset()).ToString(),
		GetGraphicsDetail(ERRGraphicsDetail::Shadows), GetGraphicsDetail(ERRGraphicsDetail::Effects),
		GetGraphicsDetail(ERRGraphicsDetail::AntiAliasing), GetGraphicsDetail(ERRGraphicsDetail::PostProcessing),
		GetGraphicsDetail(ERRGraphicsDetail::Textures), GetResolutionScalePercent(), bDisplayConfirmPending ? 1 : 0,
		*WindowRead, ReadCVarInt(TEXT("r.VSync")), ReadCVarFloat(TEXT("t.MaxFPS")), Engine.ShadowQuality, Engine.EffectsQuality,
		Engine.AntiAliasingQuality, Engine.PostProcessQuality, Engine.TextureQuality, Engine.ResolutionQuality,
		ReadCVarFloat(TEXT("r.ScreenPercentage")));
}
