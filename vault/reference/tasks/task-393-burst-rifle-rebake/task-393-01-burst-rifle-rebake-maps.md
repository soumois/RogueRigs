---
id: task-393-01-burst-rifle-rebake-maps
description: "3d: re-unwrap SM_BurstRifle (geometry kept), bake the four Meshy maps onto it, report stretch and fidelity, render source, old and new."
parent: task-393-burst-rifle-rebake
kind: task
status: todo
---
# Burst rifle mesh: re-bake the Meshy textures on clean UVs: 3d

- goal: From `Art/Meshes/burst-rifle/SM_BurstRifle.fbx` and the stored Meshy FBX, the same mesh with a new UV0 and four baked maps, per the global Q7 and Q8.
- agent: 3d
- depends_on: task-392-02-burst-rifle-flatten
- inputs: agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), mesh-fidelity-loop (vault/reference/tech/pipelines/mesh-fidelity-loop.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-368-01-extractor-rebake-maps (vault/reference/tasks/task-368-extractor-rebake/task-368-01-extractor-rebake-maps.md) (precedent recipe, its result and deviations), task-392-02-burst-rifle-flatten (vault/reference/tasks/task-392-burst-rifle-flat-faces/task-392-02-burst-rifle-flatten.md) (the mesh this subtask starts from), decisions: global `request`, `gdd_check` and `## Questions` (vault/reference/tasks/task-393-burst-rifle-rebake.md); Q1 to Q6 of task-392 (vault/reference/tasks/task-392-burst-rifle-flat-faces.md), files: `vault/reference/gdd/art-audio/burst-rifle-mesh.md`, `Art/Meshes/burst-rifle/SM_BurstRifle.fbx`, `Art/Meshes/burst-rifle/burst_rifle_flat.report.json` (regions, scale, rotation, grip point), `Art/Source/burst-rifle/meshy/` FBX and maps, `Tools/art/blender/burst_rifle_flatten.py`, `Tools/art/blender/extractor_rebake.py`, `Tools/art/blender/extractor_rebake_closeup.py`, `Tools/art/blender/extractor_preview.py`, `Tools/art/blender/rr_blender.py` (header docstring only)
- expected_output: `Tools/art/blender/burst_rifle_rebake.py` (imports rr_blender, never redefines its helpers); `Art/Meshes/burst-rifle/SM_BurstRifle.fbx` replaced (centimetres, `FBX_SCALE_UNITS`, one slot, one UV layer UVMap); `T_BurstRifle_BaseColor|Metallic|Normal|Roughness.png` replaced (2048 x 2048, base colour sRGB, others non-colour); `Art/Meshes/burst-rifle/burst_rifle_rebake.report.json` (UV method, seams, island count, margin, overlap count, stretch old versus new, bake settings: extrusion, max ray distance, missed rays, dilation; normal convention; normal tilt on the flattened regions, Meshy map versus baked; colour difference source versus old and versus new from the same cameras; geometry check; warnings); renders in `Saved/BurstRifleMesh/rebake/` (not committed): Meshy source, task-392 SM_BurstRifle, re-baked SM_BurstRifle, same cameras and light, plus close-ups of the receiver side plate and the stock; "Re-bake" section added to burst-rifle-mesh.
- done_when: reimport in a clean scene: same triangle count as the task-392 SM_BurstRifle.fbx, vertex positions identical (max difference 0.001 cm), same flat-shaded faces, same origin; one UV layer, every face mapped, 0 overlapping islands, all UVs in 0-1, margin at least 8 px at 2048; stretch metric lower than the old UV0 (numbers in the report); four 2048 x 2048 PNG, no unbaked pixel inside an island (missed rays reported, 0 or located); normal-map tilt on the flattened regions not above the tilt of the Meshy normal map on the same regions; colour difference to the source lower than the task-392 SM_BurstRifle from the same cameras; renders exist and their paths are reported so the Executor sends them to the owner; burst-rifle-mesh updated; lint clean; committed by path as `task-393-01-burst-rifle-rebake-maps: <summary>`; `Get-Process blender` empty.
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
- [[task-393-burst-rifle-rebake]] — parent global task.
