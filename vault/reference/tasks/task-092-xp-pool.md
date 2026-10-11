---
id: task-092-xp-pool
description: "Global task: XP cubes come from the pool with creation spread over at most 3 frames, so a mass kill no longer spikes the frame."
parent: tasks-done-index
kind: task
status: done
---
# XP cubes from the actor pool

- request: "pool de robots et étalement de l'initialisation (max 20 frames), pool des cubes d'XP (max 3 frames), une classe qui gère ça et généralisée, point 6 chevauchements" (owner 2026-10-10) (owner, 2026-10-11)
- goal: XP cubes come from the pool with creation spread over at most 3 frames, so a mass kill no longer spikes the frame.
- acceptance: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; before/after numbers in stress-grid-cells; docs updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-11).
- confirmed_by_owner: yes (2026-10-11)
- milestone: M2
- next_step: none
- result: Done. XP cubes, ore and heal pickups from the pool (SpawnMaxFrames 3), old 4-per-frame queue removed; game spikes 15-17 -> 1-2 per minute, p99 19.6 -> 18.1-18.9 ms; burst worst frame 204 -> 91 ms; 224/224 tests. Commits 34017e4, a70f821, 454636c, fc0845e.

## Subtasks
1. task-092-01-xp-pool — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-092-01-xp-pool]] — step 1, cpp.
