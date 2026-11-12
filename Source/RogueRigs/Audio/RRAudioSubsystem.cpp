// Rogue Rigs - audio system (audio-direction, task-511).

#include "Audio/RRAudioSubsystem.h"

#include "Audio/RRAudioSettings.h"
#include "AudioDevice.h"
#include "AudioThread.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/RROptionsSettings.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Widgets/SWindow.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRAudioSubsystem)

DEFINE_LOG_CATEGORY_STATIC(LogRRAudio, Log, All);

namespace RRAudio
{
	static const FName ButtonType(TEXT("SButton"));
	static const FName UIHoverEvent(TEXT("UIHover"));
	static const FName UIClickEvent(TEXT("UIClick"));

	static FName Refine(FName Event, FName Variant)
	{
		return Variant.IsNone() ? Event : FName(*FString::Printf(TEXT("%s.%s"), *Event.ToString(), *Variant.ToString()));
	}
}

/** Slate input pre-processor: sees every left press first (never consumes it) and hands its position to the subsystem. */
class FRRAudioClickProcessor : public IInputProcessor
{
public:
	explicit FRRAudioClickProcessor(URRAudioSubsystem* InOwner) : Owner(InOwner) {}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (URRAudioSubsystem* Subsystem = Owner.Get())
			{
				Subsystem->HandleMousePress(MouseEvent.GetScreenSpacePosition());
			}
		}
		return false;
	}

	virtual const TCHAR* GetDebugName() const override { return TEXT("RRAudioClick"); }

private:
	TWeakObjectPtr<URRAudioSubsystem> Owner;
};

URRAudioSubsystem* URRAudioSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URRAudioSubsystem>() : nullptr;
}

void URRAudioSubsystem::PlayEvent(const UObject* WorldContextObject, FName Event, FName Variant)
{
	if (URRAudioSubsystem* Subsystem = Get(WorldContextObject))
	{
		Subsystem->Play(WorldContextObject, RRAudio::Refine(Event, Variant), nullptr);
	}
}

void URRAudioSubsystem::PlayEventAt(const UObject* WorldContextObject, FName Event, const FVector& Location, FName Variant)
{
	if (URRAudioSubsystem* Subsystem = Get(WorldContextObject))
	{
		Subsystem->Play(WorldContextObject, RRAudio::Refine(Event, Variant), &Location);
	}
}

void URRAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EffectsClass = GetDefault<URROptionsSettings>()->EffectsSoundClass.LoadSynchronous();
	if (EffectsClass == nullptr)
	{
		UE_LOG(LogRRAudio, Warning, TEXT("URRAudioSubsystem: Effects sound class not found (URROptionsSettings::EffectsSoundClass); sounds play in the default class."));
	}
	// Load every table sound now, so the first play of an event never waits for a load.
	for (const FRRSoundEventRow& Row : GetDefault<URRAudioSettings>()->Events)
	{
		ResolveSound(Row);
	}

	if (FSlateApplication::IsInitialized())
	{
		ClickProcessor = MakeShared<FRRAudioClickProcessor>(this);
		FSlateApplication::Get().RegisterInputPreProcessor(ClickProcessor);
		TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &URRAudioSubsystem::TickHover));
	}
}

void URRAudioSubsystem::Deinitialize()
{
	FTSTicker::RemoveTicker(TickerHandle);
	if (ClickProcessor.IsValid() && FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(ClickProcessor);
	}
	ClickProcessor.Reset();
	ActivePlays.Reset();
	Super::Deinitialize();
}

USoundBase* URRAudioSubsystem::ResolveSound(const FRRSoundEventRow& Row)
{
	if (const TObjectPtr<USoundBase>* Loaded = LoadedSounds.Find(Row.Event))
	{
		return *Loaded;
	}
	USoundBase* Sound = Row.Sound.IsNull() ? nullptr : Row.Sound.LoadSynchronous();
	if (!Row.Sound.IsNull() && Sound == nullptr)
	{
		UE_LOG(LogRRAudio, Warning, TEXT("Sound event %s: sound %s not found; the event stays silent."), *Row.Event.ToString(), *Row.Sound.ToString());
	}
	LoadedSounds.Add(Row.Event, Sound);
	return Sound;
}

void URRAudioSubsystem::PruneActive(TArray<TWeakObjectPtr<UAudioComponent>>& Active) const
{
	Active.RemoveAll([](const TWeakObjectPtr<UAudioComponent>& Play)
	{
		const UAudioComponent* Component = Play.Get();
		return Component == nullptr || !Component->IsPlaying();
	});
}

int32 URRAudioSubsystem::GetActiveCount(FName Event)
{
	const FRRSoundEventRow* Row = GetDefault<URRAudioSettings>()->FindRow(Event);
	TArray<TWeakObjectPtr<UAudioComponent>>* Active = Row ? ActivePlays.Find(Row->Event) : nullptr;
	if (Active == nullptr)
	{
		return 0;
	}
	PruneActive(*Active);
	return Active->Num();
}

UAudioComponent* URRAudioSubsystem::Play(const UObject* WorldContextObject, FName Event, const FVector* Location)
{
	const FRRSoundEventRow* Row = GetDefault<URRAudioSettings>()->FindRow(Event);
	USoundBase* Sound = Row ? ResolveSound(*Row) : nullptr;
	if (Sound == nullptr)
	{
		// A missing row or sound is a normal state (event not given a sound yet): silent, logged once.
		if (!ReportedSilent.Contains(Event))
		{
			ReportedSilent.Add(Event);
			UE_LOG(LogRRAudio, Verbose, TEXT("Sound event %s has no sound; silent."), *Event.ToString());
		}
		return nullptr;
	}
	UWorld* World = GEngine && WorldContextObject ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (World == nullptr)
	{
		World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	}
	if (World == nullptr)
	{
		return nullptr;
	}

	TArray<TWeakObjectPtr<UAudioComponent>>& Active = ActivePlays.FindOrAdd(Row->Event);
	PruneActive(Active);
	FRRSoundEventStats& RowStats = Stats.FindOrAdd(Row->Event);
	// Cap: the oldest plays of the row stop so the new one is at most the MaxConcurrent-th.
	while (Row->MaxConcurrent > 0 && Active.Num() >= Row->MaxConcurrent)
	{
		if (UAudioComponent* Oldest = Active[0].Get())
		{
			Oldest->Stop();
		}
		Active.RemoveAt(0);
		++RowStats.CapStops;
	}

	const float Jitter = FMath::Clamp(Row->PitchJitter, 0.f, 0.5f);
	const float Pitch = Jitter > 0.f ? FMath::FRandRange(1.f - Jitter, 1.f + Jitter) : 1.f;
	UAudioComponent* Component = UGameplayStatics::CreateSound2D(World, Sound, Row->Volume, Pitch, 0.f, nullptr, false, /*bAutoDestroy*/ true);
	if (Component == nullptr)
	{
		return nullptr;
	}
	Component->SoundClassOverride = EffectsClass;
	Component->bIsUISound = Row->bUI;
	const bool bPlaced = Row->bSpatial && Location != nullptr;
	if (bPlaced)
	{
		// Panned by the place on screen (the listener is the top-down camera), never faded by distance.
		FSoundAttenuationSettings Placement;
		Placement.bAttenuate = false;
		Placement.bSpatialize = true;
		Component->bAllowSpatialization = true;
		Component->bOverrideAttenuation = true;
		Component->AttenuationOverrides = Placement;
		Component->SetWorldLocation(*Location);
	}
	Component->Play();

	Active.Add(Component);
	++RowStats.Played;
	RowStats.PeakActive = FMath::Max(RowStats.PeakActive, Active.Num());
	UE_LOG(LogRRAudio, Verbose, TEXT("Play %s (row %s): %s, class %s, active %d/%d, pitch %.3f, %s%s."), *Event.ToString(), *Row->Event.ToString(),
		*Sound->GetPathName(), *GetNameSafe(EffectsClass), Active.Num(), Row->MaxConcurrent, Pitch, bPlaced ? TEXT("placed") : TEXT("centred"),
		Row->bUI ? TEXT(", plays paused") : TEXT(""));
	return Component;
}

void URRAudioSubsystem::LogStats() const
{
	UE_LOG(LogRRAudio, Log, TEXT("Sound events (class %s):"), *GetNameSafe(EffectsClass));
	for (const FRRSoundEventRow& Row : GetDefault<URRAudioSettings>()->Events)
	{
		const FRRSoundEventStats* RowStats = Stats.Find(Row.Event);
		const TArray<TWeakObjectPtr<UAudioComponent>>* Active = ActivePlays.Find(Row.Event);
		int32 ActiveNow = 0;
		if (Active)
		{
			for (const TWeakObjectPtr<UAudioComponent>& Play : *Active)
			{
				ActiveNow += Play.IsValid() && Play->IsPlaying() ? 1 : 0;
			}
		}
		UE_LOG(LogRRAudio, Log, TEXT("  %s: sound %s, played %d, cap %d, peak %d, cap stops %d, active %d."), *Row.Event.ToString(),
			Row.Sound.IsNull() ? TEXT("none") : *Row.Sound.ToString(), RowStats ? RowStats->Played : 0, Row.MaxConcurrent,
			RowStats ? RowStats->PeakActive : 0, RowStats ? RowStats->CapStops : 0, ActiveNow);
	}

	// Volumes the audio device applies now to the Effects and Music classes (options mix included), read on the audio thread.
	const UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	FAudioDevice* Device = World ? World->GetAudioDeviceRaw() : nullptr;
	USoundClass* MusicClass = GetDefault<URROptionsSettings>()->MusicSoundClass.LoadSynchronous();
	if (Device != nullptr)
	{
		USoundClass* Effects = EffectsClass;
		FAudioThread::RunCommandOnAudioThread([Device, Effects, MusicClass]()
		{
			const FSoundClassProperties* EffectsProps = Device->GetSoundClassCurrentProperties(Effects);
			const FSoundClassProperties* MusicProps = Device->GetSoundClassCurrentProperties(MusicClass);
			UE_LOG(LogRRAudio, Log, TEXT("  device volumes: %s %.3f, %s %.3f."), *GetNameSafe(Effects), EffectsProps ? EffectsProps->Volume : -1.f,
				*GetNameSafe(MusicClass), MusicProps ? MusicProps->Volume : -1.f);
		});
	}
}

TSharedPtr<SWidget> URRAudioSubsystem::FindButtonAt(const FVector2D& ScreenPosition, bool& bOutDisabledButton) const
{
	bOutDisabledButton = false;
	const UGameInstance* GameInstance = GetGameInstance();
	const UGameViewportClient* Viewport = GameInstance ? GameInstance->GetGameViewportClient() : nullptr;
	const TSharedPtr<SWindow> GameWindow = Viewport ? Viewport->GetWindow() : nullptr;
	if (!GameWindow.IsValid() || !FSlateApplication::IsInitialized())
	{
		return nullptr;
	}
	// Only the game window: editor buttons around a PIE viewport never play.
	TArray<TSharedRef<SWindow>> Windows;
	Windows.Add(GameWindow.ToSharedRef());
	const FWidgetPath Path = FSlateApplication::Get().LocateWindowUnderMouse(ScreenPosition, Windows, /*bIgnoreEnabledStatus*/ true);
	if (!Path.IsValid())
	{
		return nullptr;
	}
	for (int32 Index = Path.Widgets.Num() - 1; Index >= 0; --Index)
	{
		const TSharedRef<SWidget>& Widget = Path.Widgets[Index].Widget;
		if (Widget->GetType() != RRAudio::ButtonType)
		{
			continue;
		}
		for (int32 Parent = 0; Parent <= Index; ++Parent)
		{
			if (!Path.Widgets[Parent].Widget->IsEnabled())
			{
				bOutDisabledButton = true;
				return nullptr;
			}
		}
		return Widget;
	}
	return nullptr;
}

bool URRAudioSubsystem::TickHover(float DeltaTime)
{
	if (!FSlateApplication::IsInitialized())
	{
		return true;
	}
	bool bDisabled = false;
	const TSharedPtr<SWidget> Button = FindButtonAt(FSlateApplication::Get().GetCursorPos(), bDisabled);
	if (Button != HoveredButton.Pin())
	{
		HoveredButton = Button;
		if (Button.IsValid())
		{
			Play(GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr, RRAudio::UIHoverEvent, nullptr);
		}
	}
	return true;
}

void URRAudioSubsystem::HandleMousePress(const FVector2D& ScreenPosition)
{
	bool bDisabled = false;
	if (FindButtonAt(ScreenPosition, bDisabled).IsValid())
	{
		Play(GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr, RRAudio::UIClickEvent, nullptr);
	}
}
