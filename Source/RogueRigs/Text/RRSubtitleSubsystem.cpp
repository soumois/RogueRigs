// Rogue Rigs - subtitle subsystem: one voice line at a time (speaker prefix, timed in C++), raised by the company alert events, shown by the HUD subtitle widget.

#include "Text/RRSubtitleSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "RogueRigs.h"
#include "Text/RRTextContentSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRSubtitleSubsystem)

#define LOCTEXT_NAMESPACE "RRSubtitles"

const FName URRSubtitleSubsystem::SpeakerKesslerVoss(TEXT("kessler_voss"));
const FName URRSubtitleSubsystem::SpeakerRook(TEXT("rook"));
const FName URRSubtitleSubsystem::SpeakerVesna(TEXT("vesna"));

URRSubtitleSubsystem::URRSubtitleSubsystem()
{
	Speakers = {
		{ SpeakerKesslerVoss, LOCTEXT("SpeakerKesslerVoss", "Kessler-Voss") },
		{ SpeakerRook, LOCTEXT("SpeakerRook", "Rook") },
		{ SpeakerVesna, LOCTEXT("SpeakerVesna", "Vesna") }
	};
}

bool URRSubtitleSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URRSubtitleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// The game instance is set on the world before InitWorld (UEngine::LoadMap), so the transition is reachable here.
	if (URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this))
	{
		BoundTransition = Transition;
		Transition->OnIntermissionStarted.AddUniqueDynamic(this, &URRSubtitleSubsystem::HandleIntermissionStarted);
		Transition->OnLoadingStarted.AddUniqueDynamic(this, &URRSubtitleSubsystem::HandleLoadingStarted);
		Transition->OnLevelReady.AddUniqueDynamic(this, &URRSubtitleSubsystem::HandleLevelReady);
		if (Transition->GetPhase() != ERRRunTransitionPhase::None)
		{
			SetSilenced(true, Transition->IsLevelLoading() ? TEXT("loading") : TEXT("intermission"));
		}
	}
}

void URRSubtitleSubsystem::Deinitialize()
{
	if (URRRunTransitionSubsystem* Transition = BoundTransition.Get())
	{
		Transition->OnIntermissionStarted.RemoveDynamic(this, &URRSubtitleSubsystem::HandleIntermissionStarted);
		Transition->OnLoadingStarted.RemoveDynamic(this, &URRSubtitleSubsystem::HandleLoadingStarted);
		Transition->OnLevelReady.RemoveDynamic(this, &URRSubtitleSubsystem::HandleLevelReady);
	}
	BoundTransition.Reset();
	Super::Deinitialize();
}

void URRSubtitleSubsystem::HandleIntermissionStarted(const FRRIntermissionRecap& Recap)
{
	SetSilenced(true, TEXT("intermission"));
}

void URRSubtitleSubsystem::HandleLoadingStarted(int32 LevelIndex, const FString& LayoutName)
{
	SetSilenced(true, TEXT("loading"));
}

void URRSubtitleSubsystem::HandleLevelReady(int32 LevelIndex, const FString& LayoutName)
{
	SetSilenced(false, TEXT("level ready"));
}

void URRSubtitleSubsystem::SetSilenced(bool bInSilenced, const TCHAR* Reason)
{
	const bool bHadSubtitle = bActive;
	if (bInSilenced)
	{
		Clear();
	}
	if (bSilenced == bInSilenced)
	{
		return;
	}
	bSilenced = bInSilenced;
	UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: subtitles %s (%s)%s."), bSilenced ? TEXT("silenced") : TEXT("allowed again"), Reason,
		bHadSubtitle ? TEXT(", current line cleared") : TEXT(""));
}

URRSubtitleSubsystem* URRSubtitleSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	return World ? World->GetSubsystem<URRSubtitleSubsystem>() : nullptr;
}

FText URRSubtitleSubsystem::ComposeSubtitle(const FText& Speaker, const FText& Text)
{
	return Speaker.IsEmpty() ? Text : FText::Format(LOCTEXT("SubtitleFormat", "{0}: {1}"), Speaker, Text);
}

bool URRSubtitleSubsystem::Show(FName SpeakerId, FText DisplayName, FText Text, float DurationOverride)
{
	if (Text.IsEmpty())
	{
		return false;
	}
	if (bSilenced)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: '%s' dropped (run transition, subtitles silenced)."), *Text.ToString());
		return false;
	}
	bActive = true;
	CurrentSpeakerId = SpeakerId;
	CurrentSpeaker = DisplayName.IsEmpty() ? GetSpeakerDisplayName(SpeakerId) : DisplayName;
	CurrentText = Text;
	CurrentSubtitle = ComposeSubtitle(CurrentSpeaker, CurrentText);
	CurrentDuration = DurationOverride > 0.f ? DurationOverride : GetDurationForText(Text);
	RemainingSeconds = CurrentDuration;
	UE_LOG(LogRogueRigs, Verbose, TEXT("URRSubtitleSubsystem: '%s' for %.2f s."), *CurrentSubtitle.ToString(), CurrentDuration);
	OnSubtitleShown.Broadcast(CurrentSpeakerId, CurrentSpeaker, CurrentText, CurrentSubtitle, CurrentDuration);
	return true;
}

bool URRSubtitleSubsystem::ShowAlertLine(FName EventId)
{
	URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(this);
	FText Line;
	if (!TextContent || !TextContent->GetAlertLine(EventId, Line))
	{
		return false;
	}
	return Show(SpeakerKesslerVoss, FText::GetEmpty(), Line);
}

void URRSubtitleSubsystem::Clear()
{
	if (!bActive)
	{
		return;
	}
	bActive = false;
	RemainingSeconds = 0.f;
	OnSubtitleCleared.Broadcast();
}

FText URRSubtitleSubsystem::GetSpeakerDisplayName(FName SpeakerId) const
{
	const FRRSubtitleSpeaker* Speaker = Speakers.FindByPredicate([SpeakerId](const FRRSubtitleSpeaker& Entry) { return Entry.Id == SpeakerId; });
	return Speaker ? Speaker->DisplayName : FText::GetEmpty();
}

float URRSubtitleSubsystem::GetDurationForText(const FText& Text) const
{
	return FMath::Max(0.f, BaseSeconds) + FMath::Max(0.f, SecondsPerCharacter) * Text.ToString().Len();
}

void URRSubtitleSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bActive)
	{
		RemainingSeconds -= DeltaTime;
		if (RemainingSeconds <= 0.f)
		{
			Clear();
		}
	}
}

TStatId URRSubtitleSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRSubtitleSubsystem, STATGROUP_Tickables);
}

#undef LOCTEXT_NAMESPACE
