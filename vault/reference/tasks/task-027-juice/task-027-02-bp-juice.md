---
id: task-027-02-bp-juice
description: "Cyan XP cube trail and collect sparkle, kamikaze core light ramp, console ground ring gauge, chevrons to the shuttle, green heal ring."
parent: task-027-juice
kind: task
status: todo
---
# Juice visuals

- goal: the juice is visible.
- agent: bp
- depends_on: task-027-01-pickup-motion
- inputs: [[bp-pickup-xp]], [[bp-kamikaze]], [[bp-objective-console]], [[bp-extraction-shuttle]], [[bp-miner]], task-027-01 result, [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: XP cubes small glowing cyan with a short trail while magnetized and a sparkle on collect; kamikaze core point light ramping to a strong orange-white during the flare; console: ground ring growing with hack progress inside the zone circle; chevrons on the floor from the miner toward the shuttle during the countdown; green ring around the miner on heal (level-up and heal pickups); damage digits: dark outline or drop shadow in T_DamageDigits / M_DamageNumber so they read on yellow robots and grey floor; bp-damage-numbers Limits entry updated (per-digit index confirmed); docs updated. PIE check. Added after task-026: WBP_HUD ObjectiveText overlaps AlertIconConsole and AlertIconShuttle (icon top 34): move ObjectiveText down about 30 px or move the icon row; update wbp-hud.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 120000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-027-juice]] — parent global task.
