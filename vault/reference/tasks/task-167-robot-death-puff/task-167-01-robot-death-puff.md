---
id: task-167-01-robot-death-puff
description: "At a robot death, play a pooled Niagara puff of sparks that hides the 0.15 s shrink (chaser and shield chaser first)."
parent: task-167-robot-death-puff
kind: task
status: done
---
# Robot death: puff hiding the shrink

- goal: At a robot death, play a pooled Niagara puff of sparks that hides the 0.15 s shrink (chaser and shield chaser first).
- agent: cpp
- depends_on: none
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-tests-enemies (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-enemies.md)
- expected_output: In ARREnemy (Source/RogueRigs/) an EditDefaultsOnly UNiagaraSystem* DeathPuffSystem (Enemy|Death, null = no puff) spawned at the robot centre when HandleDeath starts, through a pool of at most 40 Niagara components (world subsystem or the existing pool pattern, reused, never destroyed per death; pool empty = skip); auto-release when the system finishes; sys-enemies-base doc updated (death bullet); build passes
- done_when: Build passes with the editor closed; the existing enemy tests still pass (only update an expectation this change breaks); the pool cap 40 is a tunable default; doc updated; committed by path
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 44, cache write 61700, cache read 897166, output 19167
- runs: 1
- milestone: M3
- result: URRNiagaraPoolSubsystem (world subsystem, one pool per system asset, cap 40, prewarm, return on finish); ARREnemy DeathPuffSystem + DeathPuffPoolSize 40 played at HandleDeath; build OK; 15/16 enemy tests pass (known EyeMesh failure); pool not exercised (no asset yet); commit fba1715

## Relations
- [[task-167-robot-death-puff]] — parent global task.
