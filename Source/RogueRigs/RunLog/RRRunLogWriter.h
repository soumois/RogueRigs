// Rogue Rigs - run log writer: the run record (one per run, one entry per level) and its JSON file (run-log-schema).

#pragma once

#include "CoreMinimal.h"
#include "RunLog/RRRunLogTypes.h"

/** Serializes a run record to the run-log-schema JSON (snake_case field names) and writes it to disk. */
class ROGUERIGS_API FRRRunLogWriter
{
public:
	/** The JSON text of Record (pretty printed). */
	static FString ToJson(const FRRRunLogRecord& Record);

	/** Writes Record to Record.FilePath (directory created when missing). False on a write failure (logged). */
	static bool WriteFile(const FRRRunLogRecord& Record);

	/** Directory of the run files: <Project>/Saved/RunLogs/. */
	static FString GetRunLogDirectory();
};
