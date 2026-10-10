---
id: task-071-01-robot-light-movement
description: "Robots move with a light custom movement instead of CharacterMovement, same behaviour in play, game thread measured before and after on the real-game stress."
parent: task-071-robot-light-movement
kind: task
status: done
---
# Robot light movement

- goal: Robots move with a light custom movement instead of CharacterMovement, same behaviour in play, game thread measured before and after on the real-game stress.
- agent: cpp
- depends_on: none
- inputs: sys-enemies-movement (vault/reference/tech/systems/sys-enemies/sys-enemies-movement.md), sys-enemies-base (vault/reference/tech/systems/sys-enemies/sys-enemies-base.md), sys-enemy-ai (vault/reference/tech/systems/sys-enemy-ai.md), sys-flow-field (vault/reference/tech/systems/sys-flow-field.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), perf-risks (vault/reference/tech/performance/perf-risks.md), enemies-index (vault/reference/gdd/enemies/enemies-index.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), shotgun (vault/reference/gdd/weapons/shotgun.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: measure first: real-game stress (`rr.Stress.Run 250 30 60 1 game`, 2 runs) and S0 400 in editor -game, as the before values (same session, owner normal PC profile, close nothing). Then replace the robots' CharacterMovement with a light movement for a flat floor: kinematic step along the flow-field direction at the GDD speeds (180 uu/s, heavy 153), one cheap sweep (or grid lookup) against rock with sliding, no floor search, no walking physics, gravity only if a robot can leave the floor; keep the existing light separation (robots never block each other), shotgun knockback 250 uu/s per pellet hit reduced by knockback resistance (heavy high), heavy slam stop, kamikaze charge, shooter detour, relocation teleport, spawn and death paths; cut child-transform and overlap updates of the rigid parts where they are not needed. Keep the change behind a switch (`rr.Enemy.LightMovement`, default on) so before/after can be compared in one build. After: the same runs again; report game-thread avg/p99 and the movement cost per frame (trace or stat scopes) before/after; update existing tests that the change breaks only; docs sys-enemies-movement, stress-grid-cells, perf-risks updated. No gameplay value change.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; before/after numbers recorded in stress-grid-cells; robots still reach the miner around rock in the real-game stress (kills and damage to the miner logged); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 90000
- model: opus
- tokens: input 142, cache write 579900, cache read 9408622, output 74293
- runs: 2
- milestone: M2
- result: Light kinematic robot movement behind rr.Enemy.LightMovement (default on): game thread -2.4 to -4.4 ms at 250 real-game robots, movement 5.56 -> 4.39 ms per frame (Insights), spikes 140-188 -> 75-100; S0 400 game 9.12 -> 7.84. Build 0, 203/204 tests (Integration.WeaponCardEquipsShotgun already failing, card text change). Commits b28e4bf, 918f154.

## Relations
- [[task-071-robot-light-movement]] — parent global task.
