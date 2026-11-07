---
id: task-490-01-heal-value
description: "Cpp: Set HealValue of the Heal row to 30 and check one pickup heals 30 HP in game."
parent: task-490-heal-pickup-30hp
kind: task
status: todo
---
# Each heal pickup restores 30 HP: cpp

- goal: Set HealValue of the Heal row to 30 and check one pickup heals 30 HP in game.
- agent: cpp
- depends_on: none
- inputs: heal-ore (vault/reference/gdd/terrain/heal-ore.md)
- expected_output: see done_when
- done_when: A1 to A3 of the global hold; committed.
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
- [[task-490-heal-pickup-30hp]] — parent global task.
