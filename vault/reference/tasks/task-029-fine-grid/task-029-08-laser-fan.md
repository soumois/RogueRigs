---
id: task-029-08-laser-fan
description: "Drone shows one laser beam per dug block of the arc, from a C++ pool of beam components aimed every tick; tests."
parent: task-029-fine-grid
kind: task
status: in-progress
---
# Laser fan

- goal: the drone's arc dig reads as a fan of beams, one per block being cut.
- agent: cpp
- depends_on: task-029-06-bp-fine-grid
- inputs: [[drone]], [[sys-drone]], [[bp-drone]], [[color-code]], [[cpp-standard]], [[sys-automation-tests]]
- expected_output: in ARRDrone, a pool of beam mesh components (size = max arc blocks), each aimed and scaled every tick from the drone to GetDigTargetPoint of one dug cell, unused beams hidden, same mesh and material as the current LaserMesh (exposed so BP_Drone can set them); the single-beam path removed or folded into the pool; tests: beam count equals dug cells, beams hidden on release, beam end at the target point; docs sys-drone (and bp-drone if a Blueprint property must be set, listed for task-029-09).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
