---
id: task-191-debris-wall-collision
description: "Global task: debris pieces bounce off walls through distance field collision on a GPU emitter; cost measured."
parent: tasks-done-index
kind: task
status: done
---
# Robot debris: wall collision (GPU, distance fields)

- request: Est-ce que ça augmenterait énormément les coûts de prendre la collision des murs en compte ? ... Le A est très bien (owner, 2026-10-15: option A, GPU emitter with distance field collision)
- goal: Debris pieces no longer go through walls: they bounce off rock blocks, keep every current behaviour, and the cost is measured.
- acceptance: GPU emitter with distance field wall collision, all current behaviour kept and checked in -game; cost measured against task-182-03; enemy-death updated; committed.
- gdd_check: enemy-death says pieces collide with the floor only, no interaction with blocks: changed by the owner (2026-10-15) to floor and walls (blocks), still no miner or robots.
- confirmed_by_owner: yes (2026-10-15)
- milestone: M3
- next_step: none
- result: Done. Runs done but not comparable frame for frame: the PC was about 1.8x slower on the GPU and capped at 60 Hz (control S0 200 without debris 16.67 ms vs 8.98); trace: GPU debris with walls about 0.7-1.3 ms per frame, mostly the global distance field update (0.85 ms) plus Niagara about 0.4 ms; new: NS_RobotDebris compiles at first use in editor -game (0.12-0.21 s + PSO wait, 395 ms frame in S7); S7 debris plays 200 above the 80 cap (counters overstated); section in perf-stress-chaser-meshy; commit 79bb0f2d

## Subtasks
1. task-191-01-debris-gpu-df-collision — bp
2. task-191-02-debris-gpu-cost — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-191-01-debris-gpu-df-collision]] — step 1, bp.
- [[task-191-02-debris-gpu-cost]] — step 2, cpp.
