---
id: task-296-03-capture-shield-arm-forward
description: "Capture the shield chaser v2 running with the shield forward, compared with the task-291 shots (cpp)."
parent: task-296-shield-v2-arm-forward
kind: task
status: todo
---
# Capture the shield held forward

- goal: Capture in -game the shield chaser v2 running and bashing, side and top view if possible, compared with Saved/RobotMeshes/v1-clips/.
- agent: cpp
- depends_on: task-296-02-bp-shield-arm-forward
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/shield-forward/ shots and sheet, short description
- done_when: shield visibly ahead of the feet, arm forward, parts joined; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-296-shield-v2-arm-forward]] — parent global task.
