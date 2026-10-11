---
id: stress-grid-cells
description: "Cell size stress (200, 100, 67 uu); real-game limits (task-067-01); packaged vs editor, spike causes (task-068-01); robot light movement (task-071-01), robot pool (task-091-01) before/after."
parent: performance-index
kind: doc
---
# Grid cell size stress test (2026-10-05, task-023-01)

Same PC and editor `-game` procedure as [[stress-opt-m1]]. `L_ArenaM1` split at runtime by `rr.Stress.Subdivide` ([[sys-stress]]): same world, same rock. 80² / 160² / 240² cells; colliding instances 3 729 / 14 916 / 33 561. Chain: 30 barrel explosions at the same points, 100 robots. Dig: same rock area per second (8 cells of 200 uu/s, i.e. 32 or 72 small cells/s). Budget: 16.7 ms avg, 25 ms p99 ([[stress-test-m1]]). ms, avg / p99.

## Steady state (no destruction)
| Run | 200 uu | 100 uu | 67 uu |
|---|---|---|---|
| S0 100 frame | 8.44 / 10.39 | 9.06 / 10.52 | 8.99 / 10.50 |
| S0 400 frame (game, draw, GPU avg) | 10.10 / 13.49 (9.89, 8.04, 7.95) | 10.55 / 13.02 (9.08, 9.86, 9.24) | 10.47 / 12.07 (7.71, 10.44, 9.42) |
| S2 100 frame | 8.97 / 10.58 | 9.22 / 10.90 | 9.57 / 11.50 |
| S2 400 frame (game, draw, GPU avg) | 12.67 / 16.91 (12.67, 9.90, 8.40) | 12.35 / 16.61 (12.33, 10.35, 8.61) | 12.30 / 16.50 (12.19, 11.11, 9.10) |

## Destruction (bench and 100 robots)
| Measure | 200 uu | 100 uu | 67 uu |
|---|---|---|---|
| One explosion, terrain (blocks) | 1.7 ms (7) | 22.3 ms (29) | 91.6 ms (64) |
| Chain: event frame avg / max | 11.1 / 38 | 33.0 / 52 | 117 / 208 |
| Dig: frame avg / p99 (draw avg) | 9.12 / 10.67 (9.1) | 10.65 / 13.52 (9.4) | 18.59 / 23.08 (18.6) |
| One dig destruction | 0.25 ms | 0.87 ms | 2.14 ms |
| Flow-field BFS per rebuild | 0.29 ms | 1.03 ms | 2.56 ms |
| Spawn candidate scan | 0.11 ms | 0.42 ms | 1.13 ms |
| Line of sight per ray | 0.15 µs | 0.20 µs | 0.34 µs |
| Memory (used physical) / grid build | +1 MB / 33 ms | +35 MB / 135 ms | +115 MB / 309 ms |

## Cause and what-if
Insights trace (67 uu chain): 3.0 of 3.1 s explosion time is `UInstancedStaticMeshComponent::PartialNavigationUpdate` → `CalcBoundsImpl`, a full bounds pass over every instance on each add or remove. The map has no navmesh; robots use [[sys-flow-field]]. Same runs with navigation relevance off (`Subdivide <F> 1`): explosion 0.56 ms (100 uu), 1.0 ms (67 uu); chain event frames 11.6 / 13.9 (100 uu) and 19.2 / 21.4 (67 uu); dig 100 uu 9.90 / 13.27; dig 67 uu still 18.70 / 23.34, render-thread bound (HISM proxy and distance-field buffers re-uploaded every frame, `UpdateDistanceFieldObjectBuffers` up to 14 ms).

## After task-029-01 (2026-10-05)
Grid chunked in 16 × 16-cell HISMs per type, no component navigation relevant ([[sys-grid]]). Same PC, background load gone; "before" = commit 93c6585 rebuilt in a worktree, run the same hour. 100 robots, ms avg / p99 (event frame avg / max).

| Run | 200 uu before | 200 uu after | 67 uu before | 67 uu after |
|---|---|---|---|---|
| S0 frame | 8.50 / 10.26 | 8.73 / 10.91 | 9.53 / 12.18 | 9.24 / 11.77 |
| S2 frame | 8.88 / 11.50 | 8.94 / 11.51 | 9.37 / 11.95 | 9.39 / 11.93 |
| Chain frame (event) | 9.22 / 12.13 (11.1 / 16.9) | 8.88 / 11.63 (9.5 / 13.0) | 11.14 / 111.5 (121 / 189) | 9.31 / 12.07 (10.8 / 16.4) |
| Dig frame (game avg) | 9.34 / 12.07 (5.25) | 9.12 / 11.75 (5.39) | 19.00 / 23.47 (14.08) | 9.59 / 12.15 (7.77) |

- 67 uu: chain event frames 121 → 10.8 ms avg, max 189 → 16 ms; dig 19.0 → 9.6 ms (60 fps regained). Steady state unchanged (± 0.3 ms, noise). Flow-field BFS 2.7–2.8 ms per rebuild remains (task-029-02).
- Distance fields: chunks with `bGridAffectsDistanceFields` on vs off at 67 uu, chain 9.35 / 11.94 vs 9.31 / 12.07, dig 9.60 / 11.97 vs 9.59 / 12.15: no difference, so the grid stays in distance fields (default on, lighting unchanged).

## After task-029-02 (2026-10-05)
Incremental flow field ([[sys-flow-field]], budget 0.3 ms). 67 uu, 100 robots, editor `-game`, 5 s settle, 20 s measure. Background check: no game running (Steam and Epic launchers idle), GPU at 19–21 % from the desktop before the runs. ms avg / p99; flow = flow-field time of frames with work, avg / max.

| Run | Frame | Game avg / p99 | Flow work frames | Flow avg / max |
|---|---|---|---|---|
| Dig (1440 cells) | 9.19 / 11.02 | 6.33 / 9.03 | 1451 | 0.015 / 0.37 |
| Chain (1838 cells, events 10.2 / 13.6) | 9.03 / 10.88 | 6.22 / 9.27 | 32 | 0.051 / 0.22 |
| Walk 600 uu/s, budget 0.3 | 9.25 / 11.20 | 7.16 / 9.94 | 1379 | 0.29 / 0.72 |
| Walk, budget 0 (goal BFS in one frame) | 9.27 / 11.31 | 7.03 / 10.72 | 168 | 2.17 / 3.54 |

- Openings: 2.7 ms per rebuild on every dig frame before, 0.015 ms avg now; dig game thread 7.77 → 6.33 ms avg (029-01 run, other hour).
- Miner moving: the sliced goal rebuild holds 0.3 ms per frame instead of 2.2 ms spikes ~8 times a second; frames stay render bound (draw ≈ 9.2 ms), so frame averages move little. The field goal lags at most 13 frames (headless test).

## Final check at 67 uu (task-029-07, 2026-10-05)
Game defaults (67 uu, `arena_m1_fine`, no subdivide), all task-029 work in, editor `-game` 1920 × 1080, 5 s settle, 20 s measure, one launch per chain and dig run. Background: no game running; Steam, Epic launcher, Discord, Zoom, Brave idle; GPU 22 % from the desktop. ms avg / p99 / max; budget 16.6 avg, 25 p99 ([[task-029-fine-grid]]).

| Run | 100 robots | 400 robots |
|---|---|---|
| S0 frame | 9.12 / 11.05 / 12.1 | 9.97 / 12.98 / 15.8 |
| S2 frame | 9.57 / 11.74 / 12.8 | 11.16 / 14.93 / 59.7 |
| Chain 30 frame (32 explosions, 1713 blocks; event avg / max) | 9.36 / 11.50 / 18.4 (10.8 / 17.7) | 10.08 / 13.70 / 19.4 (13.2 / 19.4) |
| Dig 72 cells/s frame (1426 blocks; flow avg / max) | 9.72 / 12.11 / 13.8 (0.013 / 0.31) | 10.33 / 13.83 / 15.7 (0.013 / 0.31) |

- All within budget. Against the 029-01 / 029-02 rows at 100 robots: S0 9.24 → 9.12, S2 9.39 → 9.57, chain 9.31 → 9.36, dig 9.19–9.59 → 9.72 (± 0.4 ms, noise and the arc dig and beam pool added since); against the task-023-01 67 uu row at 400: S0 10.47 → 9.97, S2 12.30 → 11.16.
- S2 400 max 59.7 ms is one frame (p99 14.9); its cause was not captured (likely a GC, see [[sys-stress]] gotchas).

## Perf session (task-063-01, 2026-10-09)
Same commands as task-029-07, HEAD 94abe5e. PC not quiet: Brave (many tabs), Discord, Steam, Epic launcher, Creative Cloud, uTorrent, NVIDIA Share overlay and other Claude sessions running; CPU load 53–61 % idle, GPU 23 % from the desktop, on AC, Performance plan. The game window presented at 60 Hz (an external 60 Hz monitor on the NVIDIA GPU, the internal panel is 360 Hz): every run is capped at 1200 frames per 20 s, also with `-fullscreen` or `-WinX=1920`; nothing in the project sets a cap (`t.MaxFPS 0`, `r.VSync 0`, `FrameRateLimit 0`). Frame and draw times are therefore the 16.67 ms present interval, not the cost; the game thread is the usable value. ms avg / p99 / max (game avg).

| Run | 100 robots | 400 robots |
|---|---|---|
| S0 frame | 17.18 / 20.93 / 27.1 (9.29) | 19.64 / 24.86 / 28.2 (17.26) |
| S2 frame | 16.71 / 17.10 / 68.7 (6.92) | 16.67 / 18.76 / 21.0 (12.12) |
| Chain 30 (32 explosions, 1713 blocks; event avg / max) | 16.67 / 17.20 / 21.1 (7.00; 16.9 / 21.1) | 16.71 / 20.15 / 26.2 (12.26; 18.4 / 26.2) |
| Dig (1425 blocks; flow avg / max) | 16.67 / 17.26 / 19.3 (7.63; 0.019 / 0.31) | 16.72 / 19.72 / 27.1 (12.68; 0.020 / 0.31) |

- p99 within 25 ms everywhere; averages sit on the 60 Hz cap, so the 16.6 avg check cannot be judged against the task-029-07 table. GPU avg 12.9–14.4 ms (9.4 before) is read at low clocks under the cap. Game thread at 100: chain 6.22 → 7.00, dig 6.33 → 7.63 (task-029-02 rows).
- `FlowField.BudgetAt67uu` alone: 0 / 5 pass (full rebuild 4.74–5.65 ms, work p95 0.32–0.35 ms, goal age max 33–37 frames > 30); in the full run 6.65 ms, 38 frames. `FlowField.DestroyedRockOpensPocket` alone: 5 / 5 pass, and in the full run.
- Full headless run: 193 pass, 2 fail: `BudgetAt67uu`, `Integration.PressureHordeBanner` (icon positions twice the expected x, from the alert bar doubled in task-062-01, unrelated to the flow field).
- Verdict, load: `RRFlowFieldSubsystem` and `RRFlowFieldTests.cpp` are unchanged since task-029-02 (git diff from 9f1fd69); the test spawns no robot and destroys cells directly, so tasks 036, 041, 046 and 060 are not in its path. The same BFS takes 4.7–6.7 ms against 1.8–2.6 ms at task-029-02, and the goal age scales with it (≈ 13 frames then): the 30-frame check is wall-clock bound and fails on a loaded CPU.

## Worker-thread flow field in game (task-066-02, 2026-10-09)
Same commands as task-063-01, HEAD 659833f, `rr.FlowField.Async 1` (default, [[sys-flow-field]]). Owner's normal player profile, nothing closed: Brave, Discord, Steam, Epic launcher, Creative Cloud, uTorrent, NVIDIA Share, Armoury Crate, Logitech G Hub, NordVPN, ShareX, WhatsApp, Parsec, McAfee and other Claude sessions running; CPU load 0–23 % idle, Performance plan, external 60 Hz monitor on the NVIDIA GPU. This time the frames were not paced at 60 Hz (1800–2230 frames per 20 s, cause not identified), so frame times are the cost again. The stress does not log the flow-field goal age (the miner is static in every mode); flow = game-thread flow-field time of frames with work, avg / max. ms avg / p99 / max (game avg; flow), task-063-01 values beside.

| Run | 100 robots | task-063 100 | 400 robots | task-063 400 |
|---|---|---|---|---|
| S0 frame | 8.96 / 11.20 / 21.0 (4.89) | 17.18 / 20.93 / 27.1 (9.29) | 10.69 / 13.61 / 15.6 (9.26) | 19.64 / 24.86 / 28.2 (17.26) |
| S2 frame | 9.23 / 11.52 / 12.2 (5.16) | 16.71 / 17.10 / 68.7 (6.92) | 11.10 / 14.05 / 15.3 (9.62) | 16.67 / 18.76 / 21.0 (12.12) |
| Chain 30 (event avg / max) | 9.19 / 11.69 / 14.0 (5.14; 9.5 / 12.8; flow 0.032 / 0.095) | 16.67 / 17.20 / 21.1 (7.00; 16.9 / 21.1) | 10.08 / 12.88 / 18.9 (8.62; 11.4 / 18.9; flow 0.030 / 0.092) | 16.71 / 20.15 / 26.2 (12.26; 18.4 / 26.2) |
| Dig (1425 blocks) | 9.23 / 11.50 / 20.2 (5.44; flow 0.047 / 0.50) | 16.67 / 17.26 / 19.3 (7.63; 0.019 / 0.31) | 10.44 / 13.17 / 57.9 (9.28; flow 0.046 / 0.27) | 16.72 / 19.72 / 27.1 (12.68; 0.020 / 0.31) |

`rr.FlowField.Async 0` (time-sliced), same hour, 400 robots:

| Run | Async 1 | Async 0 |
|---|---|---|
| S0 frame (game) | 10.69 / 13.61 / 15.6 (9.26) | 10.12 / 12.75 / 13.5 (8.70) |
| Dig frame (game; flow work frames, avg / max) | 10.44 / 13.17 / 57.9 (9.28; 1915, 0.046 / 0.27) | 10.23 / 13.03 / 14.5 (9.04; 1431, 0.012 / 0.31) |

- All runs within 16.6 avg / 25 p99. Dig 400 Async 1 max 57.9 ms is one frame whose game time stays ≤ 15.8 ms: not game-thread work, cause not captured.
- Game thread is 1.8–8 ms lower than task-063 everywhere, also in S0 and S2 where the flow field does no work: the PC load differs (idle CPU 0–23 % vs 53–61 %), so the gain cannot be credited to the worker.
- Worker path on the game thread: work on every dig frame (launch copies of the field), avg 0.046 ms vs 0.012 ms sliced, max 0.27 vs 0.31 ms; negligible against ≈ 9 ms of game thread. Async 1 vs 0 differences (0.2–0.6 ms) are within run noise.

## Real-game stress and limits (task-067-01, 2026-10-09)
`rr.Stress.Run <cap> <settle> 60 1 game` ([[sys-stress]] game mode), HEAD e584b09, editor `-game` 1920 × 1080, Niagara on, HUD and minimap shown (checked each step), miner on the pacing bot run-and-collect profile with `rr.Pacing.RunLookahead 0` (pushes into walls, so it digs), weapons firing, real kills, XP cubes, damage numbers and kamikaze explosions; forced horde 20 s into the measure. Owner's normal profile, nothing closed. Not paced at 60 Hz (5200–5300 frames per 60 s at 150; `t.MaxFPS 0`, `r.VSync 0`). ms avg / p99 / max; flow = game-thread flow-field ms of frames with work, avg / max; spikes = frames > 25 ms (wave spawn / GC / untagged).

| Run | Robots avg (max) | Frame | Game | Draw avg | GPU avg | Flow | Spikes | Horde |
|---|---|---|---|---|---|---|---|---|
| 150 seed 1 (settle 15) | 114 (150) | 11.34 / 20.29 / 48.1 | 10.40 / 20.11 | 10.23 | 7.62 | 0.014 / 0.17 | 34 (24 / 1 / 9) | 42 planned, 24 |
| 150 seed 2 | 119 (150) | 11.45 / 19.36 / 52.5 | 10.81 / 19.61 | 10.24 | 7.83 | 0.013 / 0.12 | 25 (19 / 1 / 5) | 41, 27 |
| 150 seed 3 | 116 (150) | 11.49 / 20.11 / 48.6 | 10.63 / 20.00 | 10.33 | 7.94 | 0.013 / 0.11 | 30 (24 / 1 / 5) | 60, 60 |
| 250 (settle 30) | 212 (250) | 15.52 / 31.93 / 66.3 | 15.49 / 32.31 | 10.64 | 7.64 | 0.013 / 0.11 | 97 (69 / 1 / 27) | 41, 42 |
| 400 (settle 30) | 293 (400) | 20.21 / 39.81 / 95.0 | 20.21 / 40.35 | 10.99 | 7.56 | 0.013 / 0.09 | 322 (112 / 1 / 207) | 41, 41 |

Spike lines (frame, game, draw, GPU ms): GC at t 46 s, 150: 48.1 / 46.8 / 10.3 / 8.1; wave spawn, 150 (13 robots in one frame): 41.1 / 43.8 / 9.5 / 8.0; untagged, 250 (robots 250 → 126): 66.3 / 66.7 / 10.3 / 7.5; untagged, 400: 95.0 / 95.1 / 10.9 / 7.2.

- Limits: no step holds 16.7 ms p99 (150 is at 19.4–20.3). Highest step under 33 ms p99: 250 (31.9). 400 exceeds it (39.8): ramp stopped, 600 and 800 not run.
- Bound: the game thread at every step (game ≈ frame; draw 10–11 ms, GPU 7.6 ms flat from 150 to 400). Flow field negligible (≤ 0.17 ms, worker thread). Game thread at 150 real game ≈ 10.5 ms vs 4.9 ms for 100 static S0 robots (task-066-02).
- Spikes: robot spawning costs ≈ 1.3 ms of game thread per robot (a wave or the harness fill of 10–15 robots in one frame = 25–44 ms frames); one full GC per run (≈ 61 s, 47–60 ms); untagged frames grow with the count (9 → 207), several at falling robot counts (mass kills, kamikaze chains); cause not traced.
- Hordes: drawn 41–60, but a runner heading toward the horde side brings planned cells inside the spawn margin and the spawner skips them (24–27 spawned in 2 of 5 runs).
- The harness fill (a director wave every 0.5 s while under the cap) spawns more often than the director; real games see the same per-wave spike every 5.5–7 s.

## Packaged build and spike causes (task-068-01, 2026-10-10)
Staged Development build ([[build-toolchain]]; Test cannot be built with the launcher engine: "Targets cannot be built in the Test configuration with this engine distribution"), HEAD 0525a68, same commands as task-066-02 (S0 100+400, dig 400) and task-067-01 (game 250, `rr.Pacing.RunLookahead 0`), one launch each, all runs within 11 min. Owner's normal profile, nothing closed. ms avg / p99 / max (game avg); game spikes = frames > 25 ms (wave / GC / untagged).

| Run | Packaged | Editor `-game` | Packaged, traced |
|---|---|---|---|
| S0 100 | 8.45 / 9.70 / 14.9 (3.16) | 8.93 / 11.07 / 18.1 (5.15) | 9.01 / 10.73 / 22.4 (3.93) |
| S0 400 | 9.18 / 10.48 / 13.1 (6.06) | 10.34 / 12.94 / 15.3 (9.10) | 10.05 / 11.72 / 13.5 (7.37) |
| Dig 400 | 9.77 / 11.28 / 12.7 (6.12) | 9.87 / 12.00 / 17.9 (8.84) | 10.07 / 11.49 / 12.4 (7.28) |
| Game 250 | 11.27 / 22.31 / 44.3 (10.42); 20 spikes (8 / 0 / 12) | 14.05 / 30.14 / 84.2 (13.96); 93 (64 / 1 / 28) | 12.46 / 23.33 / 34.9 (12.11); 32 (28 / 0 / 4) |

- Packaged vs editor: game thread −2.0 ms (S0 100) to −3.5 ms (game 250); frames render bound in S0 and dig (draw ≈ frame), so frame gains are 0.1–1.2 ms there; game 250 p99 30.1 → 22.3 ms, under the 25 ms budget.
- S0 100 21 ms frame (task-066-02): reproduced in the trace (22.4 ms, game work 3.4 ms). The game thread waits 18 ms in `Sync_RenderingThread`; the render thread waits 14.5 ms on the RHI occlusion fence; `FD3D12Queue::ExecuteCommandLists` blocks 17.7 ms and the GPU frame stretches to 22 ms (TSR 14 ms vs 2.3). Cause: GPU/driver submission stall outside the game; no GC, PSO, shader or loading event.
- Dig 400 57.9 ms frame (task-066-02): not reproduced in 3 runs (max 12.4–17.9 ms); its game time was ≤ 15.8 ms, same signature as the S0 stall.
- Game 250 spikes, untagged ones explained ([[perf-trace-real-game]]): robot waves (0.74 ms per robot spawned) and XP bursts (robots killed together drop all their cubes on one frame, 195 cubes and 67 destroys = 34.9 ms). Packaged GC: 5.3 ms then 2.1 ms purge on 20 frames, never a spike; the 47–60 ms GC frames of task-067-01 are editor-only.
- Untraced runs also show 5–8 consecutive render-bound frames (draw 22–32 ms, GPU normal) early in the game measure; the trace shows render-thread frames up to 18 ms from RHI fence waits and ray-tracing instance gathering (3.5 ms), cause not isolated.

## Robot light movement (task-071-01, 2026-10-10)
Editor `-game`, same commands as task-067-01 (game 250, settle 30, measure 60, `rr.Pacing.RunLookahead 0`) and task-066-02 (S0 400). Before = 3ad37c9, after = b28e4bf (`rr.Enemy.LightMovement` 1, [[sys-enemies-movement]]), same session within 30 min, owner's normal profile, nothing closed. ms avg / p99 / max (game avg / p99); movement = Insights `-statnamedevents` trace of a third game 250 run, inclusive ms per frame of `UCharacterMovementComponent_TickComponent` before, `URRRobotMovementComponent_Tick` after (engine-path fallback 0.21 included).

| Run | Before | After |
|---|---|---|
| Game 250 #1 | 17.18 / 37.02 / 85.0 (17.14 / 37.02); 140 spikes (80 fill) | 14.79 / 32.67 / 67.5 (14.72 / 32.67); 100 (70) |
| Game 250 #2 | 17.24 / 38.98 / 109.4 (17.16 / 39.02); 188 (80) | 13.43 / 29.81 / 98.3 (12.76 / 30.40); 75 (51) |
| Game 250 traced | 16.12 / 35.20 / 94.9 (16.06 / 35.76) | 15.64 / 32.96 / 76.1 (15.48 / 33.53) |
| S0 400 | 10.40 / 13.03 / 15.0 (9.12 / 12.49) | 10.24 / 12.67 / 15.8 (7.84 / 10.23) |
| Movement ms / frame (traced, ≈ 205 robots) | 5.56 (PhysWalking 2.08, FindFloor 0.30) | 4.39 |

- Game thread at 250: −2.4 to −4.4 ms avg, −4.4 to −8.6 ms p99 untraced (run-to-run spread ≈ 2 ms, robots alive 203–210 in every run); S0 400 −1.3 ms avg, −2.3 ms p99. Frames stay game-thread bound at 250; S0 400 frames are render bound (draw ≈ frame).
- Movement scope −1.2 ms per frame traced: walking physics, floor search and the separate rotation move are gone; what remains is the swept move itself (child transforms of the parts, `HitCapsule` overlap update), ≈ 21 µs per robot.
- Spikes: fewer frames over 25 ms (75–100 vs 140–188), same causes (wave fill, XP bursts, one GC); max frame unchanged in kind.
- Kills 860–1064 per run (before 940–1099): robots still reach the miner around rock (also `RogueRigs.Enemies.LightMovementAroundRock`). Miner damage is not logged: the pacing harness keeps the miner invulnerable.

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

## Verdict
- 200 uu: holds; the navigation fix would also help it (one destruction 0.25 ms here, 0.04–0.07 ms measured with the fix at the smaller sizes).
- 100 uu: steady state and dig hold. Explosion chains break (52 ms frames) and are fixed by turning off navigation relevance on the grid components (≈ 1 h with test), back to the 200 uu level. Recommended.
- 67 uu: steady state holds (+1.5 ms GPU). After the navigation fix, chains reach 19–21 ms and fast dig 18.7 ms average (60 fps lost). Needs HISMs per chunk, e.g. 16 × 16 cells, with blocks kept out of distance fields (≈ 10–12 h), and the flow field computed on a worker thread or repaired incrementally (2.6 ms per rebuild, every frame while digging; ≈ 6–10 h). Total ≈ 17–23 h, the render gain not yet measured.
- Any smaller size also needs cell-counted tunables rescaled (spawner band, drops per ore cell, dig health): gameplay task, not measured here.

## Relations
- [[performance-index]] — parent index.
- [[sys-stress]] — subdivide, bench, chain and dig tools used here.
- [[stress-opt-m1]] — the 200 uu tables of the same procedure.
- [[sys-grid]] — the instanced components whose costs are measured.
- [[task-023-01-cell-stress]] — the task that produced this report.
- [[sys-flow-field]] — the incremental field measured in the task-029-02 section.
- [[task-029-fine-grid]] — the global task whose budget the final 67 uu check measures.
- [[task-063-01-perf-session]] — the task that produced the 2026-10-09 perf session section.
- [[task-066-02-perf-check]] — the task that produced the worker-thread flow field section.
- [[task-067-01-real-game-stress]] — the task that produced the real-game stress and limits section.
- [[task-068-01-shipping-insights]] — the task that produced the packaged build and spike causes section.
- [[perf-trace-real-game]] — the packaged real-game trace behind the spike causes.
- [[task-071-01-robot-light-movement]] — the task that produced the robot light movement section.
- [[sys-enemies-movement]] — the light movement measured in the task-071-01 section.
- [[task-091-01-robot-pool]] — the task that produced the robot pool section.
