---
id: task-278-02-eye-slot-split
description: "The chaser and shield-chaser source FBX get a last material slot robot_eye holding every eye face; nothing else changes."
parent: task-278-chaser-eye-glow
kind: task
status: done
---
# The chaser and shield-chaser source FBX get a last material slot robot_eye holding every eye face; nothing else changes

- goal: The chaser and shield-chaser source FBX get a last material slot robot_eye holding every eye face; nothing else changes.
- agent: 3d
- depends_on: none
- inputs: chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), blender-asset-route (vault/reference/tech/pipelines/blender-asset-route.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: Blender route, no Meshy call. New script Tools/art/blender/eye_slot_split.py, run headless on Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx (texture Art/Meshes/chaser-meshy/chaser_meshy_fixed_basecolor.png) and Art/Meshes/shield-chaser/SM_ShieldChaser_Meshy_Anim.fbx (only faces of the chaser-body slot meshy_chaser are candidates). A face goes to a new material slot robot_eye, appended LAST (chaser [meshy_chaser, robot_eye]; shield [meshy_chaser, M_ChaserShield, M_ChaserShield_Emblem, robot_eye]), when its UV0 triangle covers a texel with red mask saturate((R - 2 max(G,B)) x 3) > 0.1 (linear) and its centroid has z 92-108 cm and x >= 2 (front head; the neck ring z 85-91.6 is excluded). Overwrite both FBX in place with the same export settings (FBX_SCALE_UNITS, LINEAR colours); no rebake, no EXR change. Report JSON next to each FBX (eye_slot_split.report.json): eye faces, tris before and after, eye texels left in body faces inside the head zone, neck texels inside eye faces, per-vertex position, UV0, UV1, UV2 (PartId) and vertex colour comparison before vs after (max deltas), material order.
- done_when: Both FBX re-exported; tris unchanged; eye texels (mask > 0.1, head zone) left in body faces = 0; neck texels in eye faces = 0; positions, UV0-2 and vertex colours identical (float epsilon); robot_eye is the last slot; no Blender process left; chaser-meshy-pipeline updated (step, counts, revert = git checkout 2ad7698a of both FBX); committed. If any check fails, restore both FBX with git and stop.
- editor_state: closed
- token_ceiling: 150k
- model: sonnet
- tokens: input 36, cache write 54430, cache read 628160, output 5923
- runs: 1
- milestone: M1
- result: Done (commit 2d0cde72). Tools/art/blender/eye_slot_split.py moved 159 HeadTorso faces (69,771 eye texels) to a last slot robot_eye in chaser_meshy_parts.fbx [meshy_chaser, robot_eye] and SM_ShieldChaser_Meshy_Anim.fbx [meshy_chaser, M_ChaserShield, M_ChaserShield_Emblem, robot_eye]; tris 3263/3263 and 4242/4242; eye texels left in body 0; neck texels in eye 0; positions delta 1e-5 cm, UV0-2 and vertex colours delta 0. Reports next to each FBX; chaser-meshy-pipeline updated, revert git checkout 2ad7698a of both FBX.

## Relations
- [[task-278-chaser-eye-glow]] — parent global task.
