---
id: task-347-turret-flat-surfaces
description: "Global task: Make the surfaces that should be flat on the optimised Meshy turret flat again, using the unoptimised mesh as the reference."
parent: tasks-done-index
kind: task
status: done
---
# Turret mesh: flatten the bulged flat surfaces of the optimised version

- request: Point 2 of the owner message (2026-10-28, verbatim, French): "Aussi dans la version optimisée, les surfaces qui devraient être plates sont bombées. (inspire-toi aussi de la version non optimisée)". Full message and the two zips: [[task-345-turret-meshy-files]] `request`. Point 1 (canon) is [[task-346-turret-canon]]. Owner answer 2026-10-28 (relayed by the main session, English translation of his French message): "Yes, you just need to manage to redo the holes in the second layer of the cylinder so it looks like a machine gun. The budget is 10,000." Read as: "Yes" = defaults of Q1 to Q5 accepted (Q1 = B); new requirement on the canon (perforated outer shroud, real holes, task-346); Q6 changed: triangle budget 10,000. Owner decision 2026-10-28 on the task-346-01 blocker (10,000 unreachable, body outside the canon about 14,300 tris; relayed by the main session, English translation of his French message): "Keep the budget at 16,000, acceptable because this doesn't exist in many iterations" (the turret is not spawned in large numbers). Read as option 1: turret budget 16,000 triangles (was 10,000); the body of the optimised mesh stays as it is (15,455 tris, no decimation), only the canon is rebuilt with the perforated shroud holes (real geometry), within 16,000 in total.
- goal: On the game-ready (optimised) turret mesh, every surface that is flat on the unoptimised mesh is flat again, without changing the silhouette, the texture or the canon.
- acceptance: `Art/Meshes/turret/SM_Turret.fbx` updated: every planar region found on the unoptimised mesh (Q3) has its optimised vertices on one plane; UVs and textures unchanged; canon of task-346 untouched; report and grazing-light renders (before, after, unoptimised reference, same cameras) sent to the owner by the Executor; art-prompts-turret updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[turret]], [[asset-pipeline]], [[art-style-guide]] (chunky volumes, large flat facets), [[art-3d-lessons]] (one change per pass, owner judges renders). No conflict. Runs after [[task-346-turret-canon]] on its output, so both edits land on one mesh. Questions asked 2026-10-28, all DECIDED by the owner "Yes" the same day (values provisional, owner 2026-10-28):
  - Q3 which surfaces: default: found automatically on the unoptimised mesh (connected faces whose normals stay within 3 degrees of their mean, region area at least 0.5 % of the mesh surface; thresholds provisional, logged), all flattened in one pass outside the canon, then the owner points at any surface still bulged or wrongly flattened on the renders. DECIDED: default.
  - Q5 DECIDED: no Unreal import or part cut. Q6 DECIDED: turret budget 16,000 triangles (owner 2026-10-28, was 10,000); flat faces truly flat within it; no decimation that bulges faces; the canon and its shroud holes from task-346 are untouched.
- confirmed_by_owner: yes (2026-10-28, "Yes" to the plan and the Q1-Q5 defaults, budget 10,000, holes in the outer barrel layer; budget raised to 16,000 the same day after the task-346 blocker; relayed by the main session)
- milestone: M2
- next_step: none
- started: 2026-10-28T07:02:17Z
- waiting_for: none
- executor_tokens: input 14, cache write 42910, cache read 159845, output 3510
- executor_usage: runs 1, turns 7, context start 16k peak 35k, resumes 0, 9 min, units 0.09 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. Flattened 1,270 body faces (252 regions, Q3 thresholds relaxed: plane distance 0.25 cm, normal 10 deg, area 0.05 %), canon/UV/tris(15,748)/bbox unchanged, residual 0.0012 cm; bulge cause: off-plane vertices (mean 0.15 cm) plus smooth split normals, not the normal map. Commit 6de440e9.

## Subtasks
1. task-347-01-turret-flat-surfaces (vault/reference/tasks/task-347-turret-flat-surfaces/task-347-01-turret-flat-surfaces.md) — 3d: project the optimised vertices of each planar region of the unoptimised mesh onto its plane and render before, after and reference.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-347-01-turret-flat-surfaces]] — step 1, 3d.
- [[task-345-turret-meshy-files]] — must be done first: saves and identifies the two meshes.
- [[task-346-turret-canon]] — must be done first: its SM_Turret.fbx is this task's input.
