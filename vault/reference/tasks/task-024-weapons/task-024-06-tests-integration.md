---
id: task-024-06-tests-integration
description: "Integration tests with the final Blueprints: weapon card equips the shotgun, pellets pierce, turret placed by the input action fires."
parent: task-024-weapons
kind: task
status: done
---
# Weapons integration tests

- goal: the weapons rework is covered with the real Blueprints.
- agent: cpp
- depends_on: task-024-05-bp-weapons
- inputs: [[sys-automation-tests]], task-024-05 result.
- expected_output: `RogueRigs.Integration.*` tests: start loadout rifle only; level-up with a forced weapon card equips BP_Shotgun and its cards appear; pellets pierce a line of robots; PlaceTurret action places BP_Turret that kills a robot; BP_Miner LevelUpDisc: after chained level-ups and the last pick, the disc animation finishes and the disc ends hidden (DiscStep reaches 6); remove the legacy OnWaveFired and GetWaveClearDistance stubs from ARRShotgun once BP_Shotgun no longer uses them; full run.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: opus
- tokens: input 144, cache write 213645, cache read 5028715, output 60588
- runs: 2
- milestone: M2
- result: RRWeaponsIntegrationTests: StartLoadoutRifleOnly, WeaponCardEquipsShotgun (HUD ChooseUpgrade, description shown, value hidden, shotgun stat card later), ShotgunPelletsPierceRobots (3 of 4 hit), PlaceTurretInputKillsRobot (kill in 1.467 s), LevelUpDiscEndsHidden (timer survives pause; disc hidden after 5 steps, kept as intended). Legacy OnWaveFired/GetWaveClearDistance removed. Build 0, 142/142. Commit 68f652d (first run cut by an API limit).

## Relations
- [[task-024-weapons]] — parent global task.
