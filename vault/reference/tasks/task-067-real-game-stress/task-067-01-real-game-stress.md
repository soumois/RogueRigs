---
id: task-067-01-real-game-stress
description: "Stress scenario close to a real game: 150 robots, a horde, full HUD and minimap, Niagara on, miner running and digging."
parent: task-067-real-game-stress
kind: task
status: todo
---
# Real-game stress scenario

- goal: Stress scenario close to a real game: 150 robots, a horde, full HUD and minimap, Niagara on, miner running and digging.
- agent: cpp
- depends_on: none
- inputs: sys-stress (vault/reference/tech/systems/sys-stress.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), sys-progression (vault/reference/tech/systems/sys-progression.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: extend the -game stress harness with a "real game" mode: wave director live with MaxAlive 150, one horde forced (40–60 robots), the HUD and minimap shown, Niagara on (renderer, not -nullrhi), the miner driven by the pacing bot run-and-collect profile so it moves and digs, damage numbers and pickups live; 3 runs of 60 s; report frame and game-thread avg / p99 / max, flow-field cost, spikes over 25 ms with the frame time line; on the owner’s normal PC profile (background apps open, close nothing); results in stress-grid-cells; no gameplay change.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-067-real-game-stress]] — parent global task.
