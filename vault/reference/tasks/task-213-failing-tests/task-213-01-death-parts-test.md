---
id: task-213-01-death-parts-test
description: "cpp: Find why RogueRigs.Enemies.ShotHitsMovingRobotAndDeathShowsParts fails on 'EyeMesh visible during the death'"
parent: task-213-failing-tests
kind: task
status: done
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
- model: opus
- tokens: input 34, cache write 29546, cache read 407296, output 433
- runs: 1
- milestone: M2
- result: Cause: task-152-02 (674bce5f, owner 2026-10-13) hid EyeMesh in BP_Chaser (eye now in the model); the test from task-093-01 required every part visible during death. Test updated per the owner rule: each part keeps its rest visibility during the death (b1346ce2, doc 9c0f5dd9). Death code unchanged. Suite: only JuiceHealRing fails.

## Relations
- [[task-213-failing-tests]] — parent global task.
