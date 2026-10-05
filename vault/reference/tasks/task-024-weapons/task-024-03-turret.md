---
id: task-024-03-turret
description: "Turret launcher weapon placed with a button on the aimed spot: charges, recharge, lifetime, nearest-target fire with line of sight, pierce; tests."
parent: task-024-weapons
kind: task
status: done
---
# Turret launcher

- goal: turrets can be placed with a button.
- agent: cpp
- depends_on: task-024-02-shotgun-pellets
- inputs: [[turret]], [[sys-weapons]], [[sys-projectiles]], [[sys-auto-fire]], [[sys-player]], [[sys-grid]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: Enhanced Input action "PlaceTurret" (default key E and right mouse button; gamepad face button) places a turret on the aimed floor cell (or the nearest walkable cell along the aim within 400 uu); turret launcher stats: charges 2, recharge 6 s one at a time, turret lifetime 10 s, damage 6 per bullet, 3.3 shots/s, range 1500 uu, pierce 4, crit 5 % ×1.5; placed turret copies launcher stats at placement, locks the nearest robot with grid LOS, retargets; invincible, no collision with robots or projectiles; HUD charges via URRHUDWidget events; WeaponStats.csv; docs sys-weapons (turret section or new sys-turret), sys-player (input). Tests `RogueRigs.Turret.*`.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 130000
- model: opus
- tokens: input 140, cache write 206107, cache read 9284203, output 87069
- runs: 2
- milestone: M2
- result: ARRTurretLauncher (charges = Ammo, TryPlaceTurret on the aimed visible floor cell ≤ 400 uu, one-at-a-time 6 s recharge, OnTurretChargesChanged, OnTurretPlaced), ARRTurret (copies launcher stats at placement, 10 s life, nearest visible robot ≤ 1500 uu, pierce 4, no collision, not in the weapon list, OnTurretFired), ARRTurretBullet; ARRMiner AddWeapon, FindTurretLauncher, PlaceTurret, GetMouseAimPoint; ARRPlayerController PlaceTurretAction with C++ fallback mapping (E, right mouse, gamepad face bottom); HUD OnTurretChargesChanged, GetTurretRechargeProgress; WeaponStats TurretLauncher row + TurretLifetime column. 6 Turret tests; PierceFourRobots made deterministic (spread 0); build 0, 133/133. New sys-turret; sys-weapons, sys-player, sys-hud, sys-projectiles. Provisional: bullet speed 3000, stick placement 200 uu, name Lance-tourelles. Commits 032eb71, 1609557.

## Relations
- [[task-024-weapons]] — parent global task.
