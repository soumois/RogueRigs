// Rogue Rigs - automation tests of the subtitle subsystem (RogueRigs.Subtitles.*): replace, speaker prefix, duration formula, auto clear, delegate order.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Text/RRSubtitleSubsystem.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSubtitleReplaceTest, "RogueRigs.Subtitles.ReplaceAndPrefix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSubtitleReplaceTest::RunTest(const FString& Parameters)
{
	RRTestHarness::FTestWorld TestWorld(TEXT("RRSubtitleReplaceWorld"));
	URRSubtitleSubsystem* Subtitles = TestWorld.Get()->GetSubsystem<URRSubtitleSubsystem>();
	if (!TestNotNull(TEXT("subtitle subsystem"), Subtitles))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Subtitles->OnSubtitleShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleSubtitleShown);
	Subtitles->OnSubtitleCleared.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleSubtitleCleared);

	TestFalse(TEXT("nothing shown at start"), Subtitles->HasSubtitle());
	TestFalse(TEXT("empty line refused"), Subtitles->Show(URRSubtitleSubsystem::SpeakerKesslerVoss, FText::GetEmpty(), FText::GetEmpty()));
	TestEqual(TEXT("empty line: no broadcast"), Recorder->SubtitleEvents.Num(), 0);

	// Kessler-Voss lines show without a prefix (table bShowPrefix false); the speaker name is still reported.
	TestTrue(TEXT("first line shown"), Subtitles->Show(URRSubtitleSubsystem::SpeakerKesslerVoss, FText::GetEmpty(), FText::FromString(TEXT("First line."))));
	TestEqual(TEXT("Kessler-Voss: no prefix"), Subtitles->GetCurrentSubtitle().ToString(), FString(TEXT("First line.")));
	TestEqual(TEXT("speaker name"), Subtitles->GetCurrentSpeaker().ToString(), FString(TEXT("Kessler-Voss")));
	TestEqual(TEXT("speaker id"), Subtitles->GetCurrentSpeakerId(), URRSubtitleSubsystem::SpeakerKesslerVoss);
	TestEqual(TEXT("line without speaker"), Subtitles->GetCurrentText().ToString(), FString(TEXT("First line.")));
	TestEqual(TEXT("Rook name"), Subtitles->GetSpeakerDisplayName(URRSubtitleSubsystem::SpeakerRook).ToString(), FString(TEXT("Rook")));
	TestFalse(TEXT("Kessler-Voss: prefix off"), Subtitles->ShowsSpeakerPrefix(URRSubtitleSubsystem::SpeakerKesslerVoss));
	TestTrue(TEXT("Rook: prefix on"), Subtitles->ShowsSpeakerPrefix(URRSubtitleSubsystem::SpeakerRook));
	TestTrue(TEXT("Vesna: prefix on"), Subtitles->ShowsSpeakerPrefix(URRSubtitleSubsystem::SpeakerVesna));
	TestEqual(TEXT("Vesna name"), Subtitles->GetSpeakerDisplayName(URRSubtitleSubsystem::SpeakerVesna).ToString(), FString(TEXT("Vesna")));

	// A new line replaces the current one, without a clear in between.
	Subtitles->Tick(1.f);
	TestTrue(TEXT("second line shown"), Subtitles->Show(URRSubtitleSubsystem::SpeakerRook, FText::GetEmpty(), FText::FromString(TEXT("Second line."))));
	TestEqual(TEXT("replaced"), Subtitles->GetCurrentSubtitle().ToString(), FString(TEXT("Rook: Second line.")));
	TestEqual(TEXT("full time for the new line"), Subtitles->GetRemainingSeconds(), Subtitles->GetDurationForText(FText::FromString(TEXT("Second line."))));

	// Explicit display name wins; unknown speaker without a name has no prefix.
	Subtitles->Show(TEXT("someone"), FText::FromString(TEXT("Guard")), FText::FromString(TEXT("Halt.")));
	TestEqual(TEXT("explicit name"), Subtitles->GetCurrentSubtitle().ToString(), FString(TEXT("Guard: Halt.")));
	Subtitles->Show(TEXT("someone"), FText::GetEmpty(), FText::FromString(TEXT("Halt.")));
	TestEqual(TEXT("unknown speaker: no prefix"), Subtitles->GetCurrentSubtitle().ToString(), FString(TEXT("Halt.")));

	const TArray<FString> Expected = { TEXT("Shown:kessler_voss:First line."), TEXT("Shown:rook:Rook: Second line."),
		TEXT("Shown:someone:Guard: Halt."), TEXT("Shown:someone:Halt.") };
	TestEqual(TEXT("one shown per line, no clear on replace"), FString::Join(Recorder->SubtitleEvents, TEXT("|")), FString::Join(Expected, TEXT("|")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSubtitleDurationTest, "RogueRigs.Subtitles.DurationAndClear",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSubtitleDurationTest::RunTest(const FString& Parameters)
{
	RRTestHarness::FTestWorld TestWorld(TEXT("RRSubtitleDurationWorld"));
	URRSubtitleSubsystem* Subtitles = TestWorld.Get()->GetSubsystem<URRSubtitleSubsystem>();
	if (!TestNotNull(TEXT("subtitle subsystem"), Subtitles))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Subtitles->OnSubtitleShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleSubtitleShown);
	Subtitles->OnSubtitleCleared.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleSubtitleCleared);

	// Formula: 4 s + 0.06 s per character of the line (the speaker prefix does not count).
	const FText Line = FText::FromString(FString::ChrN(50, TEXT('a')));
	TestEqual(TEXT("50 characters: 7 s"), Subtitles->GetDurationForText(Line), 7.f, 1e-4f);
	TestEqual(TEXT("empty: 4 s"), Subtitles->GetDurationForText(FText::GetEmpty()), 4.f, 1e-4f);
	Subtitles->Show(URRSubtitleSubsystem::SpeakerKesslerVoss, FText::GetEmpty(), Line);
	TestEqual(TEXT("duration of the shown line"), Subtitles->GetCurrentDuration(), 7.f, 1e-4f);
	TestEqual(TEXT("duration broadcast"), Recorder->SubtitleDurations.Num() == 1 ? Recorder->SubtitleDurations[0] : -1.f, 7.f, 1e-4f);

	// Ticked down in C++, cleared once at the end.
	Subtitles->Tick(6.5f);
	TestTrue(TEXT("still shown at 6.5 s"), Subtitles->HasSubtitle());
	TestEqual(TEXT("0.5 s left"), Subtitles->GetRemainingSeconds(), 0.5f, 1e-4f);
	Subtitles->Tick(0.6f);
	TestFalse(TEXT("cleared after 7 s"), Subtitles->HasSubtitle());
	TestTrue(TEXT("no current subtitle"), Subtitles->GetCurrentSubtitle().IsEmpty());
	TestEqual(TEXT("no speaker"), Subtitles->GetCurrentSpeakerId(), FName(NAME_None));
	Subtitles->Tick(1.f);

	// Explicit duration, then Clear.
	Subtitles->Show(URRSubtitleSubsystem::SpeakerVesna, FText::GetEmpty(), Line, 2.f);
	TestEqual(TEXT("override duration"), Subtitles->GetCurrentDuration(), 2.f);
	Subtitles->Clear();
	TestFalse(TEXT("cleared by Clear"), Subtitles->HasSubtitle());
	Subtitles->Clear();

	const FString Shown = FString::Printf(TEXT(": %s"), *Line.ToString());
	const TArray<FString> Expected = { TEXT("Shown:kessler_voss:") + Line.ToString(), TEXT("Cleared"), TEXT("Shown:vesna:Vesna") + Shown, TEXT("Cleared") };
	TestEqual(TEXT("delegate order: shown, cleared once per line, nothing when idle"), FString::Join(Recorder->SubtitleEvents, TEXT("|")), FString::Join(Expected, TEXT("|")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRSubtitleBarkTest, "RogueRigs.Subtitles.Barks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRSubtitleBarkTest::RunTest(const FString& Parameters)
{
	RRTestHarness::FTestWorld TestWorld(TEXT("RRSubtitleBarkWorld"));
	UWorld* World = TestWorld.Get();
	URRSubtitleSubsystem* Subtitles = World->GetSubsystem<URRSubtitleSubsystem>();
	if (!TestNotNull(TEXT("subtitle subsystem"), Subtitles))
	{
		return false;
	}
	const FName Hacked(TEXT("console_hacked"));
	const FName Opened(TEXT("bonus_chest_opened"));

	// A5: a trigger outside the key events never shows a bark.
	TestFalse(TEXT("level_up refused"), Subtitles->ShowBark(TEXT("level_up")));
	TestFalse(TEXT("nothing shown"), Subtitles->HasSubtitle());

	// A1: first bark of the world shows at once, with the speaker name.
	if (!TestTrue(TEXT("first bark shown"), Subtitles->ShowBark(Hacked)))
	{
		return false;
	}
	const FName Speaker = Subtitles->GetCurrentSpeakerId();
	TestTrue(TEXT("speaker is Rook or Vesna"), Speaker == URRSubtitleSubsystem::SpeakerRook || Speaker == URRSubtitleSubsystem::SpeakerVesna);
	TestTrue(TEXT("speaker name in front"), Subtitles->GetCurrentSubtitle().ToString().StartsWith(Subtitles->GetCurrentSpeaker().ToString() + TEXT(": ")));
	const FString FirstLine = Subtitles->GetCurrentText().ToString();

	// A3: an alert replaces the bark at once; a bark during an alert is dropped.
	TestTrue(TEXT("alert shown"), Subtitles->ShowAlertLine(TEXT("horde_incoming")));
	TestEqual(TEXT("alert replaced the bark"), Subtitles->GetCurrentSpeakerId(), URRSubtitleSubsystem::SpeakerKesslerVoss);
	World->TimeSeconds += 31.0;
	TestFalse(TEXT("bark dropped during the alert"), Subtitles->ShowBark(Opened));
	TestEqual(TEXT("alert kept"), Subtitles->GetCurrentSpeakerId(), URRSubtitleSubsystem::SpeakerKesslerVoss);
	Subtitles->Clear();

	// A2: cooldown counted from the last bark shown.
	World->TimeSeconds -= 31.0;
	World->TimeSeconds += 10.0;
	TestFalse(TEXT("bark within the cooldown dropped"), Subtitles->ShowBark(Opened));
	TestTrue(TEXT("cooldown left about 20 s"), FMath::IsNearlyEqual(Subtitles->GetBarkCooldownRemaining(), 20.f, 0.01f));
	World->TimeSeconds += 21.0;
	TestTrue(TEXT("bark after the cooldown shown"), Subtitles->ShowBark(Hacked));

	// A4: never the same line twice in a row for the trigger.
	TestNotEqual(TEXT("no immediate repeat"), Subtitles->GetCurrentText().ToString(), FirstLine);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
