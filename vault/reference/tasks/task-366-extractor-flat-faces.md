---
id: task-366-extractor-flat-faces
description: "Global task: Save the owner Meshy extractor (Rustwing quadrotor) files, then decimate it to budget and flatten its bulged flat faces as for the turret."
parent: tasks-done-index
kind: task
status: done
---
# Extractor mesh: save the Meshy files, decimate to budget, flatten the faces

- request: "Voilà l'extracteur, il faudrait le classique raplatir les faces et il faudrait extraire les hélices pour pouvoir les faire tourner. Prépare les tâches. Budget de 50K accepté. Ne t'occupe que de l'asset pas d'intégration en jeu pour l'instant." (owner, 2026-10-29, verbatim, French; attached `C:/Users/soumo/Downloads/Meshy_AI_Rustwing_Quadrotor_20261024_texture_fbx.zip`, 129 MB: one 115 MB FBX, base colour, metallic, normal and roughness PNG) Point 1 of 2: "le classique raplatir les faces" (with the source intake and the 50K budget). Point 2, the rotors: [[task-367-extractor-rotors]]. Scope (owner words): asset only, no in-game integration: no Unreal import, no Blueprint, no C++, no map placement, no -game capture. "Prépare les tâches": nodes created, nothing launched before the owner confirms. Owner answer 2026-10-29 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4, Q5, Q6, Q7 accepted.
- goal: A game-ready extractor FBX within budget whose surfaces that are flat on the Meshy mesh are truly flat, with the Meshy texture kept, the source files saved and recorded.
- acceptance: Meshy source saved per Q3 and recorded in a new node art-prompts-extractor; `Art/Meshes/extractor/SM_Extractor.fbx` at most 48,000 triangles, scaled, oriented and pivoted per Q5; every planar region (Q4) has its vertices on one plane (residual under 0.01 cm) and flat shading; UV0 on every face and the four maps unchanged; report json; textured and grazing-light renders before and after from the same cameras sent to the owner by the Executor; no Unreal import; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[extraction]] (shuttle, 4 x 3-cell landing footprint, M1), [[gdd-glossary]] and [[lore]] (shuttle = the Rigs' extraction craft, Rigs side patched and rusty: matches "Rustwing"), [[asset-pipeline]] (no triangle budget for the shuttle: open until Q2; height rule is a Blueprint capsule, none exists: Q5), [[art-style-guide]], [[art-3d-lessons]], precedents [[task-345-turret-meshy-files]], [[task-347-turret-flat-surfaces]], [[task-281-robot-mesh-flatten]]. No gdd node names an "extractor" (Q1). No conflict. Milestone: the shuttle is M1, art asset passes run as M2 like the turret.
- confirmed_by_owner: yes (2026-10-29, "ok")
- milestone: M2
- next_step: none
- started: 2026-10-29T18:18:53Z
- waiting_for: none
- executor_tokens: input 22, cache write 82355, cache read 227829, output 4634
- executor_usage: runs 1, turns 9, context start 18k peak 43k, resumes 0, 12 min, units 0.15 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. 47,000 tris, 190 regions residual 0.000004 cm, bounds 196.55x201x86.1 cm nose +X pivot bottom centre, silhouette max 0.78 cm. DEVIATION: long side not on X (rotors make footprint near-square), only 5.3 pct faces flat-shaded, UV stretch on large panels from 55x decimation. Commit 47e67b5b

## Questions
Decided 2026-10-29 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-29.
- Q1 identity: "l'extracteur" is the Rigs' extraction craft that the GDD calls the shuttle ([[extraction]], glossary "Shuttle"); files and nodes are named `extractor`. Default: yes; a provisional look line is added to the extraction node.
- Q2 budget: "Budget de 50K" is a triangle budget: 50,000 triangles for the whole game asset (body plus the four rotors, cut caps included). The Meshy FBX (115 MB, likely several hundred thousand triangles) is decimated to at most 48,000 in task-366, leaving room for the rotor caps of task-367. Default: yes; written in asset-pipeline Mesh values.
- Q3 source storage: the 115 MB FBX is over the GitHub 100 MB file limit, so it is not committed (the turret source was 32 MB and was): it goes to the git-ignored `Art/Meshes/extractor/raw/`, its size and SHA-256 recorded in a new node art-prompts-extractor; the four texture maps are committed in `Art/Source/extractor-meshy/`; the zip stays in Downloads. Default: yes.
- Q4 "le classique raplatir les faces": the task-347 turret recipe (`turret_flatten.py`, `meshy_flatten_panels.py`): planar regions found automatically on the dense Meshy mesh (plane distance 0.25 cm, normal 10 deg, area at least 0.05 % of the surface: the task-347 thresholds that worked), decimation that never bulges them, vertices projected on each region plane, flat shading on those faces, rotor blades included; the textures are not edited (a normal-map bulge is only measured and reported). Default: yes.
- Q5 size and orientation (provisional, owner 2026-10-29): uniform scale so the horizontal bounds fit the shuttle landing footprint of 4 x 3 cells (268 x 201 cm, [[extraction]]), long side on X, nose toward +X (found on the mesh, rule logged), pivot bottom centre under the landing gear, Meshy proportions kept. Default: yes.
- Q6 rotors: each of the four rotors is extracted as its own FBX (blades plus hub, the part that spins); arms, motors and ducts stay on the body; each rotor origin on its measured spin axis at the blade plane; cut holes capped on both sides; same texture and one shared material slot; names FL, FR, RL, RR seen from +X; offsets in the body frame recorded. If a blade intersects the body during a 360 degree sweep, the agent reports where and by how much and does not reshape (owner decides). Default: yes.
- Q7 the older `Meshy_AI_Rustwing_Dropship_1023234854_texture_fbx.zip` (2026-10-23) also in Downloads: not used, not copied. Default: yes.

## Subtasks
1. task-366-01-extractor-meshy-files (vault/reference/tasks/task-366-extractor-flat-faces/task-366-01-extractor-meshy-files.md) — 3d: extract the owner zip safely, measure the Meshy mesh, store it per Q3 and create node art-prompts-extractor.
2. task-366-02-extractor-flatten (vault/reference/tasks/task-366-extractor-flat-faces/task-366-02-extractor-flatten.md) — 3d: decimate the Meshy extractor to at most 48,000 tris without bulging planar regions, project them flat, scale and pivot per Q5, render before and after.

## Relations
- [[tasks-done-index]] — parent index.
- [[task-366-01-extractor-meshy-files]] — step 1, 3d.
- [[task-366-02-extractor-flatten]] — step 2, 3d.
- [[extraction]] — the shuttle this mesh is for.
- [[task-367-extractor-rotors]] — point 2, runs after this task on its output.
- [[task-347-turret-flat-surfaces]] — the flattening recipe reused.
