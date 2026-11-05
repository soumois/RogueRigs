---
id: task-445-03-doc
description: "Doc: Document the shadow, the values and how to turn it off."
parent: task-445-miner-light-wall-shadows
kind: task
status: in-progress
---
# Miner light casts wall shadows only: doc

- goal: Document the shadow, the values and how to turn it off.
- agent: doc
- depends_on: task-445-02-casters-walls-only
- inputs: bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), perf-risks (vault/reference/tech/performance/perf-risks.md)
- expected_output: see done_when
- done_when: Nodes state the current state and the off switch; perf-risks row updated; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T21:30:14Z
- waiting_for: none
- result: open

## Relations
- [[task-445-miner-light-wall-shadows]] — parent global task.
