---
id: task-382-miner-rigged-meshy-files
description: "Global task: Save the owner rigged Meshy FBX of the miner in the repo, measure mesh, skeleton and textures in headless Blender, record in art-prompts-miner."
parent: tasks-to-do-index
kind: task
status: todo
---
# Rigged miner Meshy FBX: save, measure and record

- request: Owner, 2026-10-30, verbatim (French, relayed by the main session, no /task): "Le personnage riggé", with one attached file `C:/Users/soumo/Downloads/Meshy_AI_Character_output.fbx` (12,468,364 bytes, dated 2026-10-29 21:51). Required by the project rule "every owner image or downloaded mesh is saved in the repo and in a vault node" (pattern of task-345). Planner identification (2026-10-30, by reading the file bytes, nothing imported): binary FBX 7.4 from Meshy; one skinned mesh (one Geometry, one Deformer, one bind Pose, one material) on a humanoid skeleton named `SmartRigArmature` with Mixamo bone names (`mixamorig:Hips` to `HeadTop_End`, shoulders, arms, five fingers per hand, legs to `Toe_End`; about 65 bones) plus one `headfront` node; no animation curves (rig only, no clip); two embedded 2048 x 2048 PNG textures, `bound_output.fbm/texture_0.png` (base colour) and `normal.png`. The base colour is white, teal and rust plates with dark joints and amber touches, the miner palette of art-prompts-miner (A-pose turnaround of task-364, whose Q4 said the animation and weapon-holding task would be planned once the mesh exists). It is not the chaser: the chaser is a robot cut into rigid parts with baked material animation and no skeletal mesh (handover-2026-10-14-chaser-meshy, asset-pipeline). Reading: the owner delivers the rigged miner made from the task-364 sheet. Scope of this task by default (Q2): save, measure, record; no Unreal import. Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted.
- goal: The owner rigged Meshy FBX of the miner is saved in the repo with its two textures extracted, measured (triangles, vertices, size, skeleton, skin weights, textures, animations) by a headless Blender script, and recorded in art-prompts-miner.
- acceptance: `Art/Source/miner-meshy/` holds `Meshy_AI_Character_output.fbx` (original name, byte-identical to the download), `texture_0.png` and `normal.png` extracted from it, and `miner_meshy_identify.report.json` (triangles, vertices, bounding box in cm, bone count and names, root bone, vertex groups and weighted vertex count, material slots, UV layers, image sizes, animation actions), all tracked in git; art-prompts-miner has a section "Rigged mesh (owner 2026-10-30)" with the owner words, the files, the measured counts, the sheet line per Q3 and the open items (budget, animation, import); miner.md asset line updated; no Unreal import, no mesh edit; lint clean; committed.
- gdd_check: Read 2026-10-30: miner (spec approved; look white + teal + rust, amber visor; asset line: owner makes the 3D himself, rig and animation route open), art-prompts-miner (task-364 prompt, A-pose, Q4 animation task later), asset-pipeline (miner auto-rig deferred; rigid binding allowed for the plate armor; no miner triangle budget), art-3d-lessons, bp-miner (BP_Miner shows SKM_Quinn_Simple with ABP_Unarmed; weapon attached to the capsule, WeaponAttachSocketName None), handover-2026-10-14-chaser-meshy (the chaser has no skeleton: not this file). No conflict: the Meshy auto-rig answers the open "rig route" of miner.md (edit in the defaults). Open values reported, not decided by this task: miner triangle budget (none in asset-pipeline); animation route (the FBX has no clips: Quinn retarget, Mixamo or Meshy clips, or hand-made); Unreal import and replacing Quinn on BP_Miner; which weapon the miner holds and its hand socket. In milestone scope (M2 art). Untrusted download: inspected only by a repo script with the path as argument.
- confirmed_by_owner: yes (2026-10-30, "ok")
- milestone: M2
- next_step: task-382-01-miner-meshy-files
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-30.
- Q1 Identity: the file is the rigged miner (player character) made from the task-364 turnaround sheet, not the chaser nor another character. Default: yes, the miner (palette white, teal, rust, amber; humanoid Mixamo skeleton).
- Q2 Scope: this task only saves, measures and records the FBX (no Unreal import); importing it, animating it and replacing the Quinn mannequin on BP_Miner are planned later as their own task when you ask. Or plan the import into BP_Miner now. Default: save, measure, record only.
- Q3 Turnaround sheet: the ChatGPT image you used is not in the repo; send it to save it in vault/assets/miner-turnaround-apose and Art/Source/miner-meshy/owner/, or not now. Default: not now, the node records "turnaround sheet not in the repo".
- Q4 Triangle budget of the miner: none in the GDD; the task only measures and reports the count. Default: stays open, decided with the import task.

## Subtasks
1. task-382-01-miner-meshy-files (vault/reference/tasks/task-382-miner-rigged-meshy-files/task-382-01-miner-meshy-files.md) — 3d: measure the rigged FBX in headless Blender, save it with its two textures in Art/Source/miner-meshy/, record it in art-prompts-miner

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-382-01-miner-meshy-files]] — step 1, 3d.
- [[miner]] — the character this mesh is for.
- [[art-prompts-miner]] — the prompt node that records the rigged mesh.
- [[task-364-miner-turnaround-prompt]] — the turnaround prompt this mesh comes from.
