---
id: task-289-05-cpp-slam-lead
description: "Heavy slam-down clip starts early enough for the contact pose at the new RunRate (cpp)."
parent: task-289-robot-run-v1-motion
kind: task
status: todo
---
# Heavy slam lead for the new RunRate

- goal: The heavy strike clip (clip 2, looped by the shader with RunRate) reaches its contact frame 4 on the slam frame at the new heavy RunRate 0.7 (4 / (30 x 0.7) = 0.190 s): set the C++ default SlamContactLead from 0.15 to 0.20 s (check BP_Heavy does not override it), update the header comments and sys-enemies-heavy; fix only the tests this change breaks.
- agent: cpp
- depends_on: task-289-04-bp-chaser-v2-run
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md); files: Source/RogueRigs/Enemies/RRHeavy.h, RRHeavy.cpp
- expected_output: RRHeavy.h default 0.20 s with comments, sys-enemies-heavy updated, build ok, heavy tests pass
- done_when: build succeeds; existing heavy automation tests pass; doc updated; lint clean
- editor_state: closed
- token_ceiling: 40000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.
