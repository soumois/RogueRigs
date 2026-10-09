---
id: task-065-01-diagonal-dig
description: "Moving diagonally into a corner of two blocks still digs the nearest touching block(s)."
parent: task-065-diagonal-dig
kind: task
status: in-progress
---
# Diagonal dig fallback

- goal: Moving diagonally into a corner of two blocks still digs the nearest touching block(s).
- agent: cpp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), sys-drone (vault/reference/tech/systems/sys-drone.md), sys-grid (vault/reference/tech/systems/sys-grid.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: ARRDrone / ARRMiner contact dig: find why a miner moving diagonally into a corner formed by two blocks gets an empty arc (likely the move direction points into the gap between them so both blocks fall outside the 120° cone, or the averaged contact normal cancels); fix: when the miner touches rock and the arc selection is empty, dig the touching block(s) nearest the arc direction (at least one); update only existing dig tests that break; docs sys-drone.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-065-diagonal-dig]] — parent global task.
