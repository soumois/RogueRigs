---
id: task-067-real-game-stress
description: "Global task: Stress scenario close to a real game: 150 robots, a horde, full HUD and minimap, Niagara on, miner running and digging."
parent: tasks-done-index
kind: task
status: done
---
# Real-game stress scenario

- request: "Un scénario de stress partie réelle" (proposal after task-066-02, to run at night) (owner, 2026-10-09)
- goal: Stress scenario close to a real game: 150 robots, a horde, full HUD and minimap, Niagara on, miner running and digging.
- acceptance: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-09).
- confirmed_by_owner: yes (2026-10-09, planned; the owner launches it at night)
- milestone: M2
- next_step: none
- result: Done. Real-game stress mode (live director, fill, forced horde, run-and-collect bot). Limit 250 robots (p99 31.9 ms); no step under 16.7 ms p99 (150: 19.4-20.3 ms); game thread bounds every step (about 10.5 ms at 150), render 10-11 ms and GPU 7.6 ms flat, flow field under 0.2 ms. Spikes: one GC per run, wave spawns about 1.3 ms per robot, untagged spikes up to 95 ms during mass robot losses. 195 tests pass.

## Subtasks
1. task-067-01-real-game-stress — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-067-01-real-game-stress]] — step 1, cpp.
