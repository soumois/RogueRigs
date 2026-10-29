---
id: task-368-extractor-rebake
description: "Global task: Re-unwrap the 47,000-triangle flattened extractor and bake the Meshy high-poly look (colour, normal, roughness, metallic) onto it, fixing the stretched rust marks."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Extractor mesh: re-bake the Meshy textures on clean UVs

- request: "oui rebake" (owner, 2026-10-29, verbatim, French, relayed by the main session), answering whether to re-bake the extractor textures after [[task-366-extractor-flat-faces]], whose result says: the 55x decimation stretches the UVs on the large panels, so the rust marks look distorted against the Meshy source. Meaning (main session brief): bake the original Meshy high-poly appearance (base colour at least, plus normal, roughness and metallic since Meshy gave them) onto the decimated flattened mesh with clean (re-unwrapped or improved) UVs, in headless Blender, keeping the 47,000-triangle geometry and the 50,000 total budget (already accepted). Scope (owner words of task-366, still valid): asset only, no in-game integration: no Unreal import, no Blueprint, no C++, no map placement, no -game capture. Sequencing: the rotor cut [[task-367-extractor-rotors]] runs after this task on the re-baked SM_Extractor.fbx (its cuts keep UV0); its executor was stopped mid-run for that. Owner answer 2026-10-29 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: `Art/Meshes/extractor/SM_Extractor.fbx` with the same 47,000-triangle geometry and a clean UV0, and four maps baked from the dense Meshy mesh, so the rust and panel marks look like the source without stretch.
- acceptance: Geometry unchanged versus the task-366 SM_Extractor.fbx (47,000 triangles, same vertex positions, normals and flat shading); new UV0 per Q1 (no overlap, all islands in 0-1, stretch lower than the old UV0, numbers in the report); four maps per Q2 and Q3 baked from the dense Meshy mesh, no unbaked hole inside an island; flattened regions still read flat (normal-map tilt on them reported, not above the Meshy map tilt 3.7 deg mean); renders from the same cameras: Meshy source, old SM_Extractor and re-baked SM_Extractor, with close-ups of two large rusty panels, sent to the owner by the Executor; art-prompts-extractor updated; no Unreal import; lint clean; committed by path.
- gdd_check: read 2026-10-29: [[asset-pipeline]] Mesh values (extractor 50,000 triangles, owner 2026-10-29; texture rule A1 level 3: per-asset painted base colour, normal and roughness maps and Meshy textures allowed: no conflict); [[art-prompts-extractor]] (maps 2048 x 2048, game mesh, known limit "the maps are not re-baked", which this task lifts); [[art-3d-lessons]] (agents check by numbers, the Executor renders and sends; stop Blender before reporting). No gdd node sets a texture resolution, a UV method or a normal-map convention for the extractor: asked as Q1 and Q2. No conflict, same milestone as task-366 (M2 art pass). Partial task-367 work found at planning (untracked, not committed, no renders): `Tools/art/blender/extractor_cut_rotors.py`, the five `SM_Extractor_Body|Rotor_FL|FR|RL|RR.fbx` with `.fbm` folders and `extractor_rotors.report.json` in `Art/Meshes/extractor/` (48,154 triangles total, 0 boundary edges, sweep intersections FL 51, FR 49, RL 5, RR 439); handled by Q3.
- confirmed_by_owner: yes (2026-10-29, "ok")
- milestone: M2
- next_step: task-368-01-extractor-rebake-maps
- started: 2026-10-29T19:07:34Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-29 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-29.
- Q1 UVs and resolution: a new UV0 replaces the decimated one: seams on the flattened region borders and sharp edges (60 deg), angle-based unwrap, islands packed at uniform texel density with an 8 px margin, four maps at 2048 x 2048 like the Meshy maps (one material slot kept). Default: yes.
- Q2 what is baked: the high-poly source is the raw Meshy mesh placed in the SM_Extractor frame (rotation 90 deg about Z, scale 1.0561) with its planar-region vertices projected flat as in task-366 phase 1 (`extractor_flatten.py`), so flat faces bake flat; base colour, roughness and metallic come from the Meshy maps through the source UVs; the normal map holds the dense-mesh shape plus the Meshy normal-map detail, tangent space, OpenGL (+Y) like the Meshy map (the DirectX flip is left to the later Unreal import); 16 px dilation. Default: yes.
- Q3 files: `SM_Extractor.fbx` and `T_Extractor_BaseColor|Metallic|Normal|Roughness.png` in `Art/Meshes/extractor/` are replaced in place (old version in git history, the Meshy originals stay in `Art/Source/extractor-meshy/`); the partial task-367 outputs (five FBX, `.fbm` folders, rotor report) are deleted by the task-367 rerun, which keeps `extractor_cut_rotors.py` as its start and reruns it on the re-baked mesh. Default: yes.

## Subtasks
1. task-368-01-extractor-rebake-maps (vault/reference/tasks/task-368-extractor-rebake/task-368-01-extractor-rebake-maps.md) — 3d: re-unwrap SM_Extractor (geometry kept), bake the four Meshy maps from the dense mesh onto it, report stretch and fidelity, render source, old and new.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-368-01-extractor-rebake-maps]] — step 1, 3d.
- [[task-366-extractor-flat-faces]] — made the mesh and holds Q2, Q4, Q5 of the extractor.
- [[task-367-extractor-rotors]] — runs after this task on its output.
- [[art-prompts-extractor]] — extractor files and measures, updated here.
