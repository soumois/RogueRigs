---
id: task-355-03-shop-card-frame-details-doc
description: "Doc: update wbp-shop-card, wbp-shop-screen after the fix of owner point 5."
parent: task-355-shop-card-frame-details
kind: task
status: todo
---
# Shop card: second frame line, cramped tier tab, sold panel fit: doc

- goal: Update wbp-shop-card, wbp-shop-screen (components, anchors of the sold panel in the anchor rule field written by task-356-04, verified_redesign: remove the fixed defect from the open defects, add what the capture saw) from the commit and change summary of task-355-01-shop-card-frame-details-fix and the report of task-355-02-shop-card-frame-details-capture; decided values in the global `request` and `## Questions`.
- agent: doc
- depends_on: task-355-01-shop-card-frame-details-fix, task-355-02-shop-card-frame-details-capture
- inputs: _authoring (vault/_authoring.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md) (to update), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (to update), template `vault/templates/blueprint.md`
- expected_output: the node(s) updated; one commit.
- done_when: nodes match the commit and the capture; no value invented; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-355-shop-card-frame-details]] — parent global task.
