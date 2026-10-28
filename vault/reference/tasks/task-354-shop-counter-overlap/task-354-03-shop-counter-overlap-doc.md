---
id: task-354-03-shop-counter-overlap-doc
description: "Doc: update wbp-shop-screen after the fix of owner point 4."
parent: task-354-shop-counter-overlap
kind: task
status: todo
---
# Shop screen: crystal counter covers the shop panel corner: doc

- goal: Update wbp-shop-screen (components, sizes, verified_redesign: remove the fixed defect from the open defects, add what the capture saw) from the commit and change summary of task-354-01-shop-counter-overlap-fix and the report of task-354-02-shop-counter-overlap-capture; decided values in the global `request` and `## Questions`.
- agent: doc
- depends_on: task-354-01-shop-counter-overlap-fix, task-354-02-shop-counter-overlap-capture
- inputs: _authoring (vault/_authoring.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md) (to update), template `vault/templates/blueprint.md`
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
- [[task-354-shop-counter-overlap]] — parent global task.
