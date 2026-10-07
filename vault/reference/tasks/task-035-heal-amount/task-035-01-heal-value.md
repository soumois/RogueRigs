---
id: task-035-01-heal-value
description: "HealAmount of heal pickups set so a 9-pickup heal vein restores 60 HP."
parent: task-035-heal-amount
kind: task
status: done
---
# Heal value

- goal: heal pickups heal.
- agent: cpp
- depends_on: none
- inputs: heal-ore (vault/reference/gdd/terrain/heal-ore.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: ARRPickup heal HealAmount = 60 / 9 (≈ 6.667) as the heal pickup default (C++ default for heal pickups; check BP_PickupHeal does not override it on disk); update only existing tests that break; docs sys-pickups.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 28, cache write 39801, cache read 486927, output 6093
- runs: 1
- milestone: M2
- result: ARRPickup HealAmount default 60 / 9 (≈ 6.667); BP_PickupHeal does not override it; no test change needed. Build 0, 195/195. Commit 251b6fc.

## Relations
- [[task-035-heal-amount]] — parent global task.
