---
id: task-438-01-trigger-on-break
description: "Cpp: A block broken by the drone requests one debris burst at the block with the block colour, bounded."
parent: task-438-dig-debris
kind: task
status: todo
---
# Debris when digging: cpp

- goal: A block broken by the drone requests one debris burst at the block with the block colour, bounded.
- agent: cpp
- depends_on: none
- inputs: sys-drone-dig (vault/reference/tech/systems/combat/sys-drone/sys-drone-dig.md), sys-grid (vault/reference/tech/systems/world/sys-grid.md), sys-actor-pool (vault/reference/tech/systems/world/sys-actor-pool.md), drone (vault/reference/gdd/player/drone.md), color-code (vault/reference/gdd/art-audio/color-code.md), perf-lead-debris-gpu (vault/reference/tech/performance/perf-leads/perf-lead-debris-gpu.md)
- expected_output: see done_when
- done_when: Build ok; drone breaks request one burst with the right colour, other break sources none, a full pool skips it; Drone and Grid test families pass.
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
- [[task-438-dig-debris]] — parent global task.
