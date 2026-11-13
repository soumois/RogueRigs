// Rogue Rigs - C++ base of the options screen (WBP_Options, task-500-04): Sound, Graphics and Gameplay tabs bound to
// URRGameUserSettings / URROptionsSubsystem, Reset to defaults, Back and Escape, keep-or-revert display dialog.

#include "UI/RROptionsWidget.h"

#include "Audio/RRAudioSubsystem.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "RogueRigs.h"
#include "Settings/RROptionsSettings.h"
#include "Settings/RROptionsSubsystem.h"
#include "UI/RRButtonHover.h"

TWeakObjectPtr<URROptionsWidget> URROptionsWidget::OpenScreen;

namespace RROptionsPreview
{
	/** Step of the Effects volume now (VolumeStep steps), or INDEX_NONE without settings. */
	static int32 CurrentEffectsStep()
	{
		const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
		const float Step = FMath::Max(GetDefault<URROptionsSettings>()->VolumeStep, KINDA_SMALL_NUMBER);
		return Settings ? FMath::RoundToInt(Settings->GetVolume(ERRVolumeGroup::Effects) / Step) : INDEX_NONE;
	}
}

namespace RROptionsText
{
	static const TCHAR* const VolumeKeys[] = {TEXT("Master"), TEXT("Music"), TEXT("Effects")};
	static const TCHAR* const QualityKeys[] = {TEXT("Shadows"), TEXT("EffectsQuality"), TEXT("AntiAliasing"), TEXT("PostProcessing"), TEXT("Textures")};
	static const TCHAR* const ToggleKeys[] = {TEXT("DamageNumbers"), TEXT("AimIndicator"), TEXT("ScreenShake"), TEXT("PauseOnFocusLoss")};

	static FText Percent(float Percent) { return FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Percent))); }

	static FText QualityName(int32 Level)
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

	static FText WindowModeName(EWindowMode::Type Mode)
	{
		switch (Mode)
		{
		case EWindowMode::Fullscreen: return FText::FromString(TEXT("Fullscreen"));
		case EWindowMode::WindowedFullscreen: return FText::FromString(TEXT("Windowed fullscreen"));
		default: return FText::FromString(TEXT("Windowed"));
		}
	}

	static FText FrameLimitName(int32 Limit) { return Limit <= 0 ? FText::FromString(TEXT("Unlimited")) : FText::AsNumber(Limit); }

	static int32 QualityIndex(const FString& Key)
	{
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(QualityKeys); ++Index)
		{
			if (Key == QualityKeys[Index]) { return Index; }
		}
		return INDEX_NONE;
	}

	static int32 ToggleIndex(const FString& Key)
	{
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(ToggleKeys); ++Index)
		{
			if (Key == ToggleKeys[Index]) { return Index; }
		}
		return INDEX_NONE;
	}
}

// ---- Proxy ----

void URROptionsControlProxy::HandleClicked()
{
	if (URROptionsWidget* Widget = Screen.Get())
	{
		Widget->StepSelector(Key, Direction);
	}
}

void URROptionsControlProxy::HandleValueChanged(float Value)
{
	if (URROptionsWidget* Widget = Screen.Get())
	{
		Widget->OnSliderChanged(Key, Value);
	}
}

void URROptionsControlProxy::HandleCheckChanged(bool bIsChecked)
{
	if (URROptionsWidget* Widget = Screen.Get())
	{
		Widget->OnToggleChanged(Key, bIsChecked);
	}
}

// ---- Opening and closing ----

bool URROptionsWidget::IsOptionsAvailable()
{
	return GetDefault<URROptionsSettings>()->OptionsWidgetClass.LoadSynchronous() != nullptr;
}

URROptionsWidget* URROptionsWidget::FindOpen(const UWorld* World)
{
	URROptionsWidget* Screen = OpenScreen.Get();
	return Screen && (!World || Screen->GetWorld() == World) && Screen->IsInViewport() ? Screen : nullptr;
}

URROptionsWidget* URROptionsWidget::OpenOptions(UUserWidget* FromMenu, ERROptionsTab Tab)
{
	if (!FromMenu || !FromMenu->GetOwningPlayer())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Options: refused (no menu or no owning player)."));
		return nullptr;
	}
	if (URROptionsWidget* Existing = FindOpen(FromMenu->GetWorld()))
	{
		Existing->ShowTab(Tab);
		return Existing;
	}
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	UClass* Class = Options->OptionsWidgetClass.LoadSynchronous();
	if (!Class)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Options: refused (URROptionsSettings::OptionsWidgetClass %s does not load)."), *Options->OptionsWidgetClass.ToString());
		return nullptr;
	}
	URROptionsWidget* Screen = CreateWidget<URROptionsWidget>(FromMenu->GetOwningPlayer(), Class);
	if (!Screen)
	{
		return nullptr;
	}
	Screen->FromMenu = FromMenu;
	Screen->FromMenuVisibility = FromMenu->GetVisibility();
	Screen->CurrentTab = Tab;
	// Menus live at Z 0 (main menu) to 40 (pause menu): the screen goes above both.
	const int32 ZOrder = 40 + Options->OptionsZOrderAbove;
	Screen->AddToViewport(ZOrder);
	FromMenu->SetVisibility(ESlateVisibility::Collapsed);
	OpenScreen = Screen;
	Screen->SetKeyboardFocus();
	UE_LOG(LogRogueRigs, Log, TEXT("Options: opened from %s on the %s tab."), *FromMenu->GetClass()->GetName(), *UEnum::GetDisplayValueAsText(Tab).ToString());
	return Screen;
}

void URROptionsWidget::Back()
{
	if (bClosing)
	{
		return;
	}
	bClosing = true;
	if (URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this); Subsystem && Subsystem->IsDisplayConfirmPending())
	{
		Subsystem->RevertDisplay();
	}
	RemoveFromParent();
	if (OpenScreen.Get() == this)
	{
		OpenScreen.Reset();
	}
	if (UUserWidget* Menu = FromMenu.Get())
	{
		Menu->SetVisibility(FromMenuVisibility);
		UWidget* FocusTarget = Menu->GetWidgetFromName(TEXT("OptionsButton"));
		(FocusTarget ? FocusTarget : static_cast<UWidget*>(Menu))->SetFocus();
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Options: closed (Back)."));
}

void URROptionsWidget::HandleEscape()
{
	URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this);
	if (Subsystem && Subsystem->IsDisplayConfirmPending())
	{
		Subsystem->RevertDisplay();
		return;
	}
	Back();
}

FReply URROptionsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Gamepad_Special_Right)
	{
		HandleEscape();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

// ---- Setup ----

void URROptionsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);
	BindControls();
	URRButtonHoverSubsystem::AttachNamed(this, {TEXT("ResetButton"), TEXT("BackButton"), TEXT("KeepButton"), TEXT("RevertButton")});
}

void URROptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		SettingsChangedHandle = Settings->OnSettingsChanged.AddUObject(this, &URROptionsWidget::RefreshFromSettings);
	}
	URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this);
	if (Subsystem)
	{
		Subsystem->OnDisplayConfirmChanged.AddUniqueDynamic(this, &URROptionsWidget::HandleDisplayConfirmChanged);
	}
	ShowTab(CurrentTab);
	RefreshFromSettings();
	EffectsPreviewStep = RROptionsPreview::CurrentEffectsStep();
	bEffectsPreviewPending = false;
	HandleDisplayConfirmChanged(Subsystem && Subsystem->IsDisplayConfirmPending());
}

void URROptionsWidget::NativeDestruct()
{
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Settings->OnSettingsChanged.Remove(SettingsChangedHandle);
	}
	if (URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this))
	{
		Subsystem->OnDisplayConfirmChanged.RemoveDynamic(this, &URROptionsWidget::HandleDisplayConfirmChanged);
	}
	Super::NativeDestruct();
}

void URROptionsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateCountdownText();
	UpdateEffectsPreview(/*bFromTick*/ true);
}

void URROptionsWidget::BindControls()
{
#define RR_BIND_CLICK(Name, Function) if (UButton* Button = Find<UButton>(TEXT(Name))) { Button->OnClicked.AddUniqueDynamic(this, &URROptionsWidget::Function); }
	RR_BIND_CLICK("SoundTabButton", HandleSoundTab)
	RR_BIND_CLICK("GraphicsTabButton", HandleGraphicsTab)
	RR_BIND_CLICK("GameplayTabButton", HandleGameplayTab)
	RR_BIND_CLICK("KeepButton", HandleKeep)
	RR_BIND_CLICK("RevertButton", HandleRevert)
	RR_BIND_CLICK("BackButton", HandleBackClicked)
	RR_BIND_CLICK("ResetButton", HandleResetClicked)
#undef RR_BIND_CLICK

	for (const TCHAR* Key : RROptionsText::VolumeKeys) { BindSlider(Key); }
	BindSlider(TEXT("ResolutionScale"));
	for (const TCHAR* Key : {TEXT("WindowMode"), TEXT("Resolution"), TEXT("FrameLimit"), TEXT("Preset")}) { BindSelector(Key); }
	for (const TCHAR* Key : RROptionsText::QualityKeys) { BindSelector(Key); }
	BindToggle(TEXT("VSync"));
	for (const TCHAR* Key : RROptionsText::ToggleKeys) { BindToggle(Key); }
}

void URROptionsWidget::BindSlider(const FString& Key)
{
	if (USlider* Slider = Find<USlider>(Key + TEXT("Slider")))
	{
		URROptionsControlProxy* Proxy = NewObject<URROptionsControlProxy>(this);
		Proxy->Screen = this;
		Proxy->Key = Key;
		Proxies.Add(Proxy);
		Slider->SetMinValue(0.f);
		Slider->SetMaxValue(1.f);
		Slider->SetStepSize(Key == TEXT("ResolutionScale") ? 0.1f : GetDefault<URROptionsSettings>()->VolumeStep);
		Slider->OnValueChanged.AddUniqueDynamic(Proxy, &URROptionsControlProxy::HandleValueChanged);
	}
}

void URROptionsWidget::BindSelector(const FString& Key)
{
	for (const int32 Direction : {-1, 1})
	{
		if (UButton* Button = Find<UButton>(Key + (Direction < 0 ? TEXT("Prev") : TEXT("Next"))))
		{
			URROptionsControlProxy* Proxy = NewObject<URROptionsControlProxy>(this);
			Proxy->Screen = this;
			Proxy->Key = Key;
			Proxy->Direction = Direction;
			Proxies.Add(Proxy);
			Button->OnClicked.AddUniqueDynamic(Proxy, &URROptionsControlProxy::HandleClicked);
		}
	}
}

void URROptionsWidget::BindToggle(const FString& Key)
{
	if (UCheckBox* CheckBox = Find<UCheckBox>(Key + TEXT("Toggle")))
	{
		URROptionsControlProxy* Proxy = NewObject<URROptionsControlProxy>(this);
		Proxy->Screen = this;
		Proxy->Key = Key;
		Proxies.Add(Proxy);
		CheckBox->OnCheckStateChanged.AddUniqueDynamic(Proxy, &URROptionsControlProxy::HandleCheckChanged);
	}
}

// ---- Tabs ----

void URROptionsWidget::ShowTab(ERROptionsTab Tab)
{
	CurrentTab = Tab;
	if (UWidgetSwitcher* Switcher = Find<UWidgetSwitcher>(TEXT("TabSwitcher")))
	{
		Switcher->SetActiveWidgetIndex(static_cast<int32>(Tab));
	}
	StyleTabButton(Find<UButton>(TEXT("SoundTabButton")), Find<UTextBlock>(TEXT("SoundTabText")), Tab == ERROptionsTab::Sound);
	StyleTabButton(Find<UButton>(TEXT("GraphicsTabButton")), Find<UTextBlock>(TEXT("GraphicsTabText")), Tab == ERROptionsTab::Graphics);
	StyleTabButton(Find<UButton>(TEXT("GameplayTabButton")), Find<UTextBlock>(TEXT("GameplayTabText")), Tab == ERROptionsTab::Gameplay);
	OnTabShown(Tab);
}

void URROptionsWidget::StyleTabButton(UButton* Button, UTextBlock* Label, bool bActive)
{
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	if (Button)
	{
		UTexture2D* Texture = (bActive ? Options->ActiveTabTexture : Options->InactiveTabTexture).LoadSynchronous();
		UTexture2D* SizeTexture = Options->InactiveTabTexture.LoadSynchronous();
		if (Texture)
		{
			FButtonStyle Style = Button->GetStyle();
			// Every tab, active or not, gets the same size: the plate height of the layout (the Box margins are drawn at ImageSize)
			// and the width of the inactive plate aspect, so a click never resizes a tab (task-521).
			const float Height = Style.Normal.ImageSize.Y > 1.f ? Style.Normal.ImageSize.Y : 77.f;
			const UTexture2D* AspectTexture = SizeTexture ? SizeTexture : Texture;
			const FVector2D Size(Height * AspectTexture->GetSizeX() / FMath::Max(AspectTexture->GetSizeY(), 1), Height);

			// The active texture has a transparent border: crop it through the UV region so its plate fills the whole tab, and
			// convert the 3-slice margin (fraction of the full texture) to a fraction of the cropped region.
			const float TextureWidth = static_cast<float>(FMath::Max(Texture->GetSizeX(), 1));
			const float TextureHeight = static_cast<float>(FMath::Max(Texture->GetSizeY(), 1));
			const FMargin Border = bActive ? Options->ActiveTabTransparentBorder : FMargin(0.f);
			const float CroppedWidth = FMath::Max(TextureWidth - Border.Left - Border.Right, 1.f);
			const float MarginFraction = bActive ? Options->ActiveTabMargin : Options->InactiveTabMargin;
			const float Margin = FMath::Clamp((MarginFraction * TextureWidth - Border.Left) / CroppedWidth, 0.f, 0.49f);
			const FBox2f UVRegion(FVector2f(Border.Left / TextureWidth, Border.Top / TextureHeight),
				FVector2f(1.f - Border.Right / TextureWidth, 1.f - Border.Bottom / TextureHeight));

			for (FSlateBrush* Brush : {&Style.Normal, &Style.Hovered, &Style.Pressed})
			{
				Brush->SetResourceObject(Texture);
				Brush->DrawAs = ESlateBrushDrawType::Box;
				Brush->Margin = FMargin(Margin, 0.f);
				Brush->ImageSize = Size;
				if (Border.GetTotalSpaceAlong<Orient_Horizontal>() > 0.f || Border.GetTotalSpaceAlong<Orient_Vertical>() > 0.f)
				{
					Brush->SetUVRegion(UVRegion);
				}
				else
				{
					Brush->SetUVRegion(FBox2f(ForceInit));
				}
			}
			Style.Normal.TintColor = FSlateColor(FLinearColor::White);
			Style.Hovered.TintColor = FSlateColor(FLinearColor(1.15f, 1.15f, 1.15f, 1.f));
			Style.Pressed.TintColor = FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.f));
			Button->SetStyle(Style);
		}
	}
	if (Label)
	{
		Label->SetColorAndOpacity(FSlateColor(bActive ? Options->ActiveTabTextColor : Options->InactiveTabTextColor));
	}
}

void URROptionsWidget::HandleSoundTab() { ShowTab(ERROptionsTab::Sound); }
void URROptionsWidget::HandleGraphicsTab() { ShowTab(ERROptionsTab::Graphics); }
void URROptionsWidget::HandleGameplayTab() { ShowTab(ERROptionsTab::Gameplay); }
void URROptionsWidget::HandleBackClicked() { Back(); }
void URROptionsWidget::HandleResetClicked() { ResetCurrentTab(); }

void URROptionsWidget::HandleKeep()
{
	if (URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this))
	{
		Subsystem->KeepDisplay();
	}
}

void URROptionsWidget::HandleRevert()
{
	if (URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this))
	{
		Subsystem->RevertDisplay();
	}
}

void URROptionsWidget::ResetCurrentTab()
{
	URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this);
	if (CurrentTab == ERROptionsTab::Graphics && Subsystem)
	{
		Subsystem->ResetGraphicsToDefaults();
	}
	else if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Settings->ResetTabToDefaults(CurrentTab);
	}
}

// ---- Values ----

void URROptionsWidget::RefreshFromSettings()
{
	const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings)
	{
		return;
	}
	TGuardValue<bool> RefreshGuard(bRefreshing, true);
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	const ERRVolumeGroup Groups[] = {ERRVolumeGroup::Master, ERRVolumeGroup::Music, ERRVolumeGroup::Effects};
	for (int32 Index = 0; Index < 3; ++Index)
	{
		const float Volume = Settings->GetVolume(Groups[Index]);
		SetSlider(RROptionsText::VolumeKeys[Index], Volume, RROptionsText::Percent(Volume * 100.f));
	}
	const float Scale = Settings->GetResolutionScalePercent();
	const float ScaleRange = FMath::Max(Options->MaxResolutionScalePercent - Options->MinResolutionScalePercent, 1.f);
	SetSlider(TEXT("ResolutionScale"), (Scale - Options->MinResolutionScalePercent) / ScaleRange, RROptionsText::Percent(Scale));

	SetSelectorText(TEXT("WindowMode"), RROptionsText::WindowModeName(Settings->GetFullscreenMode()));
	const FIntPoint Resolution = Settings->GetScreenResolution();
	SetSelectorText(TEXT("Resolution"), FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y)));
	SetSelectorText(TEXT("FrameLimit"), RROptionsText::FrameLimitName(FMath::RoundToInt(Settings->GetFrameRateLimit())));
	SetSelectorText(TEXT("Preset"), RROptionsText::QualityName(static_cast<int32>(Settings->GetQualityPreset())));
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(RROptionsText::QualityKeys); ++Index)
	{
		SetSelectorText(RROptionsText::QualityKeys[Index], RROptionsText::QualityName(Settings->GetQualitySetting(static_cast<ERRQualitySetting>(Index))));
	}
	SetToggle(TEXT("VSync"), Settings->IsVSyncEnabled());
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(RROptionsText::ToggleKeys); ++Index)
	{
		SetToggle(RROptionsText::ToggleKeys[Index], Settings->GetGameplayToggle(static_cast<ERRGameplayToggle>(Index)));
	}
}

void URROptionsWidget::SetSlider(const FString& Key, float Normalized, const FText& ValueText)
{
	const float Clamped = FMath::Clamp(Normalized, 0.f, 1.f);
	if (USlider* Slider = Find<USlider>(Key + TEXT("Slider")))
	{
		Slider->SetValue(Clamped);
	}
	if (UProgressBar* Fill = Find<UProgressBar>(Key + TEXT("Fill")))
	{
		Fill->SetPercent(Clamped);
	}
	if (UTextBlock* Text = Find<UTextBlock>(Key + TEXT("Value")))
	{
		Text->SetText(ValueText);
	}
}

void URROptionsWidget::SetSelectorText(const FString& Key, const FText& ValueText)
{
	if (UTextBlock* Text = Find<UTextBlock>(Key + TEXT("Value")))
	{
		Text->SetText(ValueText);
	}
}

void URROptionsWidget::SetToggle(const FString& Key, bool bOn)
{
	if (UCheckBox* CheckBox = Find<UCheckBox>(Key + TEXT("Toggle")))
	{
		CheckBox->SetIsChecked(bOn);
	}
	if (UTextBlock* State = Find<UTextBlock>(Key + TEXT("State")))
	{
		State->SetText(FText::FromString(bOn ? TEXT("ON") : TEXT("OFF")));
		if (UOverlaySlot* StateSlot = Cast<UOverlaySlot>(State->Slot))
		{
			StateSlot->SetHorizontalAlignment(bOn ? HAlign_Left : HAlign_Right);
		}
	}
	if (UWidget* Knob = GetWidgetFromName(FName(*(Key + TEXT("Knob")))))
	{
		if (UOverlaySlot* KnobSlot = Cast<UOverlaySlot>(Knob->Slot))
		{
			KnobSlot->SetHorizontalAlignment(bOn ? HAlign_Right : HAlign_Left);
		}
		else if (UHorizontalBoxSlot* BoxSlot = Cast<UHorizontalBoxSlot>(Knob->Slot))
		{
			BoxSlot->SetHorizontalAlignment(bOn ? HAlign_Right : HAlign_Left);
		}
	}
}

void URROptionsWidget::OnSliderChanged(const FString& Key, float Value)
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings || bRefreshing)
	{
		return;
	}
	if (Key == TEXT("ResolutionScale"))
	{
		const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
		Settings->SetResolutionScalePercent(FMath::Lerp(Options->MinResolutionScalePercent, Options->MaxResolutionScalePercent, Value));
		return;
	}
	for (int32 Index = 0; Index < 3; ++Index)
	{
		if (Key == RROptionsText::VolumeKeys[Index])
		{
			Settings->SetVolume(static_cast<ERRVolumeGroup>(Index), Value);
			if (static_cast<ERRVolumeGroup>(Index) == ERRVolumeGroup::Effects)
			{
				UpdateEffectsPreview(/*bFromTick*/ false);
			}
		}
	}
}

void URROptionsWidget::UpdateEffectsPreview(bool bFromTick)
{
	if (bFromTick && !bEffectsPreviewPending)
	{
		return;
	}
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	const int32 Step = RROptionsPreview::CurrentEffectsStep();
	if (Options->EffectsPreviewEvent.IsNone() || Step == INDEX_NONE || Step == EffectsPreviewStep)
	{
		bEffectsPreviewPending = false;
		return;
	}
	const double Now = FPlatformTime::Seconds();
	if (Now - EffectsPreviewSeconds < Options->EffectsPreviewInterval)
	{
		bEffectsPreviewPending = true;
		return;
	}
	// Played after SetVolume pushed the new class volume: the sample is heard at the level just set (Effects class, plays paused).
	bEffectsPreviewPending = false;
	EffectsPreviewStep = Step;
	EffectsPreviewSeconds = Now;
	URRAudioSubsystem::PlayEvent(this, Options->EffectsPreviewEvent);
}

void URROptionsWidget::OnToggleChanged(const FString& Key, bool bOn)
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings || bRefreshing)
	{
		return;
	}
	if (Key == TEXT("VSync"))
	{
		Settings->SetVSyncAndApply(bOn);
	}
	else if (const int32 Index = RROptionsText::ToggleIndex(Key); Index != INDEX_NONE)
	{
		Settings->SetGameplayToggle(static_cast<ERRGameplayToggle>(Index), bOn);
	}
}

void URROptionsWidget::StepSelector(const FString& Key, int32 Direction)
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this);
	if (!Settings)
	{
		return;
	}
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	auto Wrap = [](int32 Index, int32 Count) { return Count > 0 ? (Index % Count + Count) % Count : 0; };

	if (Key == TEXT("WindowMode") && Subsystem)
	{
		const int32 Mode = Wrap(static_cast<int32>(Settings->GetFullscreenMode()) + Direction, 3);
		Subsystem->SetWindowMode(EWindowMode::ConvertIntToWindowMode(Mode));
	}
	else if (Key == TEXT("Resolution") && Subsystem)
	{
		const TArray<FIntPoint> Resolutions = Subsystem->GetResolutionOptions();
		const int32 Current = Resolutions.IndexOfByKey(Settings->GetScreenResolution());
		if (Resolutions.Num() > 1)
		{
			Subsystem->SetResolution(Resolutions[Wrap(Current + Direction, Resolutions.Num())]);
		}
	}
	else if (Key == TEXT("FrameLimit"))
	{
		const TArray<int32>& Limits = Options->FrameRateLimits;
		const int32 Current = Limits.IndexOfByKey(FMath::RoundToInt(Settings->GetFrameRateLimit()));
		if (Limits.Num() > 0)
		{
			const int32 Next = Current == INDEX_NONE ? (Direction > 0 ? 0 : Limits.Num() - 1) : Wrap(Current + Direction, Limits.Num());
			Settings->SetFrameRateLimitAndApply(static_cast<float>(Limits[Next]));
		}
	}
	else if (Key == TEXT("Preset"))
	{
		const int32 Current = static_cast<int32>(Settings->GetQualityPreset());
		const int32 Next = Current > 3 ? (Direction > 0 ? 0 : 3) : Wrap(Current + Direction, 4);
		Settings->SetQualityPreset(static_cast<ERRQualityPreset>(Next));
	}
	else if (const int32 Index = RROptionsText::QualityIndex(Key); Index != INDEX_NONE)
	{
		const ERRQualitySetting Setting = static_cast<ERRQualitySetting>(Index);
		Settings->SetQualitySetting(Setting, Wrap(Settings->GetQualitySetting(Setting) + Direction, 4));
	}
}

// ---- Keep-or-revert dialog ----

void URROptionsWidget::HandleDisplayConfirmChanged(bool bPending)
{
	if (UWidget* Dialog = GetWidgetFromName(TEXT("ConfirmDialog")))
	{
		Dialog->SetVisibility(bPending ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	UpdateCountdownText();
	if (bPending)
	{
		if (UWidget* Keep = GetWidgetFromName(TEXT("KeepButton")))
		{
			Keep->SetKeyboardFocus();
		}
	}
	else if (!bClosing)
	{
		SetKeyboardFocus();
	}
	RefreshFromSettings();
}

void URROptionsWidget::UpdateCountdownText()
{
	const URROptionsSubsystem* Subsystem = URROptionsSubsystem::Get(this);
	if (!Subsystem || !Subsystem->IsDisplayConfirmPending())
	{
		return;
	}
	if (UTextBlock* Text = Find<UTextBlock>(TEXT("ConfirmCountdownText")))
	{
		Text->SetText(FText::FromString(FString::Printf(TEXT("Reverting in %d s"), FMath::CeilToInt(Subsystem->GetDisplayConfirmSecondsLeft()))));
	}
}
