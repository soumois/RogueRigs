---
id: ore-blocks
description: "Ore blocks: tougher than rock, always drop ore whatever broke them, one kamikaze explosion breaks one. Look and values."
parent: terrain-index
kind: spec
status: approved
---
# Ore blocks

- Tougher than plain rock blocks: drone dig time 4.5 s at mining speed 1, against 1.5 s for rock (provisional).
- Drop ore while being dug: one pickup per 1.5 s of digging (provisional); no need to finish the block to get some.
- Drop a larger bundle when destroyed, whatever destroyed them: 3 pickups (provisional) ([[ore-economy]]).
- Any explosion destroys every block in its radius ([[explosions]]): luring a kamikaze onto a vein is a valid tactic.
- Look: same base as breakable rock, with glowing violet crystals ([[color-code]]).
- Values above are provisional, set by the owner on 2026-10-01; ore value per pickup: open.
- Milestone: M1.

## Relations
- [[terrain-index]] — parent index.
- [[explosions]] — the terrain damage tuned to break one.
