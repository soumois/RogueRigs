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

	// Speaker prefix from the table.
	TestTrue(TEXT("first line shown"), Subtitles->Show(URRSubtitleSubsystem::SpeakerKesslerVoss, FText::GetEmpty(), FText::FromString(TEXT("First line."))));
	TestEqual(TEXT("Kessler-Voss prefix"), Subtitles->GetCurrentSubtitle().ToString(), FString(TEXT("Kessler-Voss: First line.")));
	TestEqual(TEXT("speaker name"), Subtitles->GetCurrentSpeaker().ToString(), FString(TEXT("Kessler-Voss")));
	TestEqual(TEXT("speaker id"), Subtitles->GetCurrentSpeakerId(), URRSubtitleSubsystem::SpeakerKesslerVoss);
	TestEqual(TEXT("line without speaker"), Subtitles->GetCurrentText().ToString(), FString(TEXT("First line.")));
	TestEqual(TEXT("Rook name"), Subtitles->GetSpeakerDisplayName(URRSubtitleSubsystem::SpeakerRook).ToString(), FString(TEXT("Rook")));
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

	const TArray<FString> Expected = { TEXT("Shown:kessler_voss:Kessler-Voss: First line."), TEXT("Shown:rook:Rook: Second line."),
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
	const TArray<FString> Expected = { TEXT("Shown:kessler_voss:Kessler-Voss") + Shown, TEXT("Cleared"), TEXT("Shown:vesna:Vesna") + Shown, TEXT("Cleared") };
	TestEqual(TEXT("delegate order: shown, cleared once per line, nothing when idle"), FString::Join(Recorder->SubtitleEvents, TEXT("|")), FString::Join(Expected, TEXT("|")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
