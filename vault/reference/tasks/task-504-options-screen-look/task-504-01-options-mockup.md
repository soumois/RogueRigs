---
id: task-504-01-options-mockup
description: "Art: Make the options screen mockup through Kie.ai image-to-image and get the owner go."
parent: task-504-options-screen-look
kind: task
status: in-progress
---
# Options screen: Kie.ai mockup and its elements: art

- goal: Make the options screen mockup through Kie.ai image-to-image and get the owner go.
- agent: art
- depends_on: none
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-pause-menu-mockup (vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), Tools/art/kie_image.js
- expected_output: see done_when
- done_when: A1 of the global passes; the image is shown to the owner and he said "je valide" (wait-answers until then; his change requests give new rounds); nothing extracted before.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-11T21:49:08Z
- waiting_for: none
- result: open

## Relations
- [[task-504-options-screen-look]] — parent global task.
