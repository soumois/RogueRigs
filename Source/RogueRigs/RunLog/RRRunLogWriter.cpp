// Rogue Rigs - run log writer: the run record (one per run, one entry per level) and its JSON file (run-log-schema).

#include "RunLog/RRRunLogWriter.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "RogueRigs.h"
#include "Serialization/JsonWriter.h"

namespace RRRunLogJson
{
	using FWriter = TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>;

	/** Writes one level entry object (run-log-schema field names). */
	void WriteLevel(FWriter& Writer, const FRRRunLogLevelEntry& Entry)
	{
		const FRRRunLevelStats& Stats = Entry.Stats;
		Writer.WriteObjectStart();
		Writer.WriteValue(TEXT("level_index"), Stats.LevelIndex);
		Writer.WriteValue(TEXT("layout_name"), Stats.LayoutName);

		Writer.WriteObjectStart(TEXT("terrain_destroyed"));
		Writer.WriteValue(TEXT("by_source"), Stats.TerrainDestroyedBySource);
		Writer.WriteValue(TEXT("by_type"), Stats.TerrainDestroyedByType);
		Writer.WriteObjectEnd();

		Writer.WriteValue(TEXT("damage_dealt_by_weapon"), Stats.DamageDealtByWeapon);

		Writer.WriteObjectStart(TEXT("kills"));
		Writer.WriteValue(TEXT("by_robot_type"), Stats.KillsByRobotType);
		Writer.WriteValue(TEXT("by_source"), Stats.KillsBySource);
		Writer.WriteObjectEnd();

		TMap<FString, int32> Frequency;
		for (const FString& Card : Stats.UpgradesPicked)
		{
			++Frequency.FindOrAdd(Card);
		}
		Writer.WriteObjectStart(TEXT("upgrades_picked"));
		Writer.WriteValue(TEXT("order"), Stats.UpgradesPicked);
		Writer.WriteValue(TEXT("frequency"), Frequency);
		Writer.WriteObjectEnd();

		Writer.WriteValue(TEXT("time_per_level"), Entry.TimePerLevelSeconds);
		Writer.WriteValue(TEXT("damage_taken_by_source"), Stats.DamageTakenBySource);
		Writer.WriteValue(TEXT("xp_collected"), Stats.XPCollected);
		Writer.WriteValue(TEXT("level_outcome"), Entry.LevelOutcome);
		if (Entry.bReachedShuttle)
		{
			Writer.WriteValue(TEXT("time_left_at_shuttle"), Entry.TimeLeftAtShuttleSeconds);
		}
		else
		{
			Writer.WriteNull(TEXT("time_left_at_shuttle"));
		}
		Writer.WriteValue(TEXT("ore_collected_in_risk_window"), Entry.OreCollectedInRiskWindow);

		Writer.WriteArrayStart(TEXT("events_met"));
		for (const FRRRunLogEvent& Event : Entry.EventsMet)
		{
			Writer.WriteObjectStart();
			Writer.WriteValue(TEXT("event"), Event.Event);
			if (Event.Index != INDEX_NONE)
			{
				Writer.WriteValue(TEXT("index"), Event.Index);
			}
			Writer.WriteValue(TEXT("outcome"), Event.Outcome);
			Writer.WriteObjectEnd();
		}
		Writer.WriteArrayEnd();

		Writer.WriteObjectEnd();
	}
}

FString FRRRunLogWriter::ToJson(const FRRRunLogRecord& Record)
{
	FString Out;
	const TSharedRef<RRRunLogJson::FWriter> Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&Out);
	Writer->WriteObjectStart();
	Writer->WriteValue(TEXT("run_id"), Record.RunId);
	Writer->WriteValue(TEXT("start_time"), Record.StartTimeUtc.ToIso8601());
	if (Record.bEnded)
	{
		Writer->WriteValue(TEXT("end_time"), Record.EndTimeUtc.ToIso8601());
	}
	else
	{
		Writer->WriteNull(TEXT("end_time"));
	}
	Writer->WriteValue(TEXT("build_version"), Record.BuildVersion);
	Writer->WriteValue(TEXT("build_configuration"), Record.BuildConfiguration);
	Writer->WriteValue(TEXT("layouts"), Record.Layouts);
	Writer->WriteValue(TEXT("outcome"), Record.Outcome);
	Writer->WriteArrayStart(TEXT("levels"));
	for (const FRRRunLogLevelEntry& Entry : Record.Levels)
	{
		RRRunLogJson::WriteLevel(*Writer, Entry);
	}
	Writer->WriteArrayEnd();
	Writer->WriteObjectEnd();
	Writer->Close();
	return Out;
}

bool FRRRunLogWriter::WriteFile(const FRRRunLogRecord& Record)
{
	if (Record.FilePath.IsEmpty())
	{
		return false;
	}
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(Record.FilePath), true);
	const bool bSaved = FFileHelper::SaveStringToFile(ToJson(Record), *Record.FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	if (bSaved)
	{
		UE_LOG(LogRogueRigs, Log, TEXT("RunLog: run %s written (%d level(s), outcome %s) to %s."), *Record.RunId, Record.Levels.Num(), *Record.Outcome, *Record.FilePath);
	}
	else
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("RunLog: could not write the run file %s."), *Record.FilePath);
	}
	return bSaved;
}

FString FRRRunLogWriter::GetRunLogDirectory()
{
	return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("RunLogs")));
}
