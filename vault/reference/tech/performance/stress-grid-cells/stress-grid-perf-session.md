---
id: stress-grid-perf-session
description: "Perf session 063-01, worker-thread flow field 066-02 and real-game stress limits 067-01 (2026-10-09). Open for the limits found in game."
parent: stress-grid-cells
kind: doc
---
# Grid stress — perf session

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

## Relations
- [[stress-grid-cells]] — parent hub: summary and verdict.
- [[sys-flow-field]] — referenced above.
- [[sys-stress]] — referenced above.
