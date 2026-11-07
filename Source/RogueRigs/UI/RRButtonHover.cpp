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

void URRButtonHoverSubsystem::Attach(UButton* Button, bool bBrightenPlate)
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
		Subsystem->AttachButton(Button, bBrightenPlate);
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

void URRButtonHoverSubsystem::AttachButton(UButton* Button, bool bBrightenPlate)
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
	Entry.RestScale = Button->GetRenderTransform().Scale;

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
	UButton* Button = Entry.Button.Get();
	const float Eased = FMath::SmoothStep(0.0f, 1.0f, Entry.Amount);
	const float Scale = FMath::Lerp(1.0f, Settings->HoverScale, Eased);
	Button->SetRenderScale(Entry.RestScale * Scale);
	if (Entry.PlateMaterial)
	{
		const float DisplayFactor = FMath::Lerp(1.0f, Settings->HoverBrightness, Eased);
		Entry.PlateMaterial->SetScalarParameterValue(RRButtonHover::BrightnessParam, FMath::Pow(DisplayFactor, RRButtonHover::DisplayGamma));
	}
}
