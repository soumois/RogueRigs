---
id: task-420-01-weapon-tests-read-csv
description: "Cpp: slice: a test helper reading CSV rows, weapon test files use it (no literal CSV value), mutation check"
parent: task-420-single-source-game-values
kind: task
status: in-progress
---
# Game values in one place: the CSV tables: cpp

- goal: Add a test-side helper that returns a CSV row (WeaponStats, EnemyStats, ...) through `URRBaseStatsLoader::GetCachedRow`; rewrite the weapon test files to read every expected stat from it and derive the rest; scenario values that depend on a stat (health raised so a target survives N pellets) are computed from the row. Start from the uncommitted edits of RRShotgunTests.cpp, RRTurretTests.cpp, RRWeaponCardTests.cpp (Q6).
- agent: cpp
- depends_on: none
- inputs: task-420-single-source-game-values (vault/reference/tasks/task-420-single-source-game-values.md), agent-cpp (vault/reference/agents/agent-cpp.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-weapons (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapons.md), sys-tests-weapon-types (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-weapon-types.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), `Source/RogueRigs/GAS/RRBaseStatsLoader.h`, `Content/RogueRigs/Data/Tables/WeaponStats.csv`, files: RRWeaponTests.cpp, RRShotgunTests.cpp, RRTurretTests.cpp, RRWeaponCardTests.cpp, RRLevelUpTests.cpp in `Source/RogueRigs/Tests/`
- expected_output: helper in `Source/RogueRigs/Tests/` (header comment), the weapon test files rewritten; one commit; change summary (helper API, files, tests run, mutation result).
- done_when: build ok; RogueRigs.Weapons.*, RogueRigs.Shotgun.*, RogueRigs.Turret.*, weapon card and level-up tests pass; with WeaponStats.csv Shotgun Damage 13, BurstRifle Damage 12, TurretLauncher FireRate 3 they still pass; WeaponStats.csv restored (git diff empty); no `constexpr` or literal in those files equals a WeaponStats value used as an expectation.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T19:01:41Z
- waiting_for: none
- result: open

## Relations
- [[task-420-single-source-game-values]] — parent global task.
