---
id: perf-lead-pickup-simulated-roll
description: "Lead: replace the Chaos physics of landed pickups by a roll simulated in the pickup subsystem loop if hundreds of bodies cost too much (task-229)."
parent: perf-leads
kind: note
---
# Lead: simulated pickup roll instead of Chaos

- origin (owner 2026-10-17): pickups now land into real Chaos physics (simple box collisions, floor and walls only) so they roll and tumble instead of resting on a corner ([[task-229-pickup-physics]]). The owner asked to keep a simulated roll as the fallback "si besoin".
- cost to watch: one simulated body per landed pickup until it sleeps; XP bursts can leave hundreds on the floor (stress S7: 1200 cubes from 400 chasers, [[sys-pickups]]). The measured cost of task-229 goes here once known.
- option: after landing, keep the landing velocity in `URRPickupSubsystem` (no physics body), decelerate with a friction constant, turn the mesh by distance / radius about the axis perpendicular to the motion, bounce on blocking grid cells with a 2D cell test, then snap to the nearest face-down orientation when slow (the cube never stays on a corner). Expected: near zero physics cost, one extra step in the existing loop.
- measure first: physics time (stat physics, game-thread time) in the S7 burst with physics on versus off.
- measured (task-229-02, 2026-10-17): S7 `Scripts/Run-StressTest.ps1 -Counts 400 -Settle 5 -Measure 20 -Mode burst -Class Chaser -PreExec "rr.Stress.MinCells 8"`, editor `-game` 1920 x 1080, one launch each within 16 min, owner PC as found (the Armoury Crate control step was not done by the agent; no Insights trace). Before = HEAD 92929b60 (no pickup body); off = task-229-02 build with `rr.Pickup.Physics 0` (bodies exist, kinematic, no simulation); on = task-229-02 defaults. ms avg / p99 / max. CSV `Saved/Stress/Stress_20261017_150932` (before), `_152357` (on), `_152457` (off), not committed.

| Run | Frame | Game thread | Draw avg / p99 | Spikes | Pickups left on the floor |
|---|---|---|---|---|---|
| Before | 9.50 / 18.18 / 46.99 | 3.87 / 12.74 / 43.69 | 9.44 / 17.22 | 7 | 250 (379 created) |
| Off | 11.20 / 22.38 / 82.67 | 5.16 / 17.79 / 81.98 | 11.10 / 21.85 | 11 | 262 (379 created) |
| On | 11.32 / 24.09 / 77.03 | 5.47 / 19.20 / 75.62 | 11.24 / 23.52 | 15 | 287 (393 created) |

- reading: simulation itself (on vs off, same build) costs about +0.3 ms game-thread average and +1.4 ms p99 for about 270-290 landed bodies, within the usual 2 ms spread; the extra spikes are in the landing window (t 1.48-1.80 s, game 16-31 ms, mixed with the barrel explosions). The larger step from before to off (+1.3 ms game average, +5 ms p99, death frame 44 to 76-82 ms) also shows on the render thread (draw +1.7 ms), which pickup bodies should not touch, so PC state drift is likely part of it; candidate real causes: physics state of up to 393 bodies (creation in the pool burst, kinematic teleports during 1.2 s of flight). Not separated: redo with the control step and an Insights trace (`-Trace cpu,frame`) before choosing this lead. Lead stays open; the cheapest first try is to create the body only at landing (NoCollision in flight) before replacing Chaos.

## Relations
- [[perf-leads]] — parent index.
- [[sys-pickups-lifecycle]] — the pop, rest and magnet phases it would change.
