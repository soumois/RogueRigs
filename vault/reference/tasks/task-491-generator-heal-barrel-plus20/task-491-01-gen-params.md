---
id: task-491-01-gen-params
description: "Cpp: Raise heal vein and barrel counts in the Node generator params by 20 % and check two seeds with the validator."
parent: task-491-generator-heal-barrel-plus20
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-491-generator-heal-barrel-plus20]] — parent global task.
