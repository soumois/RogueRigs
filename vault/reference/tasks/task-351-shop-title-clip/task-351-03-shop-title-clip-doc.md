---
id: task-351-03-shop-title-clip-doc
description: "Doc: update wbp-shop-screen after the fix of owner point 1."
parent: task-351-shop-title-clip
kind: task
status: todo
---
# Shop screen: title clipped at the top: doc

- goal: Update wbp-shop-screen (components, sizes, verified_redesign: remove the fixed defect from the open defects, add what the capture saw) from the commit and change summary of task-351-01-shop-title-clip-fix and the report of task-351-02-shop-title-clip-capture; decided values in the global `request` and `## Questions`.
- agent: doc
- depends_on: task-351-01-shop-title-clip-fix, task-351-02-shop-title-clip-capture
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
- [[task-351-shop-title-clip]] — parent global task.
