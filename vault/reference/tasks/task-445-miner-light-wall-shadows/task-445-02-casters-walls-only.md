---
id: task-445-02-casters-walls-only
description: "Cpp: From this light, only rock, ore and heal ore blocks cast shadows; anything found casting that should not is corrected."
parent: task-445-miner-light-wall-shadows
kind: task
status: todo
---
# Miner light casts wall shadows only: cpp

- goal: From this light, only rock, ore and heal ore blocks cast shadows; anything found casting that should not is corrected.
- agent: cpp
- depends_on: task-445-01-light-shadows
- inputs: sys-grid (vault/reference/tech/systems/world/sys-grid.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), perf-risks (vault/reference/tech/performance/perf-risks.md)
- expected_output: see done_when
- done_when: Build ok; a code read and a run read show block pieces cast, and robots, miner, drone, pickups, barrels, chest and ore crystals do not from this light; Grid test family passes; a report if nothing had to change.
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
- [[task-445-miner-light-wall-shadows]] — parent global task.
