---
id: task-395-burst-rifle-rebake
description: "Global task: Re-unwrap the flattened burst rifle and bake the Meshy look (colour, normal, roughness, metallic) onto it, as for the extractor, so the textures fit the flat faces."
parent: tasks-to-do-index
kind: task
status: todo
---
# Burst rifle mesh: re-bake the Meshy textures on clean UVs

- request: "Voilà l'arme qui ira dans les mains du character, applique les techniques pour applatir les surfaces et rebake les textures" (owner, 2026-10-30, verbatim, French; attached `C:/Users/soumo/Downloads/Meshy_AI_Aqua_Wasteland_Carbine_20261030_texture_remesh_tri_fbx.zip`). Point 2 of 2: "rebake les textures", after point 1 [[task-394-burst-rifle-flat-faces]] (intake and flattening), which holds the measures, Q1 to Q6 and the scope (Q2, asset only: no Unreal import, no Blueprint, no C++, no socket, no -game capture). Recipe: [[task-368-extractor-rebake]] (new UV0, four maps baked from the source mesh in headless Blender, geometry kept). Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q7, Q8 accepted.
- goal: `Art/Meshes/burst-rifle/SM_BurstRifle.fbx` with the same geometry as task-394 and a clean UV0, and four maps baked from the Meshy rifle, so the texture fits the flattened faces without stretch.
- acceptance: Geometry unchanged versus the task-394 SM_BurstRifle.fbx (same triangles, vertex positions, normals and flat shading); new UV0 per Q7 (no overlap, all islands in 0-1, stretch lower than the old UV0, numbers in the report); four maps per Q7 and Q8 baked from the Meshy rifle, no unbaked hole inside an island; flattened regions still read flat (normal-map tilt on them reported, not above the tilt of the Meshy normal map on the same regions); renders from the same cameras: Meshy source, task-394 SM_BurstRifle and re-baked SM_BurstRifle, with two close-ups (receiver side plate, stock), sent to the owner by the Executor; burst-rifle-mesh updated; no Unreal import; lint clean; committed by path.
- gdd_check: read 2026-10-30: [[asset-pipeline]] (texture rule A1 level 3: per-asset painted base colour, normal and roughness maps and Meshy textures allowed: no conflict), [[burst-rifle]], [[art-3d-lessons]], precedent [[task-368-extractor-rebake]] (its first 3d run hit the turn limit on a 47,000-triangle mesh; this mesh is 9,892, so one subtask is kept). No gdd node sets a texture resolution, a UV method or a normal-map convention for the rifle: Q7 and Q8. No conflict, same milestone as task-394 (M2 art pass).
- confirmed_by_owner: yes (2026-10-30, "ok")
- milestone: M2
- renumbered: was task-393-burst-rifle-rebake (id clash with task-393-miner-over-lit, owner-approved, 2026-10-30); commit 49ab8c99 carries the old id.
- next_step: task-395-01-burst-rifle-rebake-maps
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-30.
- Q7 UVs and resolution: a new UV0 replaces the Meshy one: seams on the flattened region borders and sharp edges (60 deg), angle-based unwrap, islands packed at uniform texel density with an 8 px margin, four maps (base colour, normal, roughness, metallic) at 2048 x 2048 like the Meshy maps, one material slot. Default: yes.
- Q8 what is baked: the source is the Meshy rifle with its own UVs and maps, placed in the SM_BurstRifle frame with the task-394 flattening applied (same vertex projection), so flat faces bake flat; base colour, roughness and metallic come from the Meshy maps; the normal map keeps the Meshy detail (bolts, rivets, panel lines), tangent space, OpenGL (+Y) like the Meshy map (the DirectX flip is left to the later Unreal import); 16 px dilation; `SM_BurstRifle.fbx` and `T_BurstRifle_*.png` replaced in place (task-394 version in git history, Meshy originals in `Art/Source/burst-rifle/meshy/`). Default: yes.

## Subtasks
1. task-395-01-burst-rifle-rebake-maps (vault/reference/tasks/task-395-burst-rifle-rebake/task-395-01-burst-rifle-rebake-maps.md) — 3d: re-unwrap SM_BurstRifle (geometry kept), bake the four Meshy maps onto it, report stretch and fidelity, render source, old and new.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-395-01-burst-rifle-rebake-maps]] — step 1, 3d.
- [[burst-rifle]] — the weapon this mesh is for.
- [[task-394-burst-rifle-flat-faces]] — made the mesh and holds Q1 to Q6.
- [[task-368-extractor-rebake]] — the re-bake recipe reused.
