---
id: task-041-01-front-detour
description: "In the front sector the spawn detour filter is off, so robots spawn beyond the rock the miner digs toward."
parent: task-041-front-detour
kind: task
status: done
---
# Spawns beyond the rock ahead

- goal: In the front sector the spawn detour filter is off, so robots spawn beyond the rock the miner digs toward.
- agent: cpp
- depends_on: none
- inputs: enemies-index (vault/reference/gdd/enemies/enemies-index.md), sys-spawner (vault/reference/tech/systems/sys-spawner.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: ARREnemySpawner: MaxPathDetourRatio applies only to side and back sector cells (and when the miner is still); front sector cells are accepted whatever their walking detour (still reachable); update only existing spawner tests that break; docs sys-spawner.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 48, cache write 33865, cache read 589460, output 7574
- runs: 2
- milestone: M2
- result: Front-sector cells exempt from MaxPathDetourRatio while running (side, back and still unchanged); cause confirmed by reading: cells behind rock exceeded the 1.5 ratio. Limit: rock thicker than the 100–500 uu band leaves no floor ahead to spawn on. Build 0, 195/195. Commit 6a1f3bd.

## Relations
- [[task-041-front-detour]] — parent global task.
