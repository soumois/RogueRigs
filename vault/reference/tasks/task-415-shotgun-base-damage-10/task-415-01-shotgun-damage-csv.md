---
id: task-415-01-shotgun-damage-csv
description: "Cpp: set the `Shotgun` row Damage 6 -> 10 in WeaponStats.csv and update the shotgun test expectations"
parent: task-415-shotgun-base-damage-10
kind: task
status: todo
---
# Shotgun base damage 10: cpp

- goal: In `Content/RogueRigs/Data/Tables/WeaponStats.csv`, row `Shotgun`, set `Damage` to 10 (per pellet, Q1 default; read the global Questions for the decided reading). Update only the existing expectations the change breaks in `Source/RogueRigs/Tests/RRShotgunTests.cpp` (`PelletDamage` and the test texts naming 6). Return a change summary (commit hash, changed paths, values) for the doc subtask; do not edit vault doc nodes.
- agent: cpp
- depends_on: none
- inputs: task-415-shotgun-base-damage-10 (vault/reference/tasks/task-415-shotgun-base-damage-10.md) (global: request, Questions, gdd_check), shotgun (vault/reference/gdd/weapons/shotgun.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), agent-cpp (vault/reference/agents/agent-cpp.md), `Content/RogueRigs/Data/Tables/WeaponStats.csv`, `Source/RogueRigs/Tests/RRShotgunTests.cpp`
- expected_output: One commit `task-415-01-shotgun-damage-csv: <summary>` with the CSV and the test file; a change summary.
- done_when: CSV `Shotgun` Damage = 10 and no other cell changed (git diff shows one cell); build succeeds; the shotgun automation tests pass (command and result in the report); no other test file touched unless a test outside it breaks on 6 (then named in the result); no .uasset staged.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-415-shotgun-base-damage-10]] — parent global task.
