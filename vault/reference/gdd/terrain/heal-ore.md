---
id: heal-ore
description: "Heal ore: a block that drops red heal pickups when broken by the drone or an explosion; the only healing source."
parent: terrain-index
kind: spec
status: approved
---
# Heal ore

- A grid block type (`heal_ore`), the only source of healing in a level.
- Each broken heal-ore block (67 uu) drops one heal pickup, whatever destroyed it (drone or explosion); nothing drops during the dig. Dig time 1.5 s at mining speed 1, like ore ([[drone]]; owner, after handover-2026-10-06). An explosion never destroys the heal; luring a kamikaze next to it is a valid way to get it.
- Pickups are collected within the miner's pickup radius ([[player-stats]]).
- Look: red heal crystals and red pickups; static on the floor, unlike moving red enemy shots ([[color-code]]).
- Heal ore blocks are spread across the map; their density is a balance value.
- Each pickup heals a fixed amount (value open). Density: 108 cells in 12 clusters of 3 × 3 on the 240 × 240 M1 arena, the former 12 blocks of task-003 split at 67 uu.
- Milestone: the block is M1 (owner decision, task-001); pickups and the heal effect wait for block destruction, heal amount open.

## Relations
- [[terrain-index]] — parent index.
- [[destruction-rules]] — what can break it.
