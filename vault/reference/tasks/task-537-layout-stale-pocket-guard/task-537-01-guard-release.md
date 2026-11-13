---
id: task-537-01-guard-release
description: "Cpp: Release the guard of removed hidden pockets in the C++ generator, keep live pockets guarded, revalidate and measure before/after"
parent: task-537-layout-stale-pocket-guard
kind: task
status: in-progress
---
# Layout generator: the guard of a removed hidden pocket no longer blocks carving: cpp

- goal: Release the guard of removed hidden pockets in the C++ generator, keep live pockets guarded, revalidate and measure before/after
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md), task-536-layout-filler-cheaper (vault/reference/tasks/task-536-layout-filler-cheaper.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md)
- expected_output: Before measures taken on the current code first (A3 table "before", A4, A5), then the change, then the same measures after; numbers written in the subtask result.
- done_when: A1 to A6 of the global hold; the before/after table (24 starts), the reference line and the seed 17 / 422704496 filler measures with the cause verdict are written in the result. Per Q3, a start with no passing seed stops the task in wait-answers.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T19:32:29Z
- waiting_for: none
- result: open

## Relations
- [[task-537-layout-stale-pocket-guard]] — parent global task.
