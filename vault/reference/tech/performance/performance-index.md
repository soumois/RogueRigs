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
- [[perf-leads]] — optimization leads ranked after task-018, with what is done or closed.
- [[stress-test-m1]] — scenarios, thresholds, target machine, stress procedure, baseline table and ceiling.
- [[stress-final-m1]] — final run after task-016: S0, S2 fire, S7 burst tables, baseline comparison, 100-cap verdict.
- [[perf-trace-m1]] — Insights trace at 400 robots: game-thread ms per timer, CharacterMovement breakdown, 61 s GC hitch, capture commands.
- [[stress-opt-m1]] — after task-018: S0, S2, S7 vs stress-final-m1, new ceilings, staged Development build ceilings.
- [[stress-damage-numbers]] — Niagara damage numbers: screenshot findings, asset defects for the bp agent, S2 100/400 with numbers on vs off.
- [[stress-grid-cells]] — 200 vs 100 vs 67 uu cells: frame times, explosions, dig, flow field, memory, fixes, verdict; real-game stress and robot-count limits (task-067-01).
- [[perf-trace-real-game]] — packaged real-game trace at 250 robots: game-thread cost ranking, wave spawn and XP burst costs, GC, spike causes.
- [[robot-components-audit]] — robot components per class, overlap flags, what needs them, movement cost ranking (task-093-01).
