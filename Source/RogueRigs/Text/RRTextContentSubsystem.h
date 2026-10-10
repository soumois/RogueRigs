// Rogue Rigs - text content subsystem: loads the validated text files once per game world and serves flavor, alert and bark lines with fallback.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"
#include "Subsystems/WorldSubsystem.h"
#include "Text/RRTextContentLibrary.h"
#include "RRTextContentSubsystem.generated.h"

/**
 * Runtime access to the player-facing text files of Content/RogueRigs/Data/Text (sys-text-content). Game and PIE worlds. The
 * files load lazily on the first lookup, once per world (FRRTextContentLibrary). Every lookup returns false and an empty text
 * when its file is missing or unusable or the id is unknown: the caller keeps its functional text. One warning per problem
 * (per file at load, per unknown id at lookup), never a crash. No AI at runtime: the files are written offline.
 * Hooks: URRUpgradeComponent::MakeCard (upgrade flavor), URRRunFlowSubsystem (console_dropped, bonus_chest_dropped,
 * extraction_ready, shuttle_countdown once per countdown start) and URRHUDWidget (horde_incoming) pick an alert line just before
 * raising their event; Blueprint handlers read it with GetCurrentAlertLine. Barks are loaded but nothing displays them yet.
 */
UCLASS()
class ROGUERIGS_API URRTextContentSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Seeds the pick stream from the global random generator. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Game and PIE worlds only. */
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	/** The subsystem of WorldContext's world; null without a world or in an unsupported world type. */
	static URRTextContentSubsystem* Get(const UObject* WorldContext);

	/** Upgrade card description: the stat name, then the flavor line on a second line ("Damage\n<flavor>"). */
	static FText ComposeUpgradeDescription(const FText& StatName, const FText& Flavor);

	/** Flavor line of a stat by display name ("Fire rate") or id ("upg_fire_rate"). False (empty text) when unknown or not loaded. */
	UFUNCTION(BlueprintCallable, Category = "Text")
	bool GetUpgradeFlavor(const FString& StatNameOrId, FText& OutText);

	/**
	 * Picks a line of alert event EventId (alerts.json), never the previous pick when the event has more than one, and remembers it
	 * as the current line of that event (GetCurrentAlertLine). False (empty text, current line cleared) when none.
	 */
	UFUNCTION(BlueprintCallable, Category = "Text")
	bool GetAlertLine(FName EventId, FText& OutText);

	/** Line last picked for EventId (the one raised with the event); empty when none was picked or the event has no line. */
	UFUNCTION(BlueprintPure, Category = "Text")
	FText GetCurrentAlertLine(FName EventId) const;

	/** Picks a bark of TriggerId by SpeakerId (None = any speaker), avoiding an immediate repeat. False (empty text) when none. */
	UFUNCTION(BlueprintCallable, Category = "Text")
	bool GetBark(FName TriggerId, FName SpeakerId, FText& OutText);

	/** Reseeds the pick stream (deterministic picks in tests). */
	UFUNCTION(BlueprintCallable, Category = "Text")
	void SetRandomSeed(int32 Seed) { RandomStream.Initialize(Seed); }

	/** Reloads every file from Directory (empty = the shipped directory) and forgets current lines and warnings. Returns files loaded (0 to 3). */
	int32 ReloadFromDirectory(const FString& Directory);

	/** The loaded library (loads it first when needed). */
	const FRRTextContentLibrary& GetLibrary();

protected:
	/** Loads the shipped directory on the first call. */
	void EnsureLoaded();

	/** Logs Message as a warning the first time Key is seen. */
	void WarnOnce(const FString& Key, const FString& Message);

	/** Parsed lines and pick history. */
	FRRTextContentLibrary Library;

	/** Stream of the alert and bark picks. */
	FRandomStream RandomStream;

	/** Current line of each alert event. */
	TMap<FName, FText> CurrentAlertLines;

	/** Keys already warned about (unknown ids). */
	TSet<FString> WarnedKeys;

	/** True once the files were read. */
	bool bLoaded = false;
};
