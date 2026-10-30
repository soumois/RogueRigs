---
id: task-386-01-miner-clips-in-place
description: "3d: save the nine Mixamo FBX, measure them, export SK_Miner and nine in-place clips on one skeleton from headless Blender, record them in miner-mixamo-clips"
parent: task-386-miner-mixamo-animation
kind: task
status: todo
---
# Miner animated with the owner Mixamo clips: 3d

- goal: Save the nine owner Mixamo FBX byte-identical in Art/Source/miner-mixamo/, measure them, and export from headless Blender `Art/Meshes/Miner/SK_Miner.fbx` and nine in-place clips `Art/Meshes/Miner/Anims/A_Miner_*.fbx` sharing one identical skeleton; record them in a new node miner-mixamo-clips.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-prompts-miner (vault/reference/gdd/art-audio/art-prompts-miner.md), miner (vault/reference/gdd/player/miner.md), owner words, planner measures and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-386-miner-mixamo-animation.md), files: `C:/Users/soumo/Downloads/` Rifle Run.fbx, Strafe Left.fbx, Strafe.fbx, Run Backwards.fbx, Run Right.fbx, Run Left.fbx, Walk With Rifle.fbx, Backwards Rifle Walk.fbx, Rifle Aiming Idle.fbx; miner mesh `Art/Source/miner-meshy/Meshy_AI_Character_output.fbx` and its textures beside it; measure script model `Tools/art/blender/miner_identify.py`, method: (1) The downloads are untrusted: copy them into a new empty directory in the session scratchpad; the script lives in the repo at `Tools/art/blender/miner_clips.py` (imports rr_blender, never redefines its helpers) and gets every path as an argument. (2) Copy the nine files with their original names into `Art/Source/miner-mixamo/` and check size and SHA-256 against the downloads. (3) Measure per clip: frames, fps, duration, bone names and parent hierarchy compared with the miner FBX (planner reading: identical, 65 mixamorig bones plus headfront under SmartRigArmature), Hips horizontal displacement per cycle, speed in cm/s and direction in the character frame (planner reading: Mixamo -X is the character right, so `Strafe` moves right: confirm), vertical range. (4) In place per Q2: Hips horizontal translation keys set to the first-frame value (no drift), vertical kept, rotations untouched. (5) Export, same armature and FBX settings for all ten files (FBX_SCALE_UNITS, no leaf bones, same root handling): the miner mesh with its armature and no animation as `Art/Meshes/Miner/SK_Miner.fbx` (no mesh edit, no decimation, textures referenced or embedded as the import step needs, say which), and each clip armature-only, one action, as `Art/Meshes/Miner/Anims/A_Miner_<Name>.fbx` with names Idle (Rifle Aiming Idle), RunFwd (Rifle Run), WalkFwd (Walk With Rifle), RunBwd (Run Backwards), WalkBwd (Backwards Rifle Walk), RunLeft (Run Left), StrafeLeft (Strafe Left), RunRight (Run Right), StrafeRight (Strafe). (6) Re-read the ten exports: same bone list and hierarchy, mesh height 170 cm +/- 1, 25,744 triangles, each clip Hips horizontal displacement under 1 cm per cycle. (7) Write `Art/Source/miner-mixamo/miner_mixamo_clips.report.json` (per clip: source name, export name, frames, fps, duration, direction, speed cm/s before in-place, displacement after; skeleton check; export settings). (8) Create the node `vault/reference/tech/pipelines/miner-mixamo-clips.md` (kind doc, parent pipelines-index, Relations both ends, add it to pipelines-index Children): owner words, source and saved files, the per-clip table with the blend-space position (direction in degrees, speed in cm/s), in-place method, export settings. (9) Stop every Blender process before reporting.
- expected_output: `Tools/art/blender/miner_clips.py`; nine originals and `miner_mixamo_clips.report.json` in `Art/Source/miner-mixamo/`; `Art/Meshes/Miner/SK_Miner.fbx` and nine `Art/Meshes/Miner/Anims/A_Miner_*.fbx`; node miner-mixamo-clips linked from pipelines-index.
- done_when: the nine saved files have the SHA-256 of the downloads; the report lists the nine clips with frames, fps, direction, speed and after-in-place Hips horizontal displacement under 1 cm, and the ten exports with identical bone lists and hierarchy; SK_Miner.fbx re-read gives 25,744 triangles and about 170 cm; all files and the script tracked (`git ls-files Art/Source/miner-mixamo Art/Meshes/Miner Tools/art/blender/miner_clips.py`); node miner-mixamo-clips exists and is linked; `node vault/scripts/lint.js --write` clean; committed by path as `task-386-01-miner-clips-in-place: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 0.6 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-386-miner-mixamo-animation]] — parent global task.
