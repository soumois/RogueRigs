---
id: task-319-03-verify-levelup-after-intermission
description: "Rerun the task-319-01 HUD reproduction test and the automation suite on the fixed WBP_XPStatus; the test passes on both transition paths."
parent: task-319-levelup-choice-after-intermission
kind: task
status: todo
---
# Verify: level-up choice after the intermission

- goal: after the WBP_XPStatus fix (task-319-02), build and run `RogueRigs.Integration.LevelUpChoiceAfterIntermission` (the HUD version from task-319-01) and the full automation suite. No code change expected; if the test still fails, stop and report the reason (do not change the Blueprint, do not add a C++ workaround). Not a new test: it reruns the test the owner requested ("lance le test").
- agent: cpp
- depends_on: task-319-02-fix-levelup-after-intermission
- inputs: task-319-01-levelup-choice-after-intermission (vault/reference/tasks/task-319-levelup-choice-after-intermission/task-319-01-levelup-choice-after-intermission.md), task-319-02-fix-levelup-after-intermission (vault/reference/tasks/task-319-levelup-choice-after-intermission/task-319-02-fix-levelup-after-intermission.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), sys-tests-run-integration (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-run-integration.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: test results in result; sys-tests-run-integration updated if the test status line changes
- done_when: build passes; the HUD test passes on the fade path and the loading path (choice queued for the level-2 level-up, `HandleLevelUp` still bound after the HUD swap); every other automation test has the same outcome as before the fix (pre-existing unrelated failures JuiceKillDropsStreamingCubes and LevelUpUpgradeLoop listed, not fixed); lint clean. No capture, no bot run.
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-319-levelup-choice-after-intermission]] — parent global task.
- [[task-319-01-levelup-choice-after-intermission]] — the test it reruns.
- [[task-319-02-fix-levelup-after-intermission]] — the fix it verifies.
