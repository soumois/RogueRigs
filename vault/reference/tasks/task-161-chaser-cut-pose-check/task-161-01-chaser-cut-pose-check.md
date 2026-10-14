---
id: task-161-01-chaser-cut-pose-check
description: "Check the part cut of the chaser and render four fixed run poses from the baked animation data."
parent: task-161-chaser-cut-pose-check
kind: task
status: done
---
# Chaser: cut check and fixed poses

- goal: Check the part cut of the chaser and render four fixed run poses from the baked animation data.
- agent: 3d
- depends_on: none
- inputs: enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md), agent-3d (vault/reference/agents/agent-3d.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: A headless Blender script Tools/art/blender/meshy_cut_check.py and, in Art/Meshes/chaser-meshy/renders/check/: (1) a cut check of chaser_meshy_parts.fbx with a report JSON: per part the connected components (count, triangles of each), bounding box, pivot position, the distance of every vertex to its parent joint, triangles whose centroid lies on the wrong side of the joint plane of their part, stray fragments (the two foot-sole islands assigned by centroid), open edges, and a list of anomalies with numbers; per part isolated renders (ten parts in one contact sheet, front and side, each part in its own colour on a labelled tile, textured version too); (2) four fixed run poses, frames 0, 4, 8 and 12 of the baked run clip, each rendered front and side, textured, lit, in one image per view with the frame number written on each tile: the pose is computed from the values read back from the saved EXR T_ChaserAnim.exr (not from the keyed Blender poses) with a line by line port of the Custom node code of the Unreal material (reproduced in the task prompt), applied to the vertices of chaser_meshy_parts.fbx mirrored in Y to emulate the Unreal import, then mirrored back for rendering, so the images show exactly what the material formula gives; plus the same four poses computed from the keyed poses (independent path) side by side with the max vertex difference
- done_when: The script runs headless; the report states the anomalies with numbers and no verdict; the pose images exist (front and side, 4 poses each) from the EXR path and from the keyed path; the Orchestrator and the owner judge the images; nothing imported in Unreal; the script and the report committed by name, images untracked
- editor_state: closed
- token_ceiling: open
- model: sonnet
- tokens: input 20, cache write 101615, cache read 712707, output 15313
- runs: 1
- milestone: M2
- result: cut check: 10 parts single component except ShinFoot_L (18 tri sole fragment); EXR path equals keyed path within 0.034 cm; 4 fixed run poses rendered

## Relations
- [[task-161-chaser-cut-pose-check]] — parent global task.
