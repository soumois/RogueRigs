---
id: task-333-03-docs
description: "Update bp-space-background-material with the retuned MI_Space_Nebula values and the new capture finding."
parent: task-333-nebula-visible
kind: task
status: cancelled
---
# Nebula: docs

- goal: the doc node of the space material gives the current nebula values, their history and what the new capture shows.
- agent: doc
- depends_on: task-333-01-nebula-tune, task-333-02-capture
- inputs: bp-space-background-material (vault/reference/tech/blueprints/world/bp-space-background-material.md, to update: Nebula column of the parameter table with the previous value kept as revert, `tuning`, `look_rule` if N2 changed, Finding section with the task-333-02 numbers, `not_verified`); the global `request` and `gdd_check` for decided values (vault/reference/tasks/task-333-nebula-visible.md); the task-333-01 change summary and commit (run `git show`, the diff is not pasted); the task-333-02 result.
- expected_output: the updated node (under ~400 words; split the history into a child only if it overflows), Relations on both ends with [[task-333-nebula-visible]]. No other node.
- done_when: the node matches the commit values; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-333-nebula-visible]] — parent global task.
