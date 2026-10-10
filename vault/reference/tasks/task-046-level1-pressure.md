---
id: task-046-level1-pressure
description: "Global task: Level 1 pressure: 80 robots alive at most, wave interval ending at 5.5 s, packs of one wave at least 120° apart."
parent: tasks-done-index
kind: task
status: done
---
# Level 1 pressure

- request: "Garde les vagues de 2 à 3 pack à 1:30, mais donne leur plus de distance entre eux au spawn." (with the 80 alive and 5.5 s proposal accepted) (owner, 2026-10-07)
- goal: Level 1 pressure: 80 robots alive at most, wave interval ending at 5.5 s, packs of one wave at least 120° apart.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. ARREnemySpawner::MaxAlive 150 → 80 (C++, not overridden; shared by all levels); MakeDefault wave interval end 4.1 → 5.5 s; PackMinSeparationDegrees 120 (anchor angles; 3-pack waves placed at 120° steps); MakeLastLevel untouched; PlacedSpawnerMix fixed to heavy cap 3. Build 0, 195/195. Commit 5ccbb9b.

## Subtasks
1. task-046-01-level1-pressure — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-046-01-level1-pressure]] — step 1, cpp.
