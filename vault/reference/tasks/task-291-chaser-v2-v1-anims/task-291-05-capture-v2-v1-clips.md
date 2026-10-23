---
id: task-291-05-capture-v2-v1-clips
description: "Capture chaser v2 and shield chaser v2 running and attacking in -game next to their v1 versions (cpp)."
parent: task-291-chaser-v2-v1-anims
kind: task
status: done
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
- model: opus
- tokens: input 46, cache write 91837, cache read 695856, output 9194
- runs: 1
- milestone: M2
- result: -game captures in Saved/RobotMeshes/v1-clips/ (run and attack sheets, 4 robots): chaser v2 and shield chaser v2 run with the v1 motion, parts joined, shield held; attack swing hidden by the miner in the top-down view and shield chaser v2 eye hidden behind the shield (unverified); robot_shots.ps1 gained shieldchaser entry (71a5b029).

## Relations
- [[task-291-chaser-v2-v1-anims]] — parent global task.
