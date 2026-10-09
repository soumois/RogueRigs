---
id: task-066-01-async-flow-field
description: "Flow field rebuilt on a worker thread with a double buffer, so robots follow a new path within 1–2 frames under normal PC load."
parent: task-066-async-flow-field
kind: task
status: in-progress
---
# Flow field on a worker thread

- goal: Flow field rebuilt on a worker thread with a double buffer, so robots follow a new path within 1–2 frames under normal PC load.
- agent: cpp
- depends_on: none
- inputs: sys-flow-field (vault/reference/tech/systems/sys-flow-field.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: URRFlowFieldSubsystem (or its rebuild path): run goal rebuilds and incremental updates on a worker thread (UE::Tasks or FRunnable) over a copy of the walkable grid, double-buffered: robots keep reading the current field until the new one is complete, then swap on the game thread; cell destruction during a rebuild is queued and applied (no lost openings); no game-thread blocking on the worker; the 0.3 ms game-thread budget becomes only the swap and queue cost; keep the time-sliced path as a fallback (editable switch) for platforms without spare cores; update existing flow-field tests to the async behaviour (BudgetAt67uu: goal age in frames must stay within its limit; DestroyedRockOpensPocket: opening visible after the swap) without loosening limits; then one -game measurement in the same conditions as task-063 (background apps open, 60 Hz cap): S0 and S2 at 100 robots and dig, game-thread time and goal age, compared with task-063; docs sys-flow-field, stress-grid-cells.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless, BudgetAt67uu included, without loosened limits; measurement reported; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-066-async-flow-field]] — parent global task.
