---
id: task-029-02-flow-field-incremental
description: "Flow field updated incrementally from opened cells (distances only decrease) and spread over frames under a per-frame budget; tests; measured."
parent: task-029-fine-grid
kind: task
status: done
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
- model: opus
- tokens: input 100, cache write 176062, cache read 6094834, output 87008
- runs: 1
- milestone: M2
- result: Opened cells queued and relaxed decrease-only (8 neighbours, same tie rule as a full rebuild, identical result tested); miner cell change = BFS into a second buffer spread over frames then swapped (robots follow the previous field meanwhile); both share a per-frame budget rr.FlowField.BudgetMs 0.3; full rebuild kept for load and MarkDirty; queries answer from the last valid field; FlushPendingWork, IsUpdatePending etc. Stress walk mode. Tests IncrementalMatchesRebuild, BudgetAt67uu (p95 0.31 ms), RobotPathsThroughNewGap; 153/153. At 67 uu: dig flow-field 2.7 → 0.015 ms per frame, miner moving 2.2 ms spikes → 0.29 ms; frames render-bound about 9.2 ms. Commits 6f0ed7c, 55c953c.

## Relations
- [[task-029-fine-grid]] — parent global task.
