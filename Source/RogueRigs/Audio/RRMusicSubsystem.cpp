// Rogue Rigs - music per screen (task-519).

#include "Audio/RRMusicSubsystem.h"

#include "Audio/RRAudioLog.h"
#include "Audio/RRAudioSettings.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Menu/RRMainMenuGameMode.h"
#include "Run/RRRunTransitionSubsystem.h"
#include "Settings/RROptionsSettings.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundWave.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRMusicSubsystem)

const FName URRMusicSubsystem::MainMenuTrack(TEXT("MainMenu"));
const FName URRMusicSubsystem::LoadingTrack(TEXT("Loading"));
const FName URRMusicSubsystem::InGameTrack(TEXT("InGame"));

URRMusicSubsystem* URRMusicSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URRMusicSubsystem>() : nullptr;
}

void URRMusicSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MusicClass = GetDefault<URROptionsSettings>()->MusicSoundClass.LoadSynchronous();
	if (MusicClass == nullptr)
	{
		UE_LOG(LogRRAudio, Warning, TEXT("URRMusicSubsystem: Music sound class not found (URROptionsSettings::MusicSoundClass); music plays in the default class."));
	}
	// Load every track now, so a screen change never waits for a load.
	for (const FRRMusicTrackRow& Row : GetDefault<URRAudioSettings>()->Music)
	{
		USoundBase* Sound = Row.Sound.IsNull() ? nullptr : Row.Sound.LoadSynchronous();
		if (!Row.Sound.IsNull() && Sound == nullptr)
		{
			UE_LOG(LogRRAudio, Warning, TEXT("Music track %s: sound %s not found; that screen stays silent."), *Row.Track.ToString(), *Row.Sound.ToString());
		}
		LoadedSounds.Add(Row.Track, Sound);
	}

	if (URRRunTransitionSubsystem* Transition = Collection.InitializeDependency<URRRunTransitionSubsystem>())
	{
		Transition->OnLevelReady.AddUniqueDynamic(this, &URRMusicSubsystem::HandleLevelReady);
	}
	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &URRMusicSubsystem::Tick));
}

void URRMusicSubsystem::Deinitialize()
{
	FTSTicker::RemoveTicker(TickerHandle);
	if (CurrentComponent)
	{
		CurrentComponent->Stop();
	}
	for (UAudioComponent* Fading : FadingComponents)
	{
		if (Fading)
		{
			Fading->Stop();
		}
	}
	CurrentComponent = nullptr;
	FadingComponents.Reset();
	CurrentTrack = NAME_None;
	Super::Deinitialize();
}

void URRMusicSubsystem::HandleLevelReady(int32 LevelIndex, const FString& LayoutName)
{
	// A new level: the in-game track starts again from its start (Q13), even in the same world (prepared next level).
	++LevelSerial;
	UE_LOG(LogRRAudio, Verbose, TEXT("Music: level %d ready (%s), in-game track restarts."), LevelIndex, *LayoutName);
}

bool URRMusicSubsystem::GetWantedTrack(FName& OutTrack, UWorld*& OutWorld) const
{
	const UGameInstance* GameInstance = GetGameInstance();
	UWorld* World = GameInstance ? GameInstance->GetWorld() : nullptr;
	if (World == nullptr || World->bIsTearingDown || !World->IsGameWorld())
	{
		return false;
	}
	OutWorld = World;
	const URRRunTransitionSubsystem* Transition = GameInstance->GetSubsystem<URRRunTransitionSubsystem>();
	if (Transition && Transition->IsLevelLoading())
	{
		// Continue on an already prepared level: no loading screen, only a short screen fade out: the old track fades, nothing starts.
		OutTrack = Transition->GetScreenFade() == ERRTransitionFade::FadingOut ? NAME_None : LoadingTrack;
		return true;
	}
	const AGameModeBase* GameMode = World->GetAuthGameMode();
	if (GameMode == nullptr)
	{
		return false;
	}
	OutTrack = GameMode->IsA<ARRMainMenuGameMode>() ? MainMenuTrack : InGameTrack;
	return true;
}

bool URRMusicSubsystem::Tick(float DeltaTime)
{
	for (int32 Index = FadingComponents.Num() - 1; Index >= 0; --Index)
	{
		const UAudioComponent* Fading = FadingComponents[Index];
		if (Fading == nullptr || !Fading->IsPlaying())
		{
			FadingComponents.RemoveAt(Index);
		}
	}

	FName Wanted;
	UWorld* World = nullptr;
	if (!GetWantedTrack(Wanted, World))
	{
		return true;
	}
	if (Wanted != CurrentTrack)
	{
		SwitchTo(Wanted, World);
	}
	else if (Wanted == InGameTrack && (StartedLevelSerial != LevelSerial || StartedWorld.Get() != World))
	{
		UE_LOG(LogRRAudio, Verbose, TEXT("Music %s: new level, restarts from its start."), *Wanted.ToString());
		SwitchTo(Wanted, World);
	}
	else if (CurrentComponent && !CurrentComponent->IsPlaying())
	{
		// Stopped from outside (audio device flush): the screen still wants it.
		UE_LOG(LogRRAudio, Verbose, TEXT("Music %s: play stopped from outside, started again."), *Wanted.ToString());
		SwitchTo(Wanted, World, 0.f, /*bFade*/ false);
	}
	return true;
}

void URRMusicSubsystem::SwitchTo(FName Track, UWorld* World, float StartSeconds, bool bFade)
{
	const URRAudioSettings* Settings = GetDefault<URRAudioSettings>();
	const float FadeOut = bFade ? FMath::Max(Settings->MusicFadeOut, 0.f) : 0.f;
	const float FadeIn = bFade ? FMath::Max(Settings->MusicFadeIn, 0.f) : 0.f;
	const FName OldTrack = CurrentTrack;
	if (CurrentComponent)
	{
		CurrentComponent->OnAudioPlaybackPercentNative.RemoveAll(this);
		if (FadeOut > 0.f && CurrentComponent->IsPlaying())
		{
			CurrentComponent->FadeOut(FadeOut, 0.f);
			FadingComponents.Add(CurrentComponent);
		}
		else
		{
			CurrentComponent->Stop();
		}
	}
	CurrentComponent = nullptr;
	CurrentTrack = Track;
	StartedWorld = World;
	StartedLevelSerial = LevelSerial;
	LastPercent = 0.f;
	Passes = 0;

	const FRRMusicTrackRow* Row = Track.IsNone() ? nullptr : Settings->FindMusic(Track);
	const TObjectPtr<USoundBase>* Loaded = Row ? LoadedSounds.Find(Track) : nullptr;
	USoundBase* Sound = Loaded ? Loaded->Get() : nullptr;
	if (Sound == nullptr)
	{
		if (!Track.IsNone() && !ReportedSilent.Contains(Track))
		{
			ReportedSilent.Add(Track);
			UE_LOG(LogRRAudio, Verbose, TEXT("Music track %s has no sound; silent."), *Track.ToString());
		}
		UE_LOG(LogRRAudio, Log, TEXT("Music %s -> %s: silence, fade out %.2f s."), *OldTrack.ToString(), *Track.ToString(), FadeOut);
		return;
	}
	// Persists across map travels (loading on both sides of the travel) and plays as a UI sound, so a paused game keeps it (Q12).
	UAudioComponent* Component = UGameplayStatics::CreateSound2D(World, Sound, Row->Volume, 1.f, 0.f, nullptr, /*bPersistAcrossLevelTransition*/ true, /*bAutoDestroy*/ false);
	if (Component == nullptr)
	{
		UE_LOG(LogRRAudio, Verbose, TEXT("Music %s -> %s: no audio, not played."), *OldTrack.ToString(), *Track.ToString());
		return;
	}
	Component->SoundClassOverride = MusicClass;
	Component->bIsUISound = true;
	Component->OnAudioPlaybackPercentNative.AddUObject(this, &URRMusicSubsystem::HandlePlaybackPercent);
	if (FadeIn > 0.f)
	{
		Component->FadeIn(FadeIn, 1.f, StartSeconds);
	}
	else
	{
		Component->Play(StartSeconds);
	}
	CurrentComponent = Component;
	PlayStartTime = FPlatformTime::Seconds();
	PlayStartOffset = StartSeconds;
	// GetDuration of a looping wave is "indefinite": the length of one pass is the wave's own Duration.
	const USoundWave* Wave = Cast<USoundWave>(Sound);
	const float Duration = Wave ? Wave->Duration : Sound->GetDuration();
	LastPercent = Duration > 0.f ? StartSeconds / Duration : 0.f;
	bPercentValid = false;
	UE_LOG(LogRRAudio, Log, TEXT("Music %s -> %s: %s, class %s, volume %.2f, from %.1f s of %.1f s, fade in %.2f s, old track fade out %.2f s."),
		*OldTrack.ToString(), *Track.ToString(), *Sound->GetPathName(), *GetNameSafe(MusicClass), Row->Volume, StartSeconds, Duration, FadeIn, FadeOut);
}

void URRMusicSubsystem::HandlePlaybackPercent(const UAudioComponent* Component, const USoundWave* Wave, const float Percent)
{
	if (Component != CurrentComponent)
	{
		return;
	}
	// Exactly 1.0 = a source that does not know its frame count yet; the first values of a new play may also be the stale percent
	// of the source it reuses (below 1): only a step from below a whole number to above it counts as a loop.
	if (Percent == 1.f)
	{
		return;
	}
	if (!bPercentValid)
	{
		bPercentValid = true;
		LastPercent = Percent;
		return;
	}
	// A looping wave reports frames played / frames of the wave: one more whole unit = the track ran into its start again.
	if (FMath::FloorToInt(Percent) > FMath::FloorToInt(LastPercent))
	{
		++Passes;
		UE_LOG(LogRRAudio, Log, TEXT("Music %s looped: restarts from its start (pass %d, %.1f s after the play start)."), *CurrentTrack.ToString(),
			Passes + 1, FPlatformTime::Seconds() - PlayStartTime);
	}
	LastPercent = Percent;
}

void URRMusicSubsystem::LogState() const
{
	const URRAudioSettings* Settings = GetDefault<URRAudioSettings>();
	const FRRMusicTrackRow* Row = CurrentTrack.IsNone() ? nullptr : Settings->FindMusic(CurrentTrack);
	UE_LOG(LogRRAudio, Log, TEXT("Music: track %s, sound %s, volume %.2f, class %s, playing %s, %.1f s since the play start (from %.1f s), percent %.3f, loops %d, fading out %d; fades in %.2f s, out %.2f s."),
		CurrentTrack.IsNone() ? TEXT("none") : *CurrentTrack.ToString(), Row ? *Row->Sound.ToString() : TEXT("none"), Row ? Row->Volume : 0.f,
		*GetNameSafe(MusicClass), CurrentComponent && CurrentComponent->IsPlaying() ? TEXT("yes") : TEXT("no"),
		CurrentComponent ? FPlatformTime::Seconds() - PlayStartTime : 0.0, PlayStartOffset, LastPercent, Passes, FadingComponents.Num(),
		Settings->MusicFadeIn, Settings->MusicFadeOut);
}

void URRMusicSubsystem::SeekCurrent(float StartSeconds)
{
	FName Wanted;
	UWorld* World = nullptr;
	if (CurrentTrack.IsNone() || !GetWantedTrack(Wanted, World))
	{
		UE_LOG(LogRRAudio, Warning, TEXT("Music: no track to seek."));
		return;
	}
	SwitchTo(CurrentTrack, World, FMath::Max(StartSeconds, 0.f), /*bFade*/ false);
}
