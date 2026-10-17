---
id: task-213-01-death-parts-test
description: "cpp: Find why RogueRigs.Enemies.ShotHitsMovingRobotAndDeathShowsParts fails on 'EyeMesh visible during the death'"
parent: task-213-failing-tests
kind: task
status: todo
---
# death-parts-test

- goal: Find why RogueRigs.Enemies.ShotHitsMovingRobotAndDeathShowsParts fails on "EyeMesh visible during the death" (RREnemyTests.cpp:731) and fix the test or the code per the owner rule.
- agent: cpp
- depends_on: none
- inputs: sys-tests-enemies (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-enemies.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: C++ test or code fix, sys-tests-enemies updated with the cause.
- done_when: the test passes; the full suite has no new failure; doc updated; committed.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-213-failing-tests]] — parent global task.
