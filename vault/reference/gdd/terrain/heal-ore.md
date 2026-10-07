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
- Each pickup heals 60 / 9 ≈ 6.67 HP, so a whole 3 × 3 vein restores 60 HP (owner 2026-10-06). Density: 108 cells in 12 clusters of 3 × 3 on the 240 × 240 M1 arena, the former 12 blocks of task-003 split at 67 uu.
- Milestone: the block is M1 (owner decision, task-001); pickups and the heal effect wait for block destruction; heal amount set by the owner on 2026-10-06 (above).

- Veins (owner 2026-10-06 fourth test): a 3 × 3 heal vein is mined as one, like ore veins ([[ore-blocks]]); every heal pickup must be collectable even when it pops inside the miner pickup radius.

- Vein shake (owner 2026-10-07 fifth test): while a vein is mined, all its blocks shake together, not only those in the drone arc.

## Relations
- [[terrain-index]] — parent index.
- [[destruction-rules]] — what can break it.
