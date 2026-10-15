---
id: task-178-01-card-glow-hover
description: "Stronger inner rarity glow on the upgrade cards (about 2x), and stronger still on hover (about 3x), driven by the button hover events."
parent: task-178-card-glow-hover
kind: task
status: review
---
# Upgrade cards: stronger glow and hover

- goal: Stronger inner rarity glow on the upgrade cards (about 2x), and stronger still on hover (about 3x), driven by the button hover events.
- agent: art
- depends_on: none
- inputs: art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: A stronger and slightly wider inner glow layer baked at the 177 card size (T_CardFrame_Stat_Glow_Baked.png regenerated; headroom so the bp layer can show it at about 2x today at rest and about 3x on hover by opacity); a control sheet with three rows (current, new at rest, new on hover) shown to the owner before integration.
- done_when: Glow PNG regenerated at the card size, control sheet shown to the owner, committed. No Kie credits.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: Glow in card_frame_layers.js: blur 22 -> 33 (about 1.5x wider), falloff 1 - exp(-3.4 field^1.15), inner line unchanged; source and baked glow regenerated (1000 x 1440), total alpha 3.36x the old layer, centre alpha 0. Rest opacity 0.6 (about 2x the old look), hover 1.0. Control sheet glow-hover-control.png shown to the owner, waiting for his validation. 0 credits. Commit 8518633.

## Relations
- [[task-178-card-glow-hover]] — parent global task.
