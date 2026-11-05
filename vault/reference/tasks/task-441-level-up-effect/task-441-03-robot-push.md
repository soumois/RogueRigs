---
id: task-441-03-robot-push
description: "Cpp: A level-up pushes robots inside the radius a little, with no damage, following their resistance."
parent: task-441-level-up-effect
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-441-level-up-effect]] — parent global task.
