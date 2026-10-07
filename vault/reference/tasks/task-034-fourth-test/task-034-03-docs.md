---
id: task-034-03-docs
description: "Docs for the committed task-034-01 changes: projectiles, veins, heal pickup, capsules, separation, kamikaze, recycling, turret; one full headless run."
parent: task-034-fourth-test
kind: task
status: in-progress
---
# Docs for task-034-01

- goal: the vault describes the code committed in task-034-01 (03ac10a, 659b75b).
- agent: cpp
- depends_on: task-034-01-cpp-fixes
- inputs: task-034-01 result, [[sys-projectiles]], [[sys-grid]], [[sys-drone]], [[sys-pickups]], [[sys-enemies]], [[sys-spawner]], [[sys-turret]], [[sys-automation-tests]]
- expected_output: update sys-projectiles, sys-grid, sys-drone, sys-pickups, sys-enemies-base, sys-enemies-kamikaze, sys-enemies, sys-spawner, sys-turret, sys-automation-tests to the committed code (read the diffs of 03ac10a and 659b75b); no code change; one full headless run to confirm the suite passes after task-034-01.
- done_when: docs updated; lint clean; full headless run result reported.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-034-fourth-test]] — parent global task.
