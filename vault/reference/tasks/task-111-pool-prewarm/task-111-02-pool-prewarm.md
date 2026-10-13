---
id: task-111-02-pool-prewarm
description: "A measured note says whether pooling the projectiles is worth it; nothing is migrated."
parent: task-111-pool-prewarm
kind: task
status: done
---
# Projectile pooling evaluation

- goal: A measured note says whether pooling the projectiles is worth it; nothing is migrated.
- agent: cpp
- depends_on: task-111-01-pool-prewarm
- inputs: perf-trace-real-game (vault/reference/tech/performance/perf-trace-real-game.md), sys-projectiles (vault/reference/tech/systems/sys-projectiles.md), sys-weapons (vault/reference/tech/systems/sys-weapons.md), sys-actor-pool (vault/reference/tech/systems/sys-actor-pool.md), stress-grid-cells (vault/reference/tech/performance/stress-grid-cells.md), sys-stress (vault/reference/tech/systems/sys-stress.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: owner 2026-10-12: "il faudra voir si c'est intéressant de mettre les projectiles dans la pool". MEASURE, do not migrate. Count how many projectiles the game spawns per second in the continuous-fire stress (S2 and the real-game mode: read sys-stress and stress-grid-cells; rifle bursts, shotgun 7 pellets per shot, turret shots, robot shots), and measure the game-thread cost of one projectile spawn (SpawnActor, BeginPlay, components) and of its destruction, with an Insights capture as in task-068-01 and task-105-01 (traces outside the repo, never committed; no UnrealEditor.exe running: verify; owner normal PC profile, kill nothing) in editor -game. Write a short evaluation note in tech/performance (id projectile-pool-evaluation, linked from performance-index): spawns per second, ms per spawn and per frame, the share of the game thread, the spikes it causes (burst frames), what a pool would save (an estimate with its assumptions), the cost and risks of migrating (reset of the projectile state, the pierce hit lists, trails, ownership), and a recommendation (migrate or not) with a threshold. No code change.
- done_when: evaluation note written with measured numbers and a recommendation; lint clean; committed.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 58, cache write 88099, cache read 1643632, output 23662
- runs: 1
- milestone: M2
- result: Projectile pool evaluation: rifle 3.4 spawns/s at 1.13 ms (0.41 core), pellets 7 per shot at 0.54 ms, 0.11 ms per frame = 0.67 % of the game thread; saving about 0.07 ms per frame; recommendation: do not migrate now, revisit at 15+ projectiles per frame or 30+ per second. Note projectile-pool-evaluation. Commit dfdce4a.

## Relations
- [[task-111-pool-prewarm]] — parent global task.
