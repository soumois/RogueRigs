---
id: task-495-03-doc
description: "Doc: document the repair passes and the new acceptance checks of the C++ layout generator."
parent: task-495-generator-window-repair
kind: task
status: todo
---
# C++ generator enforces the layout review rules: doc

- goal: Document the repair passes and the new acceptance checks.
- agent: doc
- depends_on: task-495-02-ld-review
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md)
- expected_output: see done_when
- done_when: sys-layout-generator describes the repair passes, the three acceptance checks and the new yield (gotcha on review rules not automated replaced); layout-review-rules says which rules the generator now enforces (Automation section) and both it and layout-generator-recipe record the level-design verdict of this task.
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
- [[task-495-generator-window-repair]] — parent global task.
