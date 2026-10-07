---
id: task-034-01-cpp-fixes
description: "Projectile hitbox 14 uu, whole-vein mining, heal pickup fix, chaser capsule on rock, no robot blocking, kamikaze, pack mix, recycling, turret, ammo data."
parent: task-034-fourth-test
kind: task
status: done
---
# Fourth test: C++ fixes

- goal: the C++ side of the owner's fourth play test.
- agent: cpp
- depends_on: none
- inputs: [[burst-rifle]], [[ore-blocks]], [[heal-ore]], [[enemy-kamikaze]], [[enemies-index]], [[turret]], [[readability-rules]], [[sys-projectiles]], [[sys-grid]], [[sys-drone]], [[sys-pickups]], [[sys-enemies]], [[sys-spawner]], [[sys-spawner-director]], [[sys-turret]], [[sys-hud]], [[sys-automation-tests]], [[cpp-standard]]
- expected_output: (1) player projectile sphere 12 → 14 uu, robot shots 10; (2) a dug block that belongs to a 3 × 3 ore or heal vein (cluster of connected same-type cells) makes the whole vein one dig target: all its blocks break together after one block dig time, 9 chunks; (3) find why heal pickups were not collected (likely overlap begin never fires when spawned inside the radius) and fix it for every pickup type; (4) every robot's capsule against world static (rock) uses the chaser radius, keeping its own visual and its hit checks on the miner; (5) robot-robot blocking removed (pawn vs pawn), with a cheap light separation so they do not stack; (6) kamikaze trigger range 150 → 250 uu, fuse 0.5 → 0.65 s, verify it stops moving once armed and fix if not; (7) director: kamikazes 1–2 per pack, shooters at most 1 in one pack out of two, none before 90 s, shooter cap 6 → 4, packs 6–10 robots until 90 s; (8) recycling: RecycleOffScreenMargin 500 → 0, RecycleDistance 2500 → 1800, RecycleDelay 4 → 3; (9) turret recharge 6 → 12 s; (10) HUD data for task-034-02: per owned weapon slot the rounds left, magazine size, reloading flag and reload progress 0..1; for the turret launcher the charges, max charges and recharge progress; slot owned or locked; Blueprint-readable, listed with exact names in the report. Update only the existing tests these changes break (no new test, no bot runs, no screenshots). Docs: the matching sys-* nodes and sys-automation-tests.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 150000
- model: opus
- tokens: input 176, cache write 211498, cache read 10173682, output 68304
- runs: 3
- milestone: M2
- result: Stopped after points 1, 2, 3 (mechanism), 4, 5, 6, 8, 9 (split ordered by the owner): projectile sphere 14 uu; whole-vein mining (ARRArenaGrid::GetVeinCells, 9 blocks break together); heal pickups were collected but never healed: ARRPickup now applies URRGE_Heal (HealAmount open, 0); every robot root capsule 24 uu (TerrainCapsuleRadius) with a HitCapsule at its own size; RVO off and URRRobotSeparationSubsystem (weight 0.5); kamikaze arm 250 uu, fuse 0.65 s, already stopped when armed; recycling margin 0, 1800 uu, 3 s; turret recharge 12 s. 8 broken tests updated and passing; full suite not rerun. Points 7 and 10 and the docs moved to new subtasks. Commits 03ac10a, 659b75b.

## Relations
- [[task-034-fourth-test]] — parent global task.
