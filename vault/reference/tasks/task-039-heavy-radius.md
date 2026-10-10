---
id: task-039-heavy-radius
description: "Global task: Heavy slam radius 400 → 340 uu (trigger 0.7 × radius = 238 uu); ground ring follows."
parent: tasks-done-index
kind: task
status: done
---
# Heavy slam radius −15 %

- request: "Diminue la zone d'attaque du gros de 15%" (owner, 2026-10-07)
- goal: Heavy slam radius 400 → 340 uu (trigger 0.7 × radius = 238 uu); ground ring follows.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Heavy AttackRange 340 in EnemyStats.csv: telegraph 238 uu, chase stop 170 uu; BP_Heavy ring follows GetSlamRadius (scale 6.8); heavy tests updated (hit 330, miss 350). Build 0, 195/195. Commit f51c081.

## Subtasks
1. task-039-01-heavy-radius — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-039-01-heavy-radius]] — step 1, cpp.
