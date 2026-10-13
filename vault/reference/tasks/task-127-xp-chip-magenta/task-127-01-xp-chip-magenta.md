---
id: task-127-01-xp-chip-magenta
description: "Recolour the retained data chip B image so its glowing core is bright magenta-pink, for the big XP block."
parent: task-127-xp-chip-magenta
kind: task
status: done
---
# Big XP block: data chip B recoloured magenta

- goal: Recolour the retained data chip B image so its glowing core is bright magenta-pink, for the big XP block.
- agent: art
- depends_on: none
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md)
- expected_output: Art/Source/xp-chip/ with 1 PNG (1:1, 1K), 1 correction prompt file in prompts/, run log. Correction prompt opens with "Keep this exact image: same ..." and changes ONE thing: the glowing core and its glow become bright magenta-pink (about RGB 255, 50, 205), reference image Art/Source/xp-chip/20261013-092217-xp-chip-1.png.
- done_when: Exactly 1 image generated (image-to-image, 1:1, 1K); shape, board, legs, bolts, background and view unchanged; no cyan left on the core or its glow; no red, orange, acid green or steel blue; self-review against A1, A2, A16; image sent to the owner; nothing goes to vault/assets/.
- editor_state: any
- token_ceiling: open
- model: sonnet
- tokens: input 16, cache write 14306, cache read 93515, output 585
- runs: 1
- milestone: M2
- result: Chip B recoloured magenta, 6 credits; owner validated it for the big XP block.

## Relations
- [[task-127-xp-chip-magenta]] — parent global task.
