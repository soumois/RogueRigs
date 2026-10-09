---
id: task-059-01-damage-numbers-no-merge
description: "Damage numbers: one per hit, no merging, each starting at a random point of a 30 uu disc (45 uu on the heavy); stacking kept."
parent: task-059-damage-numbers-no-merge
kind: task
status: done
---
# Damage numbers without merging

- goal: Damage numbers: one per hit, no merging, each starting at a random point of a 30 uu disc (45 uu on the heavy); stacking kept.
- agent: cpp
- depends_on: none
- inputs: damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), sys-damage-numbers (vault/reference/tech/systems/sys-damage-numbers.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: URRDamageNumberSubsystem: remove the merge of hits on the same target (one number per hit); add a random start offset in a horizontal disc of SpawnRadius (30 uu, editable) around the hit, 45 uu for the heavy (per-class or from the robot hit capsule, editable); keep the vertical stacking against overlap; update only the existing damage-number tests this breaks (merge tests become one-number-per-hit checks); docs sys-damage-numbers.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 78, cache write 213203, cache read 2835294, output 38094
- runs: 1
- milestone: M2
- result: URRDamageNumberSubsystem: 0.25 s merge window removed (one number per hit, pushed next tick); random start in a flat disc of SpawnRadius 30 uu (ARREnemy::DamageNumberSpawnRadius, 45 on ARRHeavy) replacing the ±25 uu jitter; stacking kept; AddHit gains InSpawnRadius. MergeWindow test replaced by OnePerHit; CriticalStyle, TickBudget, NiagaraIntegration expect one number per hit. Full run 194/195: FlowField.DestroyedRockOpensPocket (0.3 ms per-tick budget, load-sensitive) failed, passes alone. Commit b709cf0.

## Relations
- [[task-059-damage-numbers-no-merge]] — parent global task.
