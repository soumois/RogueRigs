---
id: task-191-debris-wall-collision
description: "Global task: debris pieces bounce off walls through distance field collision on a GPU emitter; cost measured."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Robot debris: wall collision (GPU, distance fields)

- request: Est-ce que ça augmenterait énormément les coûts de prendre la collision des murs en compte ? ... Le A est très bien (owner, 2026-10-15: option A, GPU emitter with distance field collision)
- goal: Debris pieces no longer go through walls: they bounce off rock blocks, keep every current behaviour, and the cost is measured.
- acceptance: GPU emitter with distance field wall collision, all current behaviour kept and checked in -game; cost measured against task-182-03; enemy-death updated; committed.
- gdd_check: enemy-death says pieces collide with the floor only, no interaction with blocks: changed by the owner (2026-10-15) to floor and walls (blocks), still no miner or robots.
- confirmed_by_owner: yes (2026-10-15)
- milestone: M3
- next_step: task-191-01-debris-gpu-df-collision
- result: open

## Subtasks
1. task-191-01-debris-gpu-df-collision — bp
2. task-191-02-debris-gpu-cost — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-191-01-debris-gpu-df-collision]] — step 1, bp.
- [[task-191-02-debris-gpu-cost]] — step 2, cpp.
