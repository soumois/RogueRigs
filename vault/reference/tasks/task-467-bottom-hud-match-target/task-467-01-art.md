---
id: task-467-01-art
description: "Art: Frame sprites for the thin health strip, the flat slot cell, the slim block frame, the straight XP rail and the Lv plate, from the target image."
parent: task-467-bottom-hud-match-target
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-467-bottom-hud-match-target]] — parent global task.
