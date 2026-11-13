// Rogue Rigs - options debug console commands (task-500-03, compiled out of shipping): rr.Options.Set <name> <value> sets one
// options value through the same functions as the options screen (applied and saved); rr.Options.Dump logs every value;
// rr.Options.Keep / rr.Options.Revert answer the keep-or-revert dialog; rr.Options.Slide moves a volume slider as a drag (task-520).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Debug/RRCheats.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "RogueRigs.h"
#include "Settings/RRGameUserSettings.h"
#include "Settings/RROptionsSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Slider.h"
#include "Containers/Ticker.h"
#include "Kismet/GameplayStatics.h"
#include "Menu/RRMainMenuWidget.h"
#include "Player/RRPlayerController.h"
#include "UI/RROptionsWidget.h"
#include "UI/RRPauseMenuWidget.h"

namespace RROptionsDebug
{
	static bool ParseBool(const FString& Value)
	{
		return Value == TEXT("1") || Value.Equals(TEXT("on"), ESearchCase::IgnoreCase) || Value.Equals(TEXT("true"), ESearchCase::IgnoreCase);
	}

	static URROptionsSubsystem* FindSubsystem()
	{
		return URROptionsSubsystem::Get(RRCheats::FindCheatWorld());
	}

	/** Applies Name = Value; false on an unknown name or bad value. */
	static bool SetValue(URRGameUserSettings& Settings, const FString& Name, const FString& Value)
	{
		const FString Key = Name.ToLower();
		const float Number = FCString::Atof(*Value);
		static const TMap<FString, ERRVolumeGroup> Volumes = {{TEXT("master"), ERRVolumeGroup::Master}, {TEXT("music"), ERRVolumeGroup::Music}, {TEXT("effects"), ERRVolumeGroup::Effects}};
		static const TMap<FString, ERRGameplayToggle> Toggles = {{TEXT("damagenumbers"), ERRGameplayToggle::DamageNumbers}, {TEXT("aimindicator"), ERRGameplayToggle::AimIndicator},
			{TEXT("screenshake"), ERRGameplayToggle::ScreenShake}, {TEXT("pauseonfocusloss"), ERRGameplayToggle::PauseOnFocusLoss}};
		static const TMap<FString, ERRQualitySetting> Qualities = {{TEXT("shadows"), ERRQualitySetting::Shadows}, {TEXT("effectsquality"), ERRQualitySetting::Effects},
			{TEXT("antialiasing"), ERRQualitySetting::AntiAliasing}, {TEXT("postprocessing"), ERRQualitySetting::PostProcessing}, {TEXT("textures"), ERRQualitySetting::Textures}};
		static const TMap<FString, ERROptionsTab> Tabs = {{TEXT("sound"), ERROptionsTab::Sound}, {TEXT("graphics"), ERROptionsTab::Graphics}, {TEXT("gameplay"), ERROptionsTab::Gameplay}};

		if (const ERRVolumeGroup* Group = Volumes.Find(Key)) { Settings.SetVolume(*Group, Number / 100.f); return true; }
		if (const ERRGameplayToggle* Toggle = Toggles.Find(Key)) { Settings.SetGameplayToggle(*Toggle, ParseBool(Value)); return true; }
		if (const ERRQualitySetting* Quality = Qualities.Find(Key)) { Settings.SetQualitySetting(*Quality, FCString::Atoi(*Value)); return true; }
		if (Key == TEXT("vsync")) { Settings.SetVSyncAndApply(ParseBool(Value)); return true; }
		if (Key == TEXT("framelimit")) { Settings.SetFrameRateLimitAndApply(Number); return true; }
		if (Key == TEXT("scale")) { Settings.SetResolutionScalePercent(Number); return true; }
		if (Key == TEXT("preset")) { Settings.SetQualityPreset(static_cast<ERRQualityPreset>(FMath::Clamp(FCString::Atoi(*Value), 0, 3))); return true; }
		if (Key == TEXT("reset"))
		{
			const ERROptionsTab* Tab = Tabs.Find(Value.ToLower());
			if (!Tab) { return false; }
			URROptionsSubsystem* Options = FindSubsystem();
			if (*Tab == ERROptionsTab::Graphics && Options) { Options->ResetGraphicsToDefaults(); }
			else { Settings.ResetTabToDefaults(*Tab); }
			return true;
		}
		if (Key == TEXT("windowmode") || Key == TEXT("resolution"))
		{
			URROptionsSubsystem* Options = FindSubsystem();
			if (!Options) { return false; }
			if (Key == TEXT("windowmode")) { Options->SetWindowMode(EWindowMode::ConvertIntToWindowMode(FMath::Clamp(FCString::Atoi(*Value), 0, 2))); return true; }
			FString W, H;
			if (!Value.Split(TEXT("x"), &W, &H)) { return false; }
			Options->SetResolution(FIntPoint(FCString::Atoi(*W), FCString::Atoi(*H)));
			return true;
		}
		return false;
	}

	FAutoConsoleCommand SetCommand(
		TEXT("rr.Options.Set"),
		TEXT("rr.Options.Set <name> <value>: master|music|effects 0-100, windowmode 0-2 (Fullscreen, Windowed fullscreen, Windowed), resolution WxH, vsync 0|1, framelimit fps (0 = Unlimited), scale 50-100, preset 0-3, shadows|effectsquality|antialiasing|postprocessing|textures 0-3, damagenumbers|aimindicator|screenshake|pauseonfocusloss 0|1, reset sound|graphics|gameplay. Applied and saved as from the options screen."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
			if (!Settings || Args.Num() < 2)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Options.Set: usage rr.Options.Set <name> <value> (settings %s)."), Settings ? TEXT("found") : TEXT("missing"));
				return;
			}
			const bool bOk = SetValue(*Settings, Args[0], Args[1]);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Options.Set %s %s: %s. %s"), *Args[0], *Args[1], bOk ? TEXT("applied and saved") : TEXT("refused (unknown name or value)"), *Settings->Describe());
		}),
		ECVF_Cheat);

	FAutoConsoleCommand OpenCommand(
		TEXT("rr.Options.Open"),
		TEXT("rr.Options.Open [sound|graphics|gameplay] [dialog]: opens the options screen on a tab, over the main menu or over the pause menu (opened first during a run); 'dialog' also starts the keep-or-revert countdown on the current display values. 'close' closes it (Back)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			UWorld* World = RRCheats::FindCheatWorld();
			if (!World)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Options.Open: no game world."));
				return;
			}
			const FString TabArg = Args.Num() > 0 ? Args[0].ToLower() : FString(TEXT("sound"));
			if (TabArg == TEXT("close"))
			{
				if (URROptionsWidget* Open = URROptionsWidget::FindOpen(World)) { Open->Back(); }
				return;
			}
			const ERROptionsTab Tab = TabArg == TEXT("graphics") ? ERROptionsTab::Graphics : TabArg == TEXT("gameplay") ? ERROptionsTab::Gameplay : ERROptionsTab::Sound;
			UUserWidget* Menu = nullptr;
			if (ARRPlayerController* Controller = Cast<ARRPlayerController>(UGameplayStatics::GetPlayerController(World, 0)))
			{
				if (!Controller->IsPauseMenuOpen()) { Controller->OpenPauseMenu(TEXT("rr.Options.Open")); }
				Menu = Controller->GetPauseMenuWidget();
			}
			else
			{
				TArray<UUserWidget*> Found;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, Found, URRMainMenuWidget::StaticClass(), true);
				Menu = Found.Num() > 0 ? Found[0] : nullptr;
			}
			URROptionsWidget* Screen = Menu ? URROptionsWidget::OpenOptions(Menu, Tab) : nullptr;
			if (Screen && Args.Num() > 1 && Args[1].Equals(TEXT("dialog"), ESearchCase::IgnoreCase))
			{
				if (URROptionsSubsystem* Options = FindSubsystem()) { Options->StartDisplayConfirmOnCurrent(); }
			}
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Options.Open %s: %s."), *TabArg, Screen ? TEXT("open") : TEXT("refused (no main or pause menu, or no options class)"));
		}),
		ECVF_Cheat);

	/** Slide state of rr.Options.Slide (one at a time). */
	static FTSTicker::FDelegateHandle SlideHandle;

	FAutoConsoleCommand SlideCommand(
		TEXT("rr.Options.Slide"),
		TEXT("rr.Options.Slide <master|music|effects> <from 0-100> <to 0-100> <seconds>: moves that slider of the open options screen from one value to the other over real time, one value per frame, through the slider's change event as a mouse drag does (task-520 checks)."),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			UWorld* World = RRCheats::FindCheatWorld();
			URROptionsWidget* Screen = World ? URROptionsWidget::FindOpen(World) : nullptr;
			if (Args.Num() < 4 || Screen == nullptr)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Options.Slide: usage rr.Options.Slide <master|music|effects> <from> <to> <seconds>, options screen open (%s)."), Screen ? TEXT("open") : TEXT("not open"));
				return;
			}
			const FString Key = Args[0].Left(1).ToUpper() + Args[0].Mid(1).ToLower();
			const float From = FMath::Clamp(FCString::Atof(*Args[1]) / 100.f, 0.f, 1.f);
			const float To = FMath::Clamp(FCString::Atof(*Args[2]) / 100.f, 0.f, 1.f);
			const double Seconds = FMath::Max(FCString::Atod(*Args[3]), 0.01);
			TWeakObjectPtr<USlider> Slider = Cast<USlider>(Screen->GetWidgetFromName(FName(*(Key + TEXT("Slider")))));
			if (!Slider.IsValid())
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("rr.Options.Slide: no %sSlider on the options screen."), *Key);
				return;
			}
			FTSTicker::RemoveTicker(SlideHandle);
			const double Start = FPlatformTime::Seconds();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Options.Slide %s: %.0f %% to %.0f %% over %.2f s."), *Key, From * 100.f, To * 100.f, Seconds);
			SlideHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Slider, Key, From, To, Seconds, Start](float)
			{
				USlider* Target = Slider.Get();
				if (Target == nullptr)
				{
					return false;
				}
				const double Alpha = FMath::Clamp((FPlatformTime::Seconds() - Start) / Seconds, 0.0, 1.0);
				const float Value = FMath::Lerp(From, To, static_cast<float>(Alpha));
				Target->SetValue(Value);
				Target->OnValueChanged.Broadcast(Value);
				if (Alpha >= 1.0)
				{
					UE_LOG(LogRogueRigs, Log, TEXT("rr.Options.Slide %s: done at %.0f %%."), *Key, Value * 100.f);
					return false;
				}
				return true;
			}));
		}),
		ECVF_Cheat);

	FAutoConsoleCommand DumpCommand(
		TEXT("rr.Options.Dump"),
		TEXT("rr.Options.Dump: logs every options value (memory) and the saved file path."),
		FConsoleCommandDelegate::CreateLambda([]()
		{
			const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Options.Dump: %s; file %s."), Settings ? *Settings->Describe() : TEXT("no URRGameUserSettings"), *GGameUserSettingsIni);
		}),
		ECVF_Cheat);

	FAutoConsoleCommand KeepCommand(
		TEXT("rr.Options.Keep"),
		TEXT("rr.Options.Keep: Keep on the keep-or-revert dialog."),
		FConsoleCommandDelegate::CreateLambda([]() { if (URROptionsSubsystem* Options = FindSubsystem()) { Options->KeepDisplay(); } }),
		ECVF_Cheat);

	FAutoConsoleCommand RevertCommand(
		TEXT("rr.Options.Revert"),
		TEXT("rr.Options.Revert: Revert on the keep-or-revert dialog."),
		FConsoleCommandDelegate::CreateLambda([]() { if (URROptionsSubsystem* Options = FindSubsystem()) { Options->RevertDisplay(); } }),
		ECVF_Cheat);
}

#endif
