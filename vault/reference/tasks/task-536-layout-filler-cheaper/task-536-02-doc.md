---
id: task-536-02-doc
description: "Doc: Update sys-layout-generator with the measured filler cost and what the filler now reuses or skips."
parent: task-536-layout-filler-cheaper
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-536
- usage: included in the executor run of task-536
- runs: 1
- milestone: M2
- started: 2026-11-13T18:29:27Z
- waiting_for: none
- result: Commit b8b51ff0. A7: sys-layout-generator gotchas give the measured slow-seed times before and after (seed 17 4.9 s to 0.80 s, arena_l3 422704496 4.3 s to 0.41 s, filler share 99 % to about 90 %, idle walks), one line on what RunFiller reuses or skips with identical output, and the -carveonly filler line; reference determinism line unchanged; lint clean.

## Relations
- [[task-536-layout-filler-cheaper]] — parent global task.
