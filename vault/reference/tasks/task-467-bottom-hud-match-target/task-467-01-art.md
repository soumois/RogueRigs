---
id: task-467-01-art
description: "Art: Frame sprites for the thin health strip, the flat slot cell, the slim block frame, the straight XP rail and the Lv plate, from the target image."
parent: task-467-bottom-hud-match-target
kind: task
status: done
---
# Bottom HUD reworked in a capture loop until it matches the target image: art

- goal: Frame sprites for the thin health strip, the flat slot cell, the slim block frame, the straight XP rail and the Lv plate, from the target image.
- agent: art
- depends_on: none
- inputs: art-prompts-hud-unified-mockup (vault/reference/gdd/art-audio/art-prompts-hud-unified-mockup.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp, Art/Source/hud-unified/20261105-152627-hud-unified-1.png, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: Per Q2: the five frame pieces exist as keyed RGBA cut-outs with measured Draw As Box margins, closer to the target than the 461 sprites (thin frames, small rivets, no big bolt heads, no arrow ends), style-guide pass against the target; prompts, runs, credits, keep and temporary labels recorded in art-prompts-hud-unified-mockup; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 10, cache write 31865, cache read 90081, output 6203
- usage: turns 5, context start 10k peak 36k, resumes 0, 1 min, units 0.08 M
- runs: 1
- milestone: M2
- started: 2026-11-06T09:54:24Z
- waiting_for: none
- result: Two Kie.ai sheets x 2 variants (24 credits): runs 20261106-015539 (prompt 11, block frames) and 20261106-015950 (prompt 12, XP rail). Critic: A both pass, picks v1; B v1 fails A1 (glossy tubes, chrome dome bolts), v2 passes and is picked. Cut-outs Art/UI/HUD/T_HUD_HealthStrip, T_HUD_BlockFrame, T_HUD_SlotCell, T_HUD_XPRail, T_HUD_LevelPlate, --check clean, margins in art-prompts-hud-unified-mockup; rejected variants moved to Saved/Scratch/hud-unified. Commit a01e37e1.

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
