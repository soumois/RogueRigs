// Rogue Rigs - settings debug console command (task-505-01, compiled out of shipping): rr.Settings dump|set|keep|revert|reset
// calls the same URRGameUserSettings functions as the options screen and logs the stored and engine values.

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "AudioDevice.h"
#include "AudioThread.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"
#include "RogueRigs.h"
#include "Settings/RRGameUserSettings.h"
#include "Settings/RROptionsSettings.h"
#include "Sound/SoundClass.h"

namespace RRSettingsDebug
{
	/** Logs the effective volume of the three sound classes as the audio device computed them (read on the audio thread). */
	void LogVolumeReadBack()
	{
		FAudioDevice* Device = GEngine ? GEngine->GetMainAudioDeviceRaw() : nullptr;
		const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
		USoundClass* Classes[3] = {Cast<USoundClass>(Options->MasterSoundClass.TryLoad()),
			Cast<USoundClass>(Options->MusicSoundClass.TryLoad()), Cast<USoundClass>(Options->EffectsSoundClass.TryLoad())};
		if (!Device || !Classes[0] || !Classes[1] || !Classes[2])
		{
			UE_LOG(LogRogueRigs, Log, TEXT("RRSETTINGS volume read-back: unavailable (audio device %d, classes %d %d %d)."),
				Device ? 1 : 0, Classes[0] ? 1 : 0, Classes[1] ? 1 : 0, Classes[2] ? 1 : 0);
			return;
		}
		FAudioThread::RunCommandOnAudioThread([Device, Master = Classes[0], Music = Classes[1], Effects = Classes[2]]()
		{
			auto Read = [Device](USoundClass* Class)
			{
				const FSoundClassProperties* Properties = Device->GetSoundClassCurrentProperties(Class);
				return Properties ? Properties->Volume : -1.f;
			};
			UE_LOG(LogRogueRigs, Log, TEXT("RRSETTINGS volume read-back (effective): master=%.2f music=%.2f effects=%.2f"),
				Read(Master), Read(Music), Read(Effects));
		});
	}

	FAutoConsoleCommand SettingsCommand(
		TEXT("rr.Settings"),
		TEXT("rr.Settings dump | set <master|music|effects|mode|res|vsync|fps|preset|shadows|fx|aa|pp|textures|scale> <value> | keep | revert | reset <sound|graphics>: ")
		TEXT("calls the options screen functions (mode 0 Fullscreen, 1 Windowed fullscreen, 2 Windowed; res WxH; volumes 0..1) and logs a RRSETTINGS line."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
			if (!Settings)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Settings: no URRGameUserSettings."));
				return;
			}
			const FString Action = Args.Num() > 0 ? Args[0].ToLower() : TEXT("dump");
			if (Action == TEXT("set") && Args.Num() >= 3)
			{
				const FString Key = Args[1].ToLower();
				const FString& Value = Args[2];
				const int32 IntValue = FCString::Atoi(*Value);
				const float FloatValue = FCString::Atof(*Value);
				if (Key == TEXT("master")) { Settings->SetVolume(ERRVolumeGroup::Master, FloatValue); }
				else if (Key == TEXT("music")) { Settings->SetVolume(ERRVolumeGroup::Music, FloatValue); }
				else if (Key == TEXT("effects")) { Settings->SetVolume(ERRVolumeGroup::Effects, FloatValue); }
				else if (Key == TEXT("mode")) { Settings->RequestWindowMode(EWindowMode::ConvertIntToWindowMode(IntValue)); }
				else if (Key == TEXT("res"))
				{
					FString X, Y;
					if (Value.Split(TEXT("x"), &X, &Y, ESearchCase::IgnoreCase))
					{
						Settings->RequestResolution(FIntPoint(FCString::Atoi(*X), FCString::Atoi(*Y)));
					}
				}
				else if (Key == TEXT("vsync")) { Settings->SetVSyncOn(IntValue != 0); }
				else if (Key == TEXT("fps")) { Settings->SetFrameRateLimitValue(IntValue); }
				else if (Key == TEXT("preset")) { Settings->SetQualityPreset(IntValue); }
				else if (Key == TEXT("shadows")) { Settings->SetGraphicsDetail(ERRGraphicsDetail::Shadows, IntValue); }
				else if (Key == TEXT("fx")) { Settings->SetGraphicsDetail(ERRGraphicsDetail::Effects, IntValue); }
				else if (Key == TEXT("aa")) { Settings->SetGraphicsDetail(ERRGraphicsDetail::AntiAliasing, IntValue); }
				else if (Key == TEXT("pp")) { Settings->SetGraphicsDetail(ERRGraphicsDetail::PostProcessing, IntValue); }
				else if (Key == TEXT("textures")) { Settings->SetGraphicsDetail(ERRGraphicsDetail::Textures, IntValue); }
				else if (Key == TEXT("scale")) { Settings->SetResolutionScalePercent(FloatValue); }
				else
				{
					UE_LOG(LogRogueRigs, Warning, TEXT("rr.Settings set: unknown key '%s'."), *Key);
					return;
				}
			}
			else if (Action == TEXT("keep")) { Settings->ConfirmDisplayChange(); }
			else if (Action == TEXT("revert")) { Settings->RevertDisplayChange(); }
			else if (Action == TEXT("reset") && Args.Num() >= 2)
			{
				if (Args[1].ToLower() == TEXT("sound")) { Settings->ResetSoundToDefaults(); }
				else { Settings->ResetGraphicsToDefaults(); }
			}
			else if (Action != TEXT("dump"))
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Settings: usage rr.Settings dump|set <key> <value>|keep|revert|reset sound|graphics."));
				return;
			}
			UE_LOG(LogRogueRigs, Log, TEXT("RRSETTINGS %s: %s"), *FString::Join(Args, TEXT(" ")), *Settings->DescribeSettings());
			LogVolumeReadBack();
		}));
}

#endif // !UE_BUILD_SHIPPING
