---
id: task-037-ammo-hud
description: "Global task: weapon slot ammo HUD: ammo bar or blocks, red reload bar, turret recharge block, padlocks on slots not owned."
parent: tasks-index
kind: task
status: in-progress
---
# weapon slot ammo HUD: ammo bar or blocks, red reload bar, turret recharge block, padlocks on slots not owned

- request: owner fourth test (2026-10-06): ammo bar above 7 rounds, blocks at 7 or fewer, red reload bar, turret recharge in the leftmost empty block; "Indique juste des slots locké pour l'instant".
- goal: each weapon slot shows its ammo and reload state; empty slots look locked.
- acceptance: (1) build exits 0; tests pass; (2) HUD shows the bar, the blocks, the red reload bar and the turret recharge rule; (3) padlocks; (4) docs; lint clean; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-06). No open value.
- confirmed_by_owner: yes (2026-10-06, part of the fourth test plan)
- milestone: M2
- next_step: task-037-01-hud-ammo-data — cpp: getters; then task-037-02-bp-ammo-hud — bp: widgets, PIE
- result: open

## Subtasks
1. task-037-01-hud-ammo-data — cpp: getters; then task-037-02-bp-ammo-hud — bp: widgets, PIE

## Relations
- [[tasks-index]] — parent index.
- [[task-037-01-hud-ammo-data]] — step 1, cpp.
- [[task-037-02-bp-ammo-hud]] — step 2, bp.
