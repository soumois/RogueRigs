---
id: task-244-02-card-rarity-suspense
description: "Capture the reveal of a purple and a yellow card in the game and send the frames."
parent: task-244-card-rarity-suspense
kind: task
status: in-progress
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
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-244-card-rarity-suspense]] — parent global task.
