---
id: task-493-01-loader
description: "Cpp: Asynchronous loader that loads the layout assets and builds the grid over frames, with a 0-100 percentage and a completion event."
parent: task-493-async-layout-loader
kind: task
status: todo
---
# Asynchronous level loader from a layout JSON with a load percentage: cpp

- goal: Asynchronous loader that loads the layout assets and builds the grid over frames, with a 0-100 percentage and a completion event.
- agent: cpp
- depends_on: none
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md)
- expected_output: see done_when
- done_when: A1 to A5 of the global hold on arena_m1_fine and one task-492 layout; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-493-async-layout-loader]] — parent global task.
