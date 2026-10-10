---
id: task-045-easier-first-level
description: "Global task: about 15 % fewer robots (lone 2.35 s, waves 7.1 → 4.1 s, hordes 34–51) and rarer heavies."
parent: tasks-done-index
kind: task
status: done
---
# Easier first level

- request: "Le niveau de difficulté est trop élevé pour le premier level (je n’arrive pas à le finir), je pense qu’il faut réduire de 15% le spawn et faire que les gros soient plus rares" (owner, 2026-10-07)
- goal: About 15 % fewer robots (lone 2.35 s, waves 7.1 → 4.1 s, hordes 34–51) and rarer heavies (weight 4, 2–3 per horde, cap 3).
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. BP_EnemySpawner BP_Heavy MaxAlive 5 → 3 (placed spawner reads 3, no override on disk). Doc bp-enemy-spawner. Commit 1531665. With task-045-01: about 15 % fewer robots, rarer heavies, overtime hordes shrink too (copy of the first horde), 195/195.

## Subtasks
1. task-045-01-easier-first-level — cpp
2. task-045-02-bp-heavy-cap — bp: heavy alive cap 3 on BP_EnemySpawner

## Relations
- [[tasks-done-index]] — parent index.
- [[task-045-01-easier-first-level]] — step 1, cpp.
- [[task-045-02-bp-heavy-cap]] — step 2, bp.
