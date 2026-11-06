---
id: task-475-01-block-height
description: "Cpp: The terrain block height becomes the shrunk miner height, as one tunable value, with everything on the blocks following."
parent: task-475-wall-height-miner
kind: task
status: done
---
# Walls raised to the miner height: cpp

- goal: The terrain block height becomes the shrunk miner height, as one tunable value, with everything on the blocks following.
- agent: cpp
- depends_on: none
- inputs: arena-grid (vault/reference/gdd/terrain/arena-grid.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), miner (vault/reference/gdd/player/miner.md)
- expected_output: see done_when
- done_when: A1 to A5 and A7 hold in PIE; heights recorded in the result; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-475
- usage: included in the executor run of task-475
- runs: 1
- milestone: M2
- started: 2026-11-06T17:17:22Z
- waiting_for: none
- result: Commit bea54ef4. ARRArenaGrid BlockHeight C++ default 100 -> 153 (old 100, new 153 = head top of the 0.9 miner at rest, 170 x 0.9; exact, within A1 5 %), one EditAnywhere value, no BP or map override. New BarrelHeight 100 (render fit and collision of barrel cells) so barrels stay unchanged (A4). Rock pieces (Z scale BlockHeight/100), ore and heal crystals and deposit crystals (z = BlockHeight), block collision and the dig face point (half height) already derive from BlockHeight (A3); shot height 50 and floor clearance stay below 153, no ShotHeight warning in -game logs; line-of-sight and drone wall checks are 2D or at shot height (A5). Build exit 0. Tests RogueRigs.Dig+Barrel+Weapons+Explosions+Ore+Shooter: 62 of 63 pass; RogueRigs.Weapons.ImpactSparkAsset fails ('rifle shot plays it', RRWeaponTests.cpp:1619) and already failed at 17:07 before this change (backup log RogueRigs-backup-2026.11.06-17.07.50.log), unrelated: not fixed (no test expectation changed). Two -game runs: no new runtime error. Rock slab material sag clamp (SagParams a=100, g=60: max drop 40) not changed: a damaged top sinks at most to 113 on the 153 block.

## Relations
- [[task-475-wall-height-miner]] — parent global task.
