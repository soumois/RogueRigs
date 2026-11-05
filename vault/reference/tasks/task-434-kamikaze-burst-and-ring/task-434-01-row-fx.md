---
id: task-434-01-row-fx
description: "Bp: The Kamikaze row plays the burst and the ring."
parent: task-434-kamikaze-burst-and-ring
kind: task
status: in-progress
---
# Kamikaze explosion plays the barrel burst and ring: bp

- goal: The Kamikaze row plays the burst and the ring.
- agent: bp
- depends_on: none
- inputs: bp-explosion-fx-barrel (vault/reference/tech/blueprints/fx/bp-explosion-fx-barrel.md), bp-barrel-explosion (vault/reference/tech/blueprints/fx/bp-barrel-explosion.md), bp-explosion-shockwave (vault/reference/tech/blueprints/fx/bp-explosion-shockwave.md), sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), task-431-barrel-green-square-packaged (vault/reference/tasks/task-431-barrel-green-square-packaged.md), Content/RogueRigs/Data/Tables/ExplosionStats.csv
- expected_output: see done_when
- done_when: The Kamikaze row resolves to a class whose burst and ring defaults are read back as set, the Barrel row is unchanged, no other column of either row changed, existing explosion tests pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T16:24:11Z
- waiting_for: none
- result: open

## Relations
- [[task-434-kamikaze-burst-and-ring]] — parent global task.
