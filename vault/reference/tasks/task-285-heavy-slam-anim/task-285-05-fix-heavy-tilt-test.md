---
id: task-285-05-fix-heavy-tilt-test
description: "Update the HeavyTelegraphSlamsMiner test that still expects the BodyMesh pitch 14 removed by task-285-03 (cpp)."
parent: task-285-heavy-slam-anim
kind: task
status: done
---
# Fix the heavy telegraph test after the pitch removal

- goal: Integration.HeavyTelegraphSlamsMiner checks "BP_Heavy tilts its body during the telegraph (pitch 14)" (RRHeavyIntegrationTests.cpp:217); task-285-03 replaced that tilt by the telegraph clip (owner 2026-10-22). Replace the check by the clip check (BodyMesh custom primitive data clip id 1 during the telegraph), rest of the test unchanged.
- agent: cpp
- depends_on: task-285-04-capture-slam
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Tests/RRHeavyIntegrationTests.cpp
- expected_output: test updated, sys-automation-tests line updated if it names the pitch check
- done_when: build succeeds; HeavyTelegraphSlamsMiner passes; full run shows only the 2 known pre-existing failures
- editor_state: closed
- token_ceiling: 40000
- model: opus
- tokens: input 18, cache write 67106, cache read 202523, output 4534
- runs: 1
- milestone: M2
- result: HeavyTelegraphSlamsMiner now checks GetAnimClipId()==1 at each telegraph start instead of pitch 14; passes; full set 267/269 (2 known pre-existing failures). Commit c9a43577.

## Relations
- [[task-285-heavy-slam-anim]] — parent global task.
