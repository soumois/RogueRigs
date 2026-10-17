---
id: task-212-run-logs
description: "Global task: Run logs."
parent: tasks-to-do-index
kind: task
status: todo
---
# Run logs

- request: "fait le 1, puis le 4, puis 7,8,9." (point 4: run logs, M2), then "le reste est ok" (owner, 2026-10-17)
- goal: a structured run log is written: one JSON file per run, one entry per level with the 11 fields of run-log-schema.
- acceptance: a -game run of 2 levels (one extraction, then a death or missed shuttle) writes Saved/RunLogs/<date-time>.json with one entry per level holding every field of run-log-schema with plausible values; run-log-schema states the format and field names; tests pass except the known failures; lint clean; committed.
- gdd_check: [[run-log-schema]] (format and field names open), [[intermission]] (recap will later show DPS per weapon and robots killed), [[level-flow]], [[extraction]]. Owner 2026-10-17 defaults accepted: JSON, one file per run in Saved/RunLogs/<date-time>.json, one entry per level written at level end (extracted, death, missed shuttle), snake_case field names from the schema list, all 11 fields; no intermission recap change now.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-212-01-collector
- result: open

## Subtasks
1. task-212-01-collector — cpp
2. task-212-02-writer — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-212-01-collector]] — step 1, cpp.
- [[task-212-02-writer]] — step 2, cpp.
