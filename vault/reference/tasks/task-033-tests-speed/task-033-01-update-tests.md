---
id: task-033-01-update-tests
description: "Existing tests updated to robots at 180 uu/s and the new XP curve; no new test; full headless run passing."
parent: task-033-tests-speed
kind: task
status: in-progress
---
# Update tests to the current data

- goal: the test suite matches the owner's data and passes.
- agent: cpp
- depends_on: none
- inputs: [[sys-automation-tests]], [[xp-level-up]], [[sys-progression]], [[sys-enemies]], [[cpp-standard]]
- expected_output: EnemyStats.csv MoveSpeed 180 for every robot (already set by the Orchestrator) and XPCurve.csv (level 1 = 4, then 20, 25, 31, 36 … 64 at level 10 … 440 at level 50, already set); every existing test that fails because of these values updated from measurements (speeds, travel times, XP needs, the GrantXP cheat test); no new test, no new bot profile, no screenshot session; docs sys-automation-tests (and sys-progression curve line) updated.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 80000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-033-tests-speed]] — parent global task.
