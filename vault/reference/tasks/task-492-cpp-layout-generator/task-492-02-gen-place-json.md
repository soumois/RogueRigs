---
id: task-492-02-gen-place-json
description: "Cpp: C++ generator, second slice: heal veins, barrels, spawn, recipe seed acceptance, JSON writer and an entry point usable outside a play session."
parent: task-492-cpp-layout-generator
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-492
- usage: included in the executor run of task-492
- runs: 1
- milestone: M2
- started: 2026-11-07T06:22:51Z
- waiting_for: none
- result: Commit 43c51ac8. Entry point: UnrealEditor-Cmd.exe RogueRigs.uproject -run=RRLayoutGen -seed=<n> [-params=<file>] [-name=<snake_case>] [-out=<dir>] [-brief=<file>] (defaults: Tools/layout_gen/params/fine_default.json, Saved/GeneratedLevels); tries seeds upward (search.max_seeds) until the recipe checks and the brief pass, writes nothing when none passes. Brief 240 x 240, coverage 0.52-0.62, heal_ore 56, barrel 17-22, reach min 0.6, passage 3: start 3 -> seed 14 accepted (0.12 s, search 2.05 s, 12 seeds), start 100 -> seed 100 (0.63 s); both pass validate_layout.js with the brief (passage 3); two runs of start 3 give byte-identical JSON. Pass rate on seeds 3-14 is 1/12 in C++, 3/12 for the Node generator (same failing checks: far cell, reach above 0.86, decoration quotas). Build only (verify_scope).

## Relations
- [[task-492-cpp-layout-generator]] — parent global task.
