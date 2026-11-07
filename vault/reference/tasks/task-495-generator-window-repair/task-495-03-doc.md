---
id: task-495-03-doc
description: "Doc: Document the repair pass and the new acceptance check."
parent: task-495-generator-window-repair
kind: task
status: todo
---
# Repair pass so every generated layout meets the 24 x 24 window rule: doc

- goal: Document the repair pass and the new acceptance check.
- agent: doc
- depends_on: task-495-02-ld-review
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md)
- expected_output: see done_when
- done_when: sys-layout-generator describes the repair pass, the window check and the new yield (gotcha on 1 in 6 replaced); layout-generator-recipe and layout-review-rules record the level-design verdict of this task.
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
