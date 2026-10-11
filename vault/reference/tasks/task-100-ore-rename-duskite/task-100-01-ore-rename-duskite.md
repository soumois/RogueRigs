---
id: task-100-01-ore-rename-duskite
description: "The first ore is renamed Duskite in data, docs and tests and keeps its existing violet look (no colour written)."
parent: task-100-ore-rename-duskite
kind: task
status: done
---
# Ore renamed Duskite, violet kept

- goal: The first ore is renamed Duskite in data, docs and tests and keeps its existing violet look (no colour written).
- agent: cpp
- depends_on: task-095-01-xp-cube-scale
- inputs: ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), color-code (vault/reference/gdd/art-audio/color-code.md), sys-grid (vault/reference/tech/systems/sys-grid.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner decision 2026-10-10: the look stays violet (the existing crystals), the mineral gets a name apart from the gold colour: Duskite (provisional). In Content/RogueRigs/Data/Tables/OreTypes.csv rename the row Gold to Duskite and set its Color so that nothing is written (alpha 0, the mechanism task-088-02 added for Heal), so the grid no longer pushes the amber colour into the instance data; update every code reference, test name or comment that says gold (grep Gold/gold in Source/RogueRigs, Tests and the docs sys-grid, sys-pickups, sys-automation-tests, ore-in-rock-plan); no behaviour change (8 %, 2 x 2, value 1, 359 deposits). Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; no gold left in code and docs except historical task results; lint clean; committed.
- editor_state: closed
- token_ceiling: 30000
- model: opus
- tokens: input 42, cache write 66282, cache read 492443, output 9825
- runs: 1
- milestone: M2
- result: Ore row renamed Duskite, colour alpha 0 keeps the violet material; 359 deposits (positions moved: the placement hash includes the ore id); 227/227 tests. Commit e05cf15.

## Relations
- [[task-100-ore-rename-duskite]] — parent global task.
