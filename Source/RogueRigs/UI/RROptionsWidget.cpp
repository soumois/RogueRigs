// Rogue Rigs - C++ base of the options screen (WBP_Options, task-505-02).

#include "UI/RROptionsWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/Slider.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "RogueRigs.h"
#include "Settings/RRGameUserSettings.h"
#include "Settings/RROptionsSettings.h"
#include "Styling/CoreStyle.h"
#include "UI/RRButtonHover.h"

namespace RROptionsWidget
{
	bool IsSliderRow(ERROptionRow Row)
	{
		return Row == ERROptionRow::MasterVolume || Row == ERROptionRow::MusicVolume || Row == ERROptionRow::EffectsVolume
			|| Row == ERROptionRow::ResolutionScale;
	}

	bool IsVolumeRow(ERROptionRow Row)
	{
		return Row == ERROptionRow::MasterVolume || Row == ERROptionRow::MusicVolume || Row == ERROptionRow::EffectsVolume;
	}

	ERRVolumeGroup ToVolumeGroup(ERROptionRow Row)
	{
		return Row == ERROptionRow::MusicVolume ? ERRVolumeGroup::Music
			: Row == ERROptionRow::EffectsVolume ? ERRVolumeGroup::Effects : ERRVolumeGroup::Master;
	}

	bool ToDetail(ERROptionRow Row, ERRGraphicsDetail& OutDetail)
	{
		switch (Row)
		{
		case ERROptionRow::Shadows: OutDetail = ERRGraphicsDetail::Shadows; return true;
		case ERROptionRow::Effects: OutDetail = ERRGraphicsDetail::Effects; return true;
		case ERROptionRow::AntiAliasing: OutDetail = ERRGraphicsDetail::AntiAliasing; return true;
		case ERROptionRow::PostProcessing: OutDetail = ERRGraphicsDetail::PostProcessing; return true;
		case ERROptionRow::Textures: OutDetail = ERRGraphicsDetail::Textures; return true;
		default: return false;
		}
	}

	FSlateBrush MakeImageBrush(UTexture2D* Texture, const FLinearColor& Tint = FLinearColor::White)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(Texture);
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Brush.TintColor = FSlateColor(Tint);
		if (Texture)
		{
			Brush.ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
		}
		return Brush;
	}

	FSlateBrush MakeBoxBrush(UTexture2D* Texture, const FMargin& Margin, const FLinearColor& Tint = FLinearColor::White)
	{
		FSlateBrush Brush = MakeImageBrush(Texture, Tint);
		Brush.DrawAs = ESlateBrushDrawType::Box;
		Brush.Margin = Margin;
		return Brush;
	}

	FButtonStyle MakeButtonStyle(const FSlateBrush& Normal)
	{
		FButtonStyle Style;
		Style.SetNormal(Normal);
		Style.SetHovered(Normal);
		FSlateBrush Pressed = Normal;
		Pressed.TintColor = FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.f));
		Style.SetPressed(Pressed);
		FSlateBrush Disabled = Normal;
		Disabled.TintColor = FSlateColor(FLinearColor(0.4f, 0.4f, 0.4f, 0.6f));
		Style.SetDisabled(Disabled);
		Style.SetNormalPadding(FMargin(0.f));
		Style.SetPressedPadding(FMargin(0.f));
		return Style;
	}

	float TextureAspect(const UTexture2D* Texture)
	{
		return Texture && Texture->GetSizeY() > 0 ? float(Texture->GetSizeX()) / float(Texture->GetSizeY()) : 1.f;
	}
}

URROptionsWidget::URROptionsWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ArrowTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/Options/T_Options_ArrowButton.T_Options_ArrowButton")));
	SliderTrackTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/Options/T_Options_SliderTrack.T_Options_SliderTrack")));
	SliderKnobTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/Options/T_Options_SliderKnob.T_Options_SliderKnob")));
	ToggleTrackTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/Options/T_Options_ToggleTrack.T_Options_ToggleTrack")));
	ToggleThumbTexture = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/Options/T_Options_ToggleThumb.T_Options_ToggleThumb")));
	RowFont = FCoreStyle::GetDefaultFontStyle("Bold", 24);
}

void URROptionsRowHandler::HandleClicked()
{
	if (Owner)
	{
		Owner->StepRow(Row, Direction);
	}
}

void URROptionsRowHandler::HandleValueChanged(float Value)
{
	if (Owner)
	{
		Owner->SetRowSliderValue(Row, Value);
	}
}

FText URROptionsWidget::GetRowLabel(ERROptionRow Row)
{
	switch (Row)
	{
	case ERROptionRow::MasterVolume: return NSLOCTEXT("RogueRigs", "OptMaster", "Master");
	case ERROptionRow::MusicVolume: return NSLOCTEXT("RogueRigs", "OptMusic", "Music");
	case ERROptionRow::EffectsVolume: return NSLOCTEXT("RogueRigs", "OptEffects", "Effects");
	case ERROptionRow::WindowMode: return NSLOCTEXT("RogueRigs", "OptWindowMode", "Window mode");
	case ERROptionRow::Resolution: return NSLOCTEXT("RogueRigs", "OptResolution", "Resolution");
	case ERROptionRow::VSync: return NSLOCTEXT("RogueRigs", "OptVSync", "VSync");
	case ERROptionRow::FrameRateLimit: return NSLOCTEXT("RogueRigs", "OptFrameLimit", "Frame rate limit");
	case ERROptionRow::ResolutionScale: return NSLOCTEXT("RogueRigs", "OptResScale", "Resolution scale");
	case ERROptionRow::QualityPreset: return NSLOCTEXT("RogueRigs", "OptPreset", "Quality preset");
	case ERROptionRow::Shadows: return NSLOCTEXT("RogueRigs", "OptShadows", "Shadows");
	case ERROptionRow::Effects: return NSLOCTEXT("RogueRigs", "OptFx", "Effects");
	case ERROptionRow::AntiAliasing: return NSLOCTEXT("RogueRigs", "OptAA", "Anti-aliasing");
	case ERROptionRow::PostProcessing: return NSLOCTEXT("RogueRigs", "OptPost", "Post-processing");
	case ERROptionRow::Textures: return NSLOCTEXT("RogueRigs", "OptTextures", "Textures");
	}
	return FText::GetEmpty();
}

void URROptionsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetIsFocusable(true);

	LoadedArrow = ArrowTexture.LoadSynchronous();
	LoadedTrack = SliderTrackTexture.LoadSynchronous();
	LoadedKnob = SliderKnobTexture.LoadSynchronous();
	LoadedToggleTrack = ToggleTrackTexture.LoadSynchronous();
	LoadedThumb = ToggleThumbTexture.LoadSynchronous();

	if (SoundTabButton) { SoundTabButton->OnClicked.AddUniqueDynamic(this, &URROptionsWidget::HandleSoundTabClicked); }
	if (GraphicsTabButton) { GraphicsTabButton->OnClicked.AddUniqueDynamic(this, &URROptionsWidget::HandleGraphicsTabClicked); }
	if (ResetButton) { ResetButton->OnClicked.AddUniqueDynamic(this, &URROptionsWidget::HandleResetClicked); }
	if (BackButton) { BackButton->OnClicked.AddUniqueDynamic(this, &URROptionsWidget::HandleBackClicked); }
	if (KeepButton) { KeepButton->OnClicked.AddUniqueDynamic(this, &URROptionsWidget::HandleKeepClicked); }
	if (RevertButton) { RevertButton->OnClicked.AddUniqueDynamic(this, &URROptionsWidget::HandleRevertClicked); }

	// Plate hover of ui-button-hover: a tab grows with its selected plate and label.
	URRButtonHoverSubsystem::Attach(SoundTabButton, true, SoundTab);
	URRButtonHoverSubsystem::Attach(GraphicsTabButton, true, GraphicsTab);
	URRButtonHoverSubsystem::AttachNamed(this, {TEXT("ResetButton"), TEXT("BackButton"), TEXT("KeepButton"), TEXT("RevertButton")});

	if (!IsDesignTime())
	{
		BuildRows();
	}
}

void URROptionsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bClosing = false;
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Settings->SyncDisplayFromWindow();
		Settings->OnDisplayConfirmChanged.AddUniqueDynamic(this, &URROptionsWidget::HandleDisplayConfirmChanged);
		HandleDisplayConfirmChanged(Settings->IsDisplayConfirmPending());
	}
	else
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Options screen: no URRGameUserSettings; rows show no values."));
		if (ConfirmPanel) { ConfirmPanel->SetVisibility(ESlateVisibility::Collapsed); }
	}
	ShowTab(ERROptionsTab::Sound);
	FocusFirstTab();
	UE_LOG(LogRogueRigs, Log, TEXT("Options screen opened (%d rows)."), Rows.Num());
}

void URROptionsWidget::NativeDestruct()
{
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Settings->OnDisplayConfirmChanged.RemoveDynamic(this, &URROptionsWidget::HandleDisplayConfirmChanged);
	}
	Super::NativeDestruct();
}

void URROptionsWidget::BuildRows()
{
	Rows.Reset();
	Handlers.Reset();
	if (SoundList)
	{
		SoundList->ClearChildren();
		AddRow(SoundList, ERROptionRow::MasterVolume);
		AddRow(SoundList, ERROptionRow::MusicVolume);
		AddRow(SoundList, ERROptionRow::EffectsVolume);
		// Same row height as the Graphics list: 11 rows and the divider.
		USpacer* Spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), TEXT("SoundListSpacer"));
		UVerticalBoxSlot* SpacerSlot = SoundList->AddChildToVerticalBox(Spacer);
		FSlateChildSize SpacerSize(ESlateSizeRule::Fill);
		SpacerSize.Value = 8.f + DividerRowWeight;
		SpacerSlot->SetSize(SpacerSize);
	}
	if (GraphicsList)
	{
		GraphicsList->ClearChildren();
		AddRow(GraphicsList, ERROptionRow::WindowMode);
		AddRow(GraphicsList, ERROptionRow::Resolution);
		AddRow(GraphicsList, ERROptionRow::VSync);
		AddRow(GraphicsList, ERROptionRow::FrameRateLimit);
		AddRow(GraphicsList, ERROptionRow::ResolutionScale);
		AddDivider(GraphicsList);
		AddRow(GraphicsList, ERROptionRow::QualityPreset);
		AddRow(GraphicsList, ERROptionRow::Shadows);
		AddRow(GraphicsList, ERROptionRow::Effects);
		AddRow(GraphicsList, ERROptionRow::AntiAliasing);
		AddRow(GraphicsList, ERROptionRow::PostProcessing);
		AddRow(GraphicsList, ERROptionRow::Textures);
	}
	AppliedRowHeight = 0.f;
	ApplyRowHeight(40.f);
	AppliedRowHeight = 0.f;
}

URROptionsRowHandler* URROptionsWidget::MakeHandler(ERROptionRow Row, int32 Direction)
{
	URROptionsRowHandler* Handler = NewObject<URROptionsRowHandler>(this);
	Handler->Owner = this;
	Handler->Row = Row;
	Handler->Direction = Direction;
	Handlers.Add(Handler);
	return Handler;
}

UButton* URROptionsWidget::MakeArrow(UCanvasPanel* Canvas, ERROptionRow Row, int32 Direction, float AnchorX, USizeBox*& OutBox)
{
	const FString RowName = StaticEnum<ERROptionRow>()->GetNameStringByValue(int64(Row));
	USizeBox* Box = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(),
		*FString::Printf(TEXT("%s%sBox"), *RowName, Direction < 0 ? TEXT("Left") : TEXT("Right")));
	UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(),
		*FString::Printf(TEXT("%s%sButton"), *RowName, Direction < 0 ? TEXT("Left") : TEXT("Right")));
	Button->SetStyle(RROptionsWidget::MakeButtonStyle(RROptionsWidget::MakeImageBrush(LoadedArrow)));
	Box->AddChild(Button);
	if (Direction > 0)
	{
		// The sprite points left; the right arrow is its mirror.
		Box->SetRenderScale(FVector2D(-1.f, 1.f));
	}
	UCanvasPanelSlot* BoxSlot = Canvas->AddChildToCanvas(Box);
	BoxSlot->SetAnchors(FAnchors(AnchorX, 0.5f));
	BoxSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	BoxSlot->SetAutoSize(true);
	BoxSlot->SetPosition(FVector2D::ZeroVector);
	Button->OnClicked.AddUniqueDynamic(MakeHandler(Row, Direction), &URROptionsRowHandler::HandleClicked);
	URRButtonHoverSubsystem::Attach(Button);
	OutBox = Box;
	return Button;
}

void URROptionsWidget::AddRow(UVerticalBox* List, ERROptionRow Row)
{
	const FString RowName = StaticEnum<ERROptionRow>()->GetNameStringByValue(int64(Row));
	FRROptionsRowWidgets Widgets;
	Widgets.Row = Row;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), *(RowName + TEXT("Row")));
	Widgets.Canvas = Canvas;
	UVerticalBoxSlot* RowSlot = List->AddChildToVerticalBox(Canvas);
	RowSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	RowSlot->SetHorizontalAlignment(HAlign_Fill);
	RowSlot->SetVerticalAlignment(VAlign_Fill);

	UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(RowName + TEXT("Label")));
	Label->SetText(GetRowLabel(Row));
	Label->SetFont(RowFont);
	Label->SetColorAndOpacity(FSlateColor(LabelColour));
	UCanvasPanelSlot* LabelSlot = Canvas->AddChildToCanvas(Label);
	LabelSlot->SetAnchors(FAnchors(0.f, 0.5f));
	LabelSlot->SetAlignment(FVector2D(0.f, 0.5f));
	LabelSlot->SetAutoSize(true);
	LabelSlot->SetPosition(FVector2D::ZeroVector);

	UTextBlock* Value = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(RowName + TEXT("Value")));
	Value->SetFont(RowFont);
	Value->SetColorAndOpacity(FSlateColor(ValueColour));
	Widgets.Value = Value;

	if (Row == ERROptionRow::VSync)
	{
		Value->SetVisibility(ESlateVisibility::Collapsed);
		Canvas->AddChildToCanvas(Value);
		UButton* Toggle = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *(RowName + TEXT("Toggle")));
		Toggle->SetStyle(RROptionsWidget::MakeButtonStyle(RROptionsWidget::MakeBoxBrush(LoadedToggleTrack, ToggleTrackMargin)));
		UOverlay* Content = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), *(RowName + TEXT("ToggleContent")));
		UButtonSlot* ContentSlot = Cast<UButtonSlot>(Toggle->AddChild(Content));
		ContentSlot->SetPadding(FMargin(0.f));
		ContentSlot->SetHorizontalAlignment(HAlign_Fill);
		ContentSlot->SetVerticalAlignment(VAlign_Fill);
		UTextBlock* ToggleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *(RowName + TEXT("ToggleText")));
		ToggleText->SetFont(RowFont);
		ToggleText->SetColorAndOpacity(FSlateColor(ValueColour));
		Content->AddChildToOverlay(ToggleText)->SetVerticalAlignment(VAlign_Center);
		USizeBox* ThumbBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), *(RowName + TEXT("ThumbBox")));
		UImage* Thumb = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), *(RowName + TEXT("Thumb")));
		Thumb->SetBrush(RROptionsWidget::MakeImageBrush(LoadedThumb));
		ThumbBox->AddChild(Thumb);
		Content->AddChildToOverlay(ThumbBox)->SetVerticalAlignment(VAlign_Center);
		UCanvasPanelSlot* ToggleSlot = Canvas->AddChildToCanvas(Toggle);
		ToggleSlot->SetAnchors(FAnchors(ToggleMinX, 0.06f, 1.f, 0.94f));
		ToggleSlot->SetOffsets(FMargin(0.f));
		Toggle->OnClicked.AddUniqueDynamic(MakeHandler(Row, 1), &URROptionsRowHandler::HandleClicked);
		URRButtonHoverSubsystem::Attach(Toggle);
		Widgets.Toggle = Toggle;
		Widgets.ToggleText = ToggleText;
		Widgets.ThumbBox = ThumbBox;
	}
	else if (RROptionsWidget::IsSliderRow(Row))
	{
		USlider* Slider = WidgetTree->ConstructWidget<USlider>(USlider::StaticClass(), *(RowName + TEXT("Slider")));
		const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
		if (RROptionsWidget::IsVolumeRow(Row))
		{
			Slider->SetMinValue(0.f);
			Slider->SetMaxValue(1.f);
			Slider->SetStepSize(Options->VolumeStep);
		}
		else
		{
			Slider->SetMinValue(Options->MinResolutionScalePercent);
			Slider->SetMaxValue(Options->MaxResolutionScalePercent);
			Slider->SetStepSize(Options->ResolutionScaleStepPercent);
		}
		Slider->SetSliderBarColor(FLinearColor::White);
		Slider->SetSliderHandleColor(FLinearColor::White);
		FSliderStyle Style = Slider->GetWidgetStyle();
		const FSlateBrush Bar = RROptionsWidget::MakeBoxBrush(LoadedTrack, SliderTrackMargin);
		Style.SetNormalBarImage(Bar);
		Style.SetHoveredBarImage(Bar);
		Style.SetDisabledBarImage(Bar);
		const FSlateBrush Knob = RROptionsWidget::MakeImageBrush(LoadedKnob);
		Style.SetNormalThumbImage(Knob);
		Style.SetHoveredThumbImage(RROptionsWidget::MakeImageBrush(LoadedKnob, FLinearColor(1.25f, 1.25f, 1.25f, 1.f)));
		Style.SetDisabledThumbImage(Knob);
		Slider->SetWidgetStyle(Style);
		UCanvasPanelSlot* SliderSlot = Canvas->AddChildToCanvas(Slider);
		SliderSlot->SetAnchors(FAnchors(SliderMinX, 0.f, SliderMaxX, 1.f));
		SliderSlot->SetOffsets(FMargin(0.f));
		Slider->OnValueChanged.AddUniqueDynamic(MakeHandler(Row, 0), &URROptionsRowHandler::HandleValueChanged);
		Widgets.Slider = Slider;

		Value->SetJustification(ETextJustify::Right);
		UCanvasPanelSlot* ValueSlot = Canvas->AddChildToCanvas(Value);
		ValueSlot->SetAnchors(FAnchors(1.f, 0.5f));
		ValueSlot->SetAlignment(FVector2D(1.f, 0.5f));
		ValueSlot->SetAutoSize(true);
		ValueSlot->SetPosition(FVector2D::ZeroVector);
	}
	else
	{
		USizeBox* LeftBox = nullptr;
		USizeBox* RightBox = nullptr;
		MakeArrow(Canvas, Row, -1, LeftArrowX, LeftBox);
		Widgets.LeftBox = LeftBox;
		Widgets.LeftButton = Cast<UButton>(Widgets.LeftBox->GetContent());
		MakeArrow(Canvas, Row, 1, RightArrowX, RightBox);
		Widgets.RightBox = RightBox;
		Widgets.RightButton = Cast<UButton>(Widgets.RightBox->GetContent());
		Value->SetJustification(ETextJustify::Center);
		UCanvasPanelSlot* ValueSlot = Canvas->AddChildToCanvas(Value);
		ValueSlot->SetAnchors(FAnchors(ValueX, 0.5f));
		ValueSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		ValueSlot->SetAutoSize(true);
		ValueSlot->SetPosition(FVector2D::ZeroVector);
	}
	Rows.Add(Widgets);
}

void URROptionsWidget::AddDivider(UVerticalBox* List)
{
	UImage* Divider = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("GraphicsDivider"));
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = FVector2D(2.f, 2.f);
	Brush.TintColor = FSlateColor(DividerColour);
	Divider->SetBrush(Brush);
	UVerticalBoxSlot* DividerSlot = List->AddChildToVerticalBox(Divider);
	FSlateChildSize Size(ESlateSizeRule::Fill);
	Size.Value = DividerRowWeight;
	DividerSlot->SetSize(Size);
	DividerSlot->SetHorizontalAlignment(HAlign_Fill);
	DividerSlot->SetVerticalAlignment(VAlign_Center);
}

void URROptionsWidget::ApplyRowHeight(float RowHeight)
{
	if (RowHeight <= 1.f || FMath::Abs(RowHeight - AppliedRowHeight) < 0.5f)
	{
		return;
	}
	AppliedRowHeight = RowHeight;
	const float ArrowHeight = RowHeight * ArrowHeightFraction;
	const float ArrowWidth = ArrowHeight * RROptionsWidget::TextureAspect(LoadedArrow);
	const float KnobHeight = RowHeight * KnobHeightFraction;
	const float ThumbHeight = RowHeight * ThumbHeightFraction;
	for (FRROptionsRowWidgets& Widgets : Rows)
	{
		for (USizeBox* Box : {Widgets.LeftBox.Get(), Widgets.RightBox.Get()})
		{
			if (Box)
			{
				Box->SetHeightOverride(ArrowHeight);
				Box->SetWidthOverride(ArrowWidth);
			}
		}
		if (Widgets.ThumbBox)
		{
			Widgets.ThumbBox->SetHeightOverride(ThumbHeight);
			Widgets.ThumbBox->SetWidthOverride(ThumbHeight * RROptionsWidget::TextureAspect(LoadedThumb));
		}
		if (Widgets.Slider)
		{
			FSliderStyle Style = Widgets.Slider->GetWidgetStyle();
			FSlateBrush Knob = Style.NormalThumbImage;
			Knob.ImageSize = FVector2D(KnobHeight * RROptionsWidget::TextureAspect(LoadedKnob), KnobHeight);
			Style.SetNormalThumbImage(Knob);
			Style.SetDisabledThumbImage(Knob);
			FSlateBrush Hovered = Style.HoveredThumbImage;
			Hovered.ImageSize = Knob.ImageSize;
			Style.SetHoveredThumbImage(Hovered);
			Style.SetBarThickness(RowHeight * SliderBarFraction);
			Widgets.Slider->SetWidgetStyle(Style);
		}
	}
}

void URROptionsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	for (const FRROptionsRowWidgets& Widgets : Rows)
	{
		const float Height = Widgets.Canvas ? Widgets.Canvas->GetCachedGeometry().GetLocalSize().Y : 0.f;
		if (Height > 1.f)
		{
			ApplyRowHeight(Height);
			break;
		}
	}
	const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (ConfirmCountdownText && Settings && Settings->IsDisplayConfirmPending())
	{
		ConfirmCountdownText->SetText(FText::Format(NSLOCTEXT("RogueRigs", "OptRevertIn", "Reverting in {0} s"),
			FText::AsNumber(Settings->GetDisplayConfirmSecondsLeft())));
	}
}

FReply URROptionsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
		if (Settings && Settings->IsDisplayConfirmPending())
		{
			UE_LOG(LogRogueRigs, Log, TEXT("Options screen: Escape reverts the display change."));
			Settings->RevertDisplayChange();
		}
		else
		{
			Back();
		}
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void URROptionsWidget::ShowTab(ERROptionsTab Tab)
{
	CurrentTab = Tab;
	const bool bSound = Tab == ERROptionsTab::Sound;
	if (SoundList) { SoundList->SetVisibility(bSound ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed); }
	if (GraphicsList) { GraphicsList->SetVisibility(bSound ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible); }
	if (SoundTabSelected) { SoundTabSelected->SetVisibility(bSound ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden); }
	if (GraphicsTabSelected) { GraphicsTabSelected->SetVisibility(bSound ? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible); }
	if (SoundTabText) { SoundTabText->SetColorAndOpacity(FSlateColor(bSound ? SelectedTabTextColour : TabTextColour)); }
	if (GraphicsTabText) { GraphicsTabText->SetColorAndOpacity(FSlateColor(bSound ? TabTextColour : SelectedTabTextColour)); }
	RefreshValues();
}

void URROptionsWidget::FocusFirstTab()
{
	if (SoundTabButton)
	{
		SoundTabButton->SetKeyboardFocus();
	}
	else
	{
		SetKeyboardFocus();
	}
}

void URROptionsWidget::ResetCurrentTab()
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings)
	{
		return;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Options screen: Reset to defaults (%s tab)."), CurrentTab == ERROptionsTab::Sound ? TEXT("Sound") : TEXT("Graphics"));
	if (CurrentTab == ERROptionsTab::Sound)
	{
		Settings->ResetSoundToDefaults();
	}
	else
	{
		Settings->ResetGraphicsToDefaults();
	}
	RefreshValues();
}

void URROptionsWidget::Back()
{
	if (bClosing)
	{
		return;
	}
	bClosing = true;
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		if (Settings->IsDisplayConfirmPending())
		{
			Settings->RevertDisplayChange();
		}
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Options screen: Back, closed."));
	RemoveFromParent();
	OnOptionsClosed.Broadcast();
}

void URROptionsWidget::StepRow(ERROptionRow Row, int32 Direction)
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings || Direction == 0)
	{
		return;
	}
	const URROptionsSettings* Options = GetDefault<URROptionsSettings>();
	const int32 Dir = Direction > 0 ? 1 : -1;
	ERRGraphicsDetail Detail;
	switch (Row)
	{
	case ERROptionRow::WindowMode: Settings->StepWindowMode(Dir); break;
	case ERROptionRow::Resolution: Settings->StepResolution(Dir); break;
	case ERROptionRow::VSync: Settings->SetVSyncOn(!Settings->IsVSyncEnabled()); break;
	case ERROptionRow::FrameRateLimit: Settings->StepFrameRateLimit(Dir); break;
	case ERROptionRow::QualityPreset: Settings->StepQualityPreset(Dir); break;
	case ERROptionRow::ResolutionScale:
		Settings->SetResolutionScalePercent(Settings->GetResolutionScalePercent() + Dir * Options->ResolutionScaleStepPercent);
		break;
	case ERROptionRow::MasterVolume:
	case ERROptionRow::MusicVolume:
	case ERROptionRow::EffectsVolume:
	{
		const ERRVolumeGroup Group = RROptionsWidget::ToVolumeGroup(Row);
		Settings->SetVolume(Group, Settings->GetVolume(Group) + Dir * Options->VolumeStep);
		break;
	}
	default:
		if (RROptionsWidget::ToDetail(Row, Detail))
		{
			Settings->StepGraphicsDetail(Detail, Dir);
		}
		break;
	}
	RefreshValues();
	UE_LOG(LogRogueRigs, Log, TEXT("Options screen: %s %s -> %s"), *GetRowLabel(Row).ToString(), Dir > 0 ? TEXT("next") : TEXT("previous"),
		*GetRowValueString(Row));
}

void URROptionsWidget::SetRowSliderValue(ERROptionRow Row, float Value)
{
	URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings)
	{
		return;
	}
	if (RROptionsWidget::IsVolumeRow(Row))
	{
		Settings->SetVolume(RROptionsWidget::ToVolumeGroup(Row), Value);
	}
	else if (Row == ERROptionRow::ResolutionScale)
	{
		Settings->SetResolutionScalePercent(Value);
	}
	RefreshValues();
}

void URROptionsWidget::RefreshValues()
{
	for (FRROptionsRowWidgets& Widgets : Rows)
	{
		RefreshRow(Widgets);
	}
}

FString URROptionsWidget::GetRowValueString(ERROptionRow Row) const
{
	for (const FRROptionsRowWidgets& Widgets : Rows)
	{
		if (Widgets.Row == Row)
		{
			return Widgets.Toggle && Widgets.ToggleText ? Widgets.ToggleText->GetText().ToString()
				: Widgets.Value ? Widgets.Value->GetText().ToString() : FString();
		}
	}
	return FString();
}

void URROptionsWidget::RefreshRow(FRROptionsRowWidgets& Widgets)
{
	const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	if (!Settings || !Widgets.Value)
	{
		return;
	}
	FText Text;
	ERRGraphicsDetail Detail;
	switch (Widgets.Row)
	{
	case ERROptionRow::MasterVolume:
	case ERROptionRow::MusicVolume:
	case ERROptionRow::EffectsVolume:
	{
		const float Volume = Settings->GetVolume(RROptionsWidget::ToVolumeGroup(Widgets.Row));
		Text = URRGameUserSettings::GetPercentText(Volume, false);
		if (Widgets.Slider) { Widgets.Slider->SetValue(Volume); }
		break;
	}
	case ERROptionRow::ResolutionScale:
	{
		const float Percent = Settings->GetResolutionScalePercent();
		Text = URRGameUserSettings::GetPercentText(Percent, true);
		if (Widgets.Slider) { Widgets.Slider->SetValue(Percent); }
		break;
	}
	case ERROptionRow::WindowMode: Text = URRGameUserSettings::GetWindowModeText(Settings->GetWindowMode()); break;
	case ERROptionRow::Resolution:
	{
		Text = URRGameUserSettings::GetResolutionText(Settings->GetShownResolution());
		const bool bSelectable = Settings->IsResolutionSelectable();
		if (Widgets.LeftButton) { Widgets.LeftButton->SetIsEnabled(bSelectable); }
		if (Widgets.RightButton) { Widgets.RightButton->SetIsEnabled(bSelectable); }
		break;
	}
	case ERROptionRow::VSync:
	{
		const bool bOn = Settings->IsVSyncEnabled();
		Text = URRGameUserSettings::GetOnOffText(bOn);
		if (Widgets.ToggleText)
		{
			Widgets.ToggleText->SetText(Text);
			if (UOverlaySlot* TextSlot = Cast<UOverlaySlot>(Widgets.ToggleText->Slot))
			{
				// On: label left, thumb right; Off: thumb left, label right (options mockup).
				TextSlot->SetHorizontalAlignment(bOn ? HAlign_Left : HAlign_Right);
				const float Pad = AppliedRowHeight > 1.f ? AppliedRowHeight * 0.45f : 18.f;
				TextSlot->SetPadding(bOn ? FMargin(Pad, 0.f, 0.f, 0.f) : FMargin(0.f, 0.f, Pad, 0.f));
			}
		}
		if (Widgets.ThumbBox)
		{
			if (UOverlaySlot* ThumbSlot = Cast<UOverlaySlot>(Widgets.ThumbBox->Slot))
			{
				ThumbSlot->SetHorizontalAlignment(bOn ? HAlign_Right : HAlign_Left);
				const float Pad = AppliedRowHeight > 1.f ? AppliedRowHeight * 0.1f : 4.f;
				ThumbSlot->SetPadding(FMargin(Pad, 0.f, Pad, 0.f));
			}
		}
		break;
	}
	case ERROptionRow::FrameRateLimit: Text = URRGameUserSettings::GetFrameRateLimitText(Settings->GetFrameRateLimitValue()); break;
	case ERROptionRow::QualityPreset: Text = URRGameUserSettings::GetQualityLevelText(Settings->GetQualityPreset()); break;
	default:
		if (RROptionsWidget::ToDetail(Widgets.Row, Detail))
		{
			Text = URRGameUserSettings::GetQualityLevelText(Settings->GetGraphicsDetail(Detail));
		}
		break;
	}
	Widgets.Value->SetText(Text);
}

void URROptionsWidget::HandleSoundTabClicked()
{
	ShowTab(ERROptionsTab::Sound);
}

void URROptionsWidget::HandleGraphicsTabClicked()
{
	ShowTab(ERROptionsTab::Graphics);
}

void URROptionsWidget::HandleResetClicked()
{
	ResetCurrentTab();
}

void URROptionsWidget::HandleBackClicked()
{
	Back();
}

void URROptionsWidget::HandleKeepClicked()
{
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Settings->ConfirmDisplayChange();
	}
}

void URROptionsWidget::HandleRevertClicked()
{
	if (URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings())
	{
		Settings->RevertDisplayChange();
	}
}

void URROptionsWidget::HandleDisplayConfirmChanged(bool bPending)
{
	if (ConfirmPanel)
	{
		ConfirmPanel->SetVisibility(bPending ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	RefreshValues();
	if (bPending && KeepButton)
	{
		KeepButton->SetKeyboardFocus();
	}
	else if (!bPending && GraphicsTabButton && IsInViewport())
	{
		GraphicsTabButton->SetKeyboardFocus();
	}
}
