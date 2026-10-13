---
id: perf-stress-grid-pools-hud
description: "Robot pool (091-01), XP pickup pool (092-01), passive hit capsule (093-01), HUD after the refactor (105-01). Open for pool and HUD costs."
parent: perf-stress-grid-cells
kind: doc
---
# Grid stress — pools hud

## Robot pool (task-091-01, 2026-10-10)
Editor `-game`, `rr.Stress.Run 250 30 60 1 game` with `rr.Pacing.RunLookahead 0` (same command as task-071-01). Before = d2e8ca9 (spawn in one frame), after = robots from the actor pool, `SpawnMaxFrames` 20 ([[sys-spawner]], [[sys-actor-pool]]); two runs each, same session within 25 min, owner's normal profile, nothing closed. Wave spawn spikes = frames over 25 ms tagged `fill robots` (a fill wave on the previous frame; after: also frames where the pool created robots), GC frames excluded. ms avg / p99 / max.

| Run | Game thread | Frame | Wave spawn spikes (max frame) | All spikes | Pool: created / reused, longest wait |
|---|---|---|---|---|---|
| Before #1 | 12.21 / 27.66 / 54.39 | 12.47 / 27.40 / 54.28 | 46 (54.3) | 64 | none |
| Before #2 | 12.46 / 27.90 / 51.34 | 12.67 / 27.76 / 50.44 | 61 (46.1) | 81 | none |
| After #1 | 11.81 / 19.24 / 72.62 | 12.13 / 19.41 / 72.97 | 4 (73.0) | 16 | 325 / 809, 16 frames |
| After #2 | 12.65 / 20.34 / 66.68 | 12.84 / 20.58 / 66.84 | 4 (28.5) | 17 | 326 / 863, 15 frames |

- Wave spawn spikes 46–61 → 4 per run; game-thread p99 27.7–27.9 → 19.2–20.3 ms (now under the 25 ms budget); average unchanged within run noise (12.2–12.5 → 11.8–12.7). The pool creates about 325 robots in the first minute, then reuses (72 % of the deliveries); no request waited more than 16 frames.
- The 4 remaining spawn-tagged spikes are fill frames served by reuse (10–15 robots reactivated in one frame: 25–29 ms, one 73 ms frame at t 13 s, not traced). Reuse is not spread; spreading it is the next lead if those frames matter.
- A first pair of after runs was discarded: robots released at the death-shrink scale hit a Chaos ensure on reuse (one 2984 ms frame); fixed by restoring the scale on release before the runs above.

## XP pickup pool (task-092-01, 2026-10-10)
Same procedure as the robot pool section. Before = 1bc689b (cubes, ore and heal spawned, 4-per-frame grid queue), after = every grid and robot drop from the actor pool, `SpawnMaxFrames` 3 ([[sys-pickups]], [[sys-actor-pool]]); same session within 20 min, owner normal profile, nothing closed. Burst = S7 `rr.Stress.Run 400 5 20 1 burst Chaser` (400 chasers, 1200 cubes, 16 barrels, `rr.Stress.MinCells 8`), one launch each. XP spikes = frames over 25 ms tagged `xp drop` (cubes requested on the last two samples; tag added by this task, absent before). ms avg / p99 / max.

| Run | Game thread | Frame | Spikes (gc) | XP-burst frames | Pickup pool: created / reused, longest wait |
|---|---|---|---|---|---|
| Game before #1 | 11.76 / 19.55 / 71.07 | 12.03 / 19.97 / 71.80 | 17 (1) | not tagged | none |
| Game before #2 | 12.24 / 19.68 / 50.45 | 12.51 / 19.66 / 50.53 | 15 (1) | not tagged | none |
| Game after #1 | 12.48 / 18.90 / 47.03 | 12.63 / 19.03 / 46.34 | 2 (1) | 1 (30.9, 321 cubes, t 4.3 s) | 587 / 2493, 3 frames |
| Game after #2 | 11.37 / 18.14 / 48.63 | 12.02 / 18.25 / 47.98 | 1 (1) | 0 | 433 / 2514, 3 frames |
| Burst before | 4.42 / 17.55 / 203.71 | 8.84 / 19.29 / 204.01 | 11 | 1 frame 204.0 (t 0.37 s) | none |
| Burst after | 4.45 / 17.36 / 91.66 | 8.85 / 20.04 / 90.79 | 15 | 3 frames 90.8, 62.9, 62.2 (t 0.25-0.38 s) | 1223 / 0, 3 frames |

- Game 250: spikes 15–17 → 1–2 per minute, the only one left besides the GC frame is one mass drop of 321 cubes (30.9 ms at t 4.3 s); game-thread p99 19.6–19.7 → 18.1–18.9 ms, average unchanged within run noise. Before, 13–14 spikes per run carried no tag (no fill, no robot creation, no GC); they disappear with the pool, so they were most likely the cube spawns of mass kills (the tag did not exist yet to prove it). Cubes are reused 81–85 % of the time after the first minute (433–587 created, about 2500 reused).
- Burst: the 1200-cube frame (400 deaths) drops from 204 ms to 91 ms on the death frame (robot death and release, cube requests), then 2 frames of about 62 ms while the pool creates 400 cubes per frame in a fresh process (nothing to reuse). Spike count rises 11 → 15 (more frames over 25 ms, all lower). The 1.6–2.0 s cluster (25–68 ms, draw up to 50 ms, untagged) is the same before and after: the magnet stream of 1200 cubes, not a spawn cost.
- Longest wait 3 frames in every run. Lead if the first mass kill of a run matters: prewarm XP cubes at level start (the pool supports `Prewarm`; default none), or a longer `SpawnMaxFrames` (owner contract: 3).

## Passive hit capsule (task-093-01, 2026-10-10)
Editor `-game`, same commands as task-071-01 (game 250 settle 30 measure 60, `rr.Pacing.RunLookahead 0`; S0 400 settle 5 measure 20). Before = 9ad55cc, after = 1deed6f (`rr.Enemy.PassiveHitCapsule` 1, [[sys-enemies-movement]], audit [[perf-robot-components-audit]]); all runs within 25 min, owner normal profile, nothing closed, no editor running. Game thread avg / p99 / max; movement = `-statnamedevents` trace of a third game 250 run, `URRRobotMovementComponent_Tick` inclusive ms per frame (Insights, trace outside the repo).

| Run | Before | After |
|---|---|---|
| Game 250 #1 (alive avg) | 12.48 / 18.00 / 45.63 (202) | 12.06 / 17.42 / 46.55 (195) |
| Game 250 #2 | 12.44 / 18.32 / 46.65 (199) | 11.60 / 17.35 / 49.51 (196) |
| Game 250 traced (frame avg / p99) | 13.29 / 19.51 (13.38 / 19.60) | 13.11 / 19.28 (13.20 / 19.06) |
| S0 400 | 6.62 / 8.62 / 9.80 | 6.51 / 8.48 / 12.99 |
| Movement ms / frame (robots per frame) | 3.64 (198) | 3.16 (203) |
| … overlap update / hit capsule overlap queries | 0.83 / 0.61 | 0.19 / 0.016 |
| … child transforms (physics body moves) | 1.07 (0.64) | 1.12 (0.68) |

- Movement 18.4 → 15.6 µs per robot: the hit capsule overlap query (one per robot move) is gone; what remains is the swept move (1.6 ms) and the transform update of the visible parts and the two capsule bodies (1.1 ms).
- Game thread at 250: −0.4 to −0.8 ms avg, −0.6 to −1.0 ms p99 untraced, inside the run spread (≈ 0.5 ms); S0 400 within noise (render bound). Spikes unchanged (1–2 per run, the GC frame).

## HUD after the refactor (task-105-01, 2026-10-11)
Editor `-game`, HEAD 6561e13 (HUD of tasks 094 and 103: seven event-driven panels, no HUD tick, [[sys-hud]]), `rr.Stress.Run 250 30 60 1 game` with `rr.Pacing.RunLookahead 0`; one Insights run (`-Trace cpu,frame,gpu,bookmark,region -ExtraArgs -statnamedevents`, trace outside the repo) then two untraced runs, all within 10 min, owner normal profile, nothing closed, no editor running. Game thread avg / p99 / max; spikes = frames over 25 ms.

| Run | Game thread | Frame avg / p99 | Alive avg | Spikes (gc / fill / xp drop) |
|---|---|---|---|---|
| Traced | 13.94 / 20.49 / 55.22 | 14.00 / 20.42 | 201 | 3 (1 / 0 / 1) |
| Untraced #1 | 12.35 / 20.74 / 50.40 | 13.10 / 21.04 | 205 | 4 (1 / 1 / 1) |
| Untraced #2 | 13.72 / 20.25 / 56.80 | 13.98 / 20.69 | 193 | 8 (1 / 3 / 0) |

- Against task-093-01 after (11.6–12.1 / 17.4 untraced, 13.11 / 19.28 traced): +0.3 to +1.6 ms avg, +2.9 to +3.3 ms p99. The HUD cannot explain it: all HUD Blueprint work is 0.11 ms per frame in the trace ([[perf-trace-real-game]]). This is the PC load spread (≈ 2 ms), with no code change between the runs.
- Run #2: 4 frames of 25–27 ms at t 1.0 s are render bound (draw 22–26 ms, game 17–25), the same signature as the task-068-01 untraced runs. The other spikes are wave fills, one GC and one 270-cube drop, as before.
- HUD before/after: [[perf-trace-real-game]] (WBP_HUD Blueprint tick 0.40 ms → 0.11 ms for every panel Blueprint event together).

## Relations
- [[perf-stress-grid-cells]] — parent hub: summary and verdict.
- [[sys-spawner]] — referenced above.
- [[sys-actor-pool]] — referenced above.
- [[sys-pickups]] — referenced above.
- [[sys-enemies-movement]] — referenced above.
- [[perf-robot-components-audit]] — referenced above.
- [[sys-hud]] — referenced above.
- [[perf-trace-real-game]] — referenced above.
