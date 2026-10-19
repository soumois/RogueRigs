---
id: task-243-06-card-deal-flip
description: "Run the game with a level-up (rr.Cheat.GrantXP) and capture a frame series of the arrival and the flip with rr.Debug.Shots; compare with the look before; send the frames."
parent: task-243-card-deal-flip
kind: task
status: todo
---
# 243-06 cpp

- goal: Run the game with a level-up (rr.Cheat.GrantXP) and capture a frame series of the arrival and the flip with rr.Debug.Shots; compare with the look before; send the frames.
- agent: cpp
- depends_on: task-243-05-card-deal-flip
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md)
- expected_output: captured frames
- done_when: frames show the cards face down then face up, sent to the owner; unseen items listed
- editor_state: closed
- token_ceiling: 40000 (provisional)
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-243-card-deal-flip]] — parent global task.
