---
id: task-441-01-level-up-text
description: "Cpp: A level-up requests the gold straight-rising text, independent of the damage number toggle."
parent: task-441-level-up-effect
kind: task
status: todo
---
# Level-up effect rework: cpp

- goal: A level-up requests the gold straight-rising text, independent of the damage number toggle.
- agent: cpp
- depends_on: none
- inputs: sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), damage-numbers (vault/reference/gdd/weapons/damage-numbers.md), bp-damage-numbers (vault/reference/tech/blueprints/fx/bp-damage-numbers.md), xp-level-up (vault/reference/gdd/run/xp-level-up.md)
- expected_output: see done_when
- done_when: Build ok; one text request per level-up moment, damage number behaviour unchanged; LevelUp test family passes.
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
