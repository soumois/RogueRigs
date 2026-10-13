---
id: task-135-02-xp-pickups-pivot-export
description: "Re-export SM_XPNut.fbx and SM_XPChipBig.fbx with the pivot at the geometric centre, geometry otherwise unchanged."
parent: task-135-xp-pickups-pivot-float
kind: task
status: done
---
# XP pickups: export meshes with a centred pivot

- goal: Re-export SM_XPNut.fbx and SM_XPChipBig.fbx with the pivot at the geometric centre, geometry otherwise unchanged.
- agent: 3d
- depends_on: task-135-01-xp-pickups-pivot-float
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: Both Blender scripts (xp_nut_mesh.py, xp_chip_big_mesh.py) take the centred pivot (a constant or option PIVOT=center, default center for these two assets); FBX re-exported at the same paths Art/Meshes/xp-nut-mesh/SM_XPNut.fbx and Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.fbx in centimetres; reports updated (pivot center, dimensions).
- done_when: Report bounds: nut z -5.85 to 5.85 and chip z -5.3 to 5.3, x and y unchanged (about plus or minus 15 on x for both, plus or minus 12.99 on y for the nut); tris, materials, slot order and UV layers identical to the committed reports (nut 108 tris two slots, chip 844 tris seven slots); no change to the fidelity objects; committed by name.
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 12, cache write 28707, cache read 125609, output 2635
- runs: 1
- milestone: M2
- result: SM_XPNut and SM_XPChipBig re-exported with the pivot at the bounding-box centre (nut z -5.85 to 5.85, chip z -5.3 to 5.3); tris, materials, UVs unchanged; scripts take --pivot center|bottom.

## Relations
- [[task-135-xp-pickups-pivot-float]] — parent global task.
