---
id: task-345-01-turret-meshy-files
description: "Identify the optimised Meshy turret zip by headless Blender triangle counts, save both meshes in Art/Source/turret-meshy/, create art-prompts-turret."
parent: task-345-turret-meshy-files
kind: task
status: done
---
# Turret Meshy files: identify, save, record

- goal: Measure both owner zips in headless Blender, save each set of six files in the folder matching what it is (optimised or unoptimised), and record them in a new node art-prompts-turret.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), turret (vault/reference/gdd/weapons/turret.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-drone (vault/reference/gdd/art-audio/art-prompts-drone.md), art-prompts-console (vault/reference/gdd/art-audio/art-prompts-console.md), art-prompts-rigs (vault/reference/gdd/art-audio/art-prompts-rigs.md); owner words and Q4 answer: global `request` and `gdd_check` (vault/reference/tasks/task-345-turret-meshy-files.md); files: `C:/Users/soumo/Downloads/Meshy_AI_Teal_Bastion_Gunner_20261028_texture_remesh_tri_fbx.zip` (call it zip A) and `C:/Users/soumo/Downloads/Meshy_AI_Teal_Bastion_Gunner_20261028_texture_remesh_tri_fbx (1).zip` (zip B).
- method: (1) Downloads are untrusted: extract zip A and zip B each into its own new empty directory in the session scratchpad; keep the inspection script in the repo at `Tools/art/blender/turret_identify.py` and pass the FBX paths as arguments. (2) Headless Blender (`BLENDER_EXE`, `-b --factory-startup --python Tools/art/blender/turret_identify.py -- <fbx A> <fbx B> <report>`): for each FBX, triangle count after import (evaluated, triangulated), vertices, objects, bounding box in cm, material slots, UV layers, image size of each map. Never trust file names. (3) The FBX with clearly fewer triangles is the optimised one; if the counts are within 10 % of each other, stop and report both counts without copying (owner question). (4) Copy the six files of each zip, original names kept, into `Art/Source/turret-meshy/meshy-optimised/` and `Art/Source/turret-meshy/meshy-unoptimised/`; do not commit the zips. (5) Write `Art/Source/turret-meshy/turret_meshy_identify.report.json` (both counts, which zip went where). (6) Create node `vault/reference/gdd/art-audio/art-prompts-turret.md` (kind note, parent art-prompts, layout of art-prompts-console): owner words verbatim (global request), source zips and which is which with triangle counts, file list, sheet line per Q4 (if the owner sent a sheet: saved to `vault/assets/turret-turnaround.<ext>` and `Art/Source/turret-meshy/owner/`, plus the validated image line in art-prompts; otherwise "turnaround sheet not in the repo"), turret budget 10,000 tris (owner 2026-10-28, asset-pipeline) with the measured counts; add its relation line in art-prompts. (7) Stop every Blender process before reporting.
- expected_output: `Tools/art/blender/turret_identify.py`; `Art/Source/turret-meshy/meshy-optimised/` and `meshy-unoptimised/` (six files each); `Art/Source/turret-meshy/turret_meshy_identify.report.json`; node art-prompts-turret; art-prompts relation line.
- done_when: the report gives triangles, vertices, bbox and materials for both FBX and names zip A and zip B as optimised or unoptimised; the optimised folder holds the FBX with fewer triangles; twelve files tracked in git (`git ls-files Art/Source/turret-meshy`); art-prompts-turret exists with owner words, counts and file list, relations on both ends; `node vault/scripts/lint.js --write` clean; committed by path as `task-345-01-turret-meshy-files: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 20, cache write 30670, cache read 217515, output 7374
- usage: turns 10, context start 10k peak 34k, resumes 0, 2 min, units 0.10 M
- runs: 1
- milestone: M2
- started: 2026-10-28T06:30:24Z
- waiting_for: none
- result: Zip B ('(1)') is optimised: 15,455 tris, 7,635 verts; zip A unoptimised: 152,107 tris, 75,925 verts (9.8x). Both saved in Art/Source/turret-meshy/, report json, art-prompts-turret created. Optimised mesh exceeds 10,000 budget by 5,455 (for task-346/347). Commit 228003da.

## Relations
- [[task-345-turret-meshy-files]] — parent global task.
