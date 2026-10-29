---
id: task-366-01-extractor-meshy-files
description: "3d: extract the owner zip safely, measure the Meshy mesh, store it per Q3 and create node art-prompts-extractor."
parent: task-366-extractor-flat-faces
kind: task
status: in-progress
---
# Extractor mesh: save the Meshy files, decimate to budget, flatten the faces: 3d

- goal: The owner Meshy extractor files are measured, stored per the global Q3 and recorded in a new node art-prompts-extractor.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), extraction (vault/reference/gdd/run/extraction.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-turret (vault/reference/gdd/art-audio/art-prompts-turret.md), task-345-01-turret-meshy-files (vault/reference/tasks/task-345-turret-meshy-files/task-345-01-turret-meshy-files.md) (precedent), decisions: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-366-extractor-flat-faces.md), files: `C:/Users/soumo/Downloads/Meshy_AI_Rustwing_Quadrotor_20261024_texture_fbx.zip` (untrusted), `Tools/art/blender/turret_identify.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/extractor_identify.py`; raw FBX in `Art/Meshes/extractor/raw/` (git-ignored); the four maps in `Art/Source/extractor-meshy/` (original names); `Art/Source/extractor-meshy/extractor_meshy_identify.report.json` (triangles, vertices, objects, bounds in cm, material slots, UV layers, map sizes, FBX size and SHA-256); node `vault/reference/gdd/art-audio/art-prompts-extractor.md` (kind note, parent art-prompts, owner words verbatim, zip name, file list and where each lives, measures, budget 50,000 per Q2) and its relation line in art-prompts.
- done_when: the zip was extracted into a new empty directory under the agent session scratchpad (never the repo), nothing inside it executed, the FBX path passed as an argument to headless Blender (`-b --factory-startup`); the report gives every measure listed; `git ls-files Art/Source/extractor-meshy` lists the four PNG and the report, and no FBX over 100 MB is staged; art-prompts-extractor exists with relations on both ends; `node vault/scripts/lint.js --write` clean; committed by path as `task-366-01-extractor-meshy-files: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-29T18:18:53Z
- waiting_for: none
- result: open

## Relations
- [[task-366-extractor-flat-faces]] — parent global task.
