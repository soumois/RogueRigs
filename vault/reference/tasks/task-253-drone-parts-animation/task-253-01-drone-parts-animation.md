---
id: task-253-01-drone-parts-animation
description: "The drone has three animated parts: the antenna spins, the base turns slowly toward the mining spot and the laser pivots in height."
parent: task-253-drone-parts-animation
kind: task
status: done
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
- model: opus
- tokens: input 100, cache write 174743, cache read 6080715, output 26587
- runs: 1
- milestone: M1
- result: Components LaserPivot and AntennaMesh on ARRDrone (root Mesh = base); antenna spins 90 deg/s; base yaw turns toward the dig/hack target at 120 deg/s max; laser pitch aims the emitter at the impact point (240 deg/s, -20..+80, 0 = down); beam starts at the emitter tip (GetLaserEmitterLocation). Build OK, 25 tests pass (HealVein known), DroneParts: yaw error 104 to under 2 deg by frame 12 with no jump, arm axis within 0.46 deg of the beam, antenna 90.00 deg in 1 s. Mesh checks: antenna vertex colours had been lost at import, reimported with replace (3 colour classes read back); engine lightmap UVs removed on the three meshes. Not seen in play, hack not measured.

## Relations
- [[task-253-drone-parts-animation]] — parent global task.
