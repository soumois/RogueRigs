---
id: task-441-03-robot-push
description: "Cpp: A level-up pushes robots inside the radius a little, with no damage, following their resistance."
parent: task-441-level-up-effect
kind: task
status: done
---
# Level-up effect rework: cpp

- goal: A level-up pushes robots inside the radius a little, with no damage, following their resistance.
- agent: cpp
- depends_on: task-441-02-text-asset
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), weapons-index (vault/reference/gdd/weapons/weapons-index.md)
- expected_output: see done_when
- done_when: Build ok; a test or existing family shows robots inside move away and take no damage, robots outside do not move.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-441
- usage: included in the executor run of task-441
- runs: 1
- milestone: M2
- started: 2026-11-05T20:14:02Z
- waiting_for: none
- result: PushNearbyRobots in the burst: every living robot within PushRadius 300 uu (2D) gets ARREnemy::ApplyKnockback away from the miner at PushSpeed 800 uu/s (provisional), scaled by KnockbackResistance, no damage. Also added LevelUpWaveSystem and User.WaveRadius. Build ok. RogueRigs.LevelUp 7 of 7 under -nullrhi: inside launched once for two levels, outside not, health unchanged, travel 73.6 uu in 40 frames. TextAsset passed under -RenderOffScreen: 1 sprite, 41 frames, width 730 to 640, rises straight, gold.

## Relations
- [[task-441-level-up-effect]] — parent global task.
