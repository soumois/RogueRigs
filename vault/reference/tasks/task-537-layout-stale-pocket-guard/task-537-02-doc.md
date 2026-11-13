---
id: task-537-02-doc
description: "Doc: Record the guard release, the new reference line and the measures in the generator docs"
parent: task-537-layout-stale-pocket-guard
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-537
- usage: included in the executor run of task-537
- runs: 1
- milestone: M2
- started: 2026-11-13T19:54:45Z
- waiting_for: none
- result: Done (doc commit c9ade13f). sys-layout-generator: pocket_guard entry (guard only within pockets.guard of a cell still hidden, rebuilt after refill and passage repair), -carveonly pocket/guard line, reference line 422704489 -> 422704499 cells hash 34b074db (was 86c36f6d), 24-start revalidation (24/24, 17 same seed, 7 changed, 10.9 s vs 16.6 s), idle filler before/after with verdict yes, cache note. layout-generator-recipe: C++ releases the guard, Node script does not (Q1). layout-review-rules: 24-start result after the change. Lint clean.

## Relations
- [[task-537-layout-stale-pocket-guard]] — parent global task.
