---
id: task-039-heavy-radius
description: "Global task: Heavy slam radius 400 → 340 uu (trigger 0.7 × radius = 238 uu); ground ring follows."
parent: tasks-index
kind: task
status: in-progress
---
# Heavy slam radius −15 %

- request: "Diminue la zone d'attaque du gros de 15%" (owner, 2026-10-07)
- goal: Heavy slam radius 400 → 340 uu (trigger 0.7 × radius = 238 uu); ground ring follows.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-039-01-heavy-radius
- result: open

## Subtasks
1. task-039-01-heavy-radius — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-039-01-heavy-radius]] — step 1, cpp.
