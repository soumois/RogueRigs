---
id: perf-xp-pickups-glow
description: "XP nut and chip glow cost (task-138-02): 0–400 pickups, bloom and pulse on/off, cold vs warm, verdict per threshold: no measurable cost."
parent: performance-index
kind: doc
---
# XP pickups glow: cost check (2026-10-13)

Burst of XP nuts and chips ([[bp-pickup-xp]]) held still around an idle miner, bloom on and off, measured with [[sys-stress]] on the [[perf-stress-test-m1]] machine. Method and full table: [[perf-xp-pickups-glow-table]].

## Summary (frame ms, average / p99)
- Bloom on: 0 → 9.00 / 11.27; 50 → 8.51 / 10.56; 100 → 8.31 / 10.38; 200 → 8.32 / 9.88; 400 → 8.56 / 10.07.
- Bloom off: 0 → 8.26 / 9.73; 400 → 8.44 / 9.88 (+0.18 ms frame, +0.15 ms GPU).
- Game thread 3.2–3.8 ms, GPU 7.1–7.6 ms throughout; frame = render thread (draw).

## Cold vs warm, the 138-01 lead
- 100, bloom on: cold (first launch) 8.19 / 10.05, warm 8.31 / 10.38; no drop in either, no shader compile logged.
- The "10 fps" of task-138-01 was a misread: the bracketed log frame number is the frame counter modulo 1000; 436 → 513 over 8 s is 1077 frames, ≈ 135 fps. Zen DDC wrote nothing during that run.

## Pulse pass and on-screen worst case
- PulseAmount 0 on M_XPNut_hole, M_XPChip_core_bright, M_XPChip_core_dark (headless Python, then restored with git, never committed), 200, bloom on: 8.52 / 10.18 (pulse on: 8.32 / 9.88).
- `rr.Stress.Pickups` fills cells outward from 544 uu, so part of 200–400 is off screen. Diagnostic: 375 nuts + 25 chips placed by `rr.Debug.SpawnNear` on rings at 560–800 uu (in view): 8.42 / 10.22, bloom on.

## Verdict
- No measurable cost: every difference is within the ±0.5 ms run spread (the 0-pickup bloom-on run is the slowest). Bloom and pulse change nothing measurable; no cause to chase.
- 16.6 ms average: pass at every count, worst 9.00. 25 ms p99: pass, worst 11.27. The thresholds hold through 400 pickups, the largest count tested; no ceiling found.
- Not covered: cooked build, a truly cold shader cache (DDC not cleared), Insights per-pass GPU split.

## Task-269 note
- Glow disc radius cut (nut 50 -> 22 cm, chip 68 -> 32 cm, owner 2026-10-21): fewer overdrawn additive pixels per pickup, so the cost could only fall; not re-measured.
- Reverted by task-277-01 (owner 2026-10-21): radii back to 50 (nut) and 68 (chip), so the cost is back to the measured state above (the figures of this note hold).

## Relations
- [[performance-index]] — parent index.
- [[perf-xp-pickups-glow-table]] — method, windows and the full 0–400 table, bloom on and off.
- [[perf-stress-test-m1]] — thresholds, target machine and launcher method used.
- [[sys-stress]] — rr.Stress.Pickups and the run modes used.
- [[bp-pickup-xp]] — the nut and chip Blueprints and glow materials measured.
- [[wishlist]] — the cost check line this closes.
