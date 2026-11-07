---
id: task-493-02-doc
description: "Doc: Document the loader, its percentage and completion event."
parent: task-493-async-layout-loader
kind: task
status: done
---
# Asynchronous level loader from a layout JSON with a load percentage: doc

- goal: Document the loader, its percentage and completion event.
- agent: doc
- depends_on: task-493-01-loader
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md)
- expected_output: see done_when
- done_when: sys-grid or a new sys node describes the loader, what the percentage covers and how a loading screen reads it; lint clean; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-493
- usage: included in the executor run of task-493
- runs: 1
- milestone: M2
- started: 2026-11-07T07:11:52Z
- waiting_for: none
- result: 6d276df9. New node sys-layout-loader (vault/reference/tech/systems/world/sys-grid/sys-layout-loader.md, parent sys-grid): classes, StartLoad, LoadPercent / GetLoadPercent, OnLoadCompleted, phases, what the percentage covers (parse, assets, build units) and how a loading screen reads it, settings with provisional values, logs, measures, tests, debug command. sys-grid lifecycle and Relations updated. Lint clean. Budget: executor run over the standard budget, cause recorded in task-493-01-loader result.

## Relations
- [[task-493-async-layout-loader]] — parent global task.
