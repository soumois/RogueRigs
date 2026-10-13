---
id: task-115-01-heavy-slam-telegraph
description: "Heavy slam ground signal: orange zone and growing circle during the telegraph, gone when the robot dies."
parent: task-115-heavy-slam-telegraph
kind: task
status: done
---
# Heavy slam ground signal

- goal: The heavy slam shows its danger zone and a growing circle on the ground during the telegraph, and the signal dies with the robot.
- agent: cpp
- depends_on: task-113-01-capture-zone-visual
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/sys-enemies/sys-enemies-heavy.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-actor-pool (vault/reference/tech/systems/sys-actor-pool.md), bp-heavy (vault/reference/tech/blueprints/bp-heavy.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner 2026-10-12: same logic as the barrel and the capture zones for the heavy attack: ARRHeavy owns a URRGroundTelegraphComponent (task-112-01); during the Telegraph state (0.6 s) it shows the danger zone at the slam radius (289 uu, read the actual value from the stats: it follows the slam radius data) in Danger mode (orange) and the progress = GetTelegraphProgress (0 -> 1, the growing circle reaches the edge when the slam lands); hidden at Slam, Recover, when the heavy dies during the telegraph (the signal must disappear with the robot at once: IsDeathPending / death), and when the heavy is released to the pool (reset hook: no stale signal on a reused heavy). The slam gameplay (trigger distance, radius, damage, cadence) is unchanged. Tests: visible only during the telegraph with the right radius and progress; hidden on death mid-telegraph; hidden after pool release and reuse; slam unchanged. Docs sys-enemies-heavy, sys-ground-telegraph, sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 58, cache write 92271, cache read 1797231, output 26427
- runs: 1
- milestone: M2
- result: ARRHeavy GroundTelegraph (Danger, slam radius from Stats.AttackRange = 289, progress = GetTelegraphProgress, hidden at slam/recover/death/pool release); 3 tests; 258/258. Commits 809054d, 37766be.

## Relations
- [[task-115-heavy-slam-telegraph]] — parent global task.
