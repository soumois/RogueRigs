---
id: task-045-easier-first-level
description: "Global task: about 15 % fewer robots (lone 2.35 s, waves 7.1 → 4.1 s, hordes 34–51) and rarer heavies."
parent: tasks-index
kind: task
status: in-progress
---
# Easier first level

- request: "Le niveau de difficulté est trop élevé pour le premier level (je n’arrive pas à le finir), je pense qu’il faut réduire de 15% le spawn et faire que les gros soient plus rares" (owner, 2026-10-07)
- goal: About 15 % fewer robots (lone 2.35 s, waves 7.1 → 4.1 s, hordes 34–51) and rarer heavies (weight 4, 2–3 per horde, cap 3).
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-045-02-bp-heavy-cap
- result: open

## Subtasks
1. task-045-01-easier-first-level — cpp
2. task-045-02-bp-heavy-cap — bp: heavy alive cap 3 on BP_EnemySpawner

## Relations
- [[tasks-index]] — parent index.
- [[task-045-01-easier-first-level]] — step 1, cpp.
- [[task-045-02-bp-heavy-cap]] — step 2, bp.
