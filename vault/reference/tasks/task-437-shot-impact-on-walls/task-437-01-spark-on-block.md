---
id: task-437-01-spark-on-block
description: "Cpp: A qualifying shot stopped by a wall the drone can dig requests the same impact spark at the hit point."
parent: task-437-shot-impact-on-walls
kind: task
status: done
---
# Shot impact sparks on walls: cpp

- goal: A qualifying shot stopped by a wall the drone can dig requests the same impact spark at the hit point.
- agent: cpp
- depends_on: none
- inputs: task-432-player-shot-impact-sparks (vault/reference/tasks/task-432-player-shot-impact-sparks.md), bp-shot-impact-spark (vault/reference/tech/blueprints/fx/bp-shot-impact-spark.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md)
- expected_output: see done_when
- done_when: Build ok; a miner or turret shot stopped by a diggable block requests one spark, a robot shot none; Weapons test family passes, only expectations this change breaks updated.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-437
- usage: included in the executor run of task-437
- runs: 1
- milestone: M2
- started: 2026-11-05T17:53:42Z
- waiting_for: none
- result: Done. OnProjectileStop plays the spark on any ARRArenaGrid::IsBlockCell cell, the criterion the drone digs by (rock, ore, heal ore, special ore, barrel); no undiggable block type exists in the code. Barrel damage and shot stop unchanged; robot shots have no system; pool cap and skip unchanged. Build ok; RogueRigs.Weapons 14 tests pass under -nullrhi (ImpactSparks updated: rock now one spark on the face at shot height, rock health unchanged, turret on rock, ore and heal ore; special ore not fired at, no reachable cell in the test layout). Look unverified.

## Relations
- [[task-437-shot-impact-on-walls]] — parent global task.
