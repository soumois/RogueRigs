---
id: task-550-02-doc
description: "Doc: Make the generator docs say the pre-537 guard behaviour, the reference hash and timings, and that task-537 was undone"
parent: task-550-layout-generator-back-pre-537
kind: task
status: in-progress
---
# Générateur de layout : retour aux cartes d'avant task-537 (garde des poches): doc

- goal: Make the generator docs say the pre-537 guard behaviour, the reference hash and timings, and that task-537 was undone
- agent: doc
- depends_on: task-550-01-generator-back
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), task-539-ld-review-pocket-guard-maps (vault/reference/tasks/task-539-ld-review-pocket-guard-maps.md)
- expected_output: see done_when
- done_when: A5 of the global holds: the three nodes describe the behaviour after this task, reference 86c36f6d with seed 422704499, the timings of subtask 1, a dated line that task-537 was undone on owner request, and the task-539 line says its verdicts concern maps no longer produced; lint clean, committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-13T23:58:18Z
- waiting_for: none
- result: open

## Relations
- [[task-550-layout-generator-back-pre-537]] — parent global task.
