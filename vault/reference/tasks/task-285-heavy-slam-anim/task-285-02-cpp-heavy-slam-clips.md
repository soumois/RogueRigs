---
id: task-285-02-cpp-heavy-slam-clips
description: "ARRHeavy plays the slam clips (cpp)."
parent: task-285-heavy-slam-anim
kind: task
status: done
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
- model: opus
- tokens: input 50, cache write 62224, cache read 941843, output 15731
- runs: 1
- milestone: M2
- result: ARRHeavy: Telegraph start PlayClip(1, WindUp-0.15), timer end PlayClip(2), slam frame PlayClip(3), back to walk on Chase; TelegraphClipId/SlamClipId/RecoverClipId/SlamContactLead EditDefaultsOnly; build OK; 267 pass, 2 known pre-existing failures; -game CPD sequence 1,2,3,0 on 3 slams (clip 2 ~0.15 s before each slam). Commits d822e9d1, 68c4575d.

## Relations
- [[task-285-heavy-slam-anim]] — parent global task.
