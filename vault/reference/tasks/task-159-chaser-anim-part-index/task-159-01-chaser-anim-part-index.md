---
id: task-159-01-chaser-anim-part-index
description: "Make the chaser animation move each part separately: carry the part index in a UV channel instead of vertex colours."
parent: task-159-chaser-anim-part-index
kind: task
status: in-progress
---
# Chaser: part index in a UV channel

- goal: Make the chaser animation move each part separately: carry the part index in a UV channel instead of vertex colours.
- agent: 3d
- depends_on: none
- inputs: enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md), agent-3d (vault/reference/agents/agent-3d.md)
- expected_output: Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx re-exported by Tools/art/blender/meshy_cut_parts.py (small change, keep everything else identical: same parts, hierarchy, pivots, UV0, LightMap, PartIndex vertex colour kept, FBX_SCALE_UNITS settings) with a third UV channel named PartId on every part: both coordinates of every corner = (index + 0.5) / 10 with index Pelvis 0, HeadTorso 1, UpperArm_L 2, UpperArm_R 3, ForearmHand_L 4, ForearmHand_R 5, Thigh_L 6, Thigh_R 7, ShinFoot_L 8, ShinFoot_R 9; the report gets the check of the channel re-read from the exported FBX (distinct values per part, all corners of a part identical)
- done_when: The re-imported FBX in Blender shows ten objects with a UV layer PartId whose values per part are exactly (index + 0.5) / 10 to float32 precision, the other layers unchanged, triangles and bounds unchanged (3263, z 0 to 123); the script and the report committed by name, FBX untracked; nothing imported in Unreal
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-159-chaser-anim-part-index]] — parent global task.
