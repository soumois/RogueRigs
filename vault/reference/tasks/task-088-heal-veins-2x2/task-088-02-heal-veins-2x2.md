---
id: task-088-02-heal-veins-2x2
description: "Heal veins are mined as one 2 x 2 deposit with 15 HP per pickup (60 HP per vein), driven by the data."
parent: task-088-heal-veins-2x2
kind: task
status: done
---
# Heal veins 2 x 2 in code

- goal: Heal veins are mined as one 2 x 2 deposit with 15 HP per pickup (60 HP per vein), driven by the data.
- agent: cpp
- depends_on: task-070-01-ore-in-rock, task-088-01-heal-veins-2x2
- inputs: heal-ore (vault/reference/gdd/terrain/heal-ore.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), sys-grid (vault/reference/tech/systems/sys-grid.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-drone (vault/reference/tech/systems/sys-drone.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: also update Tools/layout_gen/arena_fine.js so the generator emits 2 x 2 heal veins identical to the committed arena_m1_fine.json (task-088-01 shrank them by a script that was not committed: each 2 x 2 keeps the top-left corner of the former 3 x 3, freed cells become rock; verify by regenerating and diffing). Then: use the deposit-size mechanism built in task-070-01 for heal ore: a 2 x 2 heal vein is one deposit (touch one block, the 4 break together after 1.5 s, shake together, 4 heal pickups at 15 HP each); the heal amount per pickup becomes 15 (data, not a literal); tests updated for 2 x 2 (existing expectations a requested change breaks only) and one new test on a 2 x 2 heal vein; every heal pickup stays collectable; docs sys-grid, sys-pickups, sys-automation-tests; no other gameplay change.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 106, cache write 226886, cache read 3936874, output 38832
- runs: 2
- milestone: M2
- result: Heal 15 HP per pickup from OreTypes.csv (HealValue), 2 x 2 veins mined as one, 209/209 tests, generator arena_fine.js regenerates arena_m1_fine.json byte for byte. Commits 8e902b2, 1831dc7, 5a72a77.

## Relations
- [[task-088-heal-veins-2x2]] — parent global task.
