---
id: heal-ore
description: "Heal ore: a block that drops red heal pickups when broken by the drone or an explosion; the only healing source."
parent: terrain-index
kind: spec
status: approved
---
# Heal ore

- A grid block type (`heal_ore`), the only source of healing in a level.
- While the drone digs it, it drops one heal pickup per 1.5 s of digging; when destroyed, whatever destroyed it (drone or explosion), it drops 3 more (provisional). Dig time 4.5 s at mining speed 1, like ore. An explosion never destroys the heal; luring a kamikaze next to it is a valid way to get it.
- Pickups are collected within the miner's pickup radius ([[player-stats]]).
- Look: red heal crystals and red pickups; static on the floor, unlike moving red enemy shots ([[color-code]]).
- Heal ore blocks are spread across the map; their density is a balance value.
- Each pickup heals a fixed amount (value open). Density: 10 to 16 per 80 × 80 arena (task-003).
- Milestone: the block is M1 (owner decision, task-001); pickups and the heal effect wait for block destruction, heal amount open.

## Relations
- [[terrain-index]] — parent index.
- [[destruction-rules]] — what can break it.
