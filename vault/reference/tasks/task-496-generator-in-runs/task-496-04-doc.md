---
id: task-496-04-doc
description: "Doc: Document the generator provider, the request timings and where layouts come from."
parent: task-496-generator-in-runs
kind: task
status: done
---
# Layout generator wired into real runs: doc

- goal: Document the generator provider, the request timings and where layouts come from.
- agent: doc
- depends_on: task-496-03-capture
- inputs: sys-run-layout (vault/reference/tech/systems/world/sys-run/sys-run-layout.md), sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md)
- expected_output: see done_when
- done_when: The three nodes describe the provider, the per-run seed, params location, fallback and the new request points; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-496
- usage: included in the executor run of task-496
- runs: 1
- milestone: M2
- started: 2026-11-10T20:21:38Z
- waiting_for: none
- result: Done (commit 43fcecb1). sys-run-layout rewritten: FRRGeneratorLayoutProvider, run and level seeds, packaged params and briefs (level 1 gen brief and why), hand JSON fallback and rr.Layout.ForceGenFail, request points (menu open, ResetRun, board contact), frame measure, gotchas. sys-layout-generator: run use, where the committed JSON came from (Node scripts of Tools/layout_gen), HashCells, Tools/ default path vs packaged copies, run determinism check. sys-run-transition-preload: WaitingForLayout fed at the board contact, New Run/Retry wait behind the loading screen. Lint clean (vault OK, no warning on these nodes). No value left open.

## Relations
- [[task-496-generator-in-runs]] — parent global task.
