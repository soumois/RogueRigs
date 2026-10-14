// Rogue Rigs - text content library: parses upgrade_texts.json, weapon_texts.json, alerts.json and barks.json (text-content-schema) and answers lookups.

#include "Text/RRTextContentLibrary.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RogueRigs.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

const TCHAR* FRRTextContentLibrary::UpgradeTextsFileName = TEXT("upgrade_texts.json");
const TCHAR* FRRTextContentLibrary::WeaponTextsFileName = TEXT("weapon_texts.json");
const TCHAR* FRRTextContentLibrary::AlertsFileName = TEXT("alerts.json");
const TCHAR* FRRTextContentLibrary::BarksFileName = TEXT("barks.json");
const TCHAR* FRRTextContentLibrary::TextDirectory = TEXT("RogueRigs/Data/Text");

const TCHAR* FRRTextContentLibrary::AlertConsoleDropped = TEXT("console_dropped");
const TCHAR* FRRTextContentLibrary::AlertBonusChestDropped = TEXT("bonus_chest_dropped");
const TCHAR* FRRTextContentLibrary::AlertHordeIncoming = TEXT("horde_incoming");
const TCHAR* FRRTextContentLibrary::AlertExtractionReady = TEXT("extraction_ready");
const TCHAR* FRRTextContentLibrary::AlertShuttleCountdown = TEXT("shuttle_countdown");

namespace RRTextContent
{
	/** True when RunId is "<8 digits>-<6 digits>-<Stem>" (a -noretrieval baseline is never loaded in game). */
	static bool IsRunIdForStem(const FString& RunId, const TCHAR* Stem)
	{
		const FString Expected = FString(Stem);
		if (RunId.Len() != 16 + Expected.Len() || RunId[8] != TEXT('-') || RunId[15] != TEXT('-') || !RunId.EndsWith(Expected, ESearchCase::CaseSensitive))
		{
			return false;
		}
		for (int32 Index = 0; Index < 15; ++Index)
		{
			if (Index != 8 && !FChar::IsDigit(RunId[Index]))
			{
				return false;
			}
		}
		return true;
	}

	/** Lowercase key "trigger|speaker" of the bark pick history. */
	static FString BarkKey(FName TriggerId, FName SpeakerId)
	{
		return (TriggerId.ToString() + TEXT("|") + SpeakerId.ToString()).ToLower();
	}
}

FString FRRTextContentLibrary::GetDefaultDirectory()
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TextDirectory);
}

const TArray<FName>& FRRTextContentLibrary::GetAlertEventIds()
{
	static const TArray<FName> Ids = {
		FName(AlertConsoleDropped), FName(AlertBonusChestDropped), FName(AlertHordeIncoming), FName(AlertExtractionReady), FName(AlertShuttleCountdown)
	};
	return Ids;
}

const TArray<FName>& FRRTextContentLibrary::GetBarkTriggerIds()
{
	static const TArray<FName> Ids = {
		FName(TEXT("level_up")), FName(TEXT("upgrade_offer")), FName(TEXT("player_damaged")), FName(TEXT("horde_incoming")),
		FName(TEXT("console_dropped")), FName(TEXT("console_hack_started")), FName(TEXT("console_hacked")),
		FName(TEXT("bonus_chest_dropped")), FName(TEXT("bonus_chest_opened")), FName(TEXT("extraction_ready")),
		FName(TEXT("shuttle_landed")), FName(TEXT("run_won")), FName(TEXT("run_lost")), FName(TEXT("game_over"))
	};
	return Ids;
}

const TArray<FName>& FRRTextContentLibrary::GetWeaponTextIds()
{
	static const TArray<FName> Ids = { FName(TEXT("wpn_shotgun")), FName(TEXT("wpn_turret_launcher")) };
	return Ids;
}

const TArray<FName>& FRRTextContentLibrary::GetSpeakerIds()
{
	static const TArray<FName> Ids = { FName(TEXT("rook")), FName(TEXT("vesna")) };
	return Ids;
}

int32 FRRTextContentLibrary::PickIndexAvoiding(int32 Count, int32 PreviousIndex, FRandomStream& Stream)
{
	if (Count <= 0)
	{
		return INDEX_NONE;
	}
	if (Count == 1)
	{
		return 0;
	}
	if (PreviousIndex < 0 || PreviousIndex >= Count)
	{
		return Stream.RandRange(0, Count - 1);
	}
	// Draw among the Count - 1 others: skip over the previous index.
	const int32 Drawn = Stream.RandRange(0, Count - 2);
	return Drawn >= PreviousIndex ? Drawn + 1 : Drawn;
}

int32 FRRTextContentLibrary::LoadFromDirectory(const FString& Directory)
{
	Reset();
	int32 Loaded = 0;
	Loaded += LoadUpgradeTexts(FPaths::Combine(Directory, UpgradeTextsFileName)) ? 1 : 0;
	Loaded += LoadWeaponTexts(FPaths::Combine(Directory, WeaponTextsFileName)) ? 1 : 0;
	Loaded += LoadAlerts(FPaths::Combine(Directory, AlertsFileName)) ? 1 : 0;
	Loaded += LoadBarks(FPaths::Combine(Directory, BarksFileName)) ? 1 : 0;
	UE_LOG(LogRogueRigs, Log, TEXT("FRRTextContentLibrary: %d/4 text files loaded from %s (%d flavors, %d weapon lore lines, %d alert lines, %d barks, %d items rejected)."),
		Loaded, *Directory, UpgradeFlavorCount, WeaponFlavorCount, GetAlertLineCount(), Barks.Num(), RejectedItemCount);
	return Loaded;
}

void FRRTextContentLibrary::Reset()
{
	FlavorByKey.Reset();
	UpgradeFlavorCount = 0;
	WeaponFlavorByKey.Reset();
	WeaponFlavorCount = 0;
	AlertLinesByEvent.Reset();
	LastAlertIndex.Reset();
	Barks.Reset();
	LastBarkIndex.Reset();
	RejectedItemCount = 0;
}

bool FRRTextContentLibrary::ReadEnvelope(const FString& FilePath, const TCHAR* ExpectedStem, TArray<TSharedPtr<FJsonValue>>& OutItems) const
{
	OutItems.Reset();
	FString FileText;
	if (!FFileHelper::LoadFileToString(FileText, *FilePath))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: text file missing or unreadable, functional texts kept: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: JSON parse error in %s (%s), functional texts kept."), *FilePath, *Reader->GetErrorMessage());
		return false;
	}

	int32 SchemaVersion = 0;
	FString RunId;
	const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
	if (!Root->TryGetNumberField(TEXT("schema_version"), SchemaVersion) || SchemaVersion != 1)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: missing or unsupported schema_version (expected 1) in %s, functional texts kept."), *FilePath);
		return false;
	}
	if (!Root->TryGetStringField(TEXT("run_id"), RunId) || !RRTextContent::IsRunIdForStem(RunId, ExpectedStem))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: run_id '%s' is not a validated %s run in %s, functional texts kept."), *RunId, ExpectedStem, *FilePath);
		return false;
	}
	if (!Root->TryGetArrayField(TEXT("items"), Items) || Items == nullptr || Items->Num() == 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: missing or empty items array in %s, functional texts kept."), *FilePath);
		return false;
	}
	OutItems = *Items;
	return true;
}

bool FRRTextContentLibrary::ReadItemStrings(const TSharedPtr<FJsonValue>& Item, int32 Index, const FString& FilePath, const TArray<const TCHAR*>& Keys, TArray<FString>& OutValues)
{
	OutValues.Reset();
	const TSharedPtr<FJsonObject>* Object = nullptr;
	if (!Item.IsValid() || !Item->TryGetObject(Object) || Object == nullptr || !Object->IsValid())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: items[%d] is not an object in %s, skipped."), Index, *FilePath);
		++RejectedItemCount;
		return false;
	}
	for (const TCHAR* Key : Keys)
	{
		FString Value;
		if (!(*Object)->TryGetStringField(Key, Value) || Value.TrimStartAndEnd().IsEmpty())
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: items[%d] has no string '%s' in %s, skipped."), Index, Key, *FilePath);
			++RejectedItemCount;
			return false;
		}
		OutValues.Add(Value);
	}
	return true;
}

bool FRRTextContentLibrary::LoadUpgradeTexts(const FString& FilePath)
{
	FlavorByKey.Reset();
	UpgradeFlavorCount = 0;
	TArray<TSharedPtr<FJsonValue>> Items;
	if (!ReadEnvelope(FilePath, TEXT("upgrade_texts"), Items))
	{
		return false;
	}
	const TArray<const TCHAR*> Keys = { TEXT("id"), TEXT("stat"), TEXT("flavor") };
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		TArray<FString> Values;
		if (!ReadItemStrings(Items[Index], Index, FilePath, Keys, Values))
		{
			continue;
		}
		const FString StatKey = Values[1].ToLower();
		if (FlavorByKey.Contains(StatKey))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: stat '%s' appears twice in %s, items[%d] skipped."), *Values[1], *FilePath, Index);
			++RejectedItemCount;
			continue;
		}
		FlavorByKey.Add(StatKey, Values[2]);
		FlavorByKey.Add(Values[0].ToLower(), Values[2]);
		++UpgradeFlavorCount;
	}
	return true;
}

bool FRRTextContentLibrary::LoadWeaponTexts(const FString& FilePath)
{
	WeaponFlavorByKey.Reset();
	WeaponFlavorCount = 0;
	TArray<TSharedPtr<FJsonValue>> Items;
	if (!ReadEnvelope(FilePath, TEXT("weapon_texts"), Items))
	{
		return false;
	}
	const TArray<const TCHAR*> Keys = { TEXT("id"), TEXT("weapon"), TEXT("flavor") };
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		TArray<FString> Values;
		if (!ReadItemStrings(Items[Index], Index, FilePath, Keys, Values))
		{
			continue;
		}
		if (!GetWeaponTextIds().Contains(FName(*Values[0])))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: weapon id '%s' is not a weapon lore id of text-content-events in %s, items[%d] skipped."), *Values[0], *FilePath, Index);
			++RejectedItemCount;
			continue;
		}
		const FString WeaponKey = Values[1].ToLower();
		const FString IdKey = Values[0].ToLower();
		if (WeaponFlavorByKey.Contains(WeaponKey) || WeaponFlavorByKey.Contains(IdKey))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: weapon '%s' appears twice in %s, items[%d] skipped."), *Values[1], *FilePath, Index);
			++RejectedItemCount;
			continue;
		}
		WeaponFlavorByKey.Add(WeaponKey, Values[2]);
		WeaponFlavorByKey.Add(IdKey, Values[2]);
		++WeaponFlavorCount;
	}
	return true;
}

bool FRRTextContentLibrary::LoadAlerts(const FString& FilePath)
{
	AlertLinesByEvent.Reset();
	LastAlertIndex.Reset();
	TArray<TSharedPtr<FJsonValue>> Items;
	if (!ReadEnvelope(FilePath, TEXT("alerts"), Items))
	{
		return false;
	}
	const TArray<const TCHAR*> Keys = { TEXT("line_id"), TEXT("event"), TEXT("text") };
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		TArray<FString> Values;
		if (!ReadItemStrings(Items[Index], Index, FilePath, Keys, Values))
		{
			continue;
		}
		const FName Event(*Values[1]);
		if (!GetAlertEventIds().Contains(Event))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: %s uses unknown alert event '%s' in %s, skipped."), *Values[0], *Values[1], *FilePath);
			++RejectedItemCount;
			continue;
		}
		AlertLinesByEvent.FindOrAdd(Event).Add(Values[2]);
	}
	return true;
}

bool FRRTextContentLibrary::LoadBarks(const FString& FilePath)
{
	Barks.Reset();
	LastBarkIndex.Reset();
	TArray<TSharedPtr<FJsonValue>> Items;
	if (!ReadEnvelope(FilePath, TEXT("barks"), Items))
	{
		return false;
	}
	const TArray<const TCHAR*> Keys = { TEXT("line_id"), TEXT("speaker_id"), TEXT("trigger"), TEXT("text") };
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		TArray<FString> Values;
		if (!ReadItemStrings(Items[Index], Index, FilePath, Keys, Values))
		{
			continue;
		}
		const FName Speaker(*Values[1]);
		const FName Trigger(*Values[2]);
		if (!GetSpeakerIds().Contains(Speaker) || !GetBarkTriggerIds().Contains(Trigger))
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("FRRTextContentLibrary: %s uses unknown speaker '%s' or trigger '%s' in %s, skipped."),
				*Values[0], *Values[1], *Values[2], *FilePath);
			++RejectedItemCount;
			continue;
		}
		Barks.Add({ Speaker, Trigger, Values[3] });
	}
	return true;
}

bool FRRTextContentLibrary::FindUpgradeFlavor(const FString& StatOrId, FString& OutFlavor) const
{
	if (const FString* Found = FlavorByKey.Find(StatOrId.ToLower()))
	{
		OutFlavor = *Found;
		return true;
	}
	return false;
}

bool FRRTextContentLibrary::FindWeaponFlavor(const FString& WeaponOrId, FString& OutFlavor) const
{
	if (const FString* Found = WeaponFlavorByKey.Find(WeaponOrId.TrimStartAndEnd().ToLower()))
	{
		OutFlavor = *Found;
		return true;
	}
	return false;
}

const TArray<FString>* FRRTextContentLibrary::FindAlertLines(FName EventId) const
{
	const TArray<FString>* Lines = AlertLinesByEvent.Find(EventId);
	return Lines != nullptr && Lines->Num() > 0 ? Lines : nullptr;
}

bool FRRTextContentLibrary::PickAlertLine(FName EventId, FRandomStream& Stream, FString& OutLine)
{
	const TArray<FString>* Lines = FindAlertLines(EventId);
	if (Lines == nullptr)
	{
		return false;
	}
	int32& Last = LastAlertIndex.FindOrAdd(EventId, INDEX_NONE);
	Last = PickIndexAvoiding(Lines->Num(), Last, Stream);
	OutLine = (*Lines)[Last];
	return true;
}

int32 FRRTextContentLibrary::CountBarks(FName TriggerId, FName SpeakerId) const
{
	int32 Count = 0;
	for (const FBark& Bark : Barks)
	{
		Count += Bark.Trigger == TriggerId && (SpeakerId.IsNone() || Bark.Speaker == SpeakerId) ? 1 : 0;
	}
	return Count;
}

bool FRRTextContentLibrary::PickBark(FName TriggerId, FName SpeakerId, FRandomStream& Stream, FString& OutText)
{
	TArray<const FBark*> Matches;
	for (const FBark& Bark : Barks)
	{
		if (Bark.Trigger == TriggerId && (SpeakerId.IsNone() || Bark.Speaker == SpeakerId))
		{
			Matches.Add(&Bark);
		}
	}
	if (Matches.Num() == 0)
	{
		return false;
	}
	int32& Last = LastBarkIndex.FindOrAdd(RRTextContent::BarkKey(TriggerId, SpeakerId), INDEX_NONE);
	Last = PickIndexAvoiding(Matches.Num(), Last, Stream);
	OutText = Matches[Last]->Text;
	return true;
}

int32 FRRTextContentLibrary::GetAlertLineCount() const
{
	int32 Count = 0;
	for (const TPair<FName, TArray<FString>>& Pair : AlertLinesByEvent)
	{
		Count += Pair.Value.Num();
	}
	return Count;
}
