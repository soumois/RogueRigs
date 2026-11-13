---
id: task-536-02-doc
description: "Doc: Update sys-layout-generator with the measured filler cost and what the filler now reuses or skips."
parent: task-536-layout-filler-cheaper
kind: task
status: todo
---
# Layout generator: cheaper far-cell filler, identical output: doc

- goal: Update sys-layout-generator with the measured filler cost and what the filler now reuses or skips.
- agent: doc
- depends_on: task-536-01-filler-optimise
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md)
- expected_output: see done_when
- done_when: A7 holds: sys-layout-generator gotchas give the measured slow-seed time after the change (replacing the ~6 s estimate when it moved) and one line on what RunFiller reuses or skips; determinism line still states the reference; lint clean, committed.
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
- [[task-536-layout-filler-cheaper]] — parent global task.
