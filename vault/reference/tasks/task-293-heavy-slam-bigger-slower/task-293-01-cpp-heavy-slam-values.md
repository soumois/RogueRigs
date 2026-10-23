---
id: task-293-01-cpp-heavy-slam-values
description: "Heavy CSV radius 361 and wind-up 1.0, telegraph clip frame driven by telegraph progress, contact lead checked (cpp)."
parent: task-293-heavy-slam-bigger-slower
kind: task
status: in-progress
---
# Heavy slam radius and wind-up

- goal: EnemyStats.csv Heavy AttackRange 289 -> 361, WindUp 0.6 -> 1.0. Stretch the clip-1 raise (rows 16..30, clamped, raise over frames 0..13) over WindUp - SlamContactLead without changing MF_RobotAnimWPO: preferred route, while Telegraph the heavy rewrites its AnimStart (CPD 1) each step so the shown frame follows the telegraph progress (heavy only, no extra tick); if this is unworkable, stop and report (alternative: a clip-1 rate parameter in the material, a bp subtask). Strike clip 2 and recover clip 3 unchanged; check BP_Heavy SlamContactLead 0.20 still lands the contact on the slam frame. Update only the tests this breaks (radius 289 / 202.3, 0.6 s).
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), sys-ground-telegraph (vault/reference/tech/systems/combat/sys-ground-telegraph.md), sys-tests-heavy (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-heavy.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md); files: Content/RogueRigs/Data/Tables/EnemyStats.csv, Source/RogueRigs/Enemies/RRHeavy.h/.cpp, Source/RogueRigs/Tests/RRHeavy*Tests.cpp
- expected_output: CSV, code, updated tests; sys-enemies-heavy and sys-enemies CSV line updated with before and after
- done_when: build clean; heavy tests pass with 361 / 252.7 uu and 1.0 s; clip-1 frame timeline reported (frame 13 reached at WindUp - lead); contact time vs slam frame reported; docs updated; lint clean
- editor_state: closed (owner closes it, or Live Coding off)
- token_ceiling: 45000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-293-heavy-slam-bigger-slower]] — parent global task.
