---
id: task-029-04-switch-67
description: "Cell size 67 uu by default with the new layout; every value expressed in cells rescaled; tests updated."
parent: task-029-fine-grid
kind: task
status: done
---
# Switch the game to 67 uu

- goal: the game runs on the fine grid.
- agent: cpp
- depends_on: task-029-03-layout-67
- inputs: [[arena-grid]], [[sys-grid]], [[sys-spawner]], [[sys-objectives]], [[sys-explosions]], [[sys-drone]], [[sys-pickups]], [[objective-console]], [[extraction]], [[ore-blocks]], [[cpp-standard]], [[sys-automation-tests]], [[stress-grid-cells]], [[sys-stress]]
- expected_output: grid cell size 67 uu and the 240 × 240 layout loaded on L_ArenaM1 (map asset changes the bp agent must make listed precisely if any); values in cells converted to keep world distances (spawn band, console 12–20 → 36–60 cells, marked radius 2 → 6, zone 3 → 9, shuttle ≥ 25 → ≥ 75, ViewMargin and MaxSpawnDistanceCells ×3), explosion radii unchanged in uu, ore drops per broken block per GDD; tests converted deterministically (layouts in tests may keep 200 uu where the test is about logic, say which); docs.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 140000
- model: opus
- tokens: input 198, cache write 338784, cache read 11663374, output 43846
- runs: 2
- milestone: M2
- result: CellSize 67, layout arena_m1_fine; dig health per GDD (rock 0.5 s, ore/heal 1.5 s, barrel 0.5 s by agent choice); spawner ViewMargin 3, MaxSpawnDistanceCells 18; console 36–60 cells, mark 6, zone 9; shuttle ≥ 75, board radius 3; DigCancelDistanceCells 4.5; shooter reposition radius 18; DestroyDropCount 1 (one chunk per block, provisional pending owner on ore-blocks conflict); explosion radii unchanged in uu. Cell-counted tests moved to the fine grid, logic tests keep arena_m1 at 200 uu. Build 0, 154/154. One non-reproduced 'pure virtual function' crash in Kamikaze.KilledExplodesAndDropsXP during a first full run: watch in 029-07. No map change needed. Commits faa3ef5, 8dc02be, 8b83b59.

## Relations
- [[task-029-fine-grid]] — parent global task.
