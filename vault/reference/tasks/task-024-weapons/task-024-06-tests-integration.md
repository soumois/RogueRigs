---
id: task-024-06-tests-integration
description: "Integration tests with the final Blueprints: weapon card equips the shotgun, pellets pierce, turret placed by the input action fires."
parent: task-024-weapons
kind: task
status: todo
---
# Weapons integration tests

- goal: the weapons rework is covered with the real Blueprints.
- agent: cpp
- depends_on: task-024-05-bp-weapons
- inputs: [[sys-automation-tests]], task-024-05 result.
- expected_output: `RogueRigs.Integration.*` tests: start loadout rifle only; level-up with a forced weapon card equips BP_Shotgun and its cards appear; pellets pierce a line of robots; PlaceTurret action places BP_Turret that kills a robot; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-024-weapons]] — parent global task.
