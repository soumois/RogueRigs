---
id: task-126-01-xp-nut-images
description: "Two candidate images of a hex nut as the basic XP pickup, from two different prompts."
parent: task-126-xp-nut-images
kind: task
status: todo
---
# XP pickup: hex nut, 2 images

- goal: Two candidate images of a hex nut as the basic XP pickup, from two different prompts.
- agent: art
- depends_on: none
- inputs: xp-level-up (vault/reference/gdd/run/xp-level-up.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md)
- expected_output: Art/Source/xp-nut/ with 2 PNG (1:1, 1K), 2 prompt files, run logs. Prompt A: thick clean hexagonal nut, glowing cyan hole at the centre. Prompt B: squat worn nut, rust chips on the edges, cyan glow ring in the thread.
- done_when: Style A1, A2, A6, A16 (plain light grey background, three-quarter top view); glowing cyan main colour, never magenta, red, orange, acid green or steel blue; two different prompts, one image each (2 variants in total, 2 prompt files); self-review each image against the A rules; send both images to the owner with SendUserFile; nothing goes to vault/assets/.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-126-xp-nut-images]] — parent global task.
