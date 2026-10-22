---
id: task-284-05-cpp-anim-clips
description: "Shared anim clip writer for the robots (cpp)."
parent: task-284-robot-walk-knee-anim
kind: task
status: in-progress
---
# Shared anim clip writer for the robots

- goal: Move the ClipId and AnimStart custom primitive data writes of ARRChaser into the shared enemy base (one generic implementation, events not polling) and use it in ARRHeavy and ARRShooter (walk loop while moving, rest pose while stopped, random phase; aim and fire clips come in task-287) and ARRKamikaze (sway loop always); chaser behaviour unchanged.
- agent: cpp
- depends_on: task-284-04-parts-bake-kamikaze
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-enemies-chaser (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-chaser.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md), sys-enemies-kamikaze (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-kamikaze.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: Source/RogueRigs/Enemies/RRChaser.cpp (SetAnimClip), the four anim reports of 284-01 to 04 (clip ids, frames)
- expected_output: C++ changes, header comments, updated sys-enemies-* docs
- done_when: build succeeds, all automation tests pass, getall CustomPrimitiveDataInternal shows the expected clip on a moving and a stopped heavy and shooter in a -game run
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
