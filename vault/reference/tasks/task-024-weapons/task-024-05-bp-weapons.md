---
id: task-024-05-bp-weapons
description: "Pellet and muzzle visuals, BP_TurretLauncher and BP_Turret grey-box, weapon-card style and weapon slots in the HUD, miner starting loadout, PIE."
parent: task-024-weapons
kind: task
status: done
---
# Weapons Blueprints

- goal: the reworked weapons are visible and playable.
- agent: bp
- depends_on: task-024-04-weapon-cards
- inputs: [[bp-shotgun]], [[bp-burst-rifle]], [[bp-miner]], [[wbp-hud]], [[turret]], [[unreal-mcp]], [[sys-cheats]], [[cpp-bp-split]], [[color-code]], [[readability-rules]], [[drgs-analysis]]
- expected_output: BP_Shotgun cleanup per bp-shotgun (delete the Event OnWaveFired chain and its runtime cubes, add Event OnShotFired(Origin, Direction, FanSpreadDeg) muzzle flash, BP_ShotgunPellet white-gold as ProjectileClass); BP_Shotgun updated (pellet projectile Blueprint, white-gold, muzzle flash on OnShotFired; old fan removed); BP_TurretLauncher (child of ARRTurretLauncher: TurretClass = BP_Turret, ProjectileClass = a white-gold bullet Blueprint) and BP_Turret (child of ARRTurret: white + teal + rust grey-box, barrel turns toward GetTarget, muzzle flash on OnTurretFired); optional IA_PlaceTurret in DefaultMappingContext (E, right mouse, gamepad face bottom) set as PlaceTurretAction on BP_RRPlayerController (C++ fallback already works); WBP_HUD turret charges via OnTurretChargesChanged and GetTurretRechargeProgress; weapon list data includes BP_TurretLauncher; BP_Miner StartingWeapons = [BP_BurstRifle] (remove BP_Shotgun), GuaranteedWeaponClasses empty, WeaponCatalogue = BP_BurstRifle, BP_Shotgun, BP_TurretLauncher with short French descriptions, MaxWeapons 4; HUD cards styled by card type (new weapon: dedicated style, title, name, description, no value); confirm the turret charges HUD binds when the launcher comes from a card; weapon list data pointing to the Blueprints; WBP_HUD: weapon slots row (owned weapons, empty slots, turret charges), new-weapon card style ("Nouvelle arme", distinct border); docs bp-shotgun, new bp-turret, bp-miner, wbp-hud. PIE: start with the rifle, take a weapon card (GrantXP and ChooseUpgrade cheats), place turrets with the button, pellets visible.
- done_when: PIE check through the cheat property route with no log error; Blueprint docs updated; lint clean.
- editor_state: open
- token_ceiling: 120000
- model: sonnet
- tokens: input 324, cache write 245190, cache read 26013968, output 86768
- runs: 6
- milestone: M2
- result: BP_Shotgun cleaned (OnWaveFired chain removed, MuzzleFlash on OnShotFired), BP_ShotgunPellet; BP_TurretBullet, BP_TurretLauncher, BP_Turret (head and barrel track GetTarget, flash on OnTurretFired; MI_GB_Turret white/teal/rust); BP_Miner StartingWeapons [BP_BurstRifle], catalogue rifle/shotgun/launcher with French descriptions, MaxWeapons 4; WBP_HUD card style per type (teal-white border for new weapons, description, no value), 4 weapon slot texts, turret charges text and recharge bar. IA_PlaceTurret skipped (MCP cannot create Enhanced Input assets; C++ fallback works: E placed a turret). PIE: rifle only at start, shotgun card → 7 pellets per shot, launcher card → E places a turret that fires. Fixed BP_Miner level-up disc runtime error (persistent LevelUpDisc component, restart on overlap); disc ending hidden not confirmed (check in 024-06). Docs bp-turret (new), bp-shotgun, bp-miner, wbp-hud, blueprints-index. Commits 02d2787, 58b4613, 2583b1c, e0c4a29, 03e6ecf.

## Relations
- [[task-024-weapons]] — parent global task.
