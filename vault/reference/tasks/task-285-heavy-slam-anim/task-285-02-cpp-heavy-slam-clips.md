---
id: task-285-02-cpp-heavy-slam-clips
description: "ARRHeavy plays the slam clips (cpp)."
parent: task-285-heavy-slam-anim
kind: task
status: in-progress
---
# ARRHeavy plays the slam clips

- goal: Write the telegraph, slam and recover clips from ARRHeavy on its state changes (native, with the shared clip writer of task-284), so that the fists touch the ground on the slam damage frame; slam rules and timings unchanged.
- agent: cpp
- depends_on: task-285-01-bake-heavy-slam
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md), sys-enemies-base (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-base.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md); files: the 285-01 anim report (clip ids, frames, contact frame)
- expected_output: C++ changes and sys-enemies-heavy doc update
- done_when: build succeeds, all automation tests pass, the clip id read in a -game run follows Telegraph, Slam, Recover, Chase
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-285-heavy-slam-anim]] — parent global task.
