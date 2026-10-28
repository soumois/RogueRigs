---
id: task-352-03-shop-continue-fit-doc
description: "Doc: update wbp-shop-screen after the fix of owner point 2."
parent: task-352-shop-continue-fit
kind: task
status: in-progress
---
# Shop screen: Continue plate cut at the bottom: doc

- goal: Update wbp-shop-screen (components, sizes, verified_redesign: remove the fixed defect from the open defects, add what the capture saw) from the commit and change summary of task-352-01-shop-continue-fit-fix and the report of task-352-02-shop-continue-fit-capture; decided values in the global `request` and `## Questions`.
- agent: doc
- depends_on: task-352-01-shop-continue-fit-fix, task-352-02-shop-continue-fit-capture
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
- started: 2026-10-28T17:45:47Z
- waiting_for: none
- result: open

## Relations
- [[task-352-shop-continue-fit]] — parent global task.
