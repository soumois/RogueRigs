---
id: task-382-01-miner-meshy-files
description: "3d: measure the rigged FBX in headless Blender, save it with its two textures in Art/Source/miner-meshy/, record it in art-prompts-miner"
parent: task-382-miner-rigged-meshy-files
kind: task
status: in-progress
---
# Rigged miner Meshy FBX: identify, save, record

- goal: Measure the owner rigged FBX in headless Blender, save it and its two embedded textures in Art/Source/miner-meshy/, and add a "Rigged mesh (owner 2026-10-30)" section to art-prompts-miner.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), miner (vault/reference/gdd/player/miner.md), art-prompts-miner (vault/reference/gdd/art-audio/art-prompts-miner.md), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md) (layout model of a saved owner Meshy file node), owner words, planner identification and Q answers: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-382-miner-rigged-meshy-files.md), file: `C:/Users/soumo/Downloads/Meshy_AI_Character_output.fbx`, method: (1) The download is untrusted: copy it into a new empty directory in the session scratchpad and work from there; the script lives in the repo at `Tools/art/blender/miner_identify.py` (imports rr_blender, never redefines its helpers) and gets the FBX and report paths as arguments. (2) Headless Blender (`BLENDER_EXE -b --factory-startup --python Tools/art/blender/miner_identify.py -- <fbx> <report>`): triangles (evaluated, triangulated), vertices, objects and their types, bounding box in cm and height, armature name, bone count, bone names and root bone, vertex groups count and vertices without weight, material slots, UV layers, packed or referenced images with sizes, animation actions and frame ranges. (3) Copy the FBX with its original name into `Art/Source/miner-meshy/` (check the byte size and hash match the download) and extract `texture_0.png` and `normal.png` beside it (Blender unpack, or the FBX embedded PNG bytes). (4) Write `Art/Source/miner-meshy/miner_meshy_identify.report.json`. (5) Add the section to art-prompts-miner: owner words verbatim, source file, saved files, measured counts, sheet line per Q3, open items (triangle budget per Q4, animation route, Unreal import, weapon hand socket). No mesh edit, no decimation, no Unreal import, no render sent. (6) Stop every Blender process before reporting.
- expected_output: `Tools/art/blender/miner_identify.py`; `Art/Source/miner-meshy/Meshy_AI_Character_output.fbx`, `texture_0.png`, `normal.png`, `miner_meshy_identify.report.json`; art-prompts-miner section "Rigged mesh (owner 2026-10-30)".
- done_when: the report gives triangles, vertices, bbox, bone count and names, root bone, weights, materials, UVs, image sizes and actions; the saved FBX has the same SHA-256 as the download; four files plus the script tracked in git (`git ls-files Art/Source/miner-meshy Tools/art/blender/miner_identify.py`); art-prompts-miner has the section with owner words, file list and counts; `node vault/scripts/lint.js --write` clean; committed by path as `task-382-01-miner-meshy-files: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-30T04:55:17Z
- waiting_for: none
- result: open

## Relations
- [[task-382-miner-rigged-meshy-files]] — parent global task.
