---
id: task-232-01-drone-single-beam
description: "The drone shows one laser beam aimed at the middle of the dug blocks, whatever their number."
parent: task-232-drone-single-beam
kind: task
status: done
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
- model: opus
- tokens: input 72, cache write 103023, cache read 2309924, output 2100
- runs: 1
- milestone: M2
- result: Commits 17210e62 (code, tests), 02a18448 (docs). Dig draws one beam (LaserMesh) to GetDigImpactPoint (mean of the dig target points projected on the nearest face; BlueprintPure for 233-02); fan pool removed; hack beam unchanged. Build 0, tests 262/264 (2 known). Captures Saved/DroneBeam/ show one beam, end point hidden by the miner; corner arcs not checked.

## Relations
- [[task-232-drone-single-beam]] — parent global task.
