---
id: task-492-04-doc
description: "Doc: Document the C++ generator and its entry point."
parent: task-492-cpp-layout-generator
kind: task
status: todo
---
# C++ layout generator writing the grid JSON, validated by the Level-Design agent: doc

- goal: Document the C++ generator and its entry point.
- agent: doc
- depends_on: task-492-03-ld-validation
- inputs: layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: A sys node for the generator exists (params, determinism, entry point, output), layout-generator-recipe points to it and records the Level-Design verdict; lint clean; committed.
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
