---
id: task-168-08-baked-card-frames
description: "Bake the card frame layers at the exact card size (2x), so Unreal draws them as plain images without 9-slice."
parent: task-168-upgrade-card-design
kind: task
status: in-progress
---
# Upgrade cards: frame layers baked at card size

- goal: The frame stays thin in game: compose the 9-slice offline at the card size instead of relying on Unreal Box brushes (owner test 2026-10-14: corners still drawn about 3 times too large after 168-06).
- agent: art
- depends_on: task-168-03-card-frames, task-169-02-weapon-card-frame
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md); files Art/UI/Cards/T_CardFrame_Stat.png, T_CardFrame_Stat_Accents.png, T_CardFrame_Stat_Glow.png, T_CardFrame_Weapon.png (1086 x 1448), Tools/art/card_frame_layers.js, Tools/art/ui_icon_prepare.js (PNG codec)
- expected_output: Tools/art/card_frame_bake.js (node, usage header): 9-slice compose of each layer to a target size with a uniform corner scale, middle bands stretched, high-quality resampling, alpha preserved; outputs at 880 x 1440 (card 440 x 720 at 2x), frame drawn so the side band is about 28 px at 1x: Art/UI/Cards/T_CardFrame_Stat_Baked.png, T_CardFrame_Stat_Accents_Baked.png, T_CardFrame_Stat_Glow_Baked.png, T_CardFrame_Weapon_Baked.png; a control sheet Art/Source/card-frames/rarity-examples/baked-control.png (the 5 rarities and the weapon card at 1x, composited as the game will: base, accents x tint, glow x tint, on #15171c); the measured opening safe rect (inset left, top, right, bottom in px of the baked textures) per frame, in the report and the script output.
- done_when: Four baked PNGs 880 x 1440 exist; control sheet shown to the owner; opening insets measured by script; script and outputs committed. No Kie credits.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
