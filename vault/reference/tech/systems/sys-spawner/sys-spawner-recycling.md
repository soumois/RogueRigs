---
id: sys-spawner-recycling
description: "Recycling by relocation: far robots moved back instead of destroyed, and the rules around it. Open to change robot recycling."
parent: sys-spawner
kind: doc
---
# Spawner — recycling

- recycling by relocation (task-036-01, [[enemies-index]], provisional; task-032-01 destroyed the robot): every tick (spawning on or off) while `bRecycleEnabled`, each robot of this spawner counts its seconds off screen (more than `RecycleOffScreenMargin` 0 uu beyond the view rectangle) since it spawned, was relocated or last left the screen; on screen it is marked seen and the timer resets. Due when farther than `RecycleDistance` 1800 uu (2D) from the miner with `RecycleDelay` 3 s off screen if seen once, `RecycleUnseenDelay` 6 s if never seen. One `GetSpawnCandidates` scan per tick with a robot due; each takes a distinct cell drawn like a lone robot (`PickSpawnCellIndex`: heading sector, then a random cell), and `ARREnemy::RelocateTo` teleports it there (capsule on the floor), clears velocity, pending knockback and input, turns the far-movement LOD off and calls `ResetStateMachine` (chaser, heavy, kamikaze back to Chase, shooter to Approach: wind-up, telegraph, flare or aim cancelled, no explosion). Class, health, stats and attack cadence are kept; it keeps its slot (not a kill, not a spawn: caps unchanged); it then counts as never seen. No cell: it stays and retries next tick. `OnEnemyRecycled(Enemy)` fires after the move; `GetRecycledCount` (relocations), `GetRecycleSeconds(Enemy)`, `HasBeenOnScreen(Enemy)`. Measured ([[sys-automation-tests]] `Spawner.RecycleKeepsSpawnsAhead`, miner running 600 uu/s, default director): 120–210 s, 206–314 arrivals (spawned or relocated) in the front sector per 30 s vs 0–62 without (two runs); 12–16 relocations a second once the cap is full (robots spawned along the long view sides, about 2800 uu away, move again after 6 s unseen). Tests that count spawns through the alive count turn recycling off (task-034-01).

## Relations
- [[sys-spawner]] — parent hub: summary and verdict.
- [[enemies-index]] — referenced above.
- [[sys-automation-tests]] — referenced above.
