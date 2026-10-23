---
id: task-300-03-capture-slam-impact
description: "Capture frames of a heavy slam impact in -game (cpp)."
parent: task-300-heavy-slam-impact-fx
kind: task
status: in-progress
---
# Capture the slam impact

- goal: Capture in -game frames from the slam frame to 0.5 s after (ring and chips).
- agent: cpp
- depends_on: task-300-02-bp-slam-impact-wire
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/slam-impact/ frames and sheet, short description
- done_when: ring reaching about the slam radius and chips visible; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-300-heavy-slam-impact-fx]] — parent global task.
