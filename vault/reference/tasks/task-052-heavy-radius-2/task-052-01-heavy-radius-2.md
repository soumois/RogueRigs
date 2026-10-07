---
id: task-052-01-heavy-radius-2
description: "Heavy slam radius 340 → 289 uu (trigger 202 uu, chase stop 145 uu); ground ring follows."
parent: task-052-heavy-radius-2
kind: task
status: in-progress
---
# Heavy slam radius −15 % again

- goal: Heavy slam radius 340 → 289 uu (trigger 202 uu, chase stop 145 uu); ground ring follows.
- agent: cpp
- depends_on: none
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/sys-enemies/sys-enemies-heavy.md), sys-enemies (vault/reference/tech/systems/sys-enemies/sys-enemies.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), bp-heavy (vault/reference/tech/blueprints/bp-heavy.md)
- expected_output: EnemyStats.csv Heavy AttackRange 340 → 289 (trigger 0.7 × and chase stop 0.5 × follow); update only existing heavy tests this breaks (edge test: hit just inside, miss just outside 289); docs sys-enemies-heavy, sys-enemies, and the ring scale line of bp-heavy (289 / 50 = 5.78).
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-052-heavy-radius-2]] — parent global task.
