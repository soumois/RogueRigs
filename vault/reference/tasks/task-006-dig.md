---
id: task-006-dig
description: "Global task: the drone digs blocks the miner pushes into (red laser, shaking block, dig times, ore and heal drops), and the floor flicker fix."
parent: tasks-index
kind: task
status: in-progress
---
# Digging

- request: "Lorsque je me colle à un mur, cela ne mine pas. J'ai semble-t-il un z-fight, ça clignote." Plus the digging rules given on 2026-10-01 (dig 1.5 s for rock; ore and heal ore tougher, drop regularly while dug and more when destroyed; red laser; the block shakes). Owner confirmed and pre-approved every subtask (2026-10-01).
- goal: in PIE, pushing the miner into a block makes the drone fly to it and cut it with a red laser while the block shakes; rock breaks in 1.5 s, ore and heal ore in 4.5 s (÷ `MiningSpeed`); ore and heal ore drop a pickup every 1.5 s of digging and 3 when destroyed; pickups fly to the miner within `PickupRadius` and are collected (no effect yet); the floor no longer flickers.
- acceptance: (1) build exits 0; (2) no floor instance under a block, floor added when a block is destroyed; (3) PIE: a rock next to the miner is destroyed about 1.5 s after the dig starts, a heal ore after about 4.5 s with 2 pickups during and 3 at the end; (4) pickups are collected; (5) docs `sys-grid`, `sys-drone`, `sys-player`, a new `sys-pickups` and the Blueprint docs match; lint clean, every subtask committed; (6) owner checks the flicker and the feel.
- gdd_check: read [[drone]], [[destruction-rules]], [[ore-blocks]], [[heal-ore]], [[explosions]], [[player-stats]], [[sys-grid]]. Owner decisions recorded 2026-10-01 (dig times, drops, laser red against [[readability-rules]], shake, explosions destroy all blocks in radius). Dig cancel rule was open: provisional rule set in [[drone]]. Explosions themselves are out (no barrel or kamikaze yet); the grid exposes the radius destruction call for them. Ore and heal values per pickup: open, pickups have no effect.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-006-01-grid-destruction
- result: open

## Subtasks
1. task-006-01-grid-destruction — cpp: floor fix, cell index map, block health, terrain damage and destruction API, shake.
2. task-006-02-drone-dig — cpp: dig trigger on the miner, drone Dig state, red laser component.
3. task-006-03-pickups — cpp: drops from the grid, `ARRPickup` collected by the miner.
4. task-006-04-bp-dig — bp: laser and pickup assets, grid and drone defaults, PIE check.

## Relations
- [[tasks-index]] — parent index.
- [[task-006-01-grid-destruction]] — step 1, cpp, grid.
- [[task-006-02-drone-dig]] — step 2, cpp, drone and miner.
- [[task-006-03-pickups]] — step 3, cpp, pickups.
- [[task-006-04-bp-dig]] — step 4, bp, assets and PIE.
