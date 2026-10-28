---
id: task-347-turret-flat-surfaces
description: "Global task: Make the surfaces that should be flat on the optimised Meshy turret flat again, using the unoptimised mesh as the reference."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Turret mesh: flatten the bulged flat surfaces of the optimised version

- request: Point 2 of the owner message (2026-10-28, verbatim, French): "Aussi dans la version optimisée, les surfaces qui devraient être plates sont bombées. (inspire-toi aussi de la version non optimisée)". Full message and the two zips: [[task-345-turret-meshy-files]] `request`. Point 1 (canon) is [[task-346-turret-canon]].
- goal: On the game-ready (optimised) turret mesh, every surface that is flat on the unoptimised mesh is flat again, without changing the silhouette, the texture or the canon.
- acceptance: `Art/Meshes/turret/SM_Turret.fbx` updated: every planar region found on the unoptimised mesh (Q3) has its optimised vertices on one plane; UVs and textures unchanged; canon of task-346 untouched; report and grazing-light renders (before, after, unoptimised reference, same cameras) sent to the owner by the Executor; art-prompts-turret updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[turret]], [[asset-pipeline]], [[art-style-guide]] (chunky volumes, large flat facets), [[art-3d-lessons]] (one change per pass, owner judges renders). No conflict. Runs after [[task-346-turret-canon]] on its output, so both edits land on one mesh. Questions asked 2026-10-28 (answers pending):
  - Q3 which surfaces: default: found automatically on the unoptimised mesh (connected faces whose normals stay within 3 degrees of their mean, region area at least 0.5 % of the mesh surface; thresholds provisional, logged), all flattened in one pass outside the canon, then the owner points at any surface still bulged or wrongly flattened on the renders.
  - Q5 and Q6 as in [[task-346-turret-canon]] (no Unreal import or part cut; counts reported, no decimation).
- confirmed_by_owner: no
- milestone: M2
- next_step: task-347-01-turret-flat-surfaces
- started: open
- waiting_for: owner confirmation of the plan and answers to Q1-Q6
- result: open

## Subtasks
1. task-347-01-turret-flat-surfaces (vault/reference/tasks/task-347-turret-flat-surfaces/task-347-01-turret-flat-surfaces.md) — 3d: project the optimised vertices of each planar region of the unoptimised mesh onto its plane and render before, after and reference.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-347-01-turret-flat-surfaces]] — step 1, 3d.
- [[task-345-turret-meshy-files]] — must be done first: saves and identifies the two meshes.
- [[task-346-turret-canon]] — must be done first: its SM_Turret.fbx is this task's input.
