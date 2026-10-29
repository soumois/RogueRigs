---
id: task-367-extractor-rotors
description: "Global task: Cut the four rotors of the flattened extractor mesh into separate meshes pivoting on their spin axes, body kept fixed, ready to spin later."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor mesh: extract the four rotors so they can spin

- request: "Voilà l'extracteur, il faudrait le classique raplatir les faces et il faudrait extraire les hélices pour pouvoir les faire tourner. Prépare les tâches. Budget de 50K accepté. Ne t'occupe que de l'asset pas d'intégration en jeu pour l'instant." (owner, 2026-10-29, verbatim, French; attached `C:/Users/soumo/Downloads/Meshy_AI_Rustwing_Quadrotor_20261024_texture_fbx.zip`, 129 MB: one 115 MB FBX, base colour, metallic, normal and roughness PNG) Point 2 of 2: "il faudrait extraire les hélices pour pouvoir les faire tourner". Point 1 (files, budget, flat faces): [[task-366-extractor-flat-faces]]. Scope (owner words): asset only, no in-game integration: no Unreal import, no Blueprint, no C++, no map placement, no -game capture. "Prépare les tâches": nodes created, nothing launched before the owner confirms. Questions Q1 to Q7 are asked once, in [[task-366-extractor-flat-faces]]; Q2, Q5 and Q6 apply here. Owner answer 2026-10-29 (relayed by the main session, verbatim): "ok" = the plan accepted. Re-sequenced 2026-10-29: the owner answered "oui rebake" to re-baking the textures, so the cut runs after [[task-368-extractor-rebake]] on its re-baked SM_Extractor.fbx (the cuts keep its new UV0); the first executor run was stopped mid-run and left untracked outputs (see task-368 `gdd_check`, handled by its Q3).
- goal: A fixed extractor body mesh and four rotor meshes, each with its origin on its spin axis, so a later integration can spin them; no in-game work.
- acceptance: `Art/Meshes/extractor/SM_Extractor_Body.fbx` and `SM_Extractor_Rotor_FL`, `_FR`, `_RL`, `_RR.fbx` per Q6; each rotor origin on its measured spin axis (within 0.1 cm); no open boundary at the cuts; 36-angle sweep of each rotor without intersection with the body, or the collisions reported; total triangles at most 50,000 (Q2); body vertices away from the cuts and all UVs unchanged versus the re-baked SM_Extractor.fbx of task-368; renders sent to the owner by the Executor; art-prompts-extractor updated; no Unreal import; lint clean; committed by path.
- gdd_check: read 2026-10-29: same nodes as [[task-366-extractor-flat-faces]] `gdd_check`; precedents [[task-349-turret-mobile-head]] (cut a moving part with its pivot, sweep check) and the drone part cut (`Tools/art/blender/drone_cut_parts.py`). No conflict; the rotor list and pivots are decided by Q6 of task-366 (the "parts list" agent-3d needs before any split).
- confirmed_by_owner: yes (2026-10-29, "ok")
- milestone: M2
- next_step: task-367-01-extractor-rotors-cut
- started: 2026-10-29T20:49:01Z
- waiting_for: none
- result: open

## Subtasks
1. task-367-01-extractor-rotors-cut (vault/reference/tasks/task-367-extractor-rotors/task-367-01-extractor-rotors-cut.md) — 3d: cut the four rotors (blades and hub) from SM_Extractor into their own FBX with origins on their spin axes, cap the cuts, sweep 360 degrees, render.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-367-01-extractor-rotors-cut]] — step 1, 3d.
- [[task-366-extractor-flat-faces]] — must be done first: its SM_Extractor.fbx is this task's input; holds the questions.
- [[task-349-turret-mobile-head]] — precedent cut with pivot and sweep.
- [[task-368-extractor-rebake]] — must be done first: re-bakes SM_Extractor.fbx (new UV0 and maps) that this task cuts.
