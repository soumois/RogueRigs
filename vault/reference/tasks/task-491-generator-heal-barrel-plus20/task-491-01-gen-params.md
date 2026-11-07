---
id: task-491-01-gen-params
description: "Cpp: Raise heal vein and barrel counts in the Node generator params by 20 % and check two seeds with the validator."
parent: task-491-generator-heal-barrel-plus20
kind: task
status: done
---
# Generator rules: 20 % more heal veins and barrels: cpp

- goal: Raise heal vein and barrel counts in the Node generator params by 20 % and check two seeds with the validator.
- agent: cpp
- depends_on: none
- inputs: layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), barrel (vault/reference/gdd/terrain/barrel.md), Tools/layout_gen/arena_gen_fine.js, Tools/layout_gen/params/fine_default.json
- expected_output: see done_when
- done_when: A1 to A3 of the global hold; layout-generator-recipe and layout-review-rules state 14 veins (5 pockets, 9 walls) and 17 to 22 barrels; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-491
- usage: included in the executor run of task-491
- runs: 1
- milestone: M2
- started: 2026-11-07T06:13:06Z
- waiting_for: none
- result: Params fine_default.json: heal pocket_veins 4->5, wall_veins 8->9 (14 veins, 56 cells), barrels min/max 14-18 -> 17-22, pairs_min/max 3-4 unchanged (chains kept); code already read the counts from params, no code change. Seeds 3 and 100 with default params and a scratch brief heal_ore [56,56], barrel [17,22]: generator OK, validate_layout.js exit 0, 56 heal_ore, 22 barrels each, min passage 3. Content/RogueRigs/Data/Levels unchanged. layout-generator-recipe and layout-review-rules updated; gdd heal-ore and barrel already stated the counts. Commit 80bc3b4d.

## Relations
- [[task-491-generator-heal-barrel-plus20]] — parent global task.
