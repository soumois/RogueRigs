---
id: perf-xp-pickups-glow-table
description: "XP pickup glow runs: launch method, settle and measure windows, and the 0–400 pickups table, bloom on and off, frame, game, draw, GPU ms."
parent: perf-xp-pickups-glow
kind: doc
---
# XP pickups glow: method and table (task-138-02)

## Method
- [[perf-stress-test-m1]] machine, editor binary `-game` 1920×1080 windowed, owner apps open (Steam, Discord, Brave, Epic launcher), no other Unreal process.
- [[sys-stress]] launcher, `-Counts 1+1+1+1+1 -Settle 0 -Measure 10`, peace, spawner off; `-PreExec "rr.Debug.Delayed 2 rr.Stress.Pickups <nuts> <chips>"` (one chip per 16: 47+3, 94+6, 188+12, 375+25; none magnetized, miner level 1 throughout). Bloom off: `r.BloomQuality 0` first in `-PreExec`.
- `rr.Stress.Run` needs one robot per step: one idle chaser in every run, reference included.
- Settle 30 s = windows 1–3; window 3 within 0.5 ms of the measure; windows 2–5 drift up ≈ 0.1 ms per 10 s in every run, pickups or not. Measure 20 s = windows 4–5: average frame-weighted, p99 = the higher window p99 (slightly pessimistic).
- CSV in `Saved/Stress/` (not committed).

## Table (ms, average / p99)

| Pickups | Bloom | Frame | Game | Draw | GPU |
|---|---|---|---|---|---|
| 0 | on | 9.00 / 11.27 | 3.82 / 5.70 | 8.98 / 11.29 | 7.50 / 8.05 |
| 50 | on | 8.51 / 10.56 | 3.54 / 5.22 | 8.50 / 10.50 | 7.39 / 7.83 |
| 100 | on | 8.31 / 10.38 | 3.51 / 5.17 | 8.30 / 10.36 | 7.06 / 7.43 |
| 200 | on | 8.32 / 9.88 | 3.20 / 4.92 | 8.31 / 9.83 | 7.34 / 7.81 |
| 400 | on | 8.56 / 10.07 | 3.64 / 5.26 | 8.55 / 10.05 | 7.55 / 7.92 |
| 0 | off | 8.26 / 9.73 | 3.53 / 5.04 | 8.25 / 9.65 | 7.36 / 7.80 |
| 50 | off | 8.32 / 9.70 | 3.47 / 5.08 | 8.31 / 9.71 | 7.43 / 7.91 |
| 100 | off | 8.35 / 9.74 | 3.41 / 5.01 | 8.34 / 9.73 | 7.47 / 8.00 |
| 200 | off | 8.39 / 9.95 | 3.57 / 5.34 | 8.38 / 9.86 | 7.46 / 7.90 |
| 400 | off | 8.44 / 9.88 | 3.53 / 5.26 | 8.42 / 9.82 | 7.51 / 7.90 |

100 bloom on is the warm run. No frame over 25 ms in any measure window (max 18.7 ms, pulse run).

## Relations
- [[perf-xp-pickups-glow]] — parent: verdict, cold vs warm, pulse pass.
- [[perf-stress-test-m1]] — thresholds and launcher method.
- [[sys-stress]] — the commands used.
