---
id: task-048-last-level-profile
description: "Global task: Current wave director values saved as a last-level profile in the data, not wired."
parent: tasks-done-index
kind: task
status: done
---
# Last-level director profile

- request: "Là les valeurs actuelles, ça serait pour le dernier niveau par exemple." (owner accepted: store only, wire when levels exist) (owner, 2026-10-07)
- goal: Current wave director values saved as a last-level profile in the data, not wired.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. FRRWaveDirectorConfig::MakeLastLevel() is a frozen copy of the current MakeDefault (waves 7.1 → 4.1 s, distinct-side packs, packs, hordes, overtime, shooter counter), unused until levels exist; the 150 alive cap stays on ARREnemySpawner::MaxAlive. Build 0; full run 194/195, the failure (PlacedSpawnerMix expecting heavy cap 5) is stale since task-045-02 and is fixed in task-046. Commit fff7142.

## Subtasks
1. task-048-01-last-level-profile — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-048-01-last-level-profile]] — step 1, cpp.
