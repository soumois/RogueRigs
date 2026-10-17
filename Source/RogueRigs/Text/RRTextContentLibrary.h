// Rogue Rigs - text content library: parses upgrade_texts.json, weapon_texts.json, alerts.json, barks.json and screen_texts.json (text-content-schema) and answers lookups.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"
#include "Text/RRScreenText.h"

class FJsonValue;

/**
 * Plain (non-UObject) store of the player-facing text files of Content/RogueRigs/Data/Text (sys-text-content). Owned by
 * URRTextContentSubsystem; tests build one directly to load any directory. Each file loads all-or-nothing at envelope level (missing
 * file, parse error, wrong schema_version or run_id stem, no items: one warning, that file contributes nothing); a malformed
 * item or an id outside the closed lists of text-content-events is skipped with one warning and counted in GetRejectedItemCount.
 * Lookups never log: the subsystem decides what to warn about.
 */
class ROGUERIGS_API FRRTextContentLibrary
{
public:
	/** File names inside the text directory. */
	static const TCHAR* UpgradeTextsFileName;
	static const TCHAR* WeaponTextsFileName;
	static const TCHAR* AlertsFileName;
	static const TCHAR* BarksFileName;
	static const TCHAR* ScreenTextsFileName;

	/** Text directory relative to FPaths::ProjectContentDir() (staged as non-UFS through RogueRigs/Data, DefaultGame.ini). */
	static const TCHAR* TextDirectory;

	/** Alert event ids (alerts.json `event`, text-content-events), used by the run flow and the HUD. */
	static const TCHAR* AlertConsoleDropped;
	static const TCHAR* AlertBonusChestDropped;
	static const TCHAR* AlertHordeIncoming;
	static const TCHAR* AlertExtractionReady;
	static const TCHAR* AlertShuttleCountdown;

	/** Absolute path of the shipped text directory (ProjectContentDir / TextDirectory). */
	static FString GetDefaultDirectory();

	/** Closed list of alert event ids of text-content-events. */
	static const TArray<FName>& GetAlertEventIds();

	/** Closed list of bark trigger ids of text-content-events. */
	static const TArray<FName>& GetBarkTriggerIds();

	/** Closed list of weapon lore item ids (weapon_texts.json `id`, text-content-events): the acquirable weapons. */
	static const TArray<FName>& GetWeaponTextIds();

	/** Bark speaker ids (rook, vesna). */
	static const TArray<FName>& GetSpeakerIds();

	/**
	 * Index in [0, Count) drawn from Stream, never PreviousIndex when Count > 1 (uniform over the others). INDEX_NONE when Count
	 * is 0; 0 when Count is 1. Used for every pick so a line does not show twice in a row.
	 */
	static int32 PickIndexAvoiding(int32 Count, int32 PreviousIndex, FRandomStream& Stream);

	/** Clears everything, then loads the five files of Directory. Returns the number of files loaded (0 to 5). */
	int32 LoadFromDirectory(const FString& Directory);

	/** Replaces the upgrade flavor lines with FilePath's. False (warning, flavors cleared) when the file is unusable. */
	bool LoadUpgradeTexts(const FString& FilePath);

	/** Replaces the weapon lore lines with FilePath's. False (warning, lore cleared) when the file is unusable. */
	bool LoadWeaponTexts(const FString& FilePath);

	/** Replaces the alert lines with FilePath's. False (warning, alerts cleared) when the file is unusable. */
	bool LoadAlerts(const FString& FilePath);

	/** Replaces the barks with FilePath's. False (warning, barks cleared) when the file is unusable. */
	bool LoadBarks(const FString& FilePath);

	/**
	 * Replaces the screen texts with FilePath's (id, title, optional subtitle, lines, labels). False (warning, screen texts
	 * cleared) when the file is unusable; an item with a missing id or title, a wrong slot type or a duplicate id is skipped.
	 */
	bool LoadScreenTexts(const FString& FilePath);

	/** Forgets every line, pick history and rejected count. */
	void Reset();

	/** Flavor line of a stat, by display name ("Fire rate") or item id ("upg_fire_rate"), case-insensitive. False when unknown. */
	bool FindUpgradeFlavor(const FString& StatOrId, FString& OutFlavor) const;

	/** Lore line of a weapon, by display name ("Shotgun") or item id ("wpn_shotgun"), case-insensitive. False when unknown. */
	bool FindWeaponFlavor(const FString& WeaponOrId, FString& OutFlavor) const;

	/** Texts of screen item Id (FName compare, case-insensitive); null when unknown or not loaded. */
	const FRRScreenText* FindScreenText(FName Id) const { return ScreenTexts.Find(Id); }

	/** Lines of an alert event in file order; null when the event has none. */
	const TArray<FString>* FindAlertLines(FName EventId) const;

	/** Picks one line of EventId from Stream, never the previous pick of that event when it has more than one. False when none. */
	bool PickAlertLine(FName EventId, FRandomStream& Stream, FString& OutLine);

	/**
	 * Picks one bark of TriggerId (any speaker when SpeakerId is None) from Stream, never the previous pick of that
	 * (trigger, speaker) when more than one matches. False when none matches.
	 */
	bool PickBark(FName TriggerId, FName SpeakerId, FRandomStream& Stream, FString& OutText);

	/** Number of barks of TriggerId (any speaker when SpeakerId is None). */
	int32 CountBarks(FName TriggerId, FName SpeakerId = NAME_None) const;

	/** Loaded counts (tests, logs). */
	int32 GetUpgradeFlavorCount() const { return UpgradeFlavorCount; }
	int32 GetWeaponFlavorCount() const { return WeaponFlavorCount; }
	int32 GetAlertLineCount() const;
	int32 GetBarkCount() const { return Barks.Num(); }
	int32 GetScreenTextCount() const { return ScreenTexts.Num(); }

	/** Items skipped since the last Reset (malformed, or id outside the closed lists). */
	int32 GetRejectedItemCount() const { return RejectedItemCount; }

protected:
	/** One bark line. */
	struct FBark
	{
		/** speaker_id. */
		FName Speaker;
		/** trigger. */
		FName Trigger;
		/** text. */
		FString Text;
	};

	/**
	 * Reads FilePath and checks the envelope: object, schema_version 1, run_id string whose stem (date-time removed, optional
	 * -noretrieval) is ExpectedStem, non-empty items array. False with one warning otherwise.
	 */
	bool ReadEnvelope(const FString& FilePath, const TCHAR* ExpectedStem, TArray<TSharedPtr<FJsonValue>>& OutItems) const;

	/** Reads the string fields Keys of item Index into OutValues; false (one warning, item rejected) when one is missing or empty. */
	bool ReadItemStrings(const TSharedPtr<FJsonValue>& Item, int32 Index, const FString& FilePath, const TArray<const TCHAR*>& Keys, TArray<FString>& OutValues);

	/** Flavor by lowercased stat name and by lowercased item id. */
	TMap<FString, FString> FlavorByKey;

	/** Number of flavor items loaded. */
	int32 UpgradeFlavorCount = 0;

	/** Weapon lore by lowercased weapon display name and by lowercased item id. */
	TMap<FString, FString> WeaponFlavorByKey;

	/** Number of weapon lore items loaded. */
	int32 WeaponFlavorCount = 0;

	/** Alert lines by event id, in file order. */
	TMap<FName, TArray<FString>> AlertLinesByEvent;

	/** Last picked line index per alert event. */
	TMap<FName, int32> LastAlertIndex;

	/** All barks, in file order. */
	TArray<FBark> Barks;

	/** Last picked index (within the matching barks) per "trigger|speaker" key. */
	TMap<FString, int32> LastBarkIndex;

	/** Screen texts by item id. */
	TMap<FName, FRRScreenText> ScreenTexts;

	/** Items skipped since the last Reset. */
	int32 RejectedItemCount = 0;
};
