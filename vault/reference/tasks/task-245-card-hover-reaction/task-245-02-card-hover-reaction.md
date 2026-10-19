---
id: task-245-02-card-hover-reaction
description: "Capture a hovered card in the game and send the frame."
parent: task-245-card-hover-reaction
kind: task
status: todo
---
# 245-02 cpp

- goal: Capture a hovered card in the game and send the frame.
- agent: cpp
- depends_on: task-245-01-card-hover-reaction
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: captured frame
- done_when: frame sent to the owner; unseen items listed
- editor_state: closed
- token_ceiling: 40000 (provisional)
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-245-card-hover-reaction]] — parent global task.
