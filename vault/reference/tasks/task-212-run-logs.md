---
id: task-212-run-logs
description: "Global task: Run logs."
parent: tasks-done-index
kind: task
status: done
---
# Run logs

- request: "fait le 1, puis le 4, puis 7,8,9." (point 4: run logs, M2), then "le reste est ok" (owner, 2026-10-17)
- goal: a structured run log is written: one JSON file per run, one entry per level with the 11 fields of run-log-schema.
- acceptance: a -game run of 2 levels (one extraction, then a death or missed shuttle) writes Saved/RunLogs/<date-time>.json with one entry per level holding every field of run-log-schema with plausible values; run-log-schema states the format and field names; tests pass except the known failures; lint clean; committed.
- gdd_check: [[run-log-schema]] (format and field names open), [[intermission]] (recap will later show DPS per weapon and robots killed), [[level-flow]], [[extraction]]. Owner 2026-10-17 defaults accepted: JSON, one file per run in Saved/RunLogs/<date-time>.json, one entry per level written at level end (extracted, death, missed shuttle), snake_case field names from the schema list, all 11 fields; no intermission recap change now.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: none
- result: Done. Level entry at every level end (EndRun, miner death): 212-01 counters plus level_outcome, time_per_level (world time), time_left_at_shuttle, ore_collected_in_risk_window, events_met (console, bonus_chest, hordes); run file Saved/RunLogs/<yyyyMMdd-HHmmss>.json rewritten after each level, outcome won / death / missed_shuttle / abandoned / in_progress; none written in automation tests (17c155de, docs 4a7d9174, run-log-schema filled, new sys-run-log-writer). -nullrhi 2-level run: 20261016-213021.json with both entries and every field. Unexercised: won, abandoned, horde and chest entries, rendered run.

## Subtasks
1. task-212-01-collector — cpp
2. task-212-02-writer — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-212-01-collector]] — step 1, cpp.
- [[task-212-02-writer]] — step 2, cpp.
