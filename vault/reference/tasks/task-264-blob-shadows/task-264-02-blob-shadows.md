---
id: task-264-02-blob-shadows
description: "A cheap blob shadow component under the robots, the miner and the drone."
parent: task-264-blob-shadows
kind: task
status: in-progress
---
# Blob shadow component

- goal: A cheap blob shadow component under the robots, the miner and the drone.
- agent: cpp
- depends_on: task-264-01-blob-shadows
- inputs: sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-drone-parts (vault/reference/tech/systems/combat/sys-drone/sys-drone-parts.md), sys-player-classes (vault/reference/tech/systems/core/sys-player/sys-player-classes.md), perf-robot-components-audit (vault/reference/tech/performance/perf-robot-components-audit.md), perf-lead-lumen-robots (vault/reference/tech/performance/perf-leads/perf-lead-lumen-robots.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: C++: a component (engine Plane static mesh, M_BlobShadow, no collision, no shadow, no navigation) on the robot base class, the miner and the drone, sized from the actor footprint (EditDefaultsOnly scale and opacity multiplier), kept just above the floor under the actor, never rotating with the actor tilt, hidden when the actor is pooled or dead, no per-frame spawning, no new tick when possible (attached component follows the actor); the cost for 400 robots checked against the existing perf nodes and stated; docs updated.
- done_when: Build passes; existing enemy, Dig, Ore, PulseDesync, FineContactDigOpensPassage and RunFlow tests pass; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-264-blob-shadows]] — parent global task.
