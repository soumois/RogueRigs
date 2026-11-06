---
id: task-473-01-light-match-loop
description: "Bp: Loop: tune the miner light colour, intensity and radius, -game capture, side-by-side with image 2, rework until it matches or the Q2 cap."
parent: task-473-miner-light-calibration
kind: task
status: in-progress
---
# Miner light calibrated on the owner in-game reference: bp

- goal: Loop: tune the miner light colour, intensity and radius, -game capture, side-by-side with image 2, rework until it matches or the Q2 cap.
- agent: bp
- depends_on: none
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), mpp-player-light-mask (vault/reference/tech/blueprints/world/mpp-player-light-mask.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: A2 to A5 and A7 pass on the last round capture, or the Q2 cap is reached with the remaining gaps listed; every round written in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T18:22:25Z
- waiting_for: none
- result: open

## Relations
- [[task-473-miner-light-calibration]] — parent global task.
