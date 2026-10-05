---
id: task-029-03-layout-67
description: "L_ArenaM1 layout converted to 240 × 240 cells with organic edges on rock masses, passages at least 3 cells wide, validator updated and passing."
parent: task-029-fine-grid
kind: task
status: todo
---
# Layout at 67 uu

- goal: the M1 arena exists at fine resolution.
- agent: level-design
- depends_on: task-029-02-flow-field-incremental
- inputs: [[arena-grid]], [[level-layout-schema]], [[layout-generator-recipe]], current arena_m1 layout JSON, [[drgs-analysis]]
- expected_output: new layout file for 240 × 240 (each former cell becomes 3 × 3, then rock mass borders roughened by a deterministic rule so edges are irregular, ore and heal veins kept as clusters, barrels as single cells near their former position, spawn kept), passages ≥ 3 cells, validator extended for the size and the minimum passage width and passing; the old 80 × 80 layout kept for tests; level-layout-schema and arena-grid notes updated.
- done_when: validator passes on the new layout; lint clean; committed.
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
