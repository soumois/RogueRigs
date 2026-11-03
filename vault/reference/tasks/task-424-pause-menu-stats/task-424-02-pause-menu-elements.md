---
id: task-424-02-pause-menu-elements
description: "Art: take the elements of the validated mockup: map reuse of existing sprites, make the new PNGs"
parent: task-424-pause-menu-stats
kind: task
status: done
---
# Pause menu: miner stats screen with Resume and Exit: art

- goal: List every visual element of the validated mockup; map each to an existing sprite when one matches (reuse list of [[art-prompts-level-end-screen-mockup]], weapon icons in Content/RogueRigs/UI/Icons); for each new element, cut it from the validated mockup when clean at 1K, otherwise regenerate it with kie_image.js image-to-image from the mockup (route, variants and budget per Q5), frames 3x3 with margins measured in px and UMG fraction.
- agent: art
- depends_on: task-424-01-pause-menu-kie-mockup
- inputs: vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md (written by task-424-01: validated mockup, element table), pause-menu (vault/reference/gdd/run/pause-menu.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: New sprites as Art/UI/PauseMenu/T_Pause_*.png (keyed RGBA, ui_icon_prepare.js --check clean) with prompts and logs in Art/Source/pause-menu/; the prompt node art-prompts-pause-menu-mockup updated with the element table (element, reused asset or new file, margins, credits).
- done_when: Every element of the validated mockup is mapped to a reused asset or a committed new PNG; credits within the Q5 cap; node updated; lint clean; committed by path.
- editor_state: any (no editor needed)
- token_ceiling: 0.15 M units
- model: sonnet
- tokens: input 18, cache write 26083, cache read 176340, output 4751
- usage: turns 9, context start 11k peak 31k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M3
- started: 2026-11-03T06:37:51Z
- waiting_for: none
- result: All elements map to existing sprites (LevelEnd frame/dividers, Shop title bar, Shop continue button); no new PNG, 0 credits (cap 24). Element table in art-prompts-pause-menu-mockup. Button margins estimated, not measured.

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
