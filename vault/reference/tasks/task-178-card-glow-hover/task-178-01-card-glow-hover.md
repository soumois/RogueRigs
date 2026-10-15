---
id: task-178-01-card-glow-hover
description: "Stronger inner rarity glow on the upgrade cards (about 2x), and stronger still on hover (about 3x), driven by the button hover events."
parent: task-178-card-glow-hover
kind: task
status: in-progress
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
- result: open

## Relations
- [[task-178-card-glow-hover]] — parent global task.
