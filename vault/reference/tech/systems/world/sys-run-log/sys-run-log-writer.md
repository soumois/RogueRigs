---
id: sys-run-log-writer
description: "Run log writer as built: run record lifecycle, level-end hooks (extraction, death, missed shuttle), end-of-level fields, JSON file. Open to change what is written."
parent: sys-run-log
kind: doc
---
# Run log writer

- responsibility: append one entry per level at level end and write the run JSON file of [[run-log-schema]] (task-212-02).
- main_classes (`Source/RogueRigs/RunLog/`): `URRRunLogSubsystem` (writer part); `RRRunLogTypes.h`: `ERRRunLevelOutcome` (Extracted, Death, MissedShuttle), `FRRRunLevelStats` (moved here from the subsystem header), `FRRRunLogEvent`, `FRRRunLogLevelEntry`, `FRRRunLogRecord`; `RRRunLogWriter.h/.cpp`: `FRRRunLogWriter` (`ToJson`, `WriteFile`, `GetRunLogDirectory`; `TJsonWriter` pretty print, floats `%g`).
- entry_points:
  - Run start: level start of a run's level 1 (or no open record): `BeginRunRecord` (id = local time, file created at once, `in_progress`). Each level start appends its layout to `layouts`.
  - Level end: `HandleLevelEnded(World, Outcome)`, once per tracked world: `URRRunFlowSubsystem::EndRun` calls it after its refusal checks, before the ore moves and before `HandleLevelExtracted` advances the level ([[sys-objectives-run-flow]]); the tracked miner's `OnDeath` (bound by the run log) gives Death. The run ends on Death, MissedShuttle, or Extracted on the last level (`won`) or without a run in progress; then the file is rewritten.
  - `URRRunSubsystem::OnRunReset` ([[sys-run]]): an open record ends `abandoned` (`rr.Run.Restart` mid-level). Reads: `GetRunRecord()`, static `GetLevelOutcomeName`.
- end-of-level fields: `time_per_level` = world time − `LevelStartWorldSeconds` (world time: pauses excluded). `time_left_at_shuttle` = `ARRShuttle::GetRemainingSeconds` when the shuttle finished with the miner on board; null otherwise (including `rr.Run.NextLevel`, no shuttle). `ore_collected_in_risk_window`: run flow `OnShuttleLanded` opens the window; `OnOreChanged` carried increases after it are summed (secure and loss decreases ignored; `GrantOre` counts). `events_met`: console (`hacked` when Complete), bonus chest (`opened` when Complete), each director horde not Pending (state name lower case) ([[sys-objectives-console-chest]], [[sys-spawner-director]]); overtime hordes not listed.
- data: none; directory `<Project>/Saved/RunLogs/` (not committed).
- depends_on: [[sys-run-log]], [[sys-objectives-run-flow]], [[sys-objectives-shuttle]], [[sys-objectives-ore-counter]], [[sys-run]].
- implements: [[run-log-schema]] (format and writer).
- gotchas: no file during automation tests (`GIsAutomationTesting`). A game quit mid-run leaves the file `in_progress`. Each death restart creates a new file. Verified by a -game -nullrhi run (Saved/Logs/task212_02_game.log, file Saved/RunLogs/20261016-213021.json): three early deaths wrote three one-level `death` files, then level 1 extracted (38.0 s left, 3 risk-window ore), `rr.Run.Continue`, level 2 missed shuttle (4 risk-window ore, console hacked), run `missed_shuttle`. No automation test (owner rule).

## Relations
- [[sys-run-log]] — parent: the collector whose counters are written.
- [[run-log-schema]] — the format written.
- [[sys-objectives-run-flow]] — EndRun calls the level end.
- [[sys-objectives-shuttle]] — time left on boarding.
- [[sys-objectives-ore-counter]] — OnOreChanged for the risk window.
- [[sys-objectives-console-chest]] — console and chest outcomes.
- [[sys-spawner-director]] — horde states.
- [[sys-run]] — run level, last level and OnRunReset.
