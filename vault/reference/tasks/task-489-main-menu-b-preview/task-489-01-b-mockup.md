---
id: task-489-01-b-mockup
description: "Art: One temporary image-to-image mockup of the current main menu with the buttons drawn as option B, shown to the owner."
parent: task-489-main-menu-b-preview
kind: task
status: done
---
# Preview image of main menu button option B (discussion only): art

- goal: One temporary image-to-image mockup of the current main menu with the buttons drawn as option B, shown to the owner.
- agent: art
- depends_on: none
- inputs: main-menu (vault/reference/gdd/run/main-menu.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Saved/Captures/task485_b1080/c_mm.png, vault/assets/main-menu-background.png
- expected_output: see done_when
- done_when: Image and verbatim prompt in Saved/Scratch/task-489/, file names marked temporary; buttons are dark semi-transparent metal plates, white labels New Run, Options (greyed), Exit, thin orange edge; background, gradient and title as in the current menu; path reported to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 4, cache write 20293, cache read 12869, output 4245
- usage: turns 2, context start 9k peak 24k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-11-07T04:21:17Z
- waiting_for: none
- result: Temporary mockup Saved/Scratch/task-489/TEMPORARY-01-main-menu-b-mockup.png (GPT Image 2 image-to-image from Saved/Captures/task485_b1080/mmrest_00.png, 1 variant, 1672x941 16:9), prompt TEMPORARY-01-b-plates-prompt.txt and run log alongside; nothing in Art/Source, Content or gdd. Dark metal plates, orange edge, white New Run/Exit, Options greyed; title, gradient, background kept. style-guide: 1 fail A1 by analogy (glossy brushed sheen, not flat); noted: no focus state on New Run, Options edge still dim orange, plates look nearly opaque, image re-rendered at 1672x941. Left to the owner (task-487 Q1).

## Relations
- [[task-489-main-menu-b-preview]] — parent global task.
