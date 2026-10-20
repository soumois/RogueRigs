---
id: task-244-02-card-rarity-suspense
description: "Capture the reveal of a purple and a yellow card in the game and send the frames."
parent: task-244-card-rarity-suspense
kind: task
status: done
---
# 244-02 cpp

- goal: Capture the reveal of a purple and a yellow card in the game and send the frames.
- agent: cpp
- depends_on: task-244-01-card-rarity-suspense
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: captured frames
- done_when: frames sent to the owner; unseen items listed
- editor_state: closed
- token_ceiling: 40000 (provisional)
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Capture -game, luck 15 via the engine set command on RRPlayerAttributeSet: yellow and purple reveal seen (order, dim, glow, punch, flash, end state clean); rays look like rotated card outlines of the glow texture, glow on the back reads as a tinted rectangle; owner validated the rays as they are (2026-10-20); the full-screen flash includes the HUD, as built. Unseen: flash peak, two rare cards, reroll and close mid-sequence, button clicks. 1 run.

## Relations
- [[task-244-card-rarity-suspense]] — parent global task.
