---
id: task-460-02-doc
description: "Doc: Record the letter and mark layering."
parent: task-460-alert-letters-readable
kind: task
status: done
---
# Alert bar letters never hidden by a horde mark: doc

- goal: Record the letter and mark layering.
- agent: doc
- depends_on: task-460-01-alert-bar
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), blueprints-ui-index (vault/reference/tech/blueprints/ui/blueprints-ui-index.md)
- expected_output: see done_when
- done_when: The node states the current layering; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-460
- usage: included in the executor run of task-460
- runs: 1
- milestone: M2
- started: 2026-11-06T04:20:45Z
- waiting_for: none
- result: Commit bbfd441c. wbp-alert-objectives updated: components (letters z 4 in front of marks z 3), layout table (marks Top 48, letters z 4), layering paragraph citing Q36, verified entry for task-460-01. Blueprints-ui-index gloss unchanged (still accurate). Lint clean.

## Relations
- [[task-460-alert-letters-readable]] — parent global task.
