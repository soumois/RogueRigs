---
id: task-293-02-capture-heavy-slam
description: "Capture the heavy telegraph circle and slow fist raise in -game, compared with run-v1 shots (cpp)."
parent: task-293-heavy-slam-bigger-slower
kind: task
status: in-progress
---
# Capture the heavy telegraph and slam

- goal: Capture in -game a heavy telegraph (several frames over the 1.0 s: circle at 361 uu, fists rising) and the slam frame, compared with Saved/RobotMeshes/run-v1/.
- agent: cpp
- depends_on: task-293-01-cpp-heavy-slam-values
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/heavy-slam-1s/ shots and sheet, short description
- done_when: circle size and raise timing visible on the frames; fists on the ground at the slam; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-293-heavy-slam-bigger-slower]] — parent global task.
