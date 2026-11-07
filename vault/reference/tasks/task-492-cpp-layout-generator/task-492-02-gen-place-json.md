---
id: task-492-02-gen-place-json
description: "Cpp: C++ generator, second slice: heal veins, barrels, spawn, recipe seed acceptance, JSON writer and an entry point usable outside a play session."
parent: task-492-cpp-layout-generator
kind: task
status: in-progress
---
# C++ layout generator writing the grid JSON, validated by the Level-Design agent: cpp

- goal: C++ generator, second slice: heal veins, barrels, spawn, recipe seed acceptance, JSON writer and an entry point usable outside a play session.
- agent: cpp
- depends_on: task-492-01-gen-carve
- inputs: layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), barrel (vault/reference/gdd/terrain/barrel.md), Tools/validate_layout.js
- expected_output: see done_when
- done_when: A1 to A3 and A5 of the global hold on at least two seeds; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T06:22:51Z
- waiting_for: none
- result: open

## Relations
- [[task-492-cpp-layout-generator]] — parent global task.
