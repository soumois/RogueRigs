---
id: task-039-01-heavy-radius
description: "Heavy slam radius 400 → 340 uu (trigger 0.7 × radius = 238 uu); ground ring follows."
parent: task-039-heavy-radius
kind: task
status: todo
---
# Heavy slam radius −15 %

- goal: Heavy slam radius 400 → 340 uu (trigger 0.7 × radius = 238 uu); ground ring follows.
- agent: cpp
- depends_on: none
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/sys-enemies/sys-enemies-heavy.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: EnemyStats.csv / heavy slam radius 400 → 340 uu (trigger stays 0.7 × radius); check BP_Heavy ring uses GetSlamRadius (no asset edit); update only existing heavy tests that break; docs sys-enemies-heavy.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-039-heavy-radius]] — parent global task.
