---
id: task-499-02-heavy-decal
description: "Bp: Emblem placed on the Heavy."
parent: task-499-robot-logo-decals
kind: task
status: done
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2: bp

- goal: Emblem placed on the Heavy.
- agent: bp
- depends_on: task-499-01-emblem-material
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md)
- expected_output: see done_when
- done_when: Acceptance A1 to A4 for the Heavy, checked on an editor view from above and on the telegraph and death pieces.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus (executor run)
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: 2026-11-10T20:54:13Z
- waiting_for: none
- result: Commit 2a5284fd. Tools/art/blender/robot_emblem_decal.py (rest-space decal: faces of HeadTorso hit by downward rays over a 30 cm square centred (16, 1.5) cm, clipped, offset 0.4 cm, planar UV emblem up = robot back, PartId 0.15 kept, slot robot_emblem; object Z_Emblem so the slot comes last) -> Art/Meshes/heavy-meshy/heavy_meshy_parts_emblem.fbx (parts byte-equal on read back, 73 emblem tris) imported as new SM_Heavy_Meshy_AnimEmblem (slots body, robot_eye, robot_emblem; bounds extension 220/220/60, build settings off/full precision/LM 1, UVCHECK emblem 73 tris at 0.15, off_bin 0, mixed 0); BP_Heavy BodyMesh points to it (eye glow swap on slot 1 unchanged). Debris: SM_HeavyDebris_Torso.fbx rewritten with the decal (original kept untracked as .pre_emblem.fbx), reimported, slot robot_emblem = M_ShieldDebris_Emblem. Fix: M_RobotEmblem (BlendMode set by property) rendered unmasked solid squares in -game; replaced by M_HeavyEmblem_Anim, a copy of the masked M_ShieldChaserV2_Anim with TextureObject T_HeavyAnimRunV1; MI_Heavy_Emblem reparented (RunRate 0.7, ClipOffset 0); M_RobotEmblem deleted. Seen in -game (Saved/Captures/task-499-check/k_00.png, k_02.png telegraph ring; Saved/Captures/task-499-debris/x_01.png torso piece with the emblem): yellow KV emblem on the top, stays during walk and telegraph and on the torso piece. Readable as a yellow mark at the default camera, KV letters only at a closer camera. SM_Heavy_Meshy_Anim kept for revert. Cost not recorded: agent-usage.js finds no transcript (known tooling issue).

## Relations
- [[task-499-robot-logo-decals]] — parent global task.
