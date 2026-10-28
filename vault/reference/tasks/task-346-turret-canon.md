---
id: task-346-turret-canon
description: "Global task: Rebuild the optimised Meshy turret canon by Blender script after the unoptimised mesh, with a perforated machine-gun shroud of real holes."
parent: tasks-done-index
kind: task
status: done
---
# Turret mesh: fix the canon of the optimised version

- request: Point 1 of the owner message (2026-10-28, verbatim, French): "Tu as une version optimisée pour le jeu et l'autre non, mais le canon de la version optimisée ne va pas. Tu as le choix de reprendre le canon de la version non optimisée et de l'intégrer dans la version optimisée ou tu t'inspires de la version non optimisée pour recréer le canon." Full message and the two zips: [[task-345-turret-meshy-files]] `request`. Point 2 (flat surfaces) is [[task-347-turret-flat-surfaces]]. Owner answer 2026-10-28 (relayed by the main session, English translation of his French message): "Yes, you just need to manage to redo the holes in the second layer of the cylinder so it looks like a machine gun. The budget is 10,000." Read as: "Yes" = defaults of Q1 to Q5 accepted (Q1 = B); new requirement on the canon (perforated outer shroud, real holes, task-346); Q6 changed: triangle budget 10,000. Owner decision 2026-10-28 on the task-346-01 blocker (10,000 unreachable, body outside the canon about 14,300 tris; relayed by the main session, English translation of his French message): "Keep the budget at 16,000, acceptable because this doesn't exist in many iterations" (the turret is not spawned in large numbers). Read as option 1: turret budget 16,000 triangles (was 10,000); the body of the optimised mesh stays as it is (15,455 tris, no decimation), only the canon is rebuilt with the perforated shroud holes (real geometry), within 16,000 in total.
- goal: The game-ready (optimised) turret mesh has a correct canon rebuilt by Blender script after the unoptimised mesh, its outer barrel layer perforated with real holes so it reads as a machine gun, the rest of the optimised mesh unchanged.
- acceptance: `Art/Meshes/turret/SM_Turret.fbx` is the optimised mesh with the new canon (Q1 = B Blender rebuild, material per Q2); the second (outer) layer of the barrel cylinder has real through holes matching the unoptimised hole pattern (count, size, spacing); flat faces truly flat; body outside the canon unchanged, no decimation; total under 16,000 triangles; canon length, radius, position and axis match the unoptimised canon within 2 % of the turret height; report and preview renders (before, after, unoptimised reference, same cameras) exist and are sent to the owner by the Executor; art-prompts-turret updated; lint clean; committed.
- gdd_check: read 2026-10-28: [[turret]] (approved; white + teal + rust body; the placed turret yaws toward its target, today a grey-box head and barrel in [[bp-turret]]), [[asset-pipeline]] (turret budget 2500 tris provisional, then 10,000, now 16,000; pivot bottom centre; Meshy textures allowed, A1 level 3), [[art-3d-lessons]] (owner wants to see what Claude can do in Blender; one change per pass, renders shown), [[agent-3d]]. No conflict. Questions asked 2026-10-28, all DECIDED by the owner "Yes" the same day (values provisional, owner 2026-10-28):
  - Q1 canon method, owner's choice, Planner does not decide: (A) cut the canon out of the unoptimised mesh and graft it on the optimised one, or (B) model a new canon in a Blender script after the unoptimised one (dimensions, segments, details measured on it). Planner recommendation and default: B. DECIDED: B. A canon is cylinders and blocks: a scripted canon has true flat facets and straight edges (the very defect of point 2), few triangles and a clean axis for a future turning head; A carries dense Meshy topology that must be decimated to fit, which is what produces bulged faces.
  - Q2 canon texture (both methods): default: its own material slot `turret_canon` with base colour, normal and roughness (1024 px) baked from the unoptimised canon textures; the optimised atlas is not repacked. DECIDED: default.
  - Holes (new owner requirement 2026-10-28): the second (outer) layer of the barrel cylinder is a perforated machine-gun shroud: rows of real holes in the geometry (not texture only), round or rectangular as on the unoptimised mesh, with count, size and spacing measured on the unoptimised mesh; the inner barrel stays visible through them.
  - Q5 no Unreal import and no cut into base and turning head in this plan — DECIDED: default (a later request).
  - Q6 DECIDED (owner changed it twice): turret triangle budget 16,000 (owner 2026-10-28 after the task-346-01 blocker; was 10,000, first 2500 provisional; written in asset-pipeline); the optimised body (15,455 tris) is kept as it is, only the canon is rebuilt; flat faces stay truly flat; no decimation.
- confirmed_by_owner: yes (2026-10-28, "Yes" to the plan and the Q1-Q5 defaults, budget 10,000, holes in the outer barrel layer; budget raised to 16,000 the same day after the blocker; relayed by the main session)
- milestone: M2
- next_step: none
- started: 2026-10-28T06:47:37Z
- waiting_for: none
- result: Done. Canon rebuilt in Blender (commit 06e565d9): 1,374 canon tris, total 15,748 under 16,000; 15 real round holes in the shroud matching the unoptimised pattern (rows 1,2,2,1,3,5,1); axis/length/radii within 2 pct; body vertices and UVs unchanged. Warnings: hook bracket at canon rear lost, underside arc not searched for holes, normal bake detail unverified.

## Subtasks
1. task-346-01-turret-canon (vault/reference/tasks/task-346-turret-canon/task-346-01-turret-canon.md) — 3d: replace the optimised turret canon (graft or Blender rebuild per Q1) and render before, after and reference.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-346-01-turret-canon]] — step 1, 3d.
- [[task-345-turret-meshy-files]] — must be done first: saves and identifies the two meshes.
- [[task-347-turret-flat-surfaces]] — point 2, runs on this task's output.
