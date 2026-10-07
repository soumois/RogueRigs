---
id: task-051-01-turret-spacing
description: "A turret placed less than 100 uu from a standing turret moves to the nearest free floor cell at least 100 uu away."
parent: task-051-turret-spacing
kind: task
status: done
---
# Turret spacing

- goal: A turret placed less than 100 uu from a standing turret moves to the nearest free floor cell at least 100 uu away.
- agent: cpp
- depends_on: none
- inputs: turret (vault/reference/gdd/weapons/turret.md), sys-turret (vault/reference/tech/systems/sys-turret.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: ARRTurretLauncher placement: if the aimed cell is within TurretMinSpacing (100 uu, editable) of a standing turret, pick the nearest reachable floor cell at least that far from every standing turret within the placement reach; if none, place where aimed as today; update only existing turret tests this breaks; docs sys-turret.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 36, cache write 49950, cache read 613117, output 10490
- runs: 1
- milestone: M2
- result: ARRTurretLauncher::FindSpacedCell: an aimed cell within TurretMinSpacing (100 uu, editable, 0 off) of a standing turret is replaced by the nearest visible floor cell within the 400 uu reach that is at least 100 uu from every turret; otherwise placed as aimed. No test changed. Build 0, 195/195. Commit cbe2a67.

## Relations
- [[task-051-turret-spacing]] — parent global task.
