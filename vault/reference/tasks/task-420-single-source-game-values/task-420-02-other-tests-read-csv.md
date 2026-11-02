---
id: task-420-02-other-tests-read-csv
description: "Cpp: expansion: the other test families read EnemyStats, PlayerBaseStats, ExplosionStats, Upgrade*, XPCurve, OreTypes through the helper"
parent: task-420-single-source-game-values
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-420-single-source-game-values]] — parent global task.
