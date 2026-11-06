---
id: task-473-03-doc
description: "Doc: Docs record the calibrated miner light values."
parent: task-473-miner-light-calibration
kind: task
status: done
---
# Miner light calibrated on the owner in-game reference: doc

- goal: Docs record the calibrated miner light values.
- agent: doc
- depends_on: task-473-02-capture
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), visual-style (vault/reference/gdd/art-audio/visual-style.md)
- expected_output: see done_when
- done_when: bp-miner holds the new light values and the revert values; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-473
- usage: included in the executor run of task-473
- runs: 1
- milestone: M2
- started: 2026-11-06T18:35:32Z
- waiting_for: none
- result: bp-miner updated (84ad9e80): new FloorWallLight values 6.5 / 700 / 3 / 255,175,60, setter script Tools/unreal/miner_floor_light.py, revert values 4 / 800 / 4 / 255,194,158; Relations on both ends; lint clean. visual-style (gdd) not edited: it still says warm cream 255/194/158 and pool about 400 uu; changing a gdd node is outside the Executor's limits, left to the owner or Planner.

## Relations
- [[task-473-miner-light-calibration]] — parent global task.
