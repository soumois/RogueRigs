---
id: task-504-01-options-mockup
description: "Art: Make the options screen mockup through Kie.ai image-to-image and get the owner go."
parent: task-504-options-screen-look
kind: task
status: done
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
- model: opus
- tokens: input 4, cache write 23434, cache read 7254, output 3410
- usage: turns 2, context start 7k peak 23k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-11-11T21:53:18Z
- waiting_for: none
- result: Round 1 mockup Art/Source/options-screen/20261111-224939-options-screen-1.png (Kie.ai i2i, 6 credits); owner 'Je valide' 2026-11-11, deviations kept on purpose; prompt node art-prompts-options-screen-mockup.

## Relations
- [[task-504-options-screen-look]] — parent global task.
