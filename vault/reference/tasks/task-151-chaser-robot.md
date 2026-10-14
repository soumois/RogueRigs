---
id: task-151-chaser-robot
description: "Global task: Build the chaser robot: turnaround sheet, parts model, baked run and attack animation, in-game integration and a cost check."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser: turnaround sheet

- request: Next robot is the chaser (the most numerous, so the performance impact shows early); animation by baked texture, modelled part by part in Blender, 10 rigid parts, two clips run and attack (owner, 2026-10-13); first step: isolate the chaser and get it from several angles so the agents can reproduce it faithfully (owner, 2026-10-14)
- goal: Build the chaser robot: turnaround sheet, parts model, baked run and attack animation, in-game integration and a cost check.
- acceptance: ref-chaser.png exists and shows only the chaser; at most 4 candidate sheets generated (24 credits at most); each has the four views of the same robot; self-review written per sheet; prompts and logs committed; no image in vault/assets/; no 3D, no Blueprint, no Unreal; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-14).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-151-12-chaser-assembly
- planned (subtasks are created when the previous step is validated): 02 the owner picks a sheet, crops of the four views go to vault/assets; 03 the 3d agent models the 10 parts on the Blender route with the fidelity loop against the four views; 04 the 3d agent authors the run and attack clips and bakes them into a texture (rigid parts: one transform per part and per frame) with the part meshes for the death effect; 05 the bp and cpp agents build the material and the chaser Blueprint (clip and phase as Custom Primitive Data), keeping the shield variant and the death effect; 06 a cost check with rr.Stress.Spawn against the capsule baseline; step 06 runs after the integration (the owner wants the cost of the most numerous robot)
- result: open

## Subtasks
1. task-151-01-chaser-robot — art (turnaround sheets, done)
2. task-151-02-chaser-sheet-simplified — art (simplified sheet, done; variant 2 validated)
3. task-151-03-chaser-model — 3d (the 10 parts in Blender, done: 1632 tris)
4. task-151-04-chaser-refine — 3d (refinement pass of the monolithic model, done: 2056 tris, saved as git tag backup-chaser-monolithic)
5. task-151-05-chaser-split — 3d (split the script by part, no geometry change)
6. task-151-06-chaser-part-headtorso — 3d (HeadTorso)
7. task-151-07-chaser-part-pelvis — 3d (Pelvis)
8. task-151-08-chaser-part-upperarm — 3d (UpperArm)
9. task-151-09-chaser-part-forearmhand — 3d (ForearmHand)
10. task-151-10-chaser-part-thigh — 3d (Thigh)
11. task-151-11-chaser-part-shinfoot — 3d (ShinFoot)
12. task-151-12-chaser-assembly — 3d (assembly and validation)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-151-01-chaser-robot]] — step 1, art.
- [[task-151-02-chaser-sheet-simplified]] — step 2, art.
- [[task-151-03-chaser-model]] — step 3, 3d.
- [[task-151-04-chaser-refine]] — step 4, 3d.
- [[task-151-05-chaser-split]] — step 05
- [[task-151-06-chaser-part-headtorso]] — step 06
- [[task-151-07-chaser-part-pelvis]] — step 07
- [[task-151-08-chaser-part-upperarm]] — step 08
- [[task-151-09-chaser-part-forearmhand]] — step 09
- [[task-151-10-chaser-part-thigh]] — step 10
- [[task-151-11-chaser-part-shinfoot]] — step 11
- [[task-151-12-chaser-assembly]] — step 12
