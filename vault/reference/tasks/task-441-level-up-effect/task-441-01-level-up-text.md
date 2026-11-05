---
id: task-441-01-level-up-text
description: "Cpp: A level-up requests the gold straight-rising text, independent of the damage number toggle."
parent: task-441-level-up-effect
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-441
- usage: included in the executor run of task-441
- runs: 1
- milestone: M2
- started: 2026-11-05T20:02:13Z
- waiting_for: none
- result: Burst in URRLevelUpComponent: PlayLevelUpBurst once per moment (BurstMinInterval 0.7 s), spawns LevelUpTextSystem through the Niagara pool at capsule top + 40 uu; no damage number subsystem, so the toggle does not hide it. Build ok; RogueRigs.LevelUp 5 of 5 including the new BurstOncePerMoment.

## Relations
- [[task-441-level-up-effect]] — parent global task.
