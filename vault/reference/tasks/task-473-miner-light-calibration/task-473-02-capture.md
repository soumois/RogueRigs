---
id: task-473-02-capture
description: "Capture: Final independent check of the miner light against image 2."
parent: task-473-miner-light-calibration
kind: task
status: todo
---
# Miner light calibrated on the owner in-game reference: capture

- goal: Final independent check of the miner light against image 2.
- agent: capture
- depends_on: task-473-01-light-match-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp, Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 warm orange centre fading to pink-violet; C2 pool size and falloff close to image 2; C3 floor under the miner below about 205/255; C4 robots and pickups inside the pool not lit by it; C5 wall shadows from the miner light still present; side-by-side sheets before / after / image 2 sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-473-miner-light-calibration]] — parent global task.
