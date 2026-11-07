---
id: task-490-01-heal-value
description: "Cpp: Set HealValue of the Heal row to 30 and check one pickup heals 30 HP in game."
parent: task-490-heal-pickup-30hp
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-490
- usage: included in the executor run of task-490
- runs: 1
- milestone: M2
- started: 2026-11-07T06:06:33Z
- waiting_for: none
- result: A1 OreTypes.csv row Heal HealValue = 30 (commit 2fc8f9e0), read back by the game grid loader in RogueRigs.Ore.HealVein (pickups worth the row value). A2 headless game world: HealFullHealthWaits 70 -> 100 with one pickup (30, capped at max 100), HealVein 10 -> 130 of 200 with the 4 vein pickups (4 x 30). A3 HealFullHealthWaits (no pickup at full health), HealPartialClamp, PickupPool.OreAndHealValuesReset pass; drop and dig code untouched; HealVein test updated (max 200, expects 10 + 4 x HealValue). Build OK, 4 tests pass; no -game run (data value only, light level).

## Relations
- [[task-490-heal-pickup-30hp]] — parent global task.
