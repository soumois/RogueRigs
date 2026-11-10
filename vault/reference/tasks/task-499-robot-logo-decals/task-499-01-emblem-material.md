---
id: task-499-01-emblem-material
description: "Art: The emblem decal source and material shared by the three robots, from the existing emblem cutout, as on the shield."
parent: task-499-robot-logo-decals
kind: task
status: done
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2: art

- goal: The emblem decal source and material shared by the three robots, from the existing emblem cutout, as on the shield.
- agent: art
- depends_on: none
- inputs: kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md), color-code (vault/reference/gdd/art-audio/color-code.md), task-218-shield-emblem (vault/reference/tasks/task-218-shield-emblem.md), task-226-shield-charcoal (vault/reference/tasks/task-226-shield-charcoal.md), art-prompts-shield (vault/reference/gdd/art-audio/art-prompts-shield.md), Art/Source/kessler-voss-logo/
- expected_output: see done_when
- done_when: One emblem texture or material usable on the three robots, yellow parts only (chaser v2 variant as Q2), no Meshy regeneration; preview image of it.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus (executor run)
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: 2026-11-10T20:42:05Z
- waiting_for: none
- result: Commit 41d0835a. Dark texture T_RobotEmblem_Dark (512, charcoal rock + yellow KV and facets, from the variation 4 cut-out, Tools/art/blender/robot_emblem_texture.py) for chaser v2 and shield chaser v2; heavy and kamikaze reuse the yellow-only T_ChaserShield_Emblem. Master M_RobotEmblem (copy of M_RobotMeshy, Masked, opacity = texture alpha, same anim WPO); instances MI_Heavy_Emblem, MI_Kamikaze_Emblem, MI_ChaserV2_Emblem (same AnimTex, RunRate, ClipOffset as their body instances), MI_ShieldChaserV2_RobotEmblem (ClipOffset 0) and _RobotEmblemPlain (2); debris material M_RobotDebrisEmblemDark (heavy and kamikaze debris reuse M_ShieldDebris_Emblem). Preview Saved/Renders/task-499/emblem_textures.png. Style-guide: 2 pass, 0 fail (watch point: outer shards close to the rock edge). Cost not recorded: agent-usage.js finds no transcript (known tooling issue).

## Relations
- [[task-499-robot-logo-decals]] — parent global task.
