---
id: task-059-damage-numbers-no-merge
description: "Global task: damage numbers one per hit, no merging, random start in a 30 uu disc (45 uu on the heavy); stacking kept."
parent: tasks-done-index
kind: task
status: done
---
# Damage numbers without merging

- request: "ne plus fusionner les chiffres de dégâts. Il devrait avoir un radius de spawn sur le monstre" (owner, 2026-10-07)
- goal: Damage numbers: one per hit, no merging, each starting at a random point of a 30 uu disc (45 uu on the heavy); stacking kept.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. URRDamageNumberSubsystem: 0.25 s merge window removed (one number per hit, pushed next tick); random start in a flat disc of SpawnRadius 30 uu (ARREnemy::DamageNumberSpawnRadius, 45 on ARRHeavy) replacing the ±25 uu jitter; stacking kept; AddHit gains InSpawnRadius. MergeWindow test replaced by OnePerHit; CriticalStyle, TickBudget, NiagaraIntegration expect one number per hit. Full run 194/195: FlowField.DestroyedRockOpensPocket (0.3 ms per-tick budget, load-sensitive) failed, passes alone. Commit b709cf0.

## Subtasks
1. task-059-01-damage-numbers-no-merge — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-059-01-damage-numbers-no-merge]] — step 1, cpp.
