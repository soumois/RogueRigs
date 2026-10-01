---
id: heal-ore
description: "Heal ore: a block that drops red heal pickups when broken by the drone or an explosion; the only healing source."
parent: terrain-index
kind: spec
status: approved
---
# Heal ore

- A grid block type (`heal_ore`), the only source of healing in a level.
- When broken, it drops heal pickups on the floor, whatever broke it (drone or explosion). An explosion never destroys the heal; luring a kamikaze next to it is a valid way to get it.
- Pickups are collected within the miner's pickup radius ([[player-stats]]).
- Each pickup heals a fixed amount (value open).
- Look: red heal crystals and red pickups; static on the floor, unlike moving red enemy shots ([[color-code]]).
- Block health, pickups per block, placement density: open.
- Milestone: open.

## Relations
- [[terrain-index]] — parent index.
- [[destruction-rules]] — what can break it.
