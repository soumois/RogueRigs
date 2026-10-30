---
id: task-394-02-burst-rifle-flatten
description: "3d: scale, orient and pivot the Meshy rifle per Q4, project its planar regions flat per Q5, export SM_BurstRifle.fbx, render before and after."
parent: task-394-burst-rifle-flat-faces
kind: task
status: todo
---
# Burst rifle mesh: save the Meshy files, flatten the faces: 3d

- goal: From the stored Meshy FBX, `Art/Meshes/burst-rifle/SM_BurstRifle.fbx` within the Q3 budget, scaled, oriented and pivoted per Q4, with its flat faces truly flat per Q5.
- agent: 3d
- depends_on: task-394-01-burst-rifle-meshy-files
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), task-347-01-turret-flat-surfaces (vault/reference/tasks/task-347-turret-flat-surfaces/task-347-01-turret-flat-surfaces.md) (precedent recipe, its method and result), task-366-02-extractor-flatten (vault/reference/tasks/task-366-extractor-flat-faces/task-366-02-extractor-flatten.md) (precedent: report fields, renders, its deviations), decisions: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-394-burst-rifle-flat-faces.md), files: `vault/reference/gdd/art-audio/burst-rifle-mesh.md` (created by step 01), `Art/Source/burst-rifle/meshy/` FBX, maps and report, `Tools/art/blender/turret_flatten.py`, `Tools/art/blender/meshy_flatten_panels.py`, `Tools/art/blender/extractor_flatten.py`, `Tools/art/blender/extractor_preview.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/burst_rifle_flatten.py` (imports rr_blender, never redefines its helpers); `Art/Meshes/burst-rifle/SM_BurstRifle.fbx` (centimetres, `FBX_SCALE_UNITS`, one slot, UV0 kept) with byte copies of the Meshy maps next to it as `T_BurstRifle_BaseColor|Metallic|Normal|Roughness.png`; `Art/Meshes/burst-rifle/burst_rifle_flat.report.json` (bulge cause measured: off-plane vertices, split normals, normal-map tilt; thresholds; regions; triangles before and after; vertices moved; max residual per region; scale factor, bounds in cm, muzzle rule, grip point and how it was found; UV check; silhouette difference; warnings); renders in `Saved/BurstRifleMesh/flat/` (not committed): source and result, textured and grazing light, same cameras (left, right, top, three-quarter); "Game mesh" section added to burst-rifle-mesh.
- done_when: triangles within the Q3 budget (9,892 when kept as delivered); every flattened region residual under 0.01 cm; flat shading on those faces; overall length 60 cm within 0.1 cm along X, muzzle toward +X, top +Z, origin at the pistol grip hand hold per Q4; UV0 on every face, no texture file edited; silhouette within 1 % of the length against the scaled source (numbers in the report); FBX reimported in a clean scene gives the reported bounds and origin; renders exist and their paths are reported so the Executor sends them to the owner; burst-rifle-mesh updated; lint clean; committed by path as `task-394-02-burst-rifle-flatten: <summary>`; `Get-Process blender` empty.
- editor_state: any
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-394-burst-rifle-flat-faces]] — parent global task.
