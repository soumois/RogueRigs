---
id: task-011-06-bp-explosions
description: "Grey-box kamikaze, orange barrel material, explosion FX, spawner mix 3 to 1, then a PIE check of barrels and kamikazes."
parent: task-011-explosions
kind: task
status: todo
---
# Explosion assets and PIE

- goal: explosions visible and wired in the arena.
- agent: bp
- depends_on: task-011-05-kamikaze
- inputs: [[sys-explosions]], [[sys-enemies]], [[sys-spawner]], [[color-code]], [[enemy-kamikaze]], [[barrel]], [[unreal-mcp]], [[bp-arena-grid]], [[bp-enemy-spawner]].
- expected_output: `MI_GB_Barrel` (orange), `MI_GB_Kamikaze` (dark gunmetal), `MI_GB_Core` (bright orange), `MI_GB_Explosion` (orange). `BP_ArenaGrid` barrel mesh and material set. `/Game/RogueRigs/Blueprints/Enemies/BP_Kamikaze` (parent `ARRKamikaze`, gunmetal sphere body about 0.6 × the miner, orange sphere `CoreMesh`, red eye, XP pickup `BP_PickupXP`). `/Game/RogueRigs/Blueprints/FX/BP_ExplosionFX` (parent `ARRExplosionFX`, engine sphere, `MI_GB_Explosion`) set as the explosion FX class wherever the C++ exposes it. `BP_EnemySpawner` weighted classes: `BP_Chaser` 3, `BP_Kamikaze` 1. Doc nodes for every new or changed Blueprint. Save all dirty packages; check `git status`.
- done_when: assets read back; PIE starts without gameplay errors in the log; barrel instance count matches the layout; kamikazes spawn among chasers; files on disk; lint clean.
- editor_state: open
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-011-explosions]] — parent global task.
