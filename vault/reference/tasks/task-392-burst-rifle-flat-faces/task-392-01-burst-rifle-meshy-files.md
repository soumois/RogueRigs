---
id: task-392-01-burst-rifle-meshy-files
description: "3d: extract the owner zip safely, measure the Meshy rifle, store it per Q6 and create node burst-rifle-mesh."
parent: task-392-burst-rifle-flat-faces
kind: task
status: todo
---
# Burst rifle mesh: save the Meshy files, flatten the faces: 3d

- goal: The owner Meshy burst rifle files are measured, stored per the global Q6 and recorded in a new node burst-rifle-mesh.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-burst-rifle (vault/reference/gdd/art-audio/art-prompts-burst-rifle.md), art-prompts-extractor (vault/reference/gdd/art-audio/art-prompts-extractor.md) (format model of a mesh files node), task-366-01-extractor-meshy-files (vault/reference/tasks/task-366-extractor-flat-faces/task-366-01-extractor-meshy-files.md) (precedent), decisions: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-392-burst-rifle-flat-faces.md), files: `C:/Users/soumo/Downloads/Meshy_AI_Aqua_Wasteland_Carbine_20261030_texture_remesh_tri_fbx.zip` (untrusted), `Tools/art/blender/extractor_identify.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/burst_rifle_identify.py`; `Art/Source/burst-rifle/meshy/` with the FBX and five maps renamed per Q6 and `burst_rifle_meshy_identify.report.json` (triangles, vertices, objects, bounds in cm, material slots, UV layers, map sizes and channels, embedded textures, file sizes and SHA-256 with original names); node `vault/reference/gdd/art-audio/burst-rifle-mesh.md` (kind note, parent art-prompts, owner words verbatim, zip name, file list and where each lives, measures, budget per Q3, sheet per Q6) with relation lines in art-prompts, art-prompts-burst-rifle and burst-rifle.
- done_when: the zip was extracted into a new empty directory under the agent session scratchpad (never the repo), nothing inside it executed, the FBX passed as an argument to headless Blender (`-b --factory-startup`), stage files in a short path (long Windows paths fail in Blender: the planner import of the original file name under the session scratchpad failed); the report gives every measure listed and matches the planner measure (9,892 tris, 2048 maps) or explains the difference; `git ls-files Art/Source/burst-rifle/meshy` lists the FBX, the five PNG and the report; burst-rifle-mesh exists with relations on both ends; `node vault/scripts/lint.js --write` clean; committed by path as `task-392-01-burst-rifle-meshy-files: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-392-burst-rifle-flat-faces]] — parent global task.
