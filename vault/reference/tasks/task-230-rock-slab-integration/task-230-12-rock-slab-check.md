---
id: task-230-12-rock-slab-check
description: "After the change: -game captures at the baseline spots and the grid perf measure, compared with step 01."
parent: task-230-rock-slab-integration
kind: task
status: done
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
- model: opus
- tokens: input 90, cache write 205266, cache read 3050447, output 3984
- runs: 1
- milestone: M2
- result: Commit cb465556 (perf-rock-slab after section). Captures in Saved/RockSlab/after/ (sbs_* side by side). Steady perf +1.2-1.4 ms GPU, within 16.7 ms; intact pieces within triangle budget, broken 120-420 tris over the old estimate. Defects: 30-barrel chain crash (Lumen LumenMeshCards.cpp:1332 assert, matrix precision errors); Duskite crystals grey; broken sag not visible; marked pulse unverified; one 3 s hitch; owner found rock collision lost (task-230-13).

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
