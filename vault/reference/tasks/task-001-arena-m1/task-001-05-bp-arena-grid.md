---
id: task-001-05-bp-arena-grid
description: "Create BP_ArenaGrid from ARRArenaGrid with layout arena_m1, the engine cube and the grey-box materials, and document it."
parent: task-001-arena-m1
kind: task
status: in-progress
---
# BP_ArenaGrid

- goal: the data-only Blueprint that turns the C++ grid into the M1 grey box.
- agent: bp
- depends_on: task-001-02-layout-arena-m1, task-001-03-arena-grid-cpp, task-001-04-greybox-materials
- inputs: [[sys-grid]], [[cpp-bp-split]], [[blueprints-index]], [[unreal-mcp]].
- expected_output: `/Game/RogueRigs/Blueprints/Terrain/BP_ArenaGrid`, parent `ARRArenaGrid`, no graph logic. Defaults: `LayoutName` = `arena_m1`; every type's mesh = `/Engine/BasicShapes/Cube`; materials floor → `MI_GB_Floor`, rock → `MI_GB_Rock`, heal_ore → `MI_GB_HealOre`, ore → `MI_GB_Ore`, special_ore → `MI_GB_SpecialOre`; other values at their C++ defaults. Doc node `bp-arena-grid` in `tech/blueprints/` (template `blueprint.md`), linked from [[blueprints-index]].
- done_when: the asset is saved and compiles without error; dropped in an empty level, it shows the asteroid; the doc node matches; lint clean.
- editor_state: open
- token_ceiling: 40000
- milestone: M1
- result: open

## Relations
- [[task-001-arena-m1]] — parent global task.
