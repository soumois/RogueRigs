---
id: task-447-02-doc
description: "Doc: Record the corrected state and how it was proved."
parent: task-447-swirls-alpha-fix
kind: task
status: done
---
# Fix the invisible extractor swirls: doc

- goal: Record the corrected state and how it was proved.
- agent: doc
- depends_on: task-447-01-swirls-compiled
- inputs: bp-extractor-rotor-wash (vault/reference/tech/blueprints/fx/bp-extractor-rotor-wash.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: see done_when
- done_when: The node says the swirls were checked in the simulation, with the figures; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-447
- usage: included in the executor run of task-447
- runs: 1
- milestone: M2
- started: 2026-11-05T22:31:22Z
- waiting_for: none
- result: Done. bp-extractor-rotor-wash gains swirls_alpha_fix (cause, fix, PIE figures after editor restart: Swirls A 0.003-0.084 at Intensity 0.6, 0.1396 at 1, error 0.00000; GroundBlast unchanged) and an updated unverified line; unreal-mcp-quirks notes the stale compiled script is saved with the asset and the plain-then-expression fix. Lint clean.

## Relations
- [[task-447-swirls-alpha-fix]] — parent global task.
