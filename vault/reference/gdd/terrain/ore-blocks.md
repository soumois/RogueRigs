---
id: ore-blocks
description: "Ore blocks: tougher than rock, always drop ore whatever broke them, explosions destroy them like any block. Look and values."
parent: terrain-index
kind: spec
status: approved
---
# Ore blocks

- Tougher than plain rock blocks: drone dig time 1.5 s per 67 uu block at mining speed 1, against 0.5 s for rock (owner, after handover-2026-10-06).
- Each broken ore block pops one ore chunk (DRGS-like arc then suction, [[xp-level-up]]); ore value per chunk: open until the ore economy is built (provisional, 2026-10-06).
- The chunk drops whatever destroyed the block, drone or explosion; no extra bundle ([[ore-economy]]; owner, after handover-2026-10-06).
- The chunk lands within about one cell of the broken block, so it never rests over unbroken rock (provisional, task-027-01, owner to judge).
- Any explosion destroys every block in its radius ([[explosions]]): luring a kamikaze onto a vein is a valid tactic.
- Look: same base as breakable rock, with glowing violet crystals ([[color-code]]).
- Values above are provisional, set by the owner on 2026-10-01; ore value per pickup: open.
- Milestone: M1.

## Relations
- [[terrain-index]] — parent index.
- [[explosions]] — destroy every block in the radius, ore included.
