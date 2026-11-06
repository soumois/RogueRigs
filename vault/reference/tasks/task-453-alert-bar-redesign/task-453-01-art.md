---
id: task-453-01-art
description: "Art: Cut the alert bar frame and mark sprites of the retained mockup."
parent: task-453-alert-bar-redesign
kind: task
status: done
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
- model: opus
- tokens: input 10, cache write 27347, cache read 82804, output 7317
- usage: turns 5, context start 9k peak 31k, resumes 0, 1 min, units 0.08 M
- runs: 1
- milestone: M2
- started: 2026-11-06T00:46:45Z
- waiting_for: none
- result: Owner picked candidate 1 (run 20261105-164216-hud-unified, 12 credits): frame Art/UI/HUD/T_HUD_AlertFrame.png 1024x131 with transparent track window, swarm mark Art/UI/HUD/T_HUD_AlertMark.png 124x256; keyed, check clean; critic passes A1/A2/A6/A7/A14, red ! reminder kept by owner; recorded in art-prompts-hud-unified-mockup

## Relations
- [[task-453-alert-bar-redesign]] — parent global task.
