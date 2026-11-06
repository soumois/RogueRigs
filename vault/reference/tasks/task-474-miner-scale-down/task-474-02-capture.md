---
id: task-474-02-capture
description: "Capture: Check the smaller miner in game."
parent: task-474-miner-scale-down
kind: task
status: todo
---
# Miner 10 % smaller: capture

- goal: Check the smaller miner in game.
- agent: capture
- depends_on: task-474-01-miner-scale
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/hud-unified/reference/owner-hud-ingame-2026-11-06.webp
- expected_output: see done_when
- done_when: C1 the miner is visibly smaller than on the look before, about 90 %; C2 rifle in hand, no floating or clipping part; C3 aim laser and muzzle flash start at the rifle muzzle; C4 numbers above the head not inside the mesh; C5 drone and camera unchanged; sheet before / after sent to the owner.
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
- [[task-474-miner-scale-down]] — parent global task.
