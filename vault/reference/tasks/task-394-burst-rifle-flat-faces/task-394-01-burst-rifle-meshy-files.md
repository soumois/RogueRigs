---
id: task-394-01-burst-rifle-meshy-files
description: "3d: extract the owner zip safely, measure the Meshy rifle, store it per Q6 and create node burst-rifle-mesh."
parent: task-394-burst-rifle-flat-faces
kind: task
status: done
---
# Burst rifle mesh: save the Meshy files, flatten the faces: 3d

- goal: The owner Meshy burst rifle files are measured, stored per the global Q6 and recorded in a new node burst-rifle-mesh.
- agent: 3d
- depends_on: none
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-burst-rifle (vault/reference/gdd/art-audio/art-prompts-burst-rifle.md), art-prompts-extractor (vault/reference/gdd/art-audio/art-prompts-extractor.md) (format model of a mesh files node), task-366-01-extractor-meshy-files (vault/reference/tasks/task-366-extractor-flat-faces/task-366-01-extractor-meshy-files.md) (precedent), decisions: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-394-burst-rifle-flat-faces.md), files: `C:/Users/soumo/Downloads/Meshy_AI_Aqua_Wasteland_Carbine_20261030_texture_remesh_tri_fbx.zip` (untrusted), `Tools/art/blender/extractor_identify.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/burst_rifle_identify.py`; `Art/Source/burst-rifle/meshy/` with the FBX and five maps renamed per Q6 and `burst_rifle_meshy_identify.report.json` (triangles, vertices, objects, bounds in cm, material slots, UV layers, map sizes and channels, embedded textures, file sizes and SHA-256 with original names); node `vault/reference/gdd/art-audio/burst-rifle-mesh.md` (kind note, parent art-prompts, owner words verbatim, zip name, file list and where each lives, measures, budget per Q3, sheet per Q6) with relation lines in art-prompts, art-prompts-burst-rifle and burst-rifle.
- done_when: the zip was extracted into a new empty directory under the agent session scratchpad (never the repo), nothing inside it executed, the FBX passed as an argument to headless Blender (`-b --factory-startup`), stage files in a short path (long Windows paths fail in Blender: the planner import of the original file name under the session scratchpad failed); the report gives every measure listed and matches the planner measure (9,892 tris, 2048 maps) or explains the difference; `git ls-files Art/Source/burst-rifle/meshy` lists the FBX, the five PNG and the report; burst-rifle-mesh exists with relations on both ends; `node vault/scripts/lint.js --write` clean; committed by path as `task-394-01-burst-rifle-meshy-files: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 28, cache write 40055, cache read 376269, output 9550
- usage: turns 14, context start 11k peak 40k, resumes 0, 4 min, units 0.14 M (agent ad12c204e50afaabd, attributed by agent-usage to task-392-01; set by hand)
- runs: 1
- milestone: M2
- started: 2026-10-30T20:47:56Z
- waiting_for: none
- renumbered: was task-392-01-burst-rifle-meshy-files (id clash with task-392-duskite-on-screen, owner-approved, 2026-10-30). Its work is already committed under the old id: 85dc033a (in-progress), 257aa367 (Meshy files, measures, node burst-rifle-mesh). Cost: its 3d worker run (agent ad12c204e50afaabd, sonnet, 14 turns, 228 s) is still attributed by agent-usage.js to `task-392-01` (from its launch description), so `record-cost.js task-394-01` finds no usage, and `record-cost.js task-392-01` must NOT be run for it (it would write into task-392-01-screen-texts-duskite and mix the Duskite cost). To close: check done_when against 257aa367, fill model/tokens/usage/runs by hand from `node vault/scripts/agent-usage.js --json` record ad12c204e50afaabd (runs 1), then done under the new id.
- result: Done under 257aa367 (old id task-392-01): FBX and five maps stored, report 9,892 tris and 2048 maps matching the planner measure, burst-rifle-mesh node with relations, lint clean. Cost written by hand, record-cost not run.

## Relations
- [[task-394-burst-rifle-flat-faces]] — parent global task.
