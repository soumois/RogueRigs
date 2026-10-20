---
id: task-251-01-drone-patrol-movement
description: "The drone patrols a point inside the miner zone and follows it with a spring (faster when far, slower when near); the point changes when reached or every 3.5-5 s; wall rules are kept."
parent: task-251-drone-patrol-movement
kind: task
status: done
---
# Drone patrol movement

- goal: The drone patrols a point inside the miner zone and follows it with a spring (faster when far, slower when near); the point changes when reached or every 3.5-5 s; wall rules are kept.
- agent: cpp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md)
- expected_output: C++ change in Source/RogueRigs/Drone/: patrol point chosen 60-160 uu from the miner on a clear cell (task-248 rules kept), re-picked when the drone is within 25 uu or after 3.5-5 s (random); follow speed grows with the distance (about 40 to 700 uu/s) with a lag spring; independent of the miner aim and movement direction, the miner moving only drags the zone; provisional values EditDefaultsOnly; drone.md (Orbit rule, provisional, owner 2026-10-20), sys-drone, sys-drone-dig updated; the DroneEscort test adapted to the patrol.
- done_when: Build passes; existing drone, dig and run tests pass (HealVein excluded, task-250); ONE headless position check (no capture): with the miner at rest the drone offset does not depend on the miner facing, it stays inside the zone and its patrol point changes within 5 s; when the miner walks away the drone lags behind then catches up; walls rule still holds (0 frames in a block); docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: opus
- tokens: input 112, cache write 196017, cache read 7636525, output 39244
- runs: 1
- milestone: M1
- result: Patrol replaces the escort: point 60-160 uu from the miner relative to it, re-picked on reach (25 uu) or 3.5-5 s, spring lag with speed 40-700 uu/s by distance; independent of miner facing. Build OK; DronePatrol check: two opposite facings give 0.000 uu offset difference, 0 frames out of the zone, drone 86.5 uu behind a miner walking at 600 uu/s then within 30 uu of its point 0.5 s after the stop; 0 frames in a block. HealVein still fails (task-250). Not seen in play: at rest the drone reaches its points in under a second (14 hops in 10 s), may look busy.

## Relations
- [[task-251-drone-patrol-movement]] — parent global task.
