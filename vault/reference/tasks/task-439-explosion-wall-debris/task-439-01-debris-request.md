---
id: task-439-01-debris-request
description: "Cpp: An explosion that destroys blocks requests one bounded debris effect carrying its centre and the destroyed block places."
parent: task-439-explosion-wall-debris
kind: task
status: in-progress
---
# Wall debris thrown away from explosions: cpp

- goal: An explosion that destroys blocks requests one bounded debris effect carrying its centre and the destroyed block places.
- agent: cpp
- depends_on: none
- inputs: sys-explosions (vault/reference/tech/systems/combat/sys-explosions.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), explosions (vault/reference/gdd/terrain/explosions.md), perf-lead-debris-gpu (vault/reference/tech/performance/perf-leads/perf-lead-debris-gpu.md)
- expected_output: see done_when
- done_when: Build ok; one request per explosion with its centre, count bounded at 60, nothing when no block broke; Explosion and Grid test families pass.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T19:19:15Z
- waiting_for: none
- result: open

## Relations
- [[task-439-explosion-wall-debris]] — parent global task.
