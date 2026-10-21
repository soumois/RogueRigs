---
id: task-264-02-blob-shadows
description: "A cheap blob shadow component under the robots, the miner and the drone."
parent: task-264-blob-shadows
kind: task
status: done
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
- model: opus
- tokens: input 106, cache write 147955, cache read 4555887, output 21432
- runs: 1
- milestone: M1
- result: URRBlobShadowComponent (engine Plane + MI_BlobShadow, no tick, no collision, no cast shadow, sort priority -10, absolute rotation) on ARREnemy (diameter = capsule radius x 2.2, hidden at death, restored on pool reuse), ARRMiner and ARRDrone (70 uu, on the floor under it, shrinks above 130 uu; updated from the drone's own tick). Build OK, 39 of 39 tests pass (Dig, Ore, PulseDesync, FineContact, RunFlow, Enemies); one existing enemy test filters the disc out of its body parts. Perf: translucent draw per robot (400 robots about 400 small draws), instanced alternative written as perf lead, not measured. Not seen on screen.

## Relations
- [[task-264-blob-shadows]] — parent global task.
