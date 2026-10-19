---
id: damage-numbers
description: "Floating damage numbers: one per hit with a spawn disc, colours for normal and critical hits, stacking, toggle. Open for combat feedback or UI work."
parent: weapons-index
kind: spec
status: provisional
---
# Damage numbers

- Normal hits: white. Critical hits: 1.6 × larger, lighter orange, linear colour (1, 0.7, 0.35), with a short pop, like DRGS (owner 2026-10-05; colour provisional, owner 2026-10-19, task-238; formerly gold (1, 0.55, 0.08), confirmed by the owner after handover-2026-10-06).
- Size grows continuously with the damage of the hit: from 0.8 × the base size for the smallest hit to 1.4 × at twice the base damage of the starting weapon (burst rifle 10, so 20) and above; a critical hit keeps its 1.6 × on top (provisional, owner 2026-10-19, task-239).
- Each number rises with its own random speed, 0.7 × to 1.3 × the base rise speed, and its own random angle, up to 35 degrees left or right of screen up; all digits of one number move together so it stays readable, and consecutive numbers no longer travel in parallel (provisional, owner 2026-10-19, task-240).
- Numbers are thrown in an arc instead of rising in a straight line: they go up, slow, then start to fall before they fade; peak about 60 uu above the spawn point at about 0.35 s, lifetime 0.7 s and fade unchanged (provisional, owner 2026-10-19, task-242).
- Every number pops (a brief scale punch, and for a critical a flash to white) over 0.25 s, up from 0.1 s; amplitudes, lifetime and fade unchanged (provisional, owner 2026-10-19, task-241).
- In a crowd, a new number stacks vertically above an open number whose footprint it would overlap (confirmed by the owner after handover-2026-10-06).
- One number per hit, never merged (since 2026-10-08, see the last line); the earlier merge rule below is history.
- The player can turn them off.
- Rendered with Niagara at horde scale (digit atlas), fed by the damage pipeline ([[sys-damage]]).
- Merging window duration: 0.25 s (owner 2026-10-05; removed on 2026-10-08).
- Milestone: M2, built now (owner 2026-10-05). M1 has a debug text version on the test target dummy (owner, 2026-10-01), extended to every enemy (owner, 2026-10-02); not pooled, so not for horde scale.

- No merging (owner 2026-10-08, after the DRGS video): one number per hit; each number starts at a random point of a 30 uu disc around the hit (45 uu on the heavy) so repeated hits on one robot do not start from the same spot; the vertical stacking against overlap is kept. Replaces the 0.25 s merge window above.

## Relations
- [[weapons-index]] — parent index.
