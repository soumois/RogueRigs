---
id: task-093-01-robot-overlap-cost
description: "Robot components audited, then unneeded attached-part transforms and overlap updates are removed, with before/after numbers."
parent: task-093-robot-overlap-cost
kind: task
status: done
---
# Robot overlap and attached parts cost

- goal: Robot components audited, then unneeded attached-part transforms and overlap updates are removed, with before/after numbers.
- agent: cpp
- depends_on: task-092-01-xp-pool
- inputs: sys-enemies-movement (vault/reference/tech/systems/sys-enemies/sys-enemies-movement.md), sys-enemies-base (vault/reference/tech/systems/sys-enemies/sys-enemies-base.md), perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: step 1, audit (a note in tech/performance, no change yet): for each robot class (chaser, shield chaser, shooter, kamikaze, heavy) list the components (root capsule, hit capsule, attached rigid parts, others), which generate overlap events, which are attached to what, collision profiles, and what the owner-facing behaviours need (shots must overlap the hit capsule; the death effect needs the rigid parts). Use an Insights capture like task-068-01 (cheap) to rank what remains of the robot movement cost after task-071 (about 21 us per robot: attached-part transform updates and the hit capsule overlap update). Step 2: remove the unneeded: parts that need no overlap (bGenerateOverlapEvents off, no update-overlaps on move), fewer attached components while the robot is alive (parts can be attached or enabled only at death), the hit capsule updating overlaps only when needed; behaviour unchanged (a shot hits where it hit before). BEFORE and AFTER with `rr.Stress.Run 250 30 60 1 game` twice each and S0 400; report game-thread avg/p99 and the Insights movement ms per frame. Update only tests this breaks plus a test that shots still hit and the death effect still plays its parts. Docs sys-enemies-movement, perf-trace-real-game or a new note, stress-grid-cells, sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; audit note and before/after numbers recorded; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 108, cache write 128351, cache read 4650075, output 42020
- runs: 1
- milestone: M2
- result: Robot hit capsule no longer runs its own overlap query on move (URRHitCapsuleComponent, rr.Enemy.PassiveHitCapsule): overlap searches 0.61 -> 0.016 ms per frame, robot movement 3.64 -> 3.16 ms per frame; game-thread gain within noise; audit note robot-components-audit; 225/225 tests. Commits e852c95, 1deed6f, c23fce0.

## Relations
- [[task-093-robot-overlap-cost]] — parent global task.
