---
id: task-033-01-update-tests
description: "Existing tests updated to robots at 180 uu/s and the new XP curve; no new test; full headless run passing."
parent: task-033-tests-speed
kind: task
status: done
---
# Update tests to the current data

- goal: the test suite matches the owner's data and passes.
- agent: cpp
- depends_on: none
- inputs: [[sys-automation-tests]], [[xp-level-up]], [[sys-progression]], [[sys-enemies]], [[cpp-standard]]
- expected_output: EnemyStats.csv MoveSpeed 180 for every robot (already set by the Orchestrator) and XPCurve.csv (level 1 = 4, then 20, 25, 31, 36 … 64 at level 10 … 440 at level 50, already set); every existing test that fails because of these values updated from measurements (speeds, travel times, XP needs, the GrantXP cheat test); no new test, no new bot profile, no screenshot session; docs sys-automation-tests (and sys-progression curve line) updated. Added by the owner (2026-10-06): player projectiles (burst rifle bullet, shotgun pellet, turret bullet) collision sphere radius × 1.2 (10 → 12 uu) without changing their visual size; robot shooter shots unchanged; check whether a Blueprint (BP_ShotgunPellet, BP_TurretBullet, rifle projectile BP) overrides the sphere radius on disk and report it if so; docs sys-projectiles.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: input 82, cache write 76827, cache read 1795183, output 22411
- runs: 2
- milestone: M2
- result: Test expectations updated to robots at 180 uu/s and the XP curve (4, 20, 25 … 440); GrantXP cheat tests grant 4; ChaserReachesAndHitsMiner measured 15.4 s (limit 20 s kept). Player projectiles collision radius 10 → 12 uu in ARRProjectile, ARREnemyProjectile kept at 10; no Blueprint overrides the radius; visuals unchanged. Build 0, 195/195. Docs sys-automation-tests, sys-progression, sys-projectiles. Commit e08c3ae.

## Relations
- [[task-033-tests-speed]] — parent global task.
