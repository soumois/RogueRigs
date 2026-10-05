---
id: task-029-fine-grid
description: "Global task: 67 uu cells with chunked rendering, incremental time-sliced flow field, converted layout, wall-contact arc mining, re-measured stress."
parent: tasks-index
kind: task
status: todo
---
# Fine grid and arc mining

- request: "67uu, si l'update du flow field est un soucis, est-ce qu'on ne sait pas l'étaler sur quelques frames ? Comme on ne fait que rajouter des chemins et pas en couper, ça n'est pas grave, si les robots prennent 2/3 frames à changer leur déplacement dans le pire des cas." (2026-10-06, answer to the stress-grid-cells report; the owner had launched all lots on 2026-10-05).
- goal: the arena uses 67 uu cells (240 × 240) at 60 fps, destruction looks finer and organic, the miner digs an arc of blocks just by touching a wall.
- acceptance: (1) build exits 0; all tests pass, updated ones included; (2) at 67 uu: S0/S2 400 robots, chain of 30 explosions and fast digging within 16.6 ms avg and 25 ms p99 on this PC; (3) flow field: incremental open-only update spread over frames under a per-frame budget, robots follow new paths within a few frames (tests); (4) L_ArenaM1 layout at 240 × 240, passages ≥ 3 cells, validator passes; (5) cell-based values rescaled (spawn band, console and shuttle distances, marked radius, drops); (6) arc mining by contact at the GDD speeds (tests); (7) PIE look; (8) docs; lint clean; committed.
- gdd_check: Read [[arena-grid]], [[drone]], [[destruction-rules]], [[ore-blocks]], [[stress-grid-cells]], [[drgs-analysis]]. Owner chose 67 uu and time-sliced flow field (2026-10-06); conflict "every unit fits a one-cell corridor" resolved as passages ≥ 3 cells (provisional, to confirm by the owner); dig speeds measured on the DRGS run (0.5 s per rock block, 3–4 at once).
- confirmed_by_owner: yes (2026-10-06, lot launched by the owner on 2026-10-05, size chosen on 2026-10-06)
- milestone: M2
- next_step: task-029-01-grid-render
- result: open

## Subtasks
1. task-029-01-grid-render — cpp: grid not navigation-relevant, chunked instanced meshes, no distance fields, measure
2. task-029-02-flow-field-incremental — cpp: incremental open-only flow field, time-sliced under a budget, tests, measure
3. task-029-03-layout-67 — level-design: L_ArenaM1 layout at 240 × 240, organic edges, passages ≥ 3 cells, validator
4. task-029-04-switch-67 — cpp: cell size 67 uu as default, load the new layout, rescale cell-based values, tests
5. task-029-05-arc-mining — cpp: wall-contact arc mining by the drone at the new speeds, tests
6. task-029-06-bp-fine-grid — bp: block meshes and materials at 67 uu, arc dig visual, PIE
7. task-029-07-final-check — cpp: integration tests and final stress at 67 uu

## Relations
- [[tasks-index]] — parent index.
- [[task-029-01-grid-render]] — step 1, cpp.
- [[task-029-02-flow-field-incremental]] — step 2, cpp.
- [[task-029-03-layout-67]] — step 3, level-design.
- [[task-029-04-switch-67]] — step 4, cpp.
- [[task-029-05-arc-mining]] — step 5, cpp.
- [[task-029-06-bp-fine-grid]] — step 6, bp.
- [[task-029-07-final-check]] — step 7, cpp.
