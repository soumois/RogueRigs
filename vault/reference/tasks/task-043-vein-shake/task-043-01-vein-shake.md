---
id: task-043-01-vein-shake
description: "All blocks of a vein being mined shake together."
parent: task-043-vein-shake
kind: task
status: done
---
# Whole-vein shake

- goal: All blocks of a vein being mined shake together.
- agent: cpp
- depends_on: none
- inputs: heal-ore (vault/reference/gdd/terrain/heal-ore.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), sys-grid (vault/reference/tech/systems/sys-grid.md), sys-drone (vault/reference/tech/systems/sys-drone.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: while a vein is dug (ARRDrone whole-vein dig, ARRArenaGrid::GetVeinCells), every block of the vein gets the dig shake, not only the arc blocks; stop all on release or break; update only existing tests that break; docs sys-drone, sys-grid.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 40, cache write 33999, cache read 542040, output 9745
- runs: 1
- milestone: M2
- result: ARRDrone shakes every block of the dug veins (ShakeCells, RefreshDigShake, CollectDigTargets); laser and dig events still arc-only; stops on release or break. Build 0, 195/195. Commit 0d8b2d9.

## Relations
- [[task-043-vein-shake]] — parent global task.
