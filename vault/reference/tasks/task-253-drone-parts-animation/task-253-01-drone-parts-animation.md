---
id: task-253-01-drone-parts-animation
description: "The drone has three animated parts: the antenna spins, the base turns slowly toward the mining spot and the laser pivots in height."
parent: task-253-drone-parts-animation
kind: task
status: in-progress
---
# Drone parts animation

- goal: The drone has three animated parts: the antenna spins, the base turns slowly toward the mining spot and the laser pivots in height.
- agent: cpp
- depends_on: task-251-01-drone-patrol-movement
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), sys-player (vault/reference/tech/systems/core/sys-player.md), sys-grid-api (vault/reference/tech/systems/world/sys-grid/sys-grid-api.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md)
- expected_output: C++ in Source/RogueRigs/Drone/: components BaseMesh (yaw), LaserPivot (pitch) and AntennaMesh (spin) on ARRDrone; antenna spin speed; base yaw turns toward the dig or hack spot at a limited rate (not instantly); laser pivot aims at GetDigImpactPoint and the beam starts at its emitter; provisional values EditDefaultsOnly; docs updated.
- done_when: Build passes; existing drone tests pass (HealVein excluded); ONE headless check (no capture): during Dig the base yaw error to the target shrinks over time without jumping and the laser pivot pitch points at the impact point within a stated tolerance; antenna rotates; docs updated; lint clean.
- editor_state: closed (C++ build)
- token_ceiling: 150k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-253-drone-parts-animation]] — parent global task.
