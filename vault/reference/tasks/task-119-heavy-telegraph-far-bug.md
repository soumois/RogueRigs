---
id: task-119-heavy-telegraph-far-bug
description: "Global task: A heavy robot shows its slam zone only during its telegraph, never while it runs toward the miner from far away."
parent: tasks-done-index
kind: task
status: done
---
# Heavy zone visible when far

- request: Bug: les robots lourds ont leur zone d'attaque visible alors qu'ils sont loin et courent vers le joueur (owner, 2026-10-13)
- goal: A heavy robot shows its slam zone only during its telegraph, never while it runs toward the miner from far away.
- acceptance: cause written in the result; build exits 0 with the editor closed; RogueRigs.Heavy.* and RogueRigs.GroundTelegraph.* pass headless; sys-enemies-heavy updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13)
- milestone: M2
- next_step: none
- result: Done. Cause: BP_Heavy ShockRing turned on by SetMeshesVisible(true) (relocation, death) and kept after ResetStateMachine; fixed in RREnemy/RRHeavy; 13 Heavy and GroundTelegraph tests pass; commit e57e4a3

## Subtasks
1. task-119-01-heavy-telegraph-far-bug — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-119-01-heavy-telegraph-far-bug]] — step 1, cpp.
