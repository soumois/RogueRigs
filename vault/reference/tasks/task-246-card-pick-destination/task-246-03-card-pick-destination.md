---
id: task-246-03-card-pick-destination
description: "Capture the pick of a targeted, an all-weapons and a player-stat card in the game and send the frames."
parent: task-246-card-pick-destination
kind: task
status: todo
---
# 246-03 cpp

- goal: Capture the pick of a targeted, an all-weapons and a player-stat card in the game and send the frames.
- agent: cpp
- depends_on: task-246-02-card-pick-destination
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
- [[task-246-card-pick-destination]] — parent global task.
