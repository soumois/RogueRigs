---
id: performance-index
description: "Performance at horde scale: cost risks at 150–200 enemies with fallbacks, and the M1 stress test protocol and thresholds."
parent: tech-index
kind: note
---
# Performance

## Relations
- [[tech-index]] — parent index.
- [[perf-risks]] — likely cost centers outside GAS, with measures and fallbacks.
- [[perf-leads]] — branch of performance improvement leads kept for later (M1 leads, robot rendering, death debris). Open before any perf work.
- [[perf-stress-test-m1]] — scenarios, thresholds, target machine, stress procedure, baseline table and ceiling.
- [[perf-stress-final-m1]] — final run after task-016: S0, S2 fire, S7 burst tables, baseline comparison, 100-cap verdict.
- [[perf-trace-m1]] — Insights trace at 400 robots: game-thread ms per timer, CharacterMovement breakdown, 61 s GC hitch, capture commands.
- [[perf-stress-opt-m1]] — after task-018: S0, S2, S7 vs stress-final-m1, new ceilings, staged Development build ceilings.
- [[perf-stress-damage-numbers]] — Niagara damage numbers: screenshot findings, asset defects for the bp agent, S2 100/400 with numbers on vs off.
- [[perf-stress-grid-cells]] — 200 vs 100 vs 67 uu cells: frame times, explosions, dig, flow field, memory, fixes, verdict; real-game stress and robot-count limits (task-067-01).
- [[perf-trace-real-game]] — packaged real-game trace at 250 robots: game-thread cost ranking, wave spawn and XP burst costs, GC, spike causes.
- [[perf-projectile-pool-evaluation]] — whether to pool projectiles: spawns per second, spawn and destroy ms, burst frames, recommendation (task-111-02).
- [[perf-xp-pickups-glow]] — XP nut and chip glow cost: 0–400 pickups, bloom and pulse on/off, cold vs warm, verdict (task-138-02).
- [[perf-robot-components-audit]] — robot components per class, overlap flags, what needs them, movement cost ranking (task-093-01).
- [[perf-stress-chaser-meshy]] — Meshy chaser and death effects (task-181-01): S0, S2, S7 chaser burst, game 100–300, puff and debris skips, M1 comparison.
