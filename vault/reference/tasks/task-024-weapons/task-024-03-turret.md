---
id: task-024-03-turret
description: "Turret launcher weapon placed with a button on the aimed spot: charges, recharge, lifetime, nearest-target fire with line of sight, pierce; tests."
parent: task-024-weapons
kind: task
status: in-progress
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
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-024-weapons]] — parent global task.
