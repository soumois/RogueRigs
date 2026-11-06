---
id: task-453-01-art
description: "Art: Cut the alert bar frame and mark sprites of the retained mockup."
parent: task-453-alert-bar-redesign
kind: task
status: wait-answers
---
# Alert bar redesigned and better integrated: art

- goal: Cut the alert bar frame and mark sprites of the retained mockup.
- agent: art
- depends_on: none
- inputs: Art/Source/hud-unified/, kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md)
- expected_output: see done_when
- done_when: Sprites retained by the owner, keyed, committed and listed in the mockup art-prompts node.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T00:41:05Z
- waiting_for: owner pick of the alert bar sprites: candidate 1 or 2 (frame Saved/Scratch/hud-alert/T_HUD_AlertFrame-c1.png / -c2.png, swarm mark T_HUD_AlertMark-c1.png / -c2.png; sheets Art/Source/hud-unified/20261105-164216-hud-unified-1.png / -2.png); critic prefers 1
- result: open

## Relations
- [[task-453-alert-bar-redesign]] — parent global task.
