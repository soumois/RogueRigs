---
id: task-409-02-upper-rifle-v2-grip
description: "3d: headless Blender: A_Miner_UpperRifle_V2Grip from V2, right hand moved onto the grip, hand rotation kept"
parent: task-409-miner-right-hand-on-grip
kind: task
status: done
---
# Miner right hand on the rifle pistol grip, rifle kept where the owner placed it: 3d

- goal: Art/Meshes/Miner/Anims/A_Miner_UpperRifle_V2Grip.fbx made from A_Miner_UpperRifle_V2.fbx: on every frame the right hand moved so that WeaponSocket_R lands where V2 puts the current grip (the socket transform applied to the RifleMesh offset), RightHand component-space rotation kept, right elbow kept at its V2 height as far as the arm allows (Q2, owner 2026-10-31: IK with the hand pinned and the elbow pole placed so the RightForeArm head keeps its V2 height), fingers unchanged in hand space, every other bone unchanged.
- agent: 3d
- depends_on: none
- inputs: miner-mixamo-clips (vault/reference/tech/pipelines/miner-mixamo-clips.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), task-408-02-upper-rifle-variants (vault/reference/tasks/task-408-miner-right-arm-in-animation/task-408-02-upper-rifle-variants.md), Art/Meshes/Miner/Anims/A_Miner_UpperRifle_V2.fbx, Tools/art/blender/miner_upper_rifle.py (the V1..V3 script to extend), Data: WeaponSocket_R on bone RightHand of SK_Miner: location (0.005868, -0.065782, -0.001565), rotation pitch -17.502942 yaw -79.928061 roll 17.606433 (Unreal rotator), scale 0.01 (RightHand bone scale about 100 in component space, so world scale 1); RifleMesh offset today (9.898091, -1.129037, -13.0) cm in the socket frame, scale 1.25 about the pistol grip pivot, Conversion check: the socket is in Unreal RightHand bone space; convert it to the Blender armature and verify on A_Miner_Idle frame 0 that the socket X axis points along the mesh forward (bp-miner: X along mesh forward, 0 deg in idle) before using it
- expected_output: One FBX, the script committed (extended or new beside miner_upper_rifle.py), miner-mixamo-clips updated (file, method, measured hand move and residuals), report with measures, commit hash.
- done_when: FBX exists with the same skeleton (66 bones, same names), frame count and rate as A_Miner_UpperRifle_V2.fbx; socket X axis check passed and stated; measured on every frame: new WeaponSocket_R point within 0.5 cm of the V2 grip target, RightHand component-space rotation within 0.5 deg of V2, finger local transforms identical, RightForeArm head at its V2 height within 1 cm on every frame where the arm allows it and the frames where it cannot listed with their height gap (Q2, not a failure), bones outside RightShoulder, RightArm, RightForeArm, RightHand identical to V2 (under 0.01 cm, 0.01 deg), no bone stretch; if a frame cannot reach within 0.5 cm, stop and report the distance per frame instead of exporting; FBX_SCALE_UNITS as in task-386; no Blender process left; committed by path `task-409-02-upper-rifle-v2-grip: ...`.
- editor_state: any
- token_ceiling: 1.2 M units
- model: sonnet
- tokens: input 32, cache write 57067, cache read 638431, output 25288
- usage: turns 16, context start 12k peak 57k, resumes 0, 4 min, units 0.26 M
- runs: 1
- milestone: M2
- started: 2026-10-31T19:39:14Z
- waiting_for: none
- result: FBX A_Miner_UpperRifle_V2Grip exported, all 94 frames reach; socket X check passed; grip 0.0001 cm, hand rot 0 deg, elbow height 0 cm, others identical; commit 0383712e

## Relations
- [[task-409-miner-right-hand-on-grip]] — parent global task.
