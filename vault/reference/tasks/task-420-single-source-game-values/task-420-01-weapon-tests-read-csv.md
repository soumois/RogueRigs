---
id: task-420-01-weapon-tests-read-csv
description: "Cpp: slice: a test helper reading CSV rows, weapon test files use it (no literal CSV value), mutation check"
parent: task-420-single-source-game-values
kind: task
status: done
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
- model: opus
- tokens: input 88, cache write 224169, cache read 7109232, output 79083
- usage: turns 44, context start 12k peak 230k, resumes 0, 13 min, units 1.39 M
- runs: 1
- milestone: M3
- started: 2026-11-02T19:01:41Z
- waiting_for: none
- result: Done: RRTestStatsRows.h helper (FindRow), weapon tests read WeaponStats rows; 36/36 pass with committed CSV and with Shotgun 13/BurstRifle 12/Turret FireRate 3; CSV restored. Commit 31b20d35. Loader has FindRow, not GetCachedRow. Remaining literals in other families go to subtask 02.

## Relations
- [[task-420-single-source-game-values]] — parent global task.
