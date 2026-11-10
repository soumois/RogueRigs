---
id: task-499-04-chaser-v2-decal
description: "Bp: Emblem placed on the Chaser v2, carried by the shield chaser v2."
parent: task-499-robot-logo-decals
kind: task
status: done
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2: bp

- goal: Emblem placed on the Chaser v2, carried by the shield chaser v2.
- agent: bp
- depends_on: task-499-03-kamikaze-decal
- inputs: enemy-chaser (vault/reference/gdd/enemies/enemy-chaser.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md)
- expected_output: see done_when
- done_when: Acceptance A1 to A4 for Chaser v2 and shield chaser v2, readable on the yellow body, checked on an editor view from above.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus (executor run)
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: 2026-11-10T21:16:40Z
- waiting_for: none
- result: Commit fe075692. robot_emblem_decal.py --robot chaser_v2 / shield_chaser_v2: HeadTorso hood top, 14 cm square centred (-4, 1.5) cm (a third of the 41 cm head-torso width, about the chaser v1 hood emblem size), offset 0.25 cm, 43 tris at PartId 0.15, dark-rock texture T_RobotEmblem_Dark so it reads on the yellow body. New SM_ChaserV2_Meshy_AnimEmblem (slots body, robot_eye, robot_emblem) on BP_ChaserV2 (overrides [None, eye] kept); new SM_ShieldChaserV2_Meshy_AnimFwdEmblem from the shield bake FBX + decal (slots body, robot_eye, M_ChaserShield, M_ChaserShield_Emblem, robot_emblem; same bounds and 7134 + 43 tris as AnimFwd) on BP_ShieldChaserV2 (overrides 0-3 kept, slot 4 from the mesh = MI_ShieldChaserV2_RobotEmblem); OnShieldBroken also sets element 4 = MI_ShieldChaserV2_RobotEmblemPlain (ClipOffset 2, follows the plain clips; read back). UVCHECK off_bin 0, mixed 0 on both; build settings applied. Material M_ChaserV2Emblem_Anim (masked shield-style copy, TextureObject T_ChaserV2AnimV1Clips) and MI_ChaserV2_Emblem (RunRate 0.7). Torso debris rewritten and reimported, slot robot_emblem = M_RobotDebrisEmblemDark. Seen in -game (Saved/Captures/task-499-check/k_00.png): dark rock emblem with yellow KV on the hood of both robots while they run. Not seen in game: the emblem after the shield breaks (BP read back only) and the chaser v2 torso debris piece (read back only). Cost not recorded (known tooling issue).

## Relations
- [[task-499-robot-logo-decals]] — parent global task.
