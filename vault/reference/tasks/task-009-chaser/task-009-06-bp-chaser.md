---
id: task-009-06-bp-chaser
description: "Chaser, XP cube and spawner Blueprints, grey-box materials, HUD widget with health bar and Game Over text, miner blink, placed in L_ArenaM1."
parent: task-009-chaser
kind: task
status: done
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
- result: MI_GB_Chaser (1, 0.75, 0), MI_GB_Eye (red), MI_GB_PickupXP (cyan); BP_PickupXP (type XP, 30 uu cube); BP_Chaser (cylinder body 48 × 124, red sphere eye, XPPickupClass BP_PickupXP); BP_EnemySpawner (EnemyClass BP_Chaser) placed in L_ArenaM1; WBP_HUD (health bar and text top-left, GAME OVER text collapsed; both events implemented through the graph DSL); BP_RRPlayerController HUDWidgetClass = WBP_HUD. PIE: 7 chasers after 8 s, no gameplay error in the log; HUD look and chaser movement not seen (owner). Docs bp-chaser, bp-pickup-xp, bp-enemy-spawner, wbp-hud, bp-rr-player-controller. About 84k tokens (turn limit hit once).

## Relations
- [[task-009-chaser]] — parent global task.
