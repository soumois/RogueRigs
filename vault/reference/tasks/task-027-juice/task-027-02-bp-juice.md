---
id: task-027-02-bp-juice
description: "Cyan XP cube trail and collect sparkle, kamikaze core light ramp, console ground ring gauge, chevrons to the shuttle, green heal ring."
parent: task-027-juice
kind: task
status: done
---
# Juice visuals

- goal: the juice is visible.
- agent: bp
- depends_on: task-027-01-pickup-motion
- inputs: [[bp-pickup-xp]], [[bp-kamikaze]], [[bp-objective-console]], [[bp-extraction-shuttle]], [[bp-miner]], task-027-01 result, [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: XP cubes small glowing cyan with a short trail while magnetized and a sparkle on collect; kamikaze core point light ramping to a strong orange-white during the flare; console: ground ring growing with hack progress inside the zone circle; chevrons on the floor from the miner toward the shuttle during the countdown; green ring around the miner on heal (level-up and heal pickups); damage digits: dark outline or drop shadow in T_DamageDigits / M_DamageNumber so they read on yellow robots and grey floor; bp-damage-numbers Limits entry updated (per-digit index confirmed); docs updated. PIE check. Added after task-026: WBP_HUD ObjectiveText overlaps AlertIconConsole and AlertIconShuttle (icon top 34): move ObjectiveText down about 30 px or move the icon row; update wbp-hud. Added after task-027-01: lower PopMaxDistance on BP_PickupOre and BP_PickupHeal so chunks land within about one 67 uu cell of the broken block (GDD ore-blocks); sparkle spawned at the OnPickupCollectedAt / OnCollected location (pickup is destroyed right after); trail on OnMagnetStart; kamikaze core light on OnFlareStart(Duration) / GetFlareProgress, off on OnFlareEnd(false); heal ring on OnPickupCollectedAt with Type Heal.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 120000
- model: sonnet
- tokens: input 142, cache write 129353, cache read 5707966, output 46372
- runs: 2
- milestone: M2
- result: WBP_HUD stack moved down (ObjectiveText 66, HackBar 100, ShuttleText 122); ore/heal pickups pop 25–67 uu; M_GroundRing + instances; BP_PickupSparkle and BP_HealRing (ARRExplosionFX children); BP_PickupXP trail on OnMagnetStart, sparkle on OnCollected; BP_HealRing on heal collect; BP_Kamikaze CoreLight = flare progress × 60000; BP_ObjectiveConsole HackRing scaled by progress; BP_ExtractionShuttle 4 chevrons toward the shuttle. PIE: console ring and chevrons checked, no game log error; flare, trail, sparkle and heal ring compiled only. Not done: damage digit outline; trail and sparkle are meshes, not Niagara. Visual checks moved into task-027-03. Commits 305d6c2, cfc92a3, 3cc22ff, 6b9f14d.

## Relations
- [[task-027-juice]] — parent global task.
