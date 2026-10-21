---
id: task-261-no-shadows
description: "Global task: No object casts a shadow for now: shadow casting is switched off on the lights of the arena level."
parent: tasks-done-index
kind: task
status: done
---
# No shadows

- request: no object should have a shadow for now (owner, 2026-10-20)
- goal: No object casts a shadow for now: shadow casting is switched off on the lights of the arena level.
- acceptance: Shadow casting reads false on every light of the arena level after save; the node lists them with the revert steps; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-20).
- confirmed_by_owner: yes (2026-10-20)
- milestone: M1
- next_step: none
- result: Done. Shadow casting switched off on the only two lights of L_ArenaM1: DirectionalLight_0 and SkyLight_0 (castShadows false, other flags kept; BP_Kamikaze CoreLight was already off); Lumen, materials and per-object flags untouched; node lights-no-shadows with revert steps; A12 and visual-style got a provisional line. Read back false after save. Not seen on screen. Lumen and virtual shadow maps cannot be switched off from here; sky light flag is the first to re-tick if the lighting looks wrong.

## Subtasks
1. task-261-01-no-shadows — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-261-01-no-shadows]] — step 1, bp.
