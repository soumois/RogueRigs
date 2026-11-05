---
id: task-452-03-hud-placement
description: "Bp: The health panel sits just above the weapon slots."
parent: task-452-health-above-weapons
kind: task
status: done
---
# Health bar just above the weapon slots: bp

- goal: The health panel sits just above the weapon slots.
- agent: bp
- depends_on: task-452-02-health-panel
- inputs: wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; anchors and offsets as in A1; PIE shows no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-452
- usage: included in the executor run of task-452
- runs: 1
- milestone: M2
- started: 2026-11-05T23:49:42Z
- waiting_for: none
- result: Commit f9221733. WBP_HealthStatus HealthPanel canvas slot: anchors 0.5,1 / 0.5,1, alignment 0.5,1, offsets 0, -148, 652 x 44 (read back): centred on the weapon row (636 wide, top 144 above the bottom), 4 above it, 8 wider each side; at 1920 x 1080 x 634..1286, y 888..932; same bottom-centre anchor as the row, so it holds at other resolutions and ratios. Nothing left under the minimap. WBP_Weapons TurretBox slot bottom padding 52 so the turret line sits above the panel; WBP_HUD Subtitles top -190 -> -240 (box y 760..840) to clear panel and turret line. All three compiled and saved. PIE L_ArenaM1: two more miner deaths with end screen and restart, no LogScript error. Not seen on screen (Q26).

## Relations
- [[task-452-health-above-weapons]] — parent global task.
