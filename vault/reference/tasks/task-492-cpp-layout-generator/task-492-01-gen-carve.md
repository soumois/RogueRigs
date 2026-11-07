---
id: task-492-01-gen-carve
description: "Cpp: C++ generator, first slice: outline, spawn clearing, main network, hidden pockets, filler, cleanup and 3-cell passage repair on 240 x 240 from data params."
parent: task-492-cpp-layout-generator
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-492
- usage: included in the executor run of task-492
- runs: 1
- milestone: M2
- started: 2026-11-07T06:14:54Z
- waiting_for: none
- result: Carving steps ported to C++ (Source/RogueRigs/Grid/RRLayoutGenerator, RRLayoutGenParams reads every value of Tools/layout_gen/params/fine_default.json, FRandomStream per seed), commit 85a2ee48. Check via commandlet -run=RRLayoutGen -carveonly: seeds 3 and 100 carve in 0.19 s and 0.62 s, identical grid on rerun, 0 narrow cells (minimum passage 3), asteroid 33628 and 33600 cells. Rock islands use a full component flood (the Node early-stop could floor rock fragments). Build only (verify_scope: new tool code, no gameplay change).

## Relations
- [[task-492-cpp-layout-generator]] — parent global task.
