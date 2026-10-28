---
id: task-346-turret-canon
description: "Global task: Replace the wrong canon of the optimised Meshy turret with one grafted from or rebuilt after the unoptimised mesh, owner choice."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Turret mesh: fix the canon of the optimised version

- request: Point 1 of the owner message (2026-10-28, verbatim, French): "Tu as une version optimisée pour le jeu et l'autre non, mais le canon de la version optimisée ne va pas. Tu as le choix de reprendre le canon de la version non optimisée et de l'intégrer dans la version optimisée ou tu t'inspires de la version non optimisée pour recréer le canon." Full message and the two zips: [[task-345-turret-meshy-files]] `request`. Point 2 (flat surfaces) is [[task-347-turret-flat-surfaces]].
- goal: The game-ready (optimised) turret mesh has a correct canon whose shape follows the unoptimised mesh, the rest of the optimised mesh unchanged.
- acceptance: `Art/Meshes/turret/SM_Turret.fbx` is the optimised mesh with the new canon (method per Q1, material per Q2); canon length, radius, position and axis match the unoptimised canon within 2 % of the turret height; report and preview renders (before, after, unoptimised reference, same cameras) exist and are sent to the owner by the Executor; art-prompts-turret updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[turret]] (approved; white + teal + rust body; the placed turret yaws toward its target, today a grey-box head and barrel in [[bp-turret]]), [[asset-pipeline]] (turret budget 2500 tris provisional; pivot bottom centre; Meshy textures allowed, A1 level 3), [[art-3d-lessons]] (owner wants to see what Claude can do in Blender; one change per pass, renders shown), [[agent-3d]]. No conflict. Questions asked 2026-10-28 (answers pending):
  - Q1 canon method, owner's choice, Planner does not decide: (A) cut the canon out of the unoptimised mesh and graft it on the optimised one, or (B) model a new canon in a Blender script after the unoptimised one (dimensions, segments, details measured on it). Planner recommendation and default: B. A canon is cylinders and blocks: a scripted canon has true flat facets and straight edges (the very defect of point 2), few triangles and a clean axis for a future turning head; A carries dense Meshy topology that must be decimated to fit, which is what produces bulged faces.
  - Q2 canon texture (both methods): default: its own material slot `turret_canon` with base colour, normal and roughness (1024 px) baked from the unoptimised canon textures; the optimised atlas is not repacked.
  - Q5 no Unreal import and no cut into base and turning head in this plan — default ok (a later request).
  - Q6 if the result exceeds the 2500-tri budget — default: counts reported, nothing decimated in this plan.
- confirmed_by_owner: no
- milestone: M2
- next_step: task-346-01-turret-canon
- started: open
- waiting_for: owner confirmation of the plan and answers to Q1-Q6
- result: open

## Subtasks
1. task-346-01-turret-canon (vault/reference/tasks/task-346-turret-canon/task-346-01-turret-canon.md) — 3d: replace the optimised turret canon (graft or Blender rebuild per Q1) and render before, after and reference.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-346-01-turret-canon]] — step 1, 3d.
- [[task-345-turret-meshy-files]] — must be done first: saves and identifies the two meshes.
- [[task-347-turret-flat-surfaces]] — point 2, runs on this task's output.
