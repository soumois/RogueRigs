---
id: task-052-heavy-radius-2
description: "Global task: Heavy slam radius 340 → 289 uu (trigger 202 uu, chase stop 145 uu); ground ring follows."
parent: tasks-done-index
kind: task
status: done
---
# Heavy slam radius −15 % again

- request: "Réduit la zone d’attaque du gros de 15%" (owner, 2026-10-07)
- goal: Heavy slam radius 340 → 289 uu (trigger 202 uu, chase stop 145 uu); ground ring follows.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. EnemyStats.csv Heavy AttackRange 340 → 289: trigger 202.3 uu, chase stop 144.5 uu, ring scale 5.78; heavy tests updated (telegraph at 202 uu, hit at 279, miss at 299). Build 0, 195/195. Commit 96fafcf.

## Subtasks
1. task-052-01-heavy-radius-2 — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-052-01-heavy-radius-2]] — step 1, cpp.
