// Rogue Rigs - shared entrance effect of the end screens (task-547).

#include "UI/RRScreenEntrance.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraComponent.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "RogueRigs.h"

namespace RRScreenEntranceLocal
{
	/** APlayerController::bShouldPerformFullTickWhenPaused is protected (Blueprint read-write): read and written through reflection. */
	FBoolProperty* FullTickProperty()
	{
		static FBoolProperty* Property = FindFProperty<FBoolProperty>(APlayerController::StaticClass(), TEXT("bShouldPerformFullTickWhenPaused"));
		return Property;
	}

	bool GetFullTickWhenPaused(const APlayerController* Controller)
	{
		FBoolProperty* Property = FullTickProperty();
		return Property && Property->GetPropertyValue_InContainer(Controller);
	}

	void SetFullTickWhenPaused(APlayerController* Controller, bool bValue)
	{
		if (FBoolProperty* Property = FullTickProperty())
		{
			Property->SetPropertyValue_InContainer(Controller, bValue);
		}
	}
}

URRScreenEntrance::FPart URRScreenEntrance::MakePart(UWidget* Widget)
{
	FPart Part;
	Part.Widget = Widget;
	if (Widget)
	{
		Part.RestTranslation = Widget->GetRenderTransform().Translation;
		Part.RestScale = Widget->GetRenderTransform().Scale;
		Part.RestOpacity = Widget->GetRenderOpacity();
	}
	return Part;
}

void URRScreenEntrance::SetOpacity(const FPart& Part, float Alpha)
{
	if (UWidget* Widget = Part.Widget.Get())
	{
		Widget->SetRenderOpacity(Part.RestOpacity * Alpha);
	}
}

void URRScreenEntrance::Setup(UUserWidget* Screen, const FRRScreenEntranceParts& Parts)
{
	Owner = Screen;
	UWidgetTree* Tree = Screen ? Screen->WidgetTree.Get() : nullptr;
	if (!Tree)
	{
		return;
	}
	auto Find = [Tree](FName Name) -> UWidget* { return Name.IsNone() ? nullptr : Tree->FindWidget(Name); };
	auto FindAll = [&Find](const TArray<FName>& Names, TArray<FPart>& Out)
	{
		Out.Reset();
		for (const FName& Name : Names)
		{
			if (UWidget* Widget = Find(Name))
			{
				Out.Add(MakePart(Widget));
			}
		}
	};
	BackdropPart = MakePart(Find(Parts.Backdrop));
	TitlePart = MakePart(Find(Parts.Title));
	FindAll(Parts.TitleGroup, TitleParts);
	FindAll(Parts.FrameGroup, FrameParts);
	FindAll(Parts.Buttons, ButtonParts);
	SkipButton = Cast<UButton>(Find(Parts.SkipButton));
	if (!SkipButton)
	{
		SkipButton = MakeSkipButton();
	}
	if (SkipButton)
	{
		SkipButton->OnClicked.AddUniqueDynamic(this, &URRScreenEntrance::HandleSkipClicked);
		SkipButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	UE_LOG(LogRogueRigs, Verbose, TEXT("URRScreenEntrance: %s parts backdrop %d title %d group %d frame %d buttons %d skip %d."), *GetNameSafe(Screen),
		BackdropPart.Widget.IsValid(), TitlePart.Widget.IsValid(), TitleParts.Num(), FrameParts.Num(), ButtonParts.Num(), SkipButton != nullptr);
}

UButton* URRScreenEntrance::MakeSkipButton()
{
	UUserWidget* Screen = Owner.Get();
	UCanvasPanel* Root = Screen ? Cast<UCanvasPanel>(Screen->GetRootWidget()) : nullptr;
	if (!Root)
	{
		return nullptr;
	}
	UButton* Button = Screen->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("EntranceSkipButton"));
	FButtonStyle Style = Button->GetStyle();
	Style.Normal.DrawAs = ESlateBrushDrawType::NoDrawType;
	Style.Hovered.DrawAs = ESlateBrushDrawType::NoDrawType;
	Style.Pressed.DrawAs = ESlateBrushDrawType::NoDrawType;
	Style.Disabled.DrawAs = ESlateBrushDrawType::NoDrawType;
	Button->SetStyle(Style);
	Button->SetRenderOpacity(0.f);
	if (UCanvasPanelSlot* Slot = Root->AddChildToCanvas(Button))
	{
		Slot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		Slot->SetOffsets(FMargin(0.f));
		Slot->SetZOrder(1000);
	}
	return Button;
}

void URRScreenEntrance::Start()
{
	Elapsed = 0.f;
	bPlaying = true;
	RestoreTickFrames = -1;
	ButtonVisibility.Reset();
	for (const FPart& Part : ButtonParts)
	{
		UWidget* Widget = Part.Widget.Get();
		ButtonVisibility.Add(Widget ? Widget->GetVisibility() : ESlateVisibility::Visible);
		if (Widget && Widget->GetVisibility() == ESlateVisibility::Visible)
		{
			Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	if (SkipButton)
	{
		SkipButton->SetVisibility(ESlateVisibility::Visible);
	}
	// The paused controller does not update its camera unless it fully ticks: needed for the desaturation to show.
	if (APlayerController* Controller = Owner.IsValid() ? Owner->GetOwningPlayer() : nullptr)
	{
		if (!bFullTickSaved)
		{
			bSavedFullTickWhenPaused = RRScreenEntranceLocal::GetFullTickWhenPaused(Controller);
			bFullTickSaved = true;
		}
		RRScreenEntranceLocal::SetFullTickWhenPaused(Controller, true);
	}
	Apply(0.f);
}

void URRScreenEntrance::Tick(float DeltaSeconds)
{
	if (RestoreTickFrames > 0 && --RestoreTickFrames == 0)
	{
		if (APlayerController* Controller = Owner.IsValid() ? Owner->GetOwningPlayer() : nullptr)
		{
			if (bFullTickSaved)
			{
				RRScreenEntranceLocal::SetFullTickWhenPaused(Controller, bSavedFullTickWhenPaused);
			}
		}
	}
	if (!bPlaying)
	{
		return;
	}
	Elapsed += DeltaSeconds;
	const URRScreenEntranceSettings* Settings = GetDefault<URRScreenEntranceSettings>();
	const APlayerController* Controller = Owner.IsValid() ? Owner->GetOwningPlayer() : nullptr;
	if (Elapsed >= Settings->TotalSeconds || (Controller && Controller->WasInputKeyJustPressed(EKeys::AnyKey)))
	{
		Skip();
		return;
	}
	Apply(Elapsed);
}

void URRScreenEntrance::Apply(float T)
{
	const URRScreenEntranceSettings* S = GetDefault<URRScreenEntranceSettings>();
	const float Backdrop = FMath::GetMappedRangeValueClamped(FVector2f(0.f, S->BackdropEnd), FVector2f(0.f, 1.f), T);
	const float TitleA = FMath::GetMappedRangeValueClamped(FVector2f(S->BackdropEnd, S->TitleEnd), FVector2f(0.f, 1.f), T);
	const float FrameA = FMath::GetMappedRangeValueClamped(FVector2f(S->TitleEnd, S->FrameEnd), FVector2f(0.f, 1.f), T);
	const float ButtonA = FMath::GetMappedRangeValueClamped(FVector2f(S->FrameEnd, S->ButtonsEnd), FVector2f(0.f, 1.f), T);

	SetOpacity(BackdropPart, Backdrop);
	SetLevelDesaturation(Backdrop * S->LevelDesaturation);

	for (const FPart& Part : TitleParts)
	{
		SetOpacity(Part, TitleA);
	}
	if (UWidget* Title = TitlePart.Widget.Get())
	{
		const float Scale = FMath::Lerp(S->TitleStartScale, 1.f, TitleA);
		Title->SetRenderScale(TitlePart.RestScale * Scale);
		// Short shake once the title has landed, fading to 0 by ShakeEnd.
		const float ShakeFade = TitleA >= 1.f ? FMath::GetMappedRangeValueClamped(FVector2f(S->TitleEnd, S->ShakeEnd), FVector2f(1.f, 0.f), T) : 0.f;
		const float ShakeX = FMath::Sin(T * S->ShakeSpeed) * S->ShakeAmplitude * ShakeFade;
		Title->SetRenderTranslation(TitlePart.RestTranslation + FVector2D(ShakeX, 0.0));
	}
	for (const FPart& Part : FrameParts)
	{
		SetOpacity(Part, FrameA);
		if (UWidget* Widget = Part.Widget.Get())
		{
			Widget->SetRenderTranslation(Part.RestTranslation + FVector2D(0.0, S->FrameSlide * (1.f - FrameA)));
		}
	}
	for (const FPart& Part : ButtonParts)
	{
		SetOpacity(Part, ButtonA);
	}
}

bool URRScreenEntrance::Skip()
{
	if (!bPlaying)
	{
		return false;
	}
	Finish();
	return true;
}

void URRScreenEntrance::HandleSkipClicked()
{
	Skip();
}

void URRScreenEntrance::Finish()
{
	bPlaying = false;
	Elapsed = GetDefault<URRScreenEntranceSettings>()->TotalSeconds;
	Apply(Elapsed);
	if (SkipButton)
	{
		SkipButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	for (int32 Index = 0; Index < ButtonParts.Num(); ++Index)
	{
		UWidget* Widget = ButtonParts[Index].Widget.Get();
		if (Widget && ButtonVisibility.IsValidIndex(Index) && Widget->GetVisibility() == ESlateVisibility::HitTestInvisible)
		{
			Widget->SetVisibility(ButtonVisibility[Index]);
		}
	}
	// Two more frames of full tick so the camera applies the final desaturation, then the controller's own setting back.
	RestoreTickFrames = 2;
	UE_LOG(LogRogueRigs, Log, TEXT("URRScreenEntrance: %s entrance finished."), *GetNameSafe(Owner.Get()));
	OnFinished.ExecuteIfBound();
}

void URRScreenEntrance::Restore()
{
	if (UCameraComponent* Camera = SavedCamera.Get())
	{
		if (bSaturationSaved)
		{
			Camera->PostProcessSettings.bOverride_ColorSaturation = bSavedOverride;
			Camera->PostProcessSettings.ColorSaturation = SavedSaturation;
		}
	}
	bSaturationSaved = false;
	SavedCamera.Reset();
	if (APlayerController* Controller = Owner.IsValid() ? Owner->GetOwningPlayer() : nullptr)
	{
		if (bFullTickSaved)
		{
			RRScreenEntranceLocal::SetFullTickWhenPaused(Controller, bSavedFullTickWhenPaused);
		}
	}
	bFullTickSaved = false;
	RestoreTickFrames = -1;
	bPlaying = false;
}

bool URRScreenEntrance::SetLevelDesaturation(float Amount)
{
	const APlayerController* Controller = Owner.IsValid() ? Owner->GetOwningPlayer() : nullptr;
	const AActor* ViewTarget = Controller ? Controller->GetViewTarget() : nullptr;
	UCameraComponent* Camera = ViewTarget ? ViewTarget->FindComponentByClass<UCameraComponent>() : nullptr;
	if (!Camera)
	{
		return false;
	}
	// Global saturation (W) scaled from the camera's own value: 1 - Amount of it, 0 = grey.
	FPostProcessSettings& Settings = Camera->PostProcessSettings;
	if (!bSaturationSaved || SavedCamera.Get() != Camera)
	{
		SavedCamera = Camera;
		SavedSaturation = Settings.bOverride_ColorSaturation ? Settings.ColorSaturation : FVector4(1.f, 1.f, 1.f, 1.f);
		bSavedOverride = Settings.bOverride_ColorSaturation;
		bSaturationSaved = true;
	}
	const float Keep = 1.f - FMath::Clamp(Amount, 0.f, 1.f);
	Settings.bOverride_ColorSaturation = bSavedOverride || Keep < 1.f;
	Settings.ColorSaturation = FVector4(SavedSaturation.X, SavedSaturation.Y, SavedSaturation.Z, SavedSaturation.W * Keep);
	return true;
}
