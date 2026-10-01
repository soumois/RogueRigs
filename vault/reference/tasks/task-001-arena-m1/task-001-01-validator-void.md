---
id: task-001-01-validator-void
description: "Add the void cell type to the layout validator and schema: coverage over asteroid cells, floor reachability from the spawn."
parent: task-001-arena-m1
kind: task
status: in-progress
---
# Validator: void cells and reachability

- goal: the layout format can describe an irregular asteroid, and the validator checks it.
- agent: cpp (this subtask edits the Node tool `Tools/validate_layout.js` and the doc node [[level-layout-schema]]; no C++).
- depends_on: none
- inputs: [[level-layout-schema]], [[arena-grid]], `Tools/validate_layout.js`.
- expected_output: (1) validator: new cell type `void` (no floor, no block; a space in the ASCII preview); block coverage = blocks / non-void cells; the 5 × 5 spawn area holds no void and no block; new stat printed in the report: share of floor cells (non-void, non-block) 4-connected to the spawn; the brief accepts `reachable_floor_min` (0 to 1) and fails below it. (2) [[level-layout-schema]] updated: `void` type, new coverage definition, `reachable_floor_min`. No other field or type.
- done_when: a hand-made 12 × 12 layout with void cells passes; the same layout with void inside the spawn area fails; a brief with `reachable_floor_min: 1.0` fails on a layout with one enclosed floor pocket; lint clean. Test files stay outside the repo.
- editor_state: any
- token_ceiling: 30000
- milestone: M1
- result: open

## Relations
- [[task-001-arena-m1]] — parent global task.
