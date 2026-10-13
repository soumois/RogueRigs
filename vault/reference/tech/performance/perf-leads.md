---
id: perf-leads
description: "Optimization leads ranked by expected gain after task-018 (stress-opt-m1), with what task-018 already did and closed; open before any perf work."
parent: performance-index
kind: doc
---
# Optimization leads (after task-018)

Ranked from [[perf-stress-opt-m1]] and the per-timer trace [[perf-trace-m1]]. Gains are estimates; no optimization is coded without the owner.

## Done or closed in task-018
- Enemy-enemy capsule collision: robot-robot sweeps skipped by mask filter, lighter RVO (task-018-02, [[sys-enemies]]); ≈ 0.3–0.5 ms at 200–400, inside the run spread.
- CharacterMovement per robot (was 53 % of the game thread at 400): empty mesh stripped, floor cache, off-screen far movement 0.1 s tick (task-018-03, [[sys-enemies]]). With 018-02: game thread −7.2 ms at 600 in S0, ceiling ≈ 600 → ≈ 1500 (editor), ≈ 1600 (staged).
- 61 s GC hitch: editor-process only; staged GC frame 10.8–16.4 ms (task-018-04). Editor `-game` max frames of 35–47 ms every 61 s are not a game cost.

## Remaining, ranked
1. Rendering floor: below ≈ 600 robots the frame equals the render thread (draw ≈ 8–9 ms, GPU ≈ 7–9 ms at 50–500) while the game thread is 4–8 ms; this is the real-play bottleneck at the M1 cap of 100. Lumen GI and reflections, virtual shadows, hardware ray tracing ([[perf-risks]] rendering row): test without them or with Lumen Lite. −2 to −4 ms frame. 0.5 h.
2. Fire cost: S2 − S0 = +4.6 ms game and +3 ms draw p99 at 600 (knockback movement, damage text, projectiles, end-of-frame render updates). Projectile and damage-text pools, trace S2 at 600 first. −1 to −3 ms. 1–2 h.
3. Render thread in explosion chains: draw p99 9.8 / 12.6 / 18.1 ms at 66 / 116 / 216 queued explosions (FX actor per explosion, terrain instance updates). Pool or cap FX, batch instance updates. −3 to −6 ms draw p99. 1–2 h.
4. High-count p99 spread: above ≈ 1200 robots p99 grows faster than avg (staged 1800: avg 16.0, p99 36.5 ms). Cause unmeasured; candidate: far-movement 0.1 s ticks bunching on the same frames. Trace before acting; only matters far above the cap.
5. Burst death frame: ≈ 0.20 ms per robot dying in one frame (100 at once: 24 ms game). Spread death work or pool pickups, if an upgrade can clear a crowd. 1 h.
6. Remaining CharacterMovement: move sweeps (needed for walls and the miner). Fallback: flat-floor pawn movement ([[perf-risks]] movement row), 2–3 h; no longer needed for M1.
7. Fixed game-thread overhead: viewport draw 0.9–1.4 ms, Slate 0.35–0.5 ms, skeletal mesh ticks 0.6–0.7 ms. −0.5 ms. 0.5 h.
8. RVO: 0.33 ms at 400. Drop unless the crowd grows.

## Relations
- [[performance-index]] — parent index.
- [[perf-risks]] — cost centers and fallbacks these leads refer to.
- [[perf-stress-opt-m1]] — the tables that ranked these leads.
- [[perf-trace-m1]] — per-timer costs behind the estimates.
- [[perf-stress-final-m1]] — pre-optimization tables.
