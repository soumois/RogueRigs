---
id: task-246-03-card-pick-destination
description: "Capture the pick of a targeted, an all-weapons and a player-stat card in the game and send the frames."
parent: task-246-card-pick-destination
kind: task
status: done
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
- runs: 3
- milestone: M2
- result: Capture -game with real clicks, 3 rounds. Defects found and fixed in 246-02 (bp, 3 more runs): CloseCards after ChooseCard left the game paused with a queued offer; burn parameters set with parameter info Index 0 instead of -1 (4708004b, plus sine pattern and Tint). Final frames: discarded cards dissolve bottom up in the tier colour, overlay same size as the card, no bare frames; icon flies to the first free slot (new weapon), the weapon slot (targeted), each owned slot (all weapons) and the XP bar (player stat); game resumes with one pending offer; next deal starts with queued offers. Seen: fire edge is pale yellow, not orange (FireColor 1, 0.45, 0.1 x intensity 4 saturates), regular flame tongues; arc barely visible. Unseen: new-weapon with non-contiguous slots. Sent to the owner. 3 runs.

## Relations
- [[task-246-card-pick-destination]] — parent global task.
