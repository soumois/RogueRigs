---
id: task-432-01-spawn-on-hit
description: "Cpp: A qualifying shot hit requests one impact spark at the hit point through the pool or a shared system, nothing when none is free."
parent: task-432-player-shot-impact-sparks
kind: task
status: in-progress
---
# Player shot impact sparks: cpp

- goal: A qualifying shot hit requests one impact spark at the hit point through the pool or a shared system, nothing when none is free.
- agent: cpp
- depends_on: none
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), sys-projectiles (vault/reference/tech/systems/combat/sys-projectiles.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), perf-lead-debris-gpu (vault/reference/tech/performance/perf-leads/perf-lead-debris-gpu.md)
- expected_output: see done_when
- done_when: Build passes and existing projectile tests pass; hits on a robot or barrel by a miner or turret shot request one spark, a stop on rock or ore and a robot shot request none; a full pool skips the spark without error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T15:45:58Z
- waiting_for: none
- result: open

## Relations
- [[task-432-player-shot-impact-sparks]] — parent global task.
