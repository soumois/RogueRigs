---
id: task-416-01-turret-fire-rate-csv
description: "Cpp: set the `TurretLauncher` row FireRate 3.3 -> 2.75 (Q3) in WeaponStats.csv and update the turret test expectations"
parent: task-416-turret-fire-rate-minus-20
kind: task
status: todo
---
# Turret fire rate lowered by 20 %: cpp

- goal: In `Content/RogueRigs/Data/Tables/WeaponStats.csv`, row `TurretLauncher`, set `FireRate` to 2.75 (0.364 s between shots; Q3 default; read the global Questions for the decided value: 2.22 if the owner chose 0.45 s). Update only the existing expectations the change breaks in `Source/RogueRigs/Tests/RRTurretTests.cpp` (`TurretFireRate`, the texts naming 3.3, the cadence test and its comment: at 2.75 shots/s, 1 s of fire gives shots at 0, 0.364, 0.727 s). Return a change summary (commit hash, changed paths, values) for the doc subtask; do not edit vault doc nodes.
- agent: cpp
- depends_on: none
- inputs: task-416-turret-fire-rate-minus-20 (vault/reference/tasks/task-416-turret-fire-rate-minus-20.md) (global: request, Questions, gdd_check), turret (vault/reference/gdd/weapons/turret.md), sys-turret (vault/reference/tech/systems/combat/sys-turret.md), agent-cpp (vault/reference/agents/agent-cpp.md), `Content/RogueRigs/Data/Tables/WeaponStats.csv`, `Source/RogueRigs/Tests/RRTurretTests.cpp`
- expected_output: One commit `task-416-01-turret-fire-rate-csv: <summary>` with the CSV and the test file; a change summary.
- done_when: CSV `TurretLauncher` FireRate = the Q3 value (2.75 by default) and no other cell changed (git diff shows one cell); build succeeds; the turret automation tests pass (command and result in the report); no other test file touched unless a test outside it breaks on 3.3 (then named in the result); no .uasset staged.
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
- [[task-416-turret-fire-rate-minus-20]] — parent global task.
