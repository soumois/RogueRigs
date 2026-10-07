---
id: task-036-01-relocate
description: "Recycled robots are relocated into the spawn band ahead of the miner (same type and health); seen-once 3 s, never seen 6 s."
parent: task-036-recycle-relocate
kind: task
status: todo
---
# Relocate recycled robots

- goal: robots left behind return ahead of the miner.
- agent: cpp
- depends_on: none
- inputs: enemies-index (vault/reference/gdd/enemies/enemies-index.md), sys-spawner (vault/reference/tech/systems/sys-spawner.md), sys-spawner-director (vault/reference/tech/systems/sys-spawner-director.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: in ARREnemySpawner recycling: a robot off screen and beyond RecycleDistance is relocated (teleported, AI and path state reset) to a spawn band cell in the heading sector draw, keeping type and health, instead of being destroyed; timers: 3 s once it has been on screen, 6 s if never seen (both editable); update only existing tests that break; docs sys-spawner.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-036-recycle-relocate]] — parent global task.
