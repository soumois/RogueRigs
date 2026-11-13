---
id: task-550-01-generator-back
description: "Cpp: Bring the C++ generator back to its pre-537 maps (task-536 kept), check the old hashes and the 24 starts, report timings"
parent: task-550-layout-generator-back-pre-537
kind: task
status: in-progress
---
# Générateur de layout : retour aux cartes d'avant task-537 (garde des poches): cpp

- goal: Bring the C++ generator back to its pre-537 maps (task-536 kept), check the old hashes and the 24 starts, report timings
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-537-layout-stale-pocket-guard (vault/reference/tasks/task-537-layout-stale-pocket-guard.md), task-537-01-guard-release (vault/reference/tasks/task-537-layout-stale-pocket-guard/task-537-01-guard-release.md), task-536-layout-filler-cheaper (vault/reference/tasks/task-536-layout-filler-cheaper.md), task-495-02-ld-review (vault/reference/tasks/task-495-generator-window-repair/task-495-02-ld-review.md), task-492-03-ld-validation (vault/reference/tasks/task-492-cpp-layout-generator/task-492-03-ld-validation.md)
- expected_output: see done_when
- done_when: A1 to A4 of the global hold: pre-537 behaviour back with task-536 kept, proved by the pre-537 `-carveonly` hashes and reference line 422704499 / 86c36f6d (two runs; a differing hash is reported with its cause, nothing tuned to compensate), 24 starts with the pre-537 accepted seeds and validator OK in a table, timings stated, tests pass, Node script, committed layouts and params unchanged; code committed by path.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T23:48:25Z
- waiting_for: none
- result: open

## Relations
- [[task-550-layout-generator-back-pre-537]] — parent global task.
