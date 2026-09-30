---
id: stress-test-m1
description: "M1 stress test: scenarios S0 to S8, frame-time thresholds, target machine (open), measuring in -game with stat unit and Insights."
parent: performance-index
kind: doc
---
# M1 stress test

Status: provisional. It measures the full cost of an enemy, not GAS alone.

- Thresholds: 16.6 ms average, 25 ms at the 99th percentile, on the target machine.
- Target machine and resolution: open; decide before S0.

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

## Relations
- [[performance-index]] — parent index.
- [[perf-risks]] — what to do when a scenario fails.
