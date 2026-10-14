---
id: task-151-05-chaser-split
description: "Split the chaser Blender script into one module per part with a part preview mode, geometry unchanged."
parent: task-151-chaser-robot
kind: task
status: done
---
# Chaser: split the model by part

- goal: Split the chaser Blender script into one module per part with a part preview mode, geometry unchanged.
- agent: 3d
- depends_on: task-151-04-chaser-refine
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: Tools/art/blender/chaser/ with common.py (palette, materials, bevel and decal helpers, the part contract), part_headtorso.py, part_pelvis.py, part_upperarm.py, part_forearmhand.py, part_thigh.py, part_shinfoot.py (each builds its object with its pivot at the joint; the arm and leg parts take a side argument and build a mirror for the other side), assemble.py (builds the ten objects, exports Art/Meshes/chaser/SM_Chaser_parts.fbx and writes the report), part_preview.py with --only <part> (renders that part alone: orthographic front, side and back at 11.7 px per cm on a light grey background, plus one close-up; output in Art/Meshes/chaser/parts/); Tools/art/blender/chaser_model.py becomes a thin wrapper that calls assemble.py with the same command line; each part writes its own report Art/Meshes/chaser/parts/<part>.report.json and assemble.py merges them into SM_Chaser_parts.report.json, keeping the fidelity and refinement objects
- done_when: The assembled FBX has exactly the same ten objects, names, pivots, hierarchy, material slots and triangle counts per part as the committed backup (compare with the report of tag backup-chaser-monolithic: tris per part, bounds, joints, slots all identical within 0.01 cm); the part preview renders each of the six parts; nothing imported in Unreal; committed by name
- context: the chaser is rebuilt part by part (owner 2026-10-13). This step only reorganises the committed script of commit bc01b6b (Tools/art/blender/chaser_model.py, 2056 triangles) without changing any geometry, so that six agents can then work on six parts in parallel, each in its own file. The previous result is saved as git tag backup-chaser-monolithic and in Art/Meshes/chaser/backup-monolithic/.
- rules: no geometry change; BLENDER ROUTE; no Unreal process, no Meshy, no screenshot tool; the part functions must not depend on each other except through common.py and the joint positions of the contract; keep the logo texture generation in common.py; document the layout in a vault doc node (vault/reference/tech/pipelines/chaser-model-pipeline.md: kind doc, parent pipelines-index, English, the files, the commands, the contract per part, the part preview, how an agent edits one part; listed in pipelines-index) and lint it.
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 38, cache write 86668, cache read 1197508, output 16121
- runs: 1
- milestone: M2
- result: Script split into Tools/art/blender/chaser/ (common.py, six part modules, assemble.py, part_preview.py); same 2056 triangles, bounds and joints as the backup tag; doc chaser-model-pipeline.

## Relations
- [[task-151-chaser-robot]] — parent global task.
