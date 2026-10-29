---
id: task-366-02-extractor-flatten
description: "3d: decimate the Meshy extractor to at most 48,000 tris without bulging planar regions, project them flat, scale and pivot per Q5, render before and after."
parent: task-366-extractor-flat-faces
kind: task
status: done
---
# Extractor mesh: save the Meshy files, decimate to budget, flatten the faces: 3d

- goal: From the raw Meshy FBX, `Art/Meshes/extractor/SM_Extractor.fbx` within budget with its flat faces truly flat, per the global Q2, Q4 and Q5.
- agent: 3d
- depends_on: task-366-01-extractor-meshy-files
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), extraction (vault/reference/gdd/run/extraction.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), task-347-01-turret-flat-surfaces (vault/reference/tasks/task-347-turret-flat-surfaces/task-347-01-turret-flat-surfaces.md) (precedent recipe, its method and result), task-281-robot-mesh-flatten (vault/reference/tasks/task-281-robot-mesh-flatten.md) (precedent), decisions: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-366-extractor-flat-faces.md), files: `vault/reference/gdd/art-audio/art-prompts-extractor.md` (created by step 01), `Art/Meshes/extractor/raw/` FBX, `Art/Source/extractor-meshy/` maps and report, `Tools/art/blender/turret_flatten.py`, `Tools/art/blender/meshy_flatten_panels.py`, `Tools/art/blender/turret_preview.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/extractor_flatten.py` (imports rr_blender, never redefines its helpers); `Art/Meshes/extractor/SM_Extractor.fbx` (centimetres, `FBX_SCALE_UNITS`) with its maps next to it; `Art/Meshes/extractor/extractor_flat.report.json` (bulge cause measured: off-plane vertices, split normals, normal-map tilt; thresholds; regions; triangles before and after; vertices moved; max residual per region; scale factor, bounds in cm, nose rule, pivot; UV check; warnings); renders in `Saved/ExtractorMesh/flat/` (not committed): source and result, textured and grazing light, same cameras; "Game mesh" section added to art-prompts-extractor.
- done_when: triangles at most 48,000; every flattened region residual under 0.01 cm; flat shading on those faces; horizontal bounds within 268 x 201 cm with the long side on X and the nose toward +X, origin bottom centre (min z = 0 within 0.1 cm); UV0 on every face, no texture file edited; silhouette within 1 % of the height against the scaled source (numbers in the report); FBX reimported in a clean scene gives the reported bounds; renders exist and their paths are reported so the Executor sends them to the owner; art-prompts-extractor updated; lint clean; committed by path as `task-366-02-extractor-flatten: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 1.2 M units
- model: sonnet
- tokens: input 68, cache write 102402, cache read 2497093, output 43219
- usage: turns 34, context start 12k peak 107k, resumes 0, 10 min, units 0.59 M
- runs: 1
- milestone: M2
- started: 2026-10-29T18:20:17Z
- waiting_for: none
- result: 47,000 tris, 190 regions residual 0.000004 cm, bounds 196.55x201x86.1 cm nose +X pivot bottom centre, silhouette max 0.78 cm. DEVIATION: long side not on X (rotors make footprint near-square), only 5.3 pct faces flat-shaded, UV stretch on large panels from 55x decimation. Commit 47e67b5b

## Relations
- [[task-366-extractor-flat-faces]] — parent global task.
