---
id: task-291-05-capture-v2-v1-clips
description: "Capture chaser v2 and shield chaser v2 running and attacking in -game next to their v1 versions (cpp)."
parent: task-291-chaser-v2-v1-anims
kind: task
status: todo
---
# Capture chaser v2 and shield chaser v2

- goal: Rerun the task-289-06 capture (robot_shots.ps1 -Label v1-clips) for chaser v2, shield chaser v2, chaser v1 and shield chaser v1, run and attack, compared with Saved/RobotMeshes/run-v1/ and shield-v2/.
- agent: cpp
- depends_on: task-291-04-bp-shield-v2-v1-clips
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/v1-clips/ shots and sheets, one line per robot
- done_when: both v2 robots run and attack with the v1 motion, parts joined, shield held, eyes on the head; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 35000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-291-chaser-v2-v1-anims]] — parent global task.
