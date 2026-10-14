---
id: task-166-01-robot-debris-pieces
description: "Export the 13 chaser debris piece meshes (torso, arms and legs whole or cut at the joint) with their rest offsets."
parent: task-166-robot-debris-pieces
kind: task
status: in-progress
---
# Robot death: Niagara debris pieces (chaser)

- goal: Export the 13 chaser debris piece meshes (torso, arms and legs whole or cut at the joint) with their rest offsets.
- agent: 3d
- depends_on: none
- inputs: enemy-death (vault/reference/gdd/enemies/enemy-death.md), enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), chaser-model-pipeline (vault/reference/tech/pipelines/chaser-model-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), orchestrator-lessons-2026-10-14 (vault/reference/agents/orchestrator-lessons-2026-10-14.md)
- expected_output: 13 piece FBX files in Art/Source/chaser-meshy/debris/ exported from Art/Source chaser_meshy_parts.fbx at rest pose: SM_ChaserDebris_Torso (head-torso block + pelvis merged), _UpperArm_L/_R, _Forearm_L/_R, _Arm_L/_R (upper arm + forearm merged), _Thigh_L/_R, _LowerLeg_L/_R, _Leg_L/_R (thigh + lower leg merged); each pivot at the piece bounds centre, centimetres, front +X, FBX_SCALE_UNITS, same UV0 and base colour texture as the chaser, cut caps kept; a Blender script in Tools/art/blender/ that makes them; a report debris_pieces.json giving per piece its name, triangle count and rest-pose centre offset in cm relative to the robot origin (floor, front +X) in the Unreal axes of the import (Y mirrored as in the pipeline); chaser-meshy-pipeline doc updated with the step
- done_when: The 13 FBX files and the report exist; triangle counts sum per whole limb equal the sum of its two parts; the offsets of left and right pieces are mirrored in Y; the script reruns cleanly; chaser-meshy-pipeline updated; committed by path
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M3
- result: open

## Relations
- [[task-166-robot-debris-pieces]] — parent global task.
