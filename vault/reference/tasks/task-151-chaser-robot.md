---
id: task-151-chaser-robot
description: "Global task: Build the chaser robot: turnaround sheet, parts model, baked run and attack animation, in-game integration and a cost check."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: turnaround sheet

- request: Next robot is the chaser (the most numerous, so the performance impact shows early); animation by baked texture, modelled part by part in Blender, 13 rigid parts, two clips run and attack (owner, 2026-10-13); first step: isolate the chaser and get it from several angles so the agents can reproduce it faithfully (owner, 2026-10-14)
- goal: Build the chaser robot: turnaround sheet, parts model, baked run and attack animation, in-game integration and a cost check.
- acceptance: ref-chaser.png exists and shows only the chaser; at most 4 candidate sheets generated (24 credits at most); each has the four views of the same robot; self-review written per sheet; prompts and logs committed; no image in vault/assets/; no 3D, no Blueprint, no Unreal; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-151-02-chaser-sheet-simplified
- planned (subtasks are created when the previous step is validated): 02 the owner picks a sheet, crops of the four views go to vault/assets; 03 the 3d agent models the 13 parts on the Blender route with the fidelity loop against the four views; 04 the 3d agent authors the run and attack clips and bakes them into a texture (rigid parts: one transform per part and per frame) with the part meshes for the death effect; 05 the bp and cpp agents build the material and the chaser Blueprint (clip and phase as Custom Primitive Data), keeping the shield variant and the death effect; 06 a cost check with rr.Stress.Spawn against the capsule baseline; step 06 runs after the integration (the owner wants the cost of the most numerous robot)
- result: open

## Subtasks
1. task-151-01-chaser-robot — art (turnaround sheets, done)
2. task-151-02-chaser-sheet-simplified — art (simplified three-view correction of A2)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-151-01-chaser-robot]] — step 1, art.
- [[task-151-02-chaser-sheet-simplified]] — step 2, art.
