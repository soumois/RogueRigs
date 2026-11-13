---
id: task-537-02-doc
description: "Doc: Record the guard release, the new reference line and the measures in the generator docs"
parent: task-537-layout-stale-pocket-guard
kind: task
status: in-progress
---
# Layout generator: the guard of a removed hidden pocket no longer blocks carving: doc

- goal: Record the guard release, the new reference line and the measures in the generator docs
- agent: doc
- depends_on: task-537-01-guard-release
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md)
- expected_output: see done_when
- done_when: sys-layout-generator states the guard rule (live pockets only), the reference determinism line as measured in 01, the revalidation result and the idle filler before/after with the cause verdict; layout-generator-recipe notes that the C++ generator releases a removed pocket's guard and the Node script does not (per Q1); layout-review-rules gets the 24-start result if it changed; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T19:54:45Z
- waiting_for: none
- result: open

## Relations
- [[task-537-layout-stale-pocket-guard]] — parent global task.
