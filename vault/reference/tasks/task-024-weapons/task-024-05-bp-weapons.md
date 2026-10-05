---
id: task-024-05-bp-weapons
description: "Pellet and muzzle visuals, BP_TurretLauncher and BP_Turret grey-box, weapon-card style and weapon slots in the HUD, miner starting loadout, PIE."
parent: task-024-weapons
kind: task
status: todo
---
# Weapons Blueprints

- goal: the reworked weapons are visible and playable.
- agent: bp
- depends_on: task-024-04-weapon-cards
- inputs: [[bp-shotgun]], [[bp-burst-rifle]], [[bp-miner]], [[wbp-hud]], [[turret]], [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: BP_Shotgun cleanup per bp-shotgun (delete the Event OnWaveFired chain and its runtime cubes, add Event OnShotFired(Origin, Direction, FanSpreadDeg) muzzle flash, BP_ShotgunPellet white-gold as ProjectileClass); BP_Shotgun updated (pellet projectile Blueprint, white-gold, muzzle flash on OnShotFired; old fan removed); BP_TurretLauncher and BP_Turret (white + teal + rust grey-box, barrel turns toward target); BP_Miner StartingWeapons [BP_BurstRifle]; weapon list data pointing to the Blueprints; WBP_HUD: weapon slots row (owned weapons, empty slots, turret charges), new-weapon card style ("Nouvelle arme", distinct border); docs bp-shotgun, new bp-turret, bp-miner, wbp-hud. PIE: start with the rifle, take a weapon card (GrantXP and ChooseUpgrade cheats), place turrets with the button, pellets visible.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 120000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-024-weapons]] — parent global task.
