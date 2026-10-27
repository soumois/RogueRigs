---
id: task-324-heal-numbers
description: "Global task: green floating +N heal number on the miner when he collects a heal pickup, rendered like the damage numbers."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Heal numbers on the miner

- request: "Lorsque le joueur ramasse des soins, il faudrait un texte similaire au dégât, mais avec "+" et en vert sur le personnage pour indiquer qu'il a récupéré des pv" (owner, 2026-10-27). Owner answer to Q1 to Q6 and the plan (2026-10-27): "ok", every default below accepted, plan of 2 subtasks confirmed.
- goal: each time a heal pickup heals the miner, a floating number "+N" appears on the miner, green, with the same look and motion as a damage number (digit atlas, arc, pop, fade).
- acceptance: in a `-game` run, collecting a heal pickup below full health shows one green "+N" above the miner, N = value decided in Q1; no number at full health (the pickup is not taken, [[heal-ore]]); damage numbers on robots unchanged (white, critical light orange); the toggle decided in Q5 hides it; capture before and after sent to the owner; committed.
- gdd_check: read [[heal-ore]] (15 HP per pickup, not taken at full health), [[miner]] (healing only from heal pickups), [[damage-numbers]] (look, size by value, arc, pop, crowd stacking, no merge, toggle; status provisional, M2 built now), [[color-code]] (damage number row white / white-gold; green is the Shooter's main colour, acid green; heal pickups are red), tech [[sys-damage-numbers]], [[sys-damage-numbers-niagara]], [[bp-damage-numbers]] (atlas has digits 0-9 in cells 0-9, cells 10-15 free, so a "+" glyph fits in cell 10), [[bp-heal-ring]] (existing green heal ring, colour (0.1, 1, 0.2)). Conflicts: none blocking; green is the Shooter colour, allowed as a small detail by the colour principle of [[color-code]] (Q2). Values decided (owner "ok" to the defaults, 2026-10-27, provisional): Q1 amount shown = HP actually gained (health after minus before, e.g. +7 at 93/100), not the nominal 15; Q2 colour = the heal ring green, linear (0.1, 1, 0.2); Q3 size and motion = same rules as a normal damage number of that value (size by value, arc, 0.25 s pop, fade), never critical; Q4 several heals close together = one number per pickup, no merge, vertical stacking against overlap like damage numbers; Q5 the "show damage numbers" setting also hides heal numbers; Q6 position = top of the miner capsule + 40 uu with the 30 uu random spawn disc, like the robots. Milestone: M2 feature already built, no scope issue. Written into [[damage-numbers]] (heal numbers line) and [[color-code]] (heal number row) by the Planner.
- confirmed_by_owner: yes (2026-10-27)
- milestone: M2
- next_step: task-324-02-heal-numbers-cpp
- started: 2026-10-27T12:38:55Z
- waiting_for: none
- result: open

## Subtasks
1. task-324-01-plus-glyph — agent-bp: add a "+" glyph in cell 10 of the damage digit atlas.
2. task-324-02-heal-numbers-cpp — agent-cpp: heal number path in the damage number subsystem, fired on heal collection, `-game` capture.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-324-01-plus-glyph]] — step 1, agent-bp.
- [[task-324-02-heal-numbers-cpp]] — step 2, agent-cpp.
