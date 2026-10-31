---
id: task-408-02-upper-rifle-variants
description: "3d: headless Blender: three new animations from the idle with the right elbow raised 3, 6, 9 cm (V1..V3), recorded in miner-mixamo-clips"
parent: task-408-miner-right-arm-in-animation
kind: task
status: todo
---
# Miner right arm corrected in the animation, variants compared on one frame: 3d

- goal: Art/Meshes/Miner/Anims/A_Miner_UpperRifle_V1..V3.fbx: three new animations based on A_Miner_Idle with, on every frame, the right elbow raised by E = 3 cm (V1), 6 cm (V2), 9 cm (V3) and the right hand kept where the idle puts it (an IK on the right arm with the hand pinned and the elbow pole raised, baked into the keys), every other bone unchanged (global request: owner answer and Q7).
- agent: 3d
- depends_on: none
- inputs: miner-mixamo-clips (vault/reference/tech/pipelines/miner-mixamo-clips.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), task-386-01-miner-clips-in-place (vault/reference/tasks/task-386-miner-mixamo-animation/task-386-01-miner-clips-in-place.md), Art/Meshes/Miner/Anims/A_Miner_Idle.fbx, Directions: mesh forward and up as in the Blender scene of task-386-01 (Unreal mesh forward = component +Y, up = +Z); elbow = head of RightForeArm, hand = head of RightHand
- expected_output: Three FBX, the Blender script committed beside the task-386-01 scripts, miner-mixamo-clips updated (files, method, measured offsets per proposal), report with the measures, commit hash.
- done_when: Three FBX exist with the same skeleton (66 bones, same names), frame count and frame rate as A_Miner_Idle.fbx; measured on every frame against the idle: RightHand head within 0.5 cm of the idle, RightForeArm head raised by E (3, 6, 9 cm) within 1 cm, every bone outside RightArm, RightForeArm, RightHand and the right fingers identical (under 0.01 cm, 0.01 deg); Hips still in place; export with FBX_SCALE_UNITS as in task-386; no Blender process left running; FBX, script and miner-mixamo-clips committed by path `task-408-02-upper-rifle-variants: ...`.
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
- [[task-408-miner-right-arm-in-animation]] — parent global task.
