---
id: task-067-01-real-game-stress
description: "Stress scenario close to a real game: 150 robots, a horde, full HUD and minimap, Niagara on, miner running and digging."
parent: task-067-real-game-stress
kind: task
status: done
---
# Real-game stress scenario

- goal: Stress scenario close to a real game: 150 robots, a horde, full HUD and minimap, Niagara on, miner running and digging.
- agent: cpp
- depends_on: none
- inputs: sys-stress (vault/reference/tech/systems/sys-stress.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), sys-progression (vault/reference/tech/systems/sys-progression.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: extend the -game stress harness with a "real game" mode: wave director live with MaxAlive 150, one horde forced (40–60 robots), the HUD and minimap shown, Niagara on (renderer, not -nullrhi), the miner driven by the pacing bot run-and-collect profile so it moves and digs, damage numbers and pickups live; 3 runs of 60 s; report frame and game-thread avg / p99 / max, flow-field cost, spikes over 25 ms with the frame time line; on the owner’s normal PC profile (background apps open, close nothing); then, to find the current limits (owner 2026-10-09: "pousser les tests pour voir les limites actuelles"), one 60 s real-game run per MaxAlive step 150, 250, 400, 600, 800, stopping after the first step whose frame p99 exceeds 33 ms; report the highest step with frame p99 under 16.7 ms and under 33 ms, and what bounds the frame at each step (game thread, render, GPU, flow field); results in stress-grid-cells; no gameplay change.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: opus
- tokens: input 94, cache write 182868, cache read 5745185, output 52450
- runs: 1
- milestone: M2
- result: Real-game stress mode (live director, fill, forced horde, run-and-collect bot). Limit 250 robots (p99 31.9 ms); no step under 16.7 ms p99 (150: 19.4-20.3 ms); game thread bounds every step (about 10.5 ms at 150), render 10-11 ms and GPU 7.6 ms flat, flow field under 0.2 ms. Spikes: one GC per run, wave spawns about 1.3 ms per robot, untagged spikes up to 95 ms during mass robot losses. 195 tests pass.

## Relations
- [[task-067-real-game-stress]] — parent global task.
