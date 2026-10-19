---
id: task-243-06-card-deal-flip
description: "Run the game with a level-up (rr.Cheat.GrantXP) and capture a frame series of the arrival and the flip with rr.Debug.Shots; compare with the look before; send the frames."
parent: task-243-card-deal-flip
kind: task
status: done
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
- runs: 3
- milestone: M2
- result: Capture -game in 3 rounds. Bugs found and fixed through 243-05 (bp, 3 runs): world timer frozen by the pause (a43a8e9c, aee3f7c4: gated widget tick), card buttons disabled drew the back at 45 percent alpha (now HitTestInvisible). Final frames: arrival from the bottom, opaque backs grey/green/blue/purple matching the tier of the face, flip mid-card narrowed, all face up, Reroll bright at the end. Unseen: yellow back, card button clicks, exact fan angles, closing mid-sequence, new-weapon tier. Sent to the owner. 3 runs.

## Relations
- [[task-243-card-deal-flip]] — parent global task.
