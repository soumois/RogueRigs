---
id: task-029-05-arc-mining
description: "Drone digs up to 4 blocks in a 120° arc when the miner touches rock; rock 0.5 s, ore and heal 1.5 s; tests."
parent: task-029-fine-grid
kind: task
status: todo
---
# Arc mining by contact

- goal: mining feels like DRGS.
- agent: cpp
- depends_on: task-029-04-switch-67
- inputs: [[drone]], [[ore-blocks]], [[heal-ore]], [[sys-drone]], [[sys-grid]], [[drgs-analysis]], [[cpp-standard]], [[sys-automation-tests]], [[stress-grid-cells]], [[sys-stress]]
- expected_output: contact detection (miner capsule within a small margin of a rock block, no push needed); arc selection: blocks within 1.5 cells of the miner front in a 120° cone toward the contact normal or movement direction, up to 4 dug in parallel, damage persists per block; speeds per GDD scaled by MiningSpeed; drone positioned at the arc centre; dig events per block for the laser and shake; tests: contact digs without input, arc limits, speeds, persistence, stops when moving away; docs sys-drone.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 120000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
