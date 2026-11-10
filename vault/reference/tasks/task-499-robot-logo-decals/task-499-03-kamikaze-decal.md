---
id: task-499-03-kamikaze-decal
description: "Bp: Emblem placed on the Kamikaze."
parent: task-499-robot-logo-decals
kind: task
status: done
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2: bp

- goal: Emblem placed on the Kamikaze.
- agent: bp
- depends_on: task-499-02-heavy-decal
- inputs: enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md)
- expected_output: see done_when
- done_when: Acceptance A1 to A4 for the Kamikaze, core pulse unchanged, checked on an editor view from above.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus (executor run)
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: 2026-11-10T21:16:24Z
- waiting_for: none
- result: Commit 6b9561be. Same script (robot_emblem_decal.py --robot kamikaze): part Body, 22 cm square centred (-8, -3) cm on the flat head top (z 167-168, away from the front core), offset 0.3 cm, 14 emblem tris at PartId 0.05; new SM_Kamikaze_Meshy_AnimEmblem (slots body, robot_eye, robot_emblem; bounds extension 60, build settings, UVCHECK emblem 14 tris at 0.05, off_bin 0, mixed 0) on BP_Kamikaze BodyMesh (inherited C++ component, set on the class default). Material M_KamikazeEmblem_Anim (masked shield-style copy, TextureObject T_KamikazeAnim) and MI_Kamikaze_Emblem (yellow T_ChaserShield_Emblem, RunRate 0.35, ClipOffset 0). Torso debris rewritten and reimported, slot robot_emblem = M_ShieldDebris_Emblem. CoreMesh, its material and CoreLight untouched. Seen in -game (Saved/Captures/task-499-check/k_00.png): yellow emblem on the dark head top, core still orange-red at the front. Not seen: the kamikaze torso debris piece (death explosion hides it; checked by read back only). Cost not recorded (known tooling issue).

## Relations
- [[task-499-robot-logo-decals]] — parent global task.
