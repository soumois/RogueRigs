---
id: task-007-05-weapon-tests
description: "Headless C++ automation tests for burst timing, magazine and reload, projectile stop on rock, GAS damage with crits and the dummy refill."
parent: task-007-burst-rifle-dummy
kind: task
status: in-progress
---
# Weapon automation tests

- goal: prove the weapon and damage rules in code, since MCP cannot drive PIE input (see [[task-006-05-dig-tests]]).
- agent: cpp
- depends_on: task-007-04-bp-rifle-dummy
- inputs: [[sys-automation-tests]], [[sys-weapons]], [[sys-auto-fire]], [[sys-projectiles]], [[sys-damage]], `sys-target-dummy`, [[burst-rifle]], [[build-toolchain]], [[cpp-standard]].
- expected_output: tests under `RogueRigs.Weapons.*`, reusing the test-world helper of `Source/RogueRigs/Tests/`: (1) burst timing: with `FireRate` 2, `BurstCount` 3, `BurstDuty` 0.32, `FireOnce` calls at 0, 0.08, 0.16 s then the next burst at 0.5 s (± 1 frame); with `FireRate` 3, gaps 0.053 s and pause 0.227 s; (2) magazine: 30 shots then a 1.5 s reload, `Ammo` back to 30; (3) projectile spawned toward the aim stops on a rock block and is destroyed without damage; (4) projectile hitting a `BP_TargetDummy` lowers `Health` by 10, or 15 with `CritChance` forced to 1; (5) dummy refills to `MaxHealth` 3 s after the last hit and never reaches a dead state. Use the Blueprint classes from task-007-04. Fix any gameplay bug found (report it). Run headless with the command in [[build-toolchain]] and add the new tests to [[sys-automation-tests]].
- done_when: build exits 0; `RogueRigs.Weapons.*` and `RogueRigs.Dig.*` all pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 100000
- milestone: M1
- result: open

## Relations
- [[task-007-burst-rifle-dummy]] — parent global task.
