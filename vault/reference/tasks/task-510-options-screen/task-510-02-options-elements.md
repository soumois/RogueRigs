---
id: task-510-02-options-elements
description: "Art: take the elements of the validated mockup: map reuse of existing sprites, make the new PNGs (slider, toggle, tab, arrow selector)"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: art

- goal: take the elements of the validated mockup: map reuse of existing sprites, make the new PNGs (slider, toggle, tab, arrow selector)
- agent: art
- depends_on: task-510-01-options-kie-mockup
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md)
- expected_output: see done_when
- done_when: Every element of the validated mockup is either mapped to an existing texture or exists as a new PNG with prompt and log, recorded in a prompt node; credits within 36 for the step.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 18, cache write 38653, cache read 178492, output 7702
- usage: turns 8, context start 7k peak 39k, resumes 1 (message x1), 3 min, units 0.10 M
- runs: 1
- milestone: M3
- started: 2026-11-12T05:30:31Z
- waiting_for: none
- result: Every mockup element mapped or made, recorded in art-prompts-options-screen-mockup: reused T_LevelEnd_Frame, T_LevelEnd_DividerV, T_Shop_TitleBar, T_Shop_ButtonContinue (buttons and active tab); new Art/UI/Options T_Options_Tab, SelectorPlate, Arrow, SliderTrack, SliderFill, SliderKnob, ToggleOn, ToggleOff, ToggleKnob (Kie.ai image-to-image from the mockup, magenta key, ui_icon_prepare.js). 5 runs, 30 credits (cap 36). style-guide: round toggle failed A1, corrected (faceted toggle run 20261112-101341), all pass; notes kept: selector plate teal caps differ from the mockup, toggle teal brighter than slider fill. 3-slice margins by eye.

## Relations
- [[task-510-options-screen]] — parent global task.
