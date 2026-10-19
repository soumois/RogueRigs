---
id: task-232-01-drone-single-beam
description: "The drone shows one laser beam aimed at the middle of the dug blocks, whatever their number."
parent: task-232-drone-single-beam
kind: task
status: in-progress
---
# One drone beam

- goal: The drone shows one laser beam aimed at the middle of the dug blocks, whatever their number.
- agent: cpp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/combat/sys-drone.md), sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), agent-cpp (vault/reference/agents/agent-cpp.md)
- expected_output: ARRDrone Dig state draws one beam (LaserMesh) from the drone to the mean of GetDigTargetPoint over the dug cells (projected on the wall face); the LaserBeams fan removed or reduced to one; hack beam unchanged; drone GDD and sys-drone-dig docs updated; -game capture of a multi-block dig.
- done_when: build exits 0 editor closed; RogueRigs.* tests pass except the 2 known pre-existing failures (tests expecting one beam per block updated); capture sent to the owner; docs updated.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-232-drone-single-beam]] — parent global task.
