---
id: heal-ore
description: "Heal ore: a block that drops red heal pickups when broken by the drone or an explosion; the only healing source."
parent: terrain-index
kind: spec
status: approved
---
# Heal ore

- A grid block type (`heal_ore`), the only source of healing in a level.
- Each broken heal-ore block (67 uu) drops one heal pickup, whatever destroyed it (drone or explosion). Loot over the dig (owner 2026-10-19, replaces "nothing drops during the dig"): a vein being dug pops its pickups one by one, evenly spread over its dig time (pickup k at progress k / N); an explosion pops the remaining ones at once ([[task-235-ore-loot-over-dig]]). Dig time 1.5 s at mining speed 1, like ore ([[drone]]; owner, after handover-2026-10-06). An explosion never destroys the heal; luring a kamikaze next to it is a valid way to get it.
- Pickups are collected within the miner's pickup radius ([[player-stats]]).
- Look: red heal crystals and red pickups; static on the floor, unlike moving red enemy shots ([[color-code]]).
- Heal ore blocks are spread across the map; their density is a balance value.
- Veins are 2 × 2 (owner 2026-10-10, was 3 × 3): each pickup heals 15 HP, so a whole vein restores 60 HP as before. Density: 12 veins of 2 × 2 (48 cells) on the 240 × 240 M1 arena, the 12 veins of the former 3 × 3 layout.
- A heal pickup is not taken when the miner is at full health (owner 2026-10-10): it stays on the floor and is collected later, once health is below the maximum and the pickup is within the pickup radius.
- Milestone: the block is M1 (owner decision, task-001); pickups and the heal effect wait for block destruction; heal amount set by the owner on 2026-10-06 (above).

- Veins (owner 2026-10-06 fourth test, 2 × 2 since 2026-10-10): a heal vein is mined as one, like ore veins ([[ore-blocks]]); every heal pickup must be collectable even when it pops inside the miner pickup radius.

- Vein shake (owner 2026-10-07 fifth test): while a vein is mined, all its blocks shake together, not only those in the drone arc.

## Relations
- [[terrain-index]] — parent index.
- [[destruction-rules]] — what can break it.
