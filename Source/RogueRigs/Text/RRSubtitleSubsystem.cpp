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
