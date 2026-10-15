---
id: task-194-01-debris-pool-counter
description: "Find why debris pool plays exceed the cap of 80 since the GPU emitter, and fix the counter or recycling."
parent: task-194-debris-pool-counter
kind: task
status: done
---
# Robot debris: pool plays above the cap

- goal: Find why debris pool plays exceed the cap of 80 (S7: 200 plays, 0 skips) since the GPU emitter, and fix the counter or the recycling.
- agent: cpp
- depends_on: none
- inputs: sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), enemy-death (vault/reference/gdd/enemies/enemy-death.md), sys-stress-diagnostics (vault/reference/tech/systems/tools/sys-stress/sys-stress-diagnostics.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: The cause proven with a -game run (URRNiagaraPoolSubsystem: when components return to the free list after Spawn on the GPU NS_RobotDebris, OnSystemFinished timing, IsActive / completion state, whether the system is still compiling at first use and the component completes at once, Verbose logs); fix in C++ so a component is only reclaimed when its effect really finished (or the counter counts real plays) and the S7 200 step shows at most 80 debris plays with the rest skipped; sys-actor-pool updated
- done_when: Cause proven by run values; after the fix an S7 burst 200 run shows debris plays <= 80 and skips for the rest, and a game-mode run still shows debris (plays > 0); build passes with the editor closed; Stress tests pass; committed by path
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 82, cache write 110628, cache read 3010827, output 25688
- runs: 1
- milestone: M3
- result: Cause: at first use in editor -game the GPU NS_RobotDebris compiles and activated components wait (inactive, tick on); the pool reclaimed them as idle (S7: 80 effects, 200 counted plays, earlier debris overwritten). Fix in URRNiagaraPoolSubsystem: waiting components not reclaimed (IsWaitingToStart), play counted only if active after Activate, editor-only background compile at prewarm; S7 200 now 80 plays / 120 skips (both bursts), game 200 219 plays; build OK, tests 29/30 (known EyeMesh); cooked build and on-screen look unverified; commits e62af85f, 978026c7

## Relations
- [[task-194-debris-pool-counter]] — parent global task.
