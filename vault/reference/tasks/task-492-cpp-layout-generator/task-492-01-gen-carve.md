---
id: task-492-01-gen-carve
description: "Cpp: C++ generator, first slice: outline, spawn clearing, main network, hidden pockets, filler, cleanup and 3-cell passage repair on 240 x 240 from data params."
parent: task-492-cpp-layout-generator
kind: task
status: todo
---
# C++ layout generator writing the grid JSON, validated by the Level-Design agent: cpp

- goal: C++ generator, first slice: outline, spawn clearing, main network, hidden pockets, filler, cleanup and 3-cell passage repair on 240 x 240 from data params.
- agent: cpp
- depends_on: none
- inputs: layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), Tools/layout_gen/arena_gen_fine.js, Tools/layout_gen/params/fine_default.json
- expected_output: see done_when
- done_when: The carving steps of the recipe run in C++ from data params with FRandomStream; same params and seed give the same grid; minimum passage 3 holds on the carved grid; builds; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-492-cpp-layout-generator]] — parent global task.
