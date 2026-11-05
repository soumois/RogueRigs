---
id: task-449-01-wave-follows
description: "Cpp: The level-up wave stays on the miner while it plays, the push staying one blow."
parent: task-449-level-up-wave-follows
kind: task
status: in-progress
---
# Level-up floor wave follows the miner: cpp

- goal: The level-up wave stays on the miner while it plays, the push staying one blow.
- agent: cpp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), bp-level-up-burst (vault/reference/tech/blueprints/fx/bp-level-up-burst.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md), task-441-level-up-effect (vault/reference/tasks/task-441-level-up-effect.md), Art/Source/level-up-fx/20261105-owner-two-level-up-texts.webp
- expected_output: see done_when
- done_when: Build ok; in the simulation ring and shard positions follow a moving miner and stay at floor height; push still applied once; LevelUp test family passes, figures given in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T22:42:52Z
- waiting_for: none
- result: open

## Relations
- [[task-449-level-up-wave-follows]] — parent global task.
