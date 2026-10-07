---
id: task-047-01-heavy-speed
description: "Heavy robot speed 180 → 153 uu/s; the other robots stay at 180."
parent: task-047-heavy-speed
kind: task
status: done
---
# Slower heavy

- goal: Heavy robot speed 180 → 153 uu/s; the other robots stay at 180.
- agent: cpp
- depends_on: none
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), sys-enemies (vault/reference/tech/systems/sys-enemies/sys-enemies.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: EnemyStats.csv Heavy MoveSpeed 180 → 153; update only existing tests this breaks; docs sys-enemies.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 26, cache write 39995, cache read 191008, output 4459
- runs: 1
- milestone: M2
- result: EnemyStats.csv Heavy MoveSpeed 180 → 153; Heavy.StatsFromCSV updated. Build 0, 195/195. Commit 6c95137.

## Relations
- [[task-047-heavy-speed]] — parent global task.
