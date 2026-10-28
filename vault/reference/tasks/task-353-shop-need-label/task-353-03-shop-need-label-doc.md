---
id: task-353-03-shop-need-label-doc
description: "Doc: update wbp-shop-card after the fix of owner point 3."
parent: task-353-shop-need-label
kind: task
status: in-progress
---
# Shop card: NEED N MORE overflows the Buy plate: doc

- goal: Update wbp-shop-card (components, sizes, verified_redesign: remove the fixed defect from the open defects, add what the capture saw) from the commit and change summary of task-353-01-shop-need-label-fix and the report of task-353-02-shop-need-label-capture; decided values in the global `request` and `## Questions`.
- agent: doc
- depends_on: task-353-01-shop-need-label-fix, task-353-02-shop-need-label-capture
- inputs: _authoring (vault/_authoring.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (to update), template `vault/templates/blueprint.md`
- expected_output: the node(s) updated; one commit.
- done_when: nodes match the commit and the capture; no value invented; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T18:03:44Z
- waiting_for: none
- result: open

## Relations
- [[task-353-shop-need-label]] — parent global task.
