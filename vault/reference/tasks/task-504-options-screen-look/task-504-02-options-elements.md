---
id: task-504-02-options-elements
description: "Art: Map every element of the validated mockup to an existing sprite or a new PNG."
parent: task-504-options-screen-look
kind: task
status: done
---
# Options screen: Kie.ai mockup and its elements: art

- goal: Map every element of the validated mockup to an existing sprite or a new PNG.
- agent: art
- depends_on: task-504-01-options-mockup
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), art-prompts-pause-menu-mockup (vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md)
- expected_output: see done_when
- done_when: A3 of the global passes: a table in the mockup node lists each element (frame, tabs and their selected state, slider track and knob, toggle on and off, left/right selector, buttons, dividers) with its reused or new file, margins and credits; new PNGs imported and labelled keep.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 8, cache write 24347, cache read 48431, output 3927
- usage: turns 4, context start 7k peak 24k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-11-11T21:53:25Z
- waiting_for: none
- result: Every element of the validated mockup mapped (table in art-prompts-options-screen-mockup): frame, title rails, buttons and selected tab reused; unselected tab, selector arrow, slider track and knob, toggle track and thumb cut from one Kie.ai sheet (6 credits, cap 36), imported to /Game/RogueRigs/UI/Options. Task credits 12. Gap: slider knob shape differs from the mockup.

## Relations
- [[task-504-options-screen-look]] — parent global task.
