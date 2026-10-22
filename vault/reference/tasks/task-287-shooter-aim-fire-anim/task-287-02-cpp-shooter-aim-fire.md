---
id: task-287-02-cpp-shooter-aim-fire
description: "ARRShooter plays aim and fire clips (cpp)."
parent: task-287-shooter-aim-fire-anim
kind: task
status: in-progress
---
# ARRShooter plays aim and fire clips

- goal: Write the aim clip on OnAimStart, the fire clip on OnFire and the walk or rest clip on OnAimEnd from ARRShooter (native, shared clip writer of task-284); firing rules and timings unchanged.
- agent: cpp
- depends_on: task-287-01-bake-shooter-aim-fire
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: the 287-01 anim report
- expected_output: C++ changes and sys-enemies-shooter doc update
- done_when: build succeeds, all automation tests pass, the clip id read in a -game run follows Aim, Fire, Chase
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-287-shooter-aim-fire-anim]] — parent global task.
