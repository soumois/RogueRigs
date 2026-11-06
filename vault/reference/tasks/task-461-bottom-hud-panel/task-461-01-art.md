---
id: task-461-01-art
description: "Art: Frame sprites of the bottom panel and of the XP bar with its Lv cap, from the retained mockup."
parent: task-461-bottom-hud-panel
kind: task
status: done
---
# Bottom HUD built as one riveted panel and a framed XP bar: art

- goal: Frame sprites of the bottom panel and of the XP bar with its Lv cap, from the retained mockup.
- agent: art
- depends_on: none
- inputs: art-prompts-hud-unified-mockup (vault/reference/gdd/art-audio/art-prompts-hud-unified-mockup.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), Art/Source/hud-unified/20261105-152627-hud-unified-1.png, Art/Source/hud-unified/reference/owner-hud-target-2026-11-05.webp
- expected_output: see done_when
- done_when: Two keyed sprites in the repo with measured box margins, critic review, keep and temporary labels written in art-prompts-hud-unified-mockup.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 12, cache write 41749, cache read 110926, output 11372
- usage: turns 6, context start 10k peak 42k, resumes 0, 2 min, units 0.12 M
- runs: 1
- milestone: M2
- started: 2026-11-06T04:22:48Z
- waiting_for: none
- result: Two Kie.ai sheets (24 credits), critic pick c1 of both, all pass: keep 20261105-202435-hud-unified-1.png (panel + slot well) and 20261105-202647-hud-unified-1.png (XP frame + level cap) with logs, reviews, prompts 09-10; -2 temporary in Saved/Scratch/hud-unified. Cut-outs Art/UI/HUD/T_HUD_BottomPanel, T_HUD_SlotWell, T_HUD_XPFrame, T_HUD_LevelCap with box margins in art-prompts-hud-unified-mockup. Commit 5459bd08.

## Relations
- [[task-461-bottom-hud-panel]] — parent global task.
