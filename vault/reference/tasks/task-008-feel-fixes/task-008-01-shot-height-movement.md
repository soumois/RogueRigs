---
id: task-008-01-shot-height-movement
description: "Shots spawn at a fixed height in front of the miner, the weapon attaches to the body, the miner capsule ignores its own projectiles, new burst values."
parent: task-008-feel-fixes
kind: task
status: in-progress
---
# Shot height and free movement

- goal: shots can hit floor-standing targets and stop on blocks, and firing never slows the miner.
- agent: cpp
- depends_on: none
- inputs: [[sys-weapons]], [[sys-projectiles]], [[sys-player]], [[sys-auto-fire]], [[sys-automation-tests]], [[burst-rifle]], [[destruction-rules]], [[cpp-standard]].
- expected_output: (1) `ARRWeapon::FireOnce` spawns projectiles at the owning miner's location projected to the floor (capsule bottom) + editable `ShotHeight` (default 50 uu, must stay below the grid's `BlockHeight`) + aim direction × editable `ShotForwardOffset` (default capsule radius + 20 uu), horizontal, spread as before; the `Muzzle` component stays only as a visual reference. (2) Weapon attach: when `WeaponAttachSocketName` is None, attach to the miner root (capsule) with an editable relative offset, so the rifle follows the aim yaw and points forward. (3) Confirm the movement bug (the miner capsule blocked by its own projectiles: Pawn response to the `Projectile` channel defaults to Block) and fix it so the miner never collides with projectiles (its own or any; enemies' shots will damage by overlap later), e.g. capsule response Ignore or Overlap for `Projectile`; document the choice. (4) `WeaponStats.csv` row `BurstRifle`: FireRate 1.25, BurstDuty 0.2 (Damage stays 10). (5) Tests: `Weapons.BurstTiming` updated for the new CSV (gaps 0.08 s, pause 0.64 s) while keeping a FireRate 3 case; `Weapons.MinerStartingWeapon` checks the spawned projectile height is ShotHeight above the floor and below `BlockHeight`; new `Weapons.MinerMovesWhileFiring` (the miner moved by `AddMovementInput` or direct velocity for 1 s while firing covers at least 90 % of the distance it covers without firing). Docs [[sys-weapons]], [[sys-projectiles]], [[sys-player]], [[sys-automation-tests]], [[burst-rifle]] stats line (owner values 2026-10-02) updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs match; lint clean.
- editor_state: closed
- token_ceiling: 100000
- milestone: M1
- result: open

## Relations
- [[task-008-feel-fixes]] — parent global task.
