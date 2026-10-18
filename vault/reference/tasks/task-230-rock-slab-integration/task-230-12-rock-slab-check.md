---
id: task-230-12-rock-slab-check
description: "After the change: -game captures at the baseline spots and the grid perf measure, compared with step 01."
parent: task-230-rock-slab-integration
kind: task
status: in-progress
---
# After the change: -game captures at the baseline spots and the grid perf measure, compared with step 01

- goal: After the change: -game captures at the baseline spots and the grid perf measure, compared with step 01.
- agent: cpp
- depends_on: task-230-11-rock-slab-assign
- inputs: perf-rock-slab node from the 01 result, vault/reference/tech/performance/perf-stress-grid-cells.md, vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-counts.md
- expected_output: `Saved/RockSlab/after/` captures at the same spots (plus a wall during dig showing cracks and plates), side-by-side with before; perf numbers versus baseline and versus the provisional triangle budget, appended to perf-rock-slab.
- done_when: captures sent to the owner; numbers recorded; budget verdict reported; committed.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
