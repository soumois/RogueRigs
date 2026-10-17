---
id: task-230-01-rock-baseline-capture
description: "Baseline before the change: -game captures of rock areas and a grid perf measure."
parent: task-230-rock-slab-integration
kind: task
status: done
---
# Baseline before the change: -game captures of rock areas and a grid perf measure

- goal: Baseline before the change: -game captures of rock areas and a grid perf measure.
- agent: cpp
- depends_on: none
- inputs: vault/reference/tech/performance/perf-stress-grid-cells.md, vault/reference/tech/systems/world/sys-grid.md, vault/reference/agents/agent-cpp.md
- expected_output: `Saved/RockSlab/before/` captures (rr.Debug.Shots: intact wall, corridor corner, ore deposit, heal vein, wall during dig) with fixed camera spots reused in step 12; game-thread and GPU times, triangle and draw counts on the 240 grid, steady and during a 30-barrel chain, in the procedure of perf-stress-grid-cells; result note appended as a section of a new node perf-rock-slab (child of performance-index).
- done_when: captures exist and are sent to the owner; numbers recorded; committed.
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 94, cache write 135632, cache read 3692207, output 3861
- runs: 1
- milestone: M2
- result: 8 captures in Saved/RockSlab/before (spots A-F with BugItGo coordinates in perf-rock-slab), perf baseline in perf-rock-slab (commit 7e537340): steady 100 robots frame 8.96 ms avg, GPU 7.34, 928 draws, 807k tris; grid alone about 107k tris visible, 763-796 draws; render-thread bound, about 650 shadow-depth draws; captures sent to the owner

## Relations
- [[task-230-rock-slab-integration]] — parent global task.
