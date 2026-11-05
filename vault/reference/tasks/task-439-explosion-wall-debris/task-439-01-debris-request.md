---
id: task-439-01-debris-request
description: "Cpp: An explosion that destroys blocks requests one bounded debris effect carrying its centre and the destroyed block places."
parent: task-439-explosion-wall-debris
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-439
- usage: included in the executor run of task-439
- runs: 1
- milestone: M2
- started: 2026-11-05T19:19:15Z
- waiting_for: none
- result: Build ok. URRExplosionSubsystem::PlayWallDebris: one request per explosion through the Niagara pool, User.ExplosionCenter, User.DebrisPositions, User.DebrisColors, User.DebrisCount; at most 60 particles (4 per block, evenly thinned beyond), nothing when no block broke, pool cap 8. Colours in Grid/RRBlockColors.h. RogueRigs.Explosions, BarrelFuse and Dig.DigDebris pass, including the new WallDebrisRequest; no RogueRigs.Grid family exists.

## Relations
- [[task-439-explosion-wall-debris]] — parent global task.
