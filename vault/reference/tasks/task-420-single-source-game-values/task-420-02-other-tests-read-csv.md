---
id: task-420-02-other-tests-read-csv
description: "Cpp: expansion: the other test families read EnemyStats, PlayerBaseStats, ExplosionStats, Upgrade*, XPCurve, OreTypes through the helper"
parent: task-420-single-source-game-values
kind: task
status: done
---
# Game values in one place: the CSV tables: cpp

- goal: Apply the 01 helper to every other test file that copies a CSV value (RREnemy, RRHeavy, RRHeavyIntegration, RRKamikaze, RRShooter, RRShooterIntegration, RRShieldChaser, RRSpawner, RRDig, RROreDeposit, RRPickupHeal, RRPickupMotion, RRCheat, RRIntegration, RRTextContent tests, and any other found), same rules as 01.
- agent: cpp
- depends_on: task-420-01-weapon-tests-read-csv
- inputs: task-420-single-source-game-values (vault/reference/tasks/task-420-single-source-game-values.md), task-420-01-weapon-tests-read-csv (vault/reference/tasks/task-420-single-source-game-values/task-420-01-weapon-tests-read-csv.md), agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), `Content/RogueRigs/Data/Tables/`
- expected_output: test files rewritten; one or more commits; change summary.
- done_when: build ok; full RogueRigs.* run passes; with the A3 mutation (WeaponStats edits of 01 plus EnemyStats Chaser MaxHealth 40) the full run still passes; CSVs restored (git diff empty).
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 276, cache write 641577, cache read 27309699, output 118752
- usage: turns 136, context start 12k peak 324k, resumes 1 (turn-limit x1), 39 min, units 4.13 M
- runs: 2
- milestone: M3
- started: 2026-11-02T19:14:52Z
- waiting_for: none
- result: Done with caveat: other test families read their CSV rows (commits ee603bca, 4f8f0f3c, c0e7bc50). Build ok; full run 274 pass, 2 pre-existing failures identical at 5a29adc9, unrelated to CSV (Integration.JuiceKillDropsStreamingCubes: BP_PickupXP collision; Integration.LevelUpUpgradeLoop: RerollButton WBP_HUD), so done_when 'full run passes' not reached. Mutation run (Shotgun 13, BurstRifle 12, Turret FireRate 3, Chaser MaxHealth 40): only Barrel.TwoRifleShotsExplode failed, fixed, Barrel 6/6; CSVs restored.

## Relations
- [[task-420-single-source-game-values]] — parent global task.
