---
id: task-029-02-flow-field-incremental
description: "Flow field updated incrementally from opened cells (distances only decrease) and spread over frames under a per-frame budget; tests; measured."
parent: task-029-fine-grid
kind: task
status: in-progress
---
# Incremental flow field

- goal: opening cells never spikes a frame.
- agent: cpp
- depends_on: task-029-01-grid-render
- inputs: [[sys-flow-field]], [[sys-enemy-ai]], [[sys-spawner]], [[sys-objectives]], [[cpp-standard]], [[sys-automation-tests]], [[stress-grid-cells]], [[sys-stress]]
- expected_output: URRFlowFieldSubsystem: opened cells queued; incremental relaxation (open-only: a new floor cell takes min neighbour distance + 1 and propagates decreases) processed under a per-frame time budget (default 0.3 ms, cvar/editable), full rebuild kept for level load and as a fallback; queries during an update return the last valid distances (valid because only openings happen); reachability queries used by console, shuttle and spawner stay correct; tests: incremental result equals a full rebuild after random openings, budget respected, robots path through a newly opened gap within N frames; measure dig sweep and chain at 67 uu; docs sys-flow-field.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean. Measurements recorded.
- editor_state: closed
- token_ceiling: 120000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-029-fine-grid]] — parent global task.
