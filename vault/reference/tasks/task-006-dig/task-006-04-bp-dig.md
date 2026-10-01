---
id: task-006-04-bp-dig
description: "Laser and pickup grey-box assets, grid and drone defaults, then a PIE check of digging rock and heal ore and of pickup collection."
parent: task-006-dig
kind: task
status: todo
---
# Digging assets and PIE check

- goal: digging visible and verified in the arena.
- agent: bp
- depends_on: task-006-03-pickups
- inputs: [[sys-grid]], [[sys-drone]], `sys-pickups`, [[color-code]], [[unreal-mcp]], [[bp-arena-grid]], [[bp-drone]].
- expected_output: materials in `/Game/RogueRigs/Materials/GreyBox/`: `MI_GB_Laser` (saturated red, brighter than heal ore), `MI_GB_PickupHeal` (red), `MI_GB_PickupOre` (cyan per [[color-code]]: resource pickups glowing cyan). `BP_PickupHeal` and `BP_PickupOre` in `/Game/RogueRigs/Blueprints/Pickups/` (parent `ARRPickup`, type set, engine cube about 30 uu). `BP_ArenaGrid`: pickup classes per type. `BP_Drone`: `LaserMesh` = engine cylinder with `MI_GB_Laser`. Doc nodes for new and changed Blueprints. Save all dirty packages.
- done_when: PIE on `L_ArenaM1`, driving the miner by setting its input or location through MCP: a rock cell adjacent to a reachable floor cell is destroyed after about 1.5 s of digging (read cell state and HISM counts before and after); the closest heal ore is destroyed after about 4.5 s with 5 heal pickups spawned in total; pickups within 300 uu of the miner are collected (log lines). If a check cannot be driven through MCP, say so precisely and leave it to the owner. Files changed on disk; lint clean.
- editor_state: open
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-006-dig]] — parent global task.
