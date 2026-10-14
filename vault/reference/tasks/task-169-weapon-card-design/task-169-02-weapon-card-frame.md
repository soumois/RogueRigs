---
id: task-169-02-weapon-card-frame
description: "Unique frame for new-weapon cards, distinct from rarity frames; owner picks one, made 9-slice ready."
parent: task-169-weapon-card-design
kind: task
status: done
---
# New-weapon cards: unique frame

- goal: Generate candidate frames for the new-weapon cards, distinct from the rarity frames, show them to the owner, then turn the picked one into a 9-slice ready transparent PNG.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), placeholder-weapon-icons (vault/reference/gdd/art-audio/placeholder-weapon-icons.md), rarities (vault/reference/gdd/weapons/rarities.md)
- expected_output: Candidates in Art/Source/weapon-card-frame/ (3 variants, 3:4, salvage frame: bolted patched metal with a teal accent, its own colours, never a rarity colour, flat magenta inside and outside); after the owner pick: Art/UI/Cards/T_CardFrame_Weapon.png, transparent centre and outside, 9-slice margins in the report.
- done_when: Candidates generated and self-reviewed; owner picked one; transparent frame PNG exists with its margins; retained images, prompts and logs committed.
- editor_state: any
- token_ceiling: open
- model: sonnet
- tokens: input 32, cache write 69993, cache read 398304, output 1599
- runs: 2
- milestone: M2
- result: Owner picked candidate 1 (20261014-115709-weapon-card-frame-1.png). Keyed to Art/UI/Cards/T_CardFrame_Weapon.png, 1086 x 1448 RGBA, colour key with despill, no pink fringe. 9-slice margins left 140, top 180, right 140, bottom 190 px; side patch plates fall in the vertical stretch zone (fine up to about 1.3x). 18 credits. Commit 67fe903.

## Relations
- [[task-169-weapon-card-design]] — parent global task.
