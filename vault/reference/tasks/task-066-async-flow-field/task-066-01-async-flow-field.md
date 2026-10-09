---
id: task-066-01-async-flow-field
description: "Flow field rebuilt on a worker thread with a double buffer, so robots follow a new path within 1–2 frames under normal PC load."
parent: task-066-async-flow-field
kind: task
status: done
---
# Flow field on a worker thread

- goal: Flow field rebuilt on a worker thread with a double buffer, so robots follow a new path within 1–2 frames under normal PC load.
- agent: cpp
- depends_on: none
- inputs: sys-flow-field (vault/reference/tech/systems/sys-flow-field.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: URRFlowFieldSubsystem (or its rebuild path): run goal rebuilds and incremental updates on a worker thread (UE::Tasks or FRunnable) over a copy of the walkable grid, double-buffered: robots keep reading the current field until the new one is complete, then swap on the game thread; cell destruction during a rebuild is queued and applied (no lost openings); no game-thread blocking on the worker; the 0.3 ms game-thread budget becomes only the swap and queue cost; keep the time-sliced path as a fallback (editable switch) for platforms without spare cores; update existing flow-field tests to the async behaviour (BudgetAt67uu: goal age in frames must stay within its limit; DestroyedRockOpensPocket: opening visible after the swap) without loosening limits; no -game measurement for now (owner 2026-10-09: his CPU is throttled, measurement later); docs sys-flow-field.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless, BudgetAt67uu included, without loosened limits; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: input 86, cache write 273212, cache read 3724689, output 49494
- runs: 1
- milestone: M2
- result: Flow field on a UE::Tasks worker with a double buffer: FRRFlowFieldData works on a walkable-cell snapshot, one job at a time, finished job swapped on the game thread without waiting, openings queued for the next job, rr.FlowField.Async (default 1) with the time-sliced fallback at 0. Headless 67 uu: goal age max 1 frame (was 33–38), game-thread flow cost 0.07 / 0.11 / 0.36 ms avg / p95 / max, worker rebuild 4.6–6.3 ms. Flow-field tests now pace each tick to 1/60 s real time (limits unchanged). Build 0, 195/195. No -game measurement (owner deferred). Commits 131d67a, a68f8b6.

## Relations
- [[task-066-async-flow-field]] — parent global task.
