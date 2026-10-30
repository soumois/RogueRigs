---
id: task-394-burst-rifle-flat-faces
description: "Global task: Save the owner Meshy burst rifle (Aqua Wasteland Carbine) files, then scale, orient and pivot it and flatten its bulged flat faces as for the turret and extractor."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Burst rifle mesh: save the Meshy files, flatten the faces

- request: "Voilà l'arme qui ira dans les mains du character, applique les techniques pour applatir les surfaces et rebake les textures" (owner, 2026-10-30, verbatim, French; attached `C:/Users/soumo/Downloads/Meshy_AI_Aqua_Wasteland_Carbine_20261030_texture_remesh_tri_fbx.zip`, the Meshy "Aqua Wasteland Carbine", texture, remesh tri, FBX). Main session brief: it is the weapon the character will hold; apply the vault surface-flattening techniques (as for the Meshy chaser, turret and extractor work) and rebake the textures; save the source file in the repo plus a vault node (owner rule: every downloaded mesh goes into the repo and a vault node). Point 1 of 2: "applique les techniques pour applatir les surfaces" (with the source intake). Point 2, "rebake les textures": [[task-395-burst-rifle-rebake]]. "qui ira dans les mains du character" states the purpose; attaching it to the miner is not asked (Q2). Planner measure 2026-10-30 (read only, headless Blender on a scratch copy): zip 27.8 MB, 6 files: FBX 12.8 MB (textures embedded), base colour, metallic, metallic_roughness, normal, roughness PNG, all 2048 x 2048; one mesh `output_unwrapped`, 9,892 triangles, 4,928 vertices, one UV `UVMap`, one material `Material.001`, bounds 190.26 x 23.08 x 65.55 cm in Blender axes (Meshy normalisation, long side on X). No denser Meshy version was given. Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6 accepted.
- goal: A game-ready burst rifle FBX scaled, oriented and pivoted for the hand, whose surfaces that should be flat are truly flat, with the Meshy source files saved and recorded.
- acceptance: Meshy source saved per Q6 and recorded in a new node burst-rifle-mesh; `Art/Meshes/burst-rifle/SM_BurstRifle.fbx` within the Q3 budget, scaled, oriented and pivoted per Q4; every planar region (Q5) has its vertices on one plane (residual under 0.01 cm) and flat shading; UV0 on every face and the Meshy maps unchanged (copied next to the FBX); silhouette within 1 % of the length against the scaled source; report json; textured and grazing-light renders before and after from the same cameras sent to the owner by the Executor; no Unreal import; lint clean; committed by path.
- gdd_check: read 2026-10-30: [[burst-rifle]] (spec approved, default weapon, M1; look line: Meshy from the owner ChatGPT sheet of task-387; mesh size, triangle budget and miner hand socket open: Q3, Q4), [[art-prompts-burst-rifle]] (turnaround prompt, same open values; the sheet image itself is not in the repo: Q6), [[asset-pipeline]] (no weapon triangle budget: Q3; "flat faces kept truly flat, no decimation that bulges them"; pivot rule bottom centre, which does not suit a hand-held weapon: Q4; step 1 "weapons are generated separately and attached to a socket"), [[miner]] (hand socket open, not needed for an asset-only task), [[bp-burst-rifle]] (placeholder cube 60 x 10 x 10 uu, the only length on record, used as the Q4 default), [[art-style-guide]] (chunky volumes, large flat facets: matches the flattening), [[art-3d-lessons]] (agents check by numbers, the Executor renders and sends; stop Blender before reporting), precedents [[task-347-turret-flat-surfaces]], [[task-366-extractor-flat-faces]], [[task-281-robot-mesh-flatten]]. Conflicts: the bottom-centre pivot rule (Q4). Open values the work needs: budget (Q3), size and pivot (Q4). Final English name stays open (files named burst-rifle). Milestone: the rifle is M1, art asset passes run as M2 like the turret and extractor.
- confirmed_by_owner: yes (2026-10-30, "ok")
- renumbered: was task-392-burst-rifle-flat-faces, point 2 was task-393-burst-rifle-rebake (id clash with task-392-duskite-on-screen and task-393-miner-over-lit, owner-approved, 2026-10-30); commits 49ab8c99, 85dc033a and 257aa367 carry the old ids.
- milestone: M2
- next_step: task-394-02-burst-rifle-flatten
- started: 2026-10-30T20:47:56Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 identity: the Meshy "Aqua Wasteland Carbine" is the burst rifle (default weapon, working name) that you made from the task-387 sheet; files and nodes are named burst-rifle. Default: yes.
- Q2 scope: asset only, as for the extractor: no Unreal import, no hand socket on the miner, no replacement of the BP_BurstRifle cube, no -game capture; the attach comes in a later task when you ask for it. Default: yes.
- Q3 triangle budget: keep the 9,892 triangles as delivered, no decimation (one weapon on screen, the miner is 25,744); or give a number (for example 5,000, decimated without bulging the flat faces). Default: as delivered, written in asset-pipeline and burst-rifle.
- Q4 size, orientation, pivot: uniform scale to 60 cm overall length, stock to muzzle (the length of the current BP_BurstRifle placeholder cube; Meshy gives 190 x 23 x 66 cm, so about 7 x 21 cm at 60), muzzle toward +X (found on the mesh, rule logged), top +Z, pivot at the pistol grip hand hold (centre of the grip where the right hand closes, for the later socket) instead of the bottom-centre pivot of asset-pipeline. Default: yes.
- Q5 flattening: the turret and extractor recipe (`turret_flatten.py`, `meshy_flatten_panels.py`, `extractor_flatten.py`) on this mesh itself, since no denser Meshy version exists: planar regions found automatically (normal 10 deg, area at least 0.05 % of the surface, plane distance 0.075 cm = the 0.25 cm used on the 2 m extractor scaled to 60 cm), vertices projected on each region plane, flat shading on those faces; the texture files are not edited here (task-395 re-bakes them). Default: yes.
- Q6 source files: the FBX (12.8 MB) and the five maps are committed in `Art/Source/burst-rifle/meshy/` renamed short (`burst_rifle_meshy_raw.fbx`, `_basecolor.png`, `_metallic.png`, `_metallic_roughness.png`, `_normal.png`, `_roughness.png`), original names and SHA-256 recorded in a new node burst-rifle-mesh (child of art-prompts); the zip stays in Downloads. The ChatGPT four-view sheet you gave Meshy is not in Downloads nor in the repo: give its path and it is copied to `Art/Source/burst-rifle/reference/` and recorded. Default: the node lists the sheet as not provided.

## Subtasks
1. task-394-01-burst-rifle-meshy-files (vault/reference/tasks/task-394-burst-rifle-flat-faces/task-394-01-burst-rifle-meshy-files.md) — 3d: extract the owner zip safely, measure the Meshy rifle, store it per Q6 and create node burst-rifle-mesh.
2. task-394-02-burst-rifle-flatten (vault/reference/tasks/task-394-burst-rifle-flat-faces/task-394-02-burst-rifle-flatten.md) — 3d: scale, orient and pivot the Meshy rifle per Q4, project its planar regions flat per Q5, export SM_BurstRifle.fbx, render before and after.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-394-01-burst-rifle-meshy-files]] — step 1, 3d.
- [[task-394-02-burst-rifle-flatten]] — step 2, 3d.
- [[burst-rifle]] — the weapon this mesh is for.
- [[task-395-burst-rifle-rebake]] — point 2, re-bakes the maps on this task output.
- [[task-366-extractor-flat-faces]] — the intake and flattening recipe reused.
