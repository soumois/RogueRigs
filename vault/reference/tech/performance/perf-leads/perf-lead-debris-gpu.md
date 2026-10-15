---
id: perf-lead-debris-gpu
description: "Lead: GPU robot death debris with wall collision costs about 3 ms per frame in game mode; three options to recover it, none tested."
parent: perf-leads
kind: note
---
# Lead: robot death debris on the GPU (task-193)

- measured ([[perf-stress-chaser-meshy]], task-193-01, valid PC state): GPU emitter with distance field wall collision against the CPU floor-only debris of task-182-03, 80 debris systems of 4.9 s: game cap 200 frame 11.08 → 14.15 ms avg; cap 300 12.96 → 15.74 ms avg and p99 21.4 ms (above the task-181 limits); S7 200 worst frame 25.9 → 54.1 ms.
- where: mostly the render thread, about 68 µs per active GPU debris system (a GPU-sim mesh renderer adds one render pass per active system and view; CPU debris cost 16–24 µs); GPU +1.3–1.5 ms, of which the global distance field update 0.9 ms. Attribution from two traces of different scenes: approximate; the total is measured.
- owner decision kept: walls are wanted ([[enemy-death]], owner 2026-10-15); this lead is about paying less for them.

## Options (owner 2026-10-15: kept for later, none chosen)
1. Fewer active systems: debris pool back to 40 or a shorter life. Cost scales with active systems; more deaths without debris. Cheapest to try (Blueprint defaults, Niagara lifetime).
2. One shared Niagara system for all deaths, each death adding its pieces to it: one render pass instead of up to 80. Most promising; needs C++ (spawn into a shared component) and Niagara rework (per-death parameters as particle attributes).
3. CPU debris with a cheaper wall test, e.g. reading a one-pixel-per-cell grid texture (as the minimap): back to the task-182-03 cost plus a little, no distance field update.

Measure any option with the task-182-03 / 193 method (game caps 200 and 300, S7 200), PC control step first (Armoury Crate in Performance).

## Relations
- [[perf-leads]] — parent index.
- [[perf-stress-chaser-meshy]] — the measurements.
- [[bp-robot-debris]] — the system as built.
- [[sys-actor-pool]] — the Niagara pool the options would change.
