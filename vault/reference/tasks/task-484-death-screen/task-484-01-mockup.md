---
id: task-484-01-mockup
description: "Art: Kie.ai mockup of the death screen in the level-end and pause style, rounds until the owner says "je valide"."
parent: task-484-death-screen
kind: task
status: wait-answers
---
# Death screen with Retry and Main Menu: art

- goal: Kie.ai mockup of the death screen in the level-end and pause style, rounds until the owner says "je valide".
- agent: art
- depends_on: none
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), art-prompts-pause-menu-mockup (vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md), pause-menu (vault/reference/gdd/run/pause-menu.md), color-code (vault/reference/gdd/art-audio/color-code.md)
- expected_output: see done_when
- done_when: One 16:9 1K mockup per round showing every element of death-screen (title, subtitle, recap lines, Retry and Main Menu, dimmed level behind), prompt, image, log and credits recorded in a new art-prompts node; the owner has said "je valide" on one image (further rounds only on his change requests).
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T00:57:21Z
- waiting_for: owner je valide or change request on mockup round 1 (Art/Source/death-screen/20261106-165747-death-screen-1.png)
- result: open

## Relations
- [[task-484-death-screen]] — parent global task.
