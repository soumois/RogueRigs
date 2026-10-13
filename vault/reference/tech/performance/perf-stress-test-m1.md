---
id: perf-stress-test-m1
description: "M1 stress test: scenarios S0 to S8, thresholds, target machine, rr.Stress procedure, baseline frame-time table, ceiling, and the final-run verdict."
parent: performance-index
kind: doc
---
# M1 stress test

Status: provisional. It measures the full cost of an enemy, not GAS alone.

- Thresholds: 16.6 ms average, 25 ms at the 99th percentile, on the target machine.
- Target machine (owner, 2026-10-03): this development PC, i9-12900H (14 cores), RTX 3070 Ti Laptop GPU, 32 GB; 1920×1080 windowed.

## Scenarios
- S0: 200 enemies moving and animating, without ASC (baseline).
- S1: same with ASC; measures the GAS delta.
- S2: 200 enemies, continuous fire.
- S3: 4 weapons.
- S7: mass death, explosion queue.
- S8: one global upgrade broadcast.
- S6 only if DoT comes back into scope. No S4 or S5.
- M1 budget covers S0, S2 and S7 ([[gas-scope]]).

## Method
- Standalone process (`-game`), `stat unit` and Unreal Insights.
- Packaged build for the final confirmation.
- No pooling or A/B tooling built for the test.
- Tool: [[sys-stress]]. Build `RogueRigsEditor` (editor closed), then from the repo root `powershell -NoProfile -ExecutionPolicy Bypass -File Scripts/Run-StressTest.ps1 [-Counts 50+100+200+300+400] [-Settle 5] [-Measure 20]`. It runs `UnrealEditor.exe RogueRigs.uproject L_ArenaM1 -game -ResX=1920 -ResY=1080 -windowed` with `t.MaxFPS 0`, `r.VSync 0`, `rr.Stress.Run <counts> <settle> <measure> 1`, waits for the exit and prints the table; CSV in `Saved/Stress/` (not committed).

## Baseline (task-013-01, 2026-10-03)
Development editor binaries in `-game`, chasers only, miner idle in peace mode (invulnerable, no fire), timed spawner off, robots topped up step by step, all crowding the miner. Robots carry their ASC: S0 with GAS, not the ASC-free S0. Milliseconds, avg / p99:

| Chasers | Frame | Game | Draw | GPU |
|---|---|---|---|---|
| 50 | 8.06 / 9.59 | 4.11 / 6.30 | 8.05 / 9.61 | 7.13 / 8.48 |
| 100 | 8.64 / 10.72 | 5.72 / 9.07 | 8.62 / 10.70 | 7.67 / 9.42 |
| 200 | 9.51 / 12.82 | 8.67 / 12.90 | 8.91 / 10.69 | 8.39 / 10.02 |
| 300 | 9.78 / 12.53 | 9.42 / 12.76 | 8.38 / 10.59 | 8.59 / 9.96 |
| 400 | 11.27 / 13.75 | 11.26 / 13.93 | 6.11 / 7.57 | 8.40 / 9.70 |

Beyond the range (other runs, frame avg / p99, game thread equal): 400 fresh spawn 13.06 / 15.70; 500: 16.54 / 21.71 and 13.49 / 15.94; 600: 16.66 / 20.79; 800: 23.98 / 50.08; 1000: 26.29 / 36.03; 1200: 29.13 / 48.64.

- 400 passes with margin. Ceiling ≈ 500 chasers: the average reaches 16.6 ms between 500 and 600; p99 breaks 25 ms by 800.
- Game-thread bound from 200 robots on (frame = game); draw and GPU stay 6–9 ms whatever the count.
- Spread between runs at one count: ±2–3 ms game thread (crowd layout, fresh spawn vs top-up). Leads: [[perf-risks]].

## Final (task-013-02, 2026-10-03)
- S0 +0.4 to +2.3 ms vs baseline (400: 13.6 / 17.5), ceiling still ≈ 500–600; S2 continuous fire 400: 15.8 / 20.1; S7 200 kamikazes + 16 barrels: p99 23.4, max 49.6 (burst frame). Periodic ≈ 47 ms hitches every 61 s (likely GC). M1 cap 100 alive: safe with ≈ 40 % average margin. Tables: [[perf-stress-final-m1]].

## Relations
- [[performance-index]] — parent index.
- [[perf-stress-final-m1]] — final S0, S2, S7 tables, comparison and the 100-cap verdict.
- [[perf-risks]] — what to do when a scenario fails; leads from the baseline.
- [[sys-stress]] — the commands and script that run the test.
