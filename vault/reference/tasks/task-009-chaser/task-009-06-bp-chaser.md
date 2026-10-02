---
id: task-009-06-bp-chaser
description: "Chaser, XP cube and spawner Blueprints, grey-box materials, HUD widget with health bar and Game Over text, miner blink, placed in L_ArenaM1."
parent: task-009-chaser
kind: task
status: in-progress
---
# Chaser assets, HUD and map

- goal: everything visible and wired in the arena.
- agent: bp
- depends_on: task-009-05-spawner
- inputs: `sys-enemies`, `sys-spawner`, `sys-hud`, [[sys-player]], [[enemy-chaser]], [[color-code]], [[unreal-mcp]], [[bp-miner]], [[blueprints-index]].
- expected_output: `MI_GB_Chaser` (company yellow), `MI_GB_Eye` (red), `MI_GB_PickupXP` (glowing cyan). `/Game/RogueRigs/Blueprints/Enemies/BP_Chaser` (parent `ARRChaser`, `StatsRowName` Chaser, grey box: yellow capsule-shaped mesh about 0.7 × the miner, a small red sphere eye on the front, XP pickup class `BP_PickupXP`). `/Game/RogueRigs/Blueprints/Pickups/BP_PickupXP` (type XP, cyan cube). `/Game/RogueRigs/Blueprints/Enemies/BP_EnemySpawner` (enemy class `BP_Chaser`) placed once in `L_ArenaM1`. `/Game/RogueRigs/UI/WBP_HUD` (parent `URRHUDWidget`): health bar top-left (red fill, value text), centered "GAME OVER" text hidden until `OnGameOver`; set as the HUD widget class on `BP_RRPlayerController`. `BP_Miner`: blink settings left at defaults unless needed. Doc nodes for every new or changed Blueprint and widget. Save all dirty packages (Ctrl+Shift+S for external actors), check `git status`.
- done_when: assets read back; spawner placed and saved; PIE starts without errors and chasers appear (count `BP_Chaser_C` actors after 6 s); files changed on disk; lint clean.
- editor_state: open
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-009-chaser]] — parent global task.
