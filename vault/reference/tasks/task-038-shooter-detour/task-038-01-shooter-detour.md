---
id: task-038-01-shooter-detour
description: "Shooter without line of sight walks around rock like the other robots until it can fire, never moving away from the miner."
parent: task-038-shooter-detour
kind: task
status: in-progress
---
# Shooter detour without line of sight

- goal: Shooter without line of sight walks around rock like the other robots until it can fire, never moving away from the miner.
- agent: cpp
- depends_on: none
- inputs: enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md), sys-enemies-shooter (vault/reference/tech/systems/sys-enemies/sys-enemies-shooter.md), sys-flow-field (vault/reference/tech/systems/sys-flow-field.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: ARRShooter: when in range but without line of sight, follow the flow field (as chasers do) until line of sight is regained, then hold and fire; never increase distance to the miner; update only existing shooter tests that break; docs sys-enemies-shooter.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-038-shooter-detour]] — parent global task.
