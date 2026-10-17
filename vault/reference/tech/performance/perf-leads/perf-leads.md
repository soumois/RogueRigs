---
id: perf-leads
description: "Index of performance improvement leads not yet coded: M1 leads after task-018, robot rendering, robot death debris. Open before any perf work."
parent: performance-index
kind: note
---
# Performance leads

Ideas to win frame time, kept until the owner picks one. A lead is only coded on the owner's request. Each node gives the measured cost behind it, the options with their expected effect, and what to measure first. Add a node here whenever a measurement reveals a cost worth recovering later.

## Relations
- [[performance-index]] — parent index.
- [[perf-leads-m1]] — leads ranked after task-018 (rendering floor, fire cost, explosion chains, burst death frame), with what is done or closed.
- [[perf-lead-debris-gpu]] — robot death debris on the GPU with wall collision costs about 3 ms per frame in game mode: three options to recover it (task-193).
- [[perf-lead-lumen-robots]] — draw and GPU grow about 5.6 µs per robot since the Meshy chaser, Lumen card captures suspected (task-181).
- [[perf-lead-pickup-simulated-roll]] — landed pickups as Chaos bodies (task-229): fallback of a roll simulated in the pickup loop if the cost is too high.
