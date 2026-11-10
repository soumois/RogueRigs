---
id: task-496-04-doc
description: "Doc: Document the generator provider, the request timings and where layouts come from."
parent: task-496-generator-in-runs
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-10T20:21:38Z
- waiting_for: none
- result: open

## Relations
- [[task-496-generator-in-runs]] — parent global task.
