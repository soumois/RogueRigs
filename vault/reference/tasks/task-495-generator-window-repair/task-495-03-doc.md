---
id: task-495-03-doc
description: "Doc: document the repair passes and the new acceptance checks of the C++ layout generator."
parent: task-495-generator-window-repair
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-495
- usage: included in the executor run of task-495
- runs: 1
- milestone: M2
- started: 2026-11-07T08:08:50Z
- waiting_for: none
- result: Commit 5ab27305. sys-layout-generator: review_repairs entry (passes, dead-window filler walk, outcrop or wall bump, lane bump or niche, pocket distance checked only), params review section, the three acceptance checks and the new yield (24 of 24 starts, 22 on the same first seed, reach caveat) replacing the not-automated gotcha. layout-review-rules: intro and lane line updated, Automation section says what is enforced and gives the task-495-02 verdict. layout-generator-recipe: C++ port section records the verdict. Lint clean. Global over the standard budget (see the ALERT causes of 01 and 02).

## Relations
- [[task-495-generator-window-repair]] — parent global task.
