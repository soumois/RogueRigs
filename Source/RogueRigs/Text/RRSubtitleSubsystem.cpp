// Rogue Rigs - subtitle subsystem: one voice line at a time (speaker prefix, timed in C++), raised by the company alert events, shown by the HUD subtitle widget.

#include "Text/RRSubtitleSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRGameMode.h"
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
		{ SpeakerKesslerVoss, LOCTEXT("SpeakerKesslerVoss", "Kessler-Voss"), false },
		{ SpeakerRook, LOCTEXT("SpeakerRook", "Rook") },
		{ SpeakerVesna, LOCTEXT("SpeakerVesna", "Vesna") }
	};
	BarkTriggers = {
		FName(TEXT("horde_incoming")), FName(TEXT("console_dropped")), FName(TEXT("console_hacked")),
		FName(TEXT("bonus_chest_opened")), FName(TEXT("shuttle_landed"))
	};
	BarkTriggersAfterAlert = { FName(TEXT("horde_incoming")), FName(TEXT("console_dropped")), FName(TEXT("shuttle_landed")) };
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
	if (URRRunFlowSubsystem* RunFlow = BoundRunFlow.Get())
	{
		RunFlow->OnRunEnded.RemoveDynamic(this, &URRSubtitleSubsystem::HandleRunEnded);
	}
	if (ARRGameMode* GameMode = BoundGameMode.Get())
	{
		GameMode->OnGameOver.RemoveDynamic(this, &URRSubtitleSubsystem::HandleGameOver);
	}
	BoundRunFlow.Reset();
	BoundGameMode.Reset();
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
	// The run end keeps the silence for the rest of this world (task-211-01).
	if (!bRunEndSilenced)
	{
		SetSilenced(false, TEXT("level ready"));
	}
	// A new level starts without bark cooldown (task-497 Q4).
	LastBarkTime = -1.0;
}

void URRSubtitleSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (URRRunFlowSubsystem* RunFlow = InWorld.GetSubsystem<URRRunFlowSubsystem>())
	{
		BoundRunFlow = RunFlow;
		RunFlow->OnRunEnded.AddUniqueDynamic(this, &URRSubtitleSubsystem::HandleRunEnded);
	}
	if (ARRGameMode* GameMode = InWorld.GetAuthGameMode<ARRGameMode>())
	{
		BoundGameMode = GameMode;
		GameMode->OnGameOver.AddUniqueDynamic(this, &URRSubtitleSubsystem::HandleGameOver);
	}
	UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: world begin play, subtitles %s (run end %s, game over %s)."),
		bSilenced ? TEXT("silenced") : TEXT("allowed"), BoundRunFlow.IsValid() ? TEXT("bound") : TEXT("unbound"),
		BoundGameMode.IsValid() ? TEXT("bound") : TEXT("unbound"));
}

void URRSubtitleSubsystem::HandleRunEnded(bool bSuccess)
{
	bRunEndSilenced = true;
	SetSilenced(true, bSuccess ? TEXT("run ended, run won") : TEXT("run ended, shuttle missed"));
}

void URRSubtitleSubsystem::HandleGameOver()
{
	bRunEndSilenced = true;
	SetSilenced(true, TEXT("game over, miner dead"));
}

void URRSubtitleSubsystem::SetSilenced(bool bInSilenced, const TCHAR* Reason)
{
	const bool bHadSubtitle = bActive;
	if (bInSilenced)
	{
		PendingBarkTrigger = NAME_None;
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
	CurrentSubtitle = ShowsSpeakerPrefix(SpeakerId) ? ComposeSubtitle(CurrentSpeaker, CurrentText) : CurrentText;
	CurrentDuration = DurationOverride > 0.f ? DurationOverride : GetDurationForText(Text);
	RemainingSeconds = CurrentDuration;
	UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: '%s' for %.2f s."), *CurrentSubtitle.ToString(), CurrentDuration);
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
	const bool bReplacesBark = bActive && CurrentSpeakerId != SpeakerKesslerVoss;
	const FName ReplacedSpeaker = CurrentSpeakerId;
	const bool bShown = Show(SpeakerKesslerVoss, FText::GetEmpty(), Line);
	if (bShown && bReplacesBark)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: alert '%s' replaces the %s bark."), *EventId.ToString(), *ReplacedSpeaker.ToString());
	}
	return bShown;
}

float URRSubtitleSubsystem::GetBarkCooldownRemaining() const
{
	const UWorld* World = GetWorld();
	if (LastBarkTime < 0.0 || !World)
	{
		return 0.f;
	}
	return FMath::Max(0.f, static_cast<float>(LastBarkTime + BarkCooldownSeconds - World->GetTimeSeconds()));
}

bool URRSubtitleSubsystem::ShowBark(FName TriggerId)
{
	const TCHAR* DropReason = nullptr;
	if (!BarkTriggers.Contains(TriggerId))
	{
		DropReason = TEXT("not a bark trigger");
	}
	else if (bSilenced)
	{
		DropReason = TEXT("subtitles silenced");
	}
	else if (bActive)
	{
		DropReason = CurrentSpeakerId == SpeakerKesslerVoss ? TEXT("alert line shown") : TEXT("a bark is shown");
	}
	else if (GetBarkCooldownRemaining() > 0.f)
	{
		DropReason = TEXT("cooldown");
	}
	if (DropReason && bActive && CurrentSpeakerId == SpeakerKesslerVoss && !bSilenced && BarkTriggersAfterAlert.Contains(TriggerId))
	{
		PendingBarkTrigger = TriggerId;
		UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: bark '%s' waits for the alert line to end (%.1f s left)."), *TriggerId.ToString(),
			RemainingSeconds);
		return false;
	}
	if (DropReason)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: bark '%s' dropped (%s, cooldown left %.1f s)."), *TriggerId.ToString(), DropReason,
			GetBarkCooldownRemaining());
		return false;
	}

	URRTextContentSubsystem* TextContent = URRTextContentSubsystem::Get(this);
	FName SpeakerId;
	FText Line;
	if (!TextContent || !TextContent->PickBarkLine(TriggerId, SpeakerId, Line) || !Show(SpeakerId, FText::GetEmpty(), Line))
	{
		return false;
	}
	const UWorld* World = GetWorld();
	LastBarkTime = World ? World->GetTimeSeconds() : 0.0;
	UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: bark '%s' shown (%s) at %.2f s, next bark after %.0f s."), *TriggerId.ToString(),
		*SpeakerId.ToString(), LastBarkTime, BarkCooldownSeconds);
	return true;
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

bool URRSubtitleSubsystem::ShowsSpeakerPrefix(FName SpeakerId) const
{
	const FRRSubtitleSpeaker* Speaker = Speakers.FindByPredicate([SpeakerId](const FRRSubtitleSpeaker& Entry) { return Entry.Id == SpeakerId; });
	return !Speaker || Speaker->bShowPrefix;
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
			const bool bAlertEnded = CurrentSpeakerId == SpeakerKesslerVoss;
			Clear();
			if (bAlertEnded && !PendingBarkTrigger.IsNone())
			{
				const FName Pending = PendingBarkTrigger;
				PendingBarkTrigger = NAME_None;
				UE_LOG(LogRogueRigs, Log, TEXT("URRSubtitleSubsystem: alert line ended, trying the waiting bark '%s'."), *Pending.ToString());
				ShowBark(Pending);
			}
		}
	}
}

TStatId URRSubtitleSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRSubtitleSubsystem, STATGROUP_Tickables);
}

#undef LOCTEXT_NAMESPACE
