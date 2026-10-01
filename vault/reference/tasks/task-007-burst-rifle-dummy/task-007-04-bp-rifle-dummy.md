---
id: task-007-04-bp-rifle-dummy
description: "Burst rifle, projectile and target dummy Blueprints and materials, rifle on BP_Miner, three dummies near the spawn, PIE check of bursts and damage."
parent: task-007-burst-rifle-dummy
kind: task
status: done
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
- result: MI_GB_Shot (white-gold), MI_GB_Dummy; BP_Projectile (ShotMesh 15 uu sphere), BP_BurstRifle (60×10×10 cube, StatsRowName BurstRifle, BP_Projectile, Muzzle (40, 0, 0)), BP_TargetDummy (120 uu sphere); BP_Miner StartingWeapons = BP_BurstRifle on socket hand_r (untested). Dummies at (8900, 8300, 60), (7300, 8500, 60), (8700, 8900, 60), saved with Ctrl+Shift+S (`save_actor` fails on new external actors). PIE not run: moved to task-007-05 and to the owner. Docs bp-burst-rifle, bp-projectile, bp-target-dummy, bp-miner. MCP quirks (StartPIE options, PressKey modifiers, PrimitiveTools add_sphere and add_cube, save_actor) still to record in [[unreal-mcp]]. About 100k tokens (over ceiling, turn limit hit once).

## Relations
- [[task-007-burst-rifle-dummy]] — parent global task.
