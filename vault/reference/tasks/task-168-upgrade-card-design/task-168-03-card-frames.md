---
id: task-168-03-card-frames
description: "Three neutral grey frame styles for stat cards, tinted by rarity in game; owner picks one, made 9-slice ready."
parent: task-168-upgrade-card-design
kind: task
status: in-progress
---
# Upgrade cards: three frame styles

- goal: Generate three frame styles for the stat cards in neutral grey (tinted by rarity in game), show them to the owner, then turn the picked one into a 9-slice ready transparent PNG.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), placeholder-weapon-icons (vault/reference/gdd/art-audio/placeholder-weapon-icons.md), rarities (vault/reference/gdd/weapons/rarities.md)
- expected_output: Candidates in Art/Source/card-frames/ (3 styles, for example bolted salvage metal, bevelled corners, riveted plate; 2 variants each; 3:4; frame only, light neutral grey so a rarity tint multiplies cleanly, flat magenta inside and outside); after the owner pick: Art/UI/Cards/T_CardFrame_Stat.png, transparent centre and outside, with the 9-slice margins written in the report.
- done_when: Candidates generated and self-reviewed; owner picked one; transparent frame PNG exists with its margins; retained images, prompts and logs committed.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
