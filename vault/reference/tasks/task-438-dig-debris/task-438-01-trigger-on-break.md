---
id: task-438-01-trigger-on-break
description: "Cpp: A block broken by the drone requests one debris burst at the block with the block colour, bounded."
parent: task-438-dig-debris
kind: task
status: done
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
- model: opus
- tokens: included in the executor run of task-438
- usage: included in the executor run of task-438
- runs: 1
- milestone: M2
- started: 2026-11-05T18:01:17Z
- waiting_for: none
- result: ARRDrone::PlayDigDebris from TickDig only (block broken and not a barrel): pool cap DigDebrisPoolSize 16, prewarmed in SetOwnerMiner, at cell centre + DigDebrisSpawnHeight 70, User.ChipColor = GetChipColorForType, User.ThrowDirection = face normal toward the miner. Build ok. RogueRigs.Dig 22 and RogueRigs.Ore 5 pass (no Grid family), new Dig.DigDebrisBurst: rock 6 blocks 6 bursts, ore and heal veins colour per block, barrel none, other sources none, full pool skipped.

## Relations
- [[task-438-dig-debris]] — parent global task.
