// Rogue Rigs - automation tests of the text content loader (RogueRigs.Text.*): real files, ids, fallback, alert picks, upgrade card and alert event wiring.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Enemies/RREnemySpawner.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestHUDWidget.h"
#include "Text/RRTextContentLibrary.h"
#include "Text/RRSubtitleSubsystem.h"
#include "Text/RRTextContentSubsystem.h"
#include "UObject/StrongObjectPtr.h"
#include "Upgrades/RRUpgradeComponent.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRTextContentTests
{
	/** Every stat of ERRUpgradeStat (13). */
	const ERRUpgradeStat AllStats[] = {
		ERRUpgradeStat::Damage, ERRUpgradeStat::FireRate, ERRUpgradeStat::ReloadSpeed, ERRUpgradeStat::CritChance, ERRUpgradeStat::CritDamage,
		ERRUpgradeStat::MagazineSize, ERRUpgradeStat::MaxHealth, ERRUpgradeStat::MoveSpeed, ERRUpgradeStat::PickupRadius, ERRUpgradeStat::XPGain,
		ERRUpgradeStat::MiningSpeed, ERRUpgradeStat::Luck, ERRUpgradeStat::Pierce
	};

	/** Scratch directory of the fallback tests (Saved, never Content). */
	FString ScratchDirectory()
	{
		return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("RRTextContent"));
	}

	/** Items array of a real content file parsed independently of the loader; empty (test failed) when unreadable. */
	TArray<TSharedPtr<FJsonValue>> ReadRawItems(FAutomationTestBase& Test, const TCHAR* FileName)
	{
		FString Text;
		const FString Path = FPaths::Combine(FRRTextContentLibrary::GetDefaultDirectory(), FileName);
		TSharedPtr<FJsonObject> Root;
		const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
		if (!Test.TestTrue(FString::Printf(TEXT("%s reads"), FileName), FFileHelper::LoadFileToString(Text, *Path))
			|| !Test.TestTrue(FString::Printf(TEXT("%s parses"), FileName), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) && Root.IsValid())
			|| !Test.TestTrue(FString::Printf(TEXT("%s has items"), FileName), Root->TryGetArrayField(TEXT("items"), Items) && Items != nullptr))
		{
			return {};
		}
		return *Items;
	}

	/** Grid, miner with the rifle only (weapons stopped), its upgrade component and the world's text subsystem. */
	struct FCardSetup
	{
		ARRMiner* Miner = nullptr;
		URRUpgradeComponent* Upgrades = nullptr;
		URRTextContentSubsystem* TextContent = nullptr;
	};

	bool MakeCardSetup(FAutomationTestBase& Test, const RRTestHarness::FTestWorld& TestWorld, FCardSetup& Out)
	{
		ARRArenaGrid* Grid = RRTestHarness::SpawnGrid(Test, TestWorld);
		Out.Miner = Grid ? RRTestHarness::SpawnMiner(Test, TestWorld, Grid, RRTestHarness::RockWestFloorCell, RRTestHarness::KeepRifleOnly) : nullptr;
		Out.TextContent = TestWorld.Get()->GetSubsystem<URRTextContentSubsystem>();
		if (!Out.Miner || !Test.TestNotNull(TEXT("text subsystem"), Out.TextContent))
		{
			return false;
		}
		for (ARRWeapon* Weapon : Out.Miner->GetWeapons())
		{
			if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
			{
				Weapon->GetAutoFireComponent()->StopFiring();
			}
		}
		Out.Upgrades = Out.Miner->GetUpgrades();
		return Test.TestNotNull(TEXT("upgrade component"), Out.Upgrades);
	}

	/** Builds a grey (green for piercing, which has no grey row: blue) card of Stat on its natural target; false when no row. */
	bool MakeStatCard(const URRUpgradeComponent& Upgrades, ARRWeapon* Rifle, ERRUpgradeStat Stat, FRRUpgradeCard& OutCard)
	{
		const ERRUpgradeRarity Tier = Stat == ERRUpgradeStat::Pierce ? ERRUpgradeRarity::Blue : ERRUpgradeRarity::Grey;
		const ERRUpgradeTarget Target = URRUpgradeComponent::IsWeaponStat(Stat) ? ERRUpgradeTarget::AllWeapons : ERRUpgradeTarget::Player;
		return Upgrades.MakeCard(Stat, Tier, Target, Rifle, OutCard);
	}

	/** True when Text holds a digit. */
	bool HasDigit(const FString& Text)
	{
		for (const TCHAR Char : Text)
		{
			if (FChar::IsDigit(Char))
			{
				return true;
			}
		}
		return false;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTextLoadsRealFilesTest, "RogueRigs.Text.LoadsRealFiles",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTextLoadsRealFilesTest::RunTest(const FString& Parameters)
{
	using namespace RRTextContentTests;

	FRRTextContentLibrary Library;
	TestEqual(TEXT("three files load"), Library.LoadFromDirectory(FRRTextContentLibrary::GetDefaultDirectory()), 3);
	TestEqual(TEXT("no item rejected"), Library.GetRejectedItemCount(), 0);
	TestEqual(TEXT("one flavor per stat"), Library.GetUpgradeFlavorCount(), static_cast<int32>(UE_ARRAY_COUNT(AllStats)));
	for (const ERRUpgradeStat Stat : AllStats)
	{
		const FString Name = URRUpgradeComponent::GetStatDisplayName(Stat).ToString();
		FString Flavor;
		TestTrue(FString::Printf(TEXT("flavor of '%s'"), *Name), Library.FindUpgradeFlavor(Name, Flavor) && !Flavor.IsEmpty());
		TestFalse(FString::Printf(TEXT("flavor of '%s' has no digit"), *Name), HasDigit(Flavor));
	}
	FString ById;
	TestTrue(TEXT("lookup by id upg_fire_rate"), Library.FindUpgradeFlavor(TEXT("upg_fire_rate"), ById));

	for (const FName& Event : FRRTextContentLibrary::GetAlertEventIds())
	{
		const TArray<FString>* Lines = Library.FindAlertLines(Event);
		TestTrue(FString::Printf(TEXT("alert '%s' has lines"), *Event.ToString()), Lines != nullptr && Lines->Num() > 0);
	}
	TestEqual(TEXT("alert lines = raw items"), Library.GetAlertLineCount(), ReadRawItems(*this, FRRTextContentLibrary::AlertsFileName).Num());
	TestEqual(TEXT("barks = raw items"), Library.GetBarkCount(), ReadRawItems(*this, FRRTextContentLibrary::BarksFileName).Num());
	TestTrue(TEXT("some barks loaded"), Library.GetBarkCount() > 0);

	// Bark lookup with and without a speaker.
	FRandomStream Stream(20261010);
	FString Bark;
	if (Library.CountBarks(TEXT("level_up"), TEXT("rook")) > 0)
	{
		TestTrue(TEXT("rook level_up bark"), Library.PickBark(TEXT("level_up"), TEXT("rook"), Stream, Bark) && !Bark.IsEmpty());
	}
	TestEqual(TEXT("any-speaker count = sum"), Library.CountBarks(TEXT("level_up")),
		Library.CountBarks(TEXT("level_up"), TEXT("rook")) + Library.CountBarks(TEXT("level_up"), TEXT("vesna")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTextIdsInEventsListTest, "RogueRigs.Text.IdsInEventsList",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTextIdsInEventsListTest::RunTest(const FString& Parameters)
{
	using namespace RRTextContentTests;

	struct FCheck { const TCHAR* FileName; const TCHAR* Key; const TArray<FName>* Allowed; };
	const FCheck Checks[] = {
		{ FRRTextContentLibrary::AlertsFileName, TEXT("event"), &FRRTextContentLibrary::GetAlertEventIds() },
		{ FRRTextContentLibrary::BarksFileName, TEXT("trigger"), &FRRTextContentLibrary::GetBarkTriggerIds() },
		{ FRRTextContentLibrary::BarksFileName, TEXT("speaker_id"), &FRRTextContentLibrary::GetSpeakerIds() }
	};
	for (const FCheck& Check : Checks)
	{
		const TArray<TSharedPtr<FJsonValue>> Items = ReadRawItems(*this, Check.FileName);
		TestTrue(FString::Printf(TEXT("%s not empty"), Check.FileName), Items.Num() > 0);
		for (int32 Index = 0; Index < Items.Num(); ++Index)
		{
			const TSharedPtr<FJsonObject>* Object = nullptr;
			FString Id;
			const bool bRead = Items[Index].IsValid() && Items[Index]->TryGetObject(Object) && Object && (*Object)->TryGetStringField(Check.Key, Id);
			TestTrue(FString::Printf(TEXT("%s items[%d].%s '%s' is in the events list"), Check.FileName, Index, Check.Key, *Id),
				bRead && Check.Allowed->Contains(FName(*Id)));
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTextFallbackTest, "RogueRigs.Text.Fallback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTextFallbackTest::RunTest(const FString& Parameters)
{
	using namespace RRTextContentTests;

	// Scratch directory: alerts.json copied, upgrade_texts.json broken, barks.json missing.
	const FString Directory = ScratchDirectory();
	IFileManager::Get().DeleteDirectory(*Directory, /*RequireExists*/ false, /*Tree*/ true);
	IFileManager::Get().MakeDirectory(*Directory, /*Tree*/ true);
	TestEqual(TEXT("alerts copied"), IFileManager::Get().Copy(*FPaths::Combine(Directory, FRRTextContentLibrary::AlertsFileName),
		*FPaths::Combine(FRRTextContentLibrary::GetDefaultDirectory(), FRRTextContentLibrary::AlertsFileName)), static_cast<uint32>(COPY_OK));
	TestTrue(TEXT("broken file written"), FFileHelper::SaveStringToFile(TEXT("{\"schema_version\": 1, \"items\": ["),
		*FPaths::Combine(Directory, FRRTextContentLibrary::UpgradeTextsFileName)));

	// One warning per problem file, none for the good one.
	AddExpectedMessagePlain(TEXT("JSON parse error"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
	// Missing file: barks.json here, then the three files of an absent directory below.
	AddExpectedMessagePlain(TEXT("text file missing or unreadable"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 4);
	FRRTextContentLibrary Library;
	TestEqual(TEXT("only alerts load"), Library.LoadFromDirectory(Directory), 1);
	FString Out;
	FRandomStream Stream(1);
	TestFalse(TEXT("broken upgrade file: no flavor"), Library.FindUpgradeFlavor(TEXT("Damage"), Out));
	TestFalse(TEXT("missing barks file: no bark"), Library.PickBark(TEXT("level_up"), NAME_None, Stream, Out));
	TestTrue(TEXT("alerts still served"), Library.PickAlertLine(FRRTextContentLibrary::AlertHordeIncoming, Stream, Out));
	TestFalse(TEXT("unknown alert event"), Library.PickAlertLine(TEXT("no_such_event"), Stream, Out));

	// A valid envelope with one unknown event: that item is skipped with one warning, the others load.
	const FString UnknownEvent = TEXT("{\"schema_version\":1,\"run_id\":\"20261010-120000-alerts\",\"items\":["
		"{\"line_id\":\"kv_x_01\",\"event\":\"no_such_event\",\"register\":\"bureaucratic\",\"text\":\"Some line here.\"},"
		"{\"line_id\":\"kv_horde_incoming_01\",\"event\":\"horde_incoming\",\"register\":\"bureaucratic\",\"text\":\"Other line here.\"}]}");
	const FString UnknownPath = FPaths::Combine(Directory, TEXT("unknown_alerts.json"));
	FFileHelper::SaveStringToFile(UnknownEvent, *UnknownPath);
	AddExpectedMessagePlain(TEXT("unknown alert event 'no_such_event'"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
	TestTrue(TEXT("file with an unknown event loads"), Library.LoadAlerts(UnknownPath));
	TestEqual(TEXT("one item rejected"), Library.GetRejectedItemCount(), 1);
	TestEqual(TEXT("one line kept"), Library.GetAlertLineCount(), 1);

	// Subsystem: unknown ids answer empty and warn once each, however often they are asked.
	RRTestHarness::FTestWorld TestWorld(TEXT("RRTextFallbackWorld"));
	URRTextContentSubsystem* TextContent = TestWorld.Get()->GetSubsystem<URRTextContentSubsystem>();
	if (!TestNotNull(TEXT("text subsystem"), TextContent))
	{
		return false;
	}
	TestEqual(TEXT("real files load"), TextContent->ReloadFromDirectory(FString()), 3);
	AddExpectedMessagePlain(TEXT("no flavor line for stat 'Laser focus'"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
	AddExpectedMessagePlain(TEXT("no alert line for event 'no_such_event'"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
	FText Text;
	for (int32 Repeat = 0; Repeat < 3; ++Repeat)
	{
		TestFalse(TEXT("unknown stat"), TextContent->GetUpgradeFlavor(TEXT("Laser focus"), Text));
		TestTrue(TEXT("unknown stat: empty text"), Text.IsEmpty());
		TestFalse(TEXT("unknown event"), TextContent->GetAlertLine(TEXT("no_such_event"), Text));
		TestTrue(TEXT("unknown event: no current line"), TextContent->GetCurrentAlertLine(TEXT("no_such_event")).IsEmpty());
	}

	// Whole directory missing: every lookup falls back, one warning per file (counted above).
	TestEqual(TEXT("missing directory: nothing loads"), TextContent->ReloadFromDirectory(FPaths::Combine(Directory, TEXT("absent"))), 0);
	TestFalse(TEXT("missing: no flavor"), TextContent->GetUpgradeFlavor(TEXT("Damage"), Text));
	TestFalse(TEXT("missing: no alert"), TextContent->GetAlertLine(FRRTextContentLibrary::AlertConsoleDropped, Text));
	TestFalse(TEXT("missing: no bark"), TextContent->GetBark(TEXT("level_up"), NAME_None, Text));

	IFileManager::Get().DeleteDirectory(*Directory, /*RequireExists*/ false, /*Tree*/ true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTextAlertPickTest, "RogueRigs.Text.AlertPickNoRepeat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTextAlertPickTest::RunTest(const FString& Parameters)
{
	FRandomStream Single(7);
	TestEqual(TEXT("one line: always it"), FRRTextContentLibrary::PickIndexAvoiding(1, 0, Single), 0);
	TestEqual(TEXT("no line: none"), FRRTextContentLibrary::PickIndexAvoiding(0, INDEX_NONE, Single), static_cast<int32>(INDEX_NONE));

	FRRTextContentLibrary Library;
	TestEqual(TEXT("three files load"), Library.LoadFromDirectory(FRRTextContentLibrary::GetDefaultDirectory()), 3);
	for (const FName& Event : FRRTextContentLibrary::GetAlertEventIds())
	{
		const TArray<FString>* Lines = Library.FindAlertLines(Event);
		if (!TestTrue(FString::Printf(TEXT("'%s' has several lines"), *Event.ToString()), Lines != nullptr && Lines->Num() > 1))
		{
			continue;
		}
		FRandomStream Stream(20261010);
		TArray<FString> Sequence;
		TSet<FString> Seen;
		bool bRepeated = false;
		for (int32 Pick = 0; Pick < 200; ++Pick)
		{
			FString Line;
			Library.PickAlertLine(Event, Stream, Line);
			bRepeated |= Sequence.Num() > 0 && Sequence.Last() == Line;
			Sequence.Add(Line);
			Seen.Add(Line);
		}
		TestFalse(FString::Printf(TEXT("'%s': never twice in a row"), *Event.ToString()), bRepeated);
		TestEqual(FString::Printf(TEXT("'%s': every line used"), *Event.ToString()), Seen.Num(), Lines->Num());

		// Same seed, fresh history: same sequence.
		FRRTextContentLibrary Again;
		Again.LoadAlerts(FPaths::Combine(FRRTextContentLibrary::GetDefaultDirectory(), FRRTextContentLibrary::AlertsFileName));
		FRandomStream SameSeed(20261010);
		bool bSame = true;
		for (int32 Pick = 0; Pick < Sequence.Num(); ++Pick)
		{
			FString Line;
			Again.PickAlertLine(Event, SameSeed, Line);
			bSame &= Line == Sequence[Pick];
		}
		TestTrue(FString::Printf(TEXT("'%s': deterministic for a seed"), *Event.ToString()), bSame);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTextUpgradeCardTest, "RogueRigs.Text.UpgradeCardFlavor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTextUpgradeCardTest::RunTest(const FString& Parameters)
{
	using namespace RRTextContentTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRTextUpgradeCardWorld"));
	FCardSetup Setup;
	if (!MakeCardSetup(*this, TestWorld, Setup))
	{
		return false;
	}
	ARRWeapon* Rifle = Setup.Miner->GetWeapons().Num() > 0 ? Setup.Miner->GetWeapons()[0] : nullptr;
	FRRTextContentLibrary Reference;
	Reference.LoadFromDirectory(FRRTextContentLibrary::GetDefaultDirectory());

	int32 Built = 0;
	for (const ERRUpgradeStat Stat : AllStats)
	{
		FRRUpgradeCard Card;
		if (!MakeStatCard(*Setup.Upgrades, Rifle, Stat, Card))
		{
			continue; // No CSV row (MagazineSize in M1).
		}
		++Built;
		const FString Name = URRUpgradeComponent::GetStatDisplayName(Stat).ToString();
		FString Flavor;
		Reference.FindUpgradeFlavor(Name, Flavor);
		TestEqual(FString::Printf(TEXT("%s: stat name kept"), *Name), Card.DisplayName.ToString(), Name);
		TestEqual(FString::Printf(TEXT("%s: value text from the code"), *Name), Card.ValueText.ToString(),
			URRUpgradeComponent::FormatValueText(Stat, Card.Value).ToString());
		TestEqual(FString::Printf(TEXT("%s: description = name, then flavor line"), *Name), Card.DescriptionText.ToString(), Name + TEXT("\n") + Flavor);
		TestFalse(FString::Printf(TEXT("%s: description holds no number"), *Name), HasDigit(Card.DescriptionText.ToString()));
	}
	TestTrue(TEXT("stat cards built"), Built >= 12);

	// Fallback: without the files the card is the functional one, numbers unchanged.
	FRRUpgradeCard WithFlavor;
	MakeStatCard(*Setup.Upgrades, Rifle, ERRUpgradeStat::Damage, WithFlavor);
	AddExpectedMessagePlain(TEXT("text file missing or unreadable"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 3);
	Setup.TextContent->ReloadFromDirectory(FPaths::Combine(ScratchDirectory(), TEXT("absent")));
	FRRUpgradeCard Functional;
	TestTrue(TEXT("card builds without the files"), MakeStatCard(*Setup.Upgrades, Rifle, ERRUpgradeStat::Damage, Functional));
	TestTrue(TEXT("fallback: empty description"), Functional.DescriptionText.IsEmpty());
	TestEqual(TEXT("fallback: same value text"), Functional.ValueText.ToString(), WithFlavor.ValueText.ToString());
	TestEqual(TEXT("fallback: same value"), Functional.Value, WithFlavor.Value);
	TestEqual(TEXT("fallback: same name"), Functional.DisplayName.ToString(), WithFlavor.DisplayName.ToString());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRTextAlertEventsTest, "RogueRigs.Text.AlertEventsWired",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRTextAlertEventsTest::RunTest(const FString& Parameters)
{
	using namespace RRTextContentTests;

	RRTestHarness::FTestWorld TestWorld(TEXT("RRTextAlertEventsWorld"));
	const ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? RRTestHarness::SpawnMiner(*this, TestWorld, Grid, RRTestHarness::FineCell(FIntPoint(40, 40))) : nullptr;
	URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>();
	URRTextContentSubsystem* TextContent = TestWorld.Get()->GetSubsystem<URRTextContentSubsystem>();
	URRSubtitleSubsystem* Subtitles = TestWorld.Get()->GetSubsystem<URRSubtitleSubsystem>();
	if (!Miner || !TestNotNull(TEXT("run flow"), RunFlow) || !TestNotNull(TEXT("text subsystem"), TextContent)
		|| !TestNotNull(TEXT("subtitle subsystem"), Subtitles))
	{
		return false;
	}
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Subtitles->OnSubtitleShown.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandleSubtitleShown);
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}
	RunFlow->SetConfig(FRRRunFlowConfig());
	RunFlow->SetRandomSeed(20261003);
	TextContent->SetRandomSeed(20261010);
	const FRRTextContentLibrary& Library = TextContent->GetLibrary();

	// Each event picks one of its lines and shows it as a Kessler-Voss subtitle (ShownIndex = index of its OnSubtitleShown in the recorder).
	auto CheckLine = [this, TextContent, &Library, &Recorder](const TCHAR* Event, int32 ShownIndex)
	{
		const FString Line = TextContent->GetCurrentAlertLine(Event).ToString();
		const TArray<FString>* Lines = Library.FindAlertLines(Event);
		TestTrue(FString::Printf(TEXT("'%s' raised with one of its lines ('%s')"), Event, *Line), Lines != nullptr && Lines->Contains(Line));
		const FString Expected = FString::Printf(TEXT("Shown:kessler_voss:Kessler-Voss: %s"), *Line);
		TestTrue(FString::Printf(TEXT("'%s' shown as a Kessler-Voss subtitle"), Event),
			Recorder->SubtitleEvents.IsValidIndex(ShownIndex) && Recorder->SubtitleEvents[ShownIndex] == Expected);
		return Line;
	};

	for (const FName& Event : FRRTextContentLibrary::GetAlertEventIds())
	{
		TestTrue(FString::Printf(TEXT("'%s' empty before its event"), *Event.ToString()), TextContent->GetCurrentAlertLine(Event).IsEmpty());
	}

	// Console drop.
	TestNotNull(TEXT("console dropped"), RunFlow->SkipToConsole());
	CheckLine(FRRTextContentLibrary::AlertConsoleDropped, 0);
	TestEqual(TEXT("console subtitle is current"), Subtitles->GetCurrentSpeakerId(), URRSubtitleSubsystem::SpeakerKesslerVoss);

	// Bonus chest drop (alert past its threshold once the console is done).
	TestTrue(TEXT("console completed"), RunFlow->CompleteConsoleHack());
	RunFlow->SetAlert(RunFlow->GetConfig().BonusChestAlertThreshold + 0.05f);
	for (int32 Frame = 0; Frame < 5 && RunFlow->GetBonusChest() == nullptr; ++Frame)
	{
		TestWorld.Tick();
	}
	if (TestNotNull(TEXT("bonus chest dropped"), RunFlow->GetBonusChest()))
	{
		CheckLine(FRRTextContentLibrary::AlertBonusChestDropped, 1);
	}

	// Extraction ready, and one countdown line for the whole countdown (OnShuttleCountdown fires every frame).
	TestTrue(TEXT("extraction"), RunFlow->SkipToExtraction());
	const int32 ExtractionIndex = Recorder->SubtitleEvents.Num();
	CheckLine(FRRTextContentLibrary::AlertExtractionReady, ExtractionIndex - 2);
	const FString Countdown = CheckLine(FRRTextContentLibrary::AlertShuttleCountdown, ExtractionIndex - 1);
	TestEqual(TEXT("countdown subtitle is current"), Subtitles->GetCurrentText().ToString(), Countdown);
	for (int32 Frame = 0; Frame < 30; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("countdown line unchanged over 30 countdown frames"), TextContent->GetCurrentAlertLine(FRRTextContentLibrary::AlertShuttleCountdown).ToString(), Countdown);
	TestEqual(TEXT("countdown subtitle shown once over 30 countdown frames"), Recorder->SubtitleEvents.Num(), ExtractionIndex);

	// Horde banner through the HUD: line picked before OnHordeIncoming, readable from the HUD getter.
	TStrongObjectPtr<URRTestHUDWidget> HUD(CreateWidget<URRTestHUDWidget>(TestWorld.Get(), URRTestHUDWidget::StaticClass()));
	ARREnemySpawner* Spawner = TestWorld.Spawn<ARREnemySpawner>(ARREnemySpawner::StaticClass(), FVector::ZeroVector);
	if (!TestNotNull(TEXT("test HUD"), HUD.Get()) || !TestNotNull(TEXT("spawner"), Spawner))
	{
		return false;
	}
	Spawner->SetSpawningEnabled(false);
	HUD->BindToSpawner(Spawner);
	Spawner->OnHordeIncoming.Broadcast(0, 2.f);
	TestEqual(TEXT("banner raised once"), HUD->HordeIncomingCalls.Num(), 1);
	const FString Horde = CheckLine(FRRTextContentLibrary::AlertHordeIncoming, Recorder->SubtitleEvents.Num() - 1);
	TestEqual(TEXT("horde subtitle is current"), Subtitles->GetCurrentText().ToString(), Horde);
	TestEqual(TEXT("HUD getter returns the same line"), HUD->GetAlertLine(FRRTextContentLibrary::AlertHordeIncoming).ToString(), Horde);
	Spawner->OnHordeIncoming.Broadcast(1, 2.f);
	TestNotEqual(TEXT("next horde banner: another line"), HUD->GetAlertLine(FRRTextContentLibrary::AlertHordeIncoming).ToString(), Horde);
	HUD->BindToSpawner(nullptr);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
