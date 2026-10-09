---
id: task-065-01-diagonal-dig
description: "Moving diagonally into a corner of two blocks still digs the nearest touching block(s)."
parent: task-065-diagonal-dig
kind: task
status: done
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
- model: opus
- tokens: input 36, cache write 82289, cache read 499451, output 14064
- runs: 1
- milestone: M2
- result: Cause: UpdateDigTrigger let any move input pointing slightly toward the wall steer the arc up to 90° off the wall, outside the ±60° cone, so a diagonal corner gave an empty arc and the dig released (reproduced in a script sweep). Fix: the miner passes its touching cells; RequestArcDig overload falls back to the touching block(s) closest to the arc direction (ties kept, capped at MaxArcBlocks) when the arc is empty. No test changed. Build 0; full run 193/195 (known two). Commit e5290cc.

## Relations
- [[task-065-diagonal-dig]] — parent global task.
