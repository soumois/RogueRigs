---
id: task-168-03-card-frames
description: "Three neutral grey frame styles for stat cards, tinted by rarity in game; owner picks one, made 9-slice ready."
parent: task-168-upgrade-card-design
kind: task
status: in-progress
---
# Upgrade cards: three frame styles

- goal: Stat card frame for rarity display. Owner 2026-10-14: the 3 proposed styles rejected (too clean); the stat frame is the owner-picked weapon frame (task-169-02) without its header plate; rarity shown as a mix of tinted accents and an inner rarity glow.
- agent: art
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), placeholder-weapon-icons (vault/reference/gdd/art-audio/placeholder-weapon-icons.md), rarities (vault/reference/gdd/weapons/rarities.md)
- expected_output: Phase 1 (rejected): 3 styles x 2 in Art/Source/card-frames/. Phase 2: image-to-image from the weapon frame without the header plate; three aligned layers in Art/UI/Cards/ (1086 x 1448): T_CardFrame_Stat.png (frame, accents neutral, untinted), T_CardFrame_Stat_Accents.png (white accents, tinted by rarity), T_CardFrame_Stat_Glow.png (white inner line and glow, tinted by rarity); example sheet option-AC-mix.png; builder Tools/art/card_frame_layers.js; 9-slice margins in the report.
- done_when: Owner picked the derived frame and the A+C rarity display; three layer PNGs exist with measured margins; retained candidate, prompt, log, sheet and script committed.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
