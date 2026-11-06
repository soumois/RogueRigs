---
id: task-475-02-capture
description: "Capture: Check the raised walls in game."
parent: task-475-wall-height-miner
kind: task
status: in-progress
---
# Walls raised to the miner height: capture

- goal: Check the raised walls in game.
- agent: capture
- depends_on: task-475-01-block-height
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp, Art/Source/cartoon-look/reference/owner-2026-11-05-ingame-light-walls.webp
- expected_output: see done_when
- done_when: C1 wall tops level with the top of the miner standing beside a wall; C2 ore crystals and damaged plates sit on the new tops; C3 a robot right behind a wall is still spotted; C4 barrels and void edge unchanged; sheet before / after sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T17:29:45Z
- waiting_for: none
- result: open

## Relations
- [[task-475-wall-height-miner]] — parent global task.
