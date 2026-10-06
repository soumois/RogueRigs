---
id: task-029-04-switch-67
description: "Cell size 67 uu by default with the new layout; every value expressed in cells rescaled; tests updated."
parent: task-029-fine-grid
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
