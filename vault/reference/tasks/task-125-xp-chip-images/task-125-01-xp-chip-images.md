---
id: task-125-01-xp-chip-images
description: "Two candidate images of a data chip as the basic XP pickup, from two different prompts."
parent: task-125-xp-chip-images
kind: task
status: in-progress
---
# XP pickup: data chip, 2 images

- goal: Two candidate images of a data chip as the basic XP pickup, from two different prompts.
- agent: art
- depends_on: none
- inputs: xp-level-up (vault/reference/gdd/run/xp-level-up.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md)
- expected_output: Art/Source/xp-chip/ with 2 PNG (1:1, 1K), 2 prompt files, run logs. Prompt A: thick rectangular plate, gold contacts on one edge, thin cyan glowing line. Prompt B: square micro-chip on a small board, glowing cyan core in the centre.
- done_when: Style A1, A2, A6, A16 (plain light grey background, three-quarter top view); glowing cyan main colour, never magenta, red, orange, acid green or steel blue; two different prompts, one image each (2 variants in total, 2 prompt files); self-review each image against the A rules; send both images to the owner with SendUserFile; nothing goes to vault/assets/.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-125-xp-chip-images]] — parent global task.
