---
id: task-007-04-bp-rifle-dummy
description: "Burst rifle, projectile and target dummy Blueprints and materials, rifle on BP_Miner, three dummies near the spawn, PIE check of bursts and damage."
parent: task-007-burst-rifle-dummy
kind: task
status: todo
---
# Rifle and dummy assets, PIE check

- goal: shooting the dummies works in the arena.
- agent: bp
- depends_on: task-007-03-target-dummy
- inputs: [[sys-weapons]], [[sys-projectiles]], `sys-target-dummy`, [[color-code]], [[unreal-mcp]], [[bp-miner]].
- expected_output: `MI_GB_Shot` (white-gold) and `MI_GB_Dummy` (neutral grey-orange, not an enemy or terrain color) in `/Game/RogueRigs/Materials/GreyBox/`. In `/Game/RogueRigs/Blueprints/Weapons/`: `BP_BurstRifle` (parent `ARRWeapon`, small cube mesh, projectile class) and `BP_Projectile` (sphere about 15 uu, `MI_GB_Shot`). `/Game/RogueRigs/Blueprints/Debug/BP_TargetDummy` (sphere about 120 uu, `MI_GB_Dummy`). `BP_Miner` `StartingWeapons` = [`BP_BurstRifle`]. Three `BP_TargetDummy` in `L_ArenaM1` on open floor reachable from the spawn, 3 to 8 cells away (read cells from `Content/RogueRigs/Data/Levels/arena_m1.json`; cell (x, y) center = ((x+0.5)·200, (y+0.5)·200)). Doc nodes for every new or changed Blueprint. Save all dirty packages.
- done_when: PIE: projectiles appear in bursts (count `BP_Projectile_C` actors or log lines over 1 s); aim the miner at a dummy (set its rotation or aim through MCP) and read the dummy's `Health` dropping by 10 or 15 per hit, refilling after 3 s; after 30 shots a reload pause appears. If a check cannot be driven through MCP, say so precisely and leave it to the owner. Files changed on disk; lint clean.
- editor_state: open
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-007-burst-rifle-dummy]] — parent global task.
