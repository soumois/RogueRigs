---
id: task-463-01-aim-assist
description: "Cpp: Burst rifle bullets get the light aim assist decided in the global node; other shots unchanged."
parent: task-463-aim-assist
kind: task
status: todo
---
# Light aim assist on miner shots: cpp

- goal: Burst rifle bullets get the light aim assist decided in the global node; other shots unchanged.
- agent: cpp
- depends_on: none
- inputs: miner (vault/reference/gdd/player/miner.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), barrel (vault/reference/gdd/terrain/barrel.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), sys-auto-fire (vault/reference/tech/systems/combat/sys-auto-fire.md)
- expected_output: see done_when
- done_when: Build ok; in PIE or a test: a near-miss bullet is deflected at most 5 degrees onto a robot, a bullet already on a robot or a barrel is not, a robot behind rock or outside the cone is not targeted; the angle is read from data; weapon test families pass, figures in the result.
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
- [[task-463-aim-assist]] — parent global task.
