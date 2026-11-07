// Rogue Rigs - shared mouse hover effect of the screen buttons (ui-button-hover, task-485).

#include "UI/RRButtonHover.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "RogueRigs.h"

namespace RRButtonHover
{
	static const FName BrightnessParam(TEXT("Brightness"));
	static const FName TextureParam(TEXT("PlateTexture"));

	/** Display gamma used to turn the sRGB brightness factor of the settings into the linear factor of the material. */
	static constexpr float DisplayGamma = 2.2f;
}

URRButtonHoverSettings::URRButtonHoverSettings()
	: HoverScale(1.05f)
	, HoverBrightness(1.25f)
	, EaseSeconds(0.1f)
	, PlateMaterial(FSoftObjectPath(TEXT("/Game/RogueRigs/UI/M_UI_ButtonPlate.M_UI_ButtonPlate")))
{
}

void URRButtonHoverSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PlateMaterialBase = GetDefault<URRButtonHoverSettings>()->PlateMaterial.LoadSynchronous();
	if (!PlateMaterialBase)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("ButtonHover: plate material %s not found; plate buttons will only grow."),
			*GetDefault<URRButtonHoverSettings>()->PlateMaterial.ToString());
	}
	// Core ticker: real time, ticks while the game is paused (pause, level-up, stats, shop, run-lost screens).
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &URRButtonHoverSubsystem::Tick));
}

void URRButtonHoverSubsystem::Deinitialize()
{
	FTSTicker::RemoveTicker(TickHandle);
	TickHandle.Reset();
	Entries.Reset();
	Super::Deinitialize();
}

void URRButtonHoverSubsystem::Attach(UButton* Button, bool bBrightenPlate, UWidget* ScaleWidget)
{
	if (!Button || Button->IsDesignTime())
	{
		return;
	}
	const UWorld* World = Button->GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	URRButtonHoverSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<URRButtonHoverSubsystem>() : nullptr;
	if (Subsystem)
	{
		Subsystem->AttachButton(Button, bBrightenPlate, ScaleWidget);
	}
}

void URRButtonHoverSubsystem::AttachNamed(UUserWidget* Owner, const TArray<FName>& ButtonNames, bool bBrightenPlate)
{
	if (!Owner || Owner->IsDesignTime())
	{
		return;
	}
	for (const FName& Name : ButtonNames)
	{
		if (UButton* Button = Cast<UButton>(Owner->GetWidgetFromName(Name)))
		{
			Attach(Button, bBrightenPlate);
		}
		else
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("ButtonHover: %s has no button named %s."), *Owner->GetName(), *Name.ToString());
		}
	}
}

float URRButtonHoverSubsystem::GetHoverAmount(const UButton* Button) const
{
	for (const FRRButtonHoverEntry& Entry : Entries)
	{
		if (Entry.Button.Get() == Button)
		{
			return Entry.Amount;
		}
	}
	return -1.0f;
}

void URRButtonHoverSubsystem::LogEntries() const
{
	for (const FRRButtonHoverEntry& Entry : Entries)
	{
		if (const UButton* Button = Entry.Button.Get())
		{
			const UUserWidget* Owner = Button->GetTypedOuter<UUserWidget>();
			UE_LOG(LogRogueRigs, Log, TEXT("ButtonHover: %s.%s hovered %d enabled %d amount %.2f plate %s"), Owner ? *Owner->GetClass()->GetName() : TEXT("?"),
				*Button->GetName(), Button->IsHovered() ? 1 : 0, Button->GetIsEnabled() ? 1 : 0, Entry.Amount,
				Entry.PlateMaterial ? TEXT("material") : Entry.bTintPlate ? TEXT("tint") : TEXT("none"));
		}
	}
}

void URRButtonHoverSubsystem::AttachButton(UButton* Button, bool bBrightenPlate, UWidget* ScaleWidget)
{
	// Drop entries of destroyed buttons, and never attach one button twice.
	Entries.RemoveAll([](const FRRButtonHoverEntry& Entry) { return !Entry.Button.IsValid(); });
	for (const FRRButtonHoverEntry& Entry : Entries)
	{
		if (Entry.Button.Get() == Button)
		{
			return;
		}
	}

	FRRButtonHoverEntry& Entry = Entries.AddDefaulted_GetRef();
	Entry.Button = Button;
	Entry.ScaleWidget = ScaleWidget ? ScaleWidget : static_cast<UWidget*>(Button);
	Entry.RestScale = Entry.ScaleWidget->GetRenderTransform().Scale;

	FButtonStyle Style = Button->GetStyle();
	UTexture* PlateTexture = Cast<UTexture>(Style.Normal.GetResourceObject());
	if (bBrightenPlate && PlateMaterialBase && PlateTexture && Style.Normal.DrawAs != ESlateBrushDrawType::NoDrawType)
	{
		Entry.PlateMaterial = UMaterialInstanceDynamic::Create(PlateMaterialBase, Button);
		Entry.PlateMaterial->SetTextureParameterValue(RRButtonHover::TextureParam, PlateTexture);
		Entry.PlateMaterial->SetScalarParameterValue(RRButtonHover::BrightnessParam, 1.0f);
		// Same draw mode, margins, size and tint as the Normal brush; only the resource changes. The Hovered brush becomes the
		// same brush, so the effect comes only from the eased Brightness (a tint above 1 is clamped by Slate vertex colours).
		Style.Normal.SetResourceObject(Entry.PlateMaterial);
		Style.Hovered = Style.Normal;
		Button->SetStyle(Style);
	}
	else if (bBrightenPlate && Style.Normal.DrawAs != ESlateBrushDrawType::NoDrawType && !Style.Normal.GetResourceObject()
		&& Style.Normal.TintColor.IsColorSpecified())
	{
		// Untextured plate (engine default grey rounded box: level-up Reroll, run-won end button): its tint is below 1, so the
		// eased brightening goes through the Normal tint. The Hovered brush becomes the Normal one, so the engine's instant hover
		// tint no longer jumps.
		Entry.bTintPlate = true;
		Entry.RestTint = Style.Normal.TintColor.GetSpecifiedColor();
		Style.Hovered = Style.Normal;
		Button->SetStyle(Style);
	}
	// Text buttons (bBrightenPlate false): the hover look is the growth plus the widget's own colours; brushes stay as authored.
}

bool URRButtonHoverSubsystem::IsButtonHoverable(const UButton* Button)
{
	if (!Button || !Button->IsHovered())
	{
		return false;
	}
	// A disabled button (or one inside a disabled parent, nested user widgets included) never reacts.
	for (const UWidget* Widget = Button; Widget;)
	{
		if (!Widget->GetIsEnabled())
		{
			return false;
		}
		const UWidget* Parent = Widget->GetParent();
		Widget = Parent ? Parent : static_cast<const UWidget*>(Widget->GetTypedOuter<UUserWidget>());
	}
	return true;
}

bool URRButtonHoverSubsystem::Tick(float DeltaTime)
{
	const URRButtonHoverSettings* Settings = GetDefault<URRButtonHoverSettings>();
	const float Step = Settings->EaseSeconds > KINDA_SMALL_NUMBER ? FMath::Min(DeltaTime, 0.1f) / Settings->EaseSeconds : 1.0f;
	for (int32 Index = Entries.Num() - 1; Index >= 0; --Index)
	{
		FRRButtonHoverEntry& Entry = Entries[Index];
		UButton* Button = Entry.Button.Get();
		if (!Button)
		{
			Entries.RemoveAtSwap(Index);
			continue;
		}
		const float Target = IsButtonHoverable(Button) ? 1.0f : 0.0f;
		if (Entry.Amount == Target)
		{
			continue;
		}
		Entry.Amount = Target > Entry.Amount ? FMath::Min(Entry.Amount + Step, 1.0f) : FMath::Max(Entry.Amount - Step, 0.0f);
		Apply(Entry);
	}
	return true;
}

void URRButtonHoverSubsystem::Apply(FRRButtonHoverEntry& Entry) const
{
	const URRButtonHoverSettings* Settings = GetDefault<URRButtonHoverSettings>();
	const float Eased = FMath::SmoothStep(0.0f, 1.0f, Entry.Amount);
	const float Scale = FMath::Lerp(1.0f, Settings->HoverScale, Eased);
	if (UWidget* ScaleWidget = Entry.ScaleWidget.Get())
	{
		ScaleWidget->SetRenderScale(Entry.RestScale * Scale);
	}
	if (Entry.PlateMaterial)
	{
		const float DisplayFactor = FMath::Lerp(1.0f, Settings->HoverBrightness, Eased);
		Entry.PlateMaterial->SetScalarParameterValue(RRButtonHover::BrightnessParam, FMath::Pow(DisplayFactor, RRButtonHover::DisplayGamma));
	}
	else if (Entry.bTintPlate)
	{
		UButton* Button = Entry.Button.Get();
		// Linear factor here (not the display one): these grey plates carry white labels, and the display factor took them to
		// near white, unreadable (task-485 capture of the run-won button).
		const float LinearFactor = FMath::Lerp(1.0f, Settings->HoverBrightness, Eased);
		FLinearColor Tint = Entry.RestTint;
		Tint.R = FMath::Min(Tint.R * LinearFactor, 1.0f);
		Tint.G = FMath::Min(Tint.G * LinearFactor, 1.0f);
		Tint.B = FMath::Min(Tint.B * LinearFactor, 1.0f);
		FButtonStyle Style = Button->GetStyle();
		Style.Normal.TintColor = FSlateColor(Tint);
		Style.Hovered = Style.Normal;
		Button->SetStyle(Style);
	}
}
