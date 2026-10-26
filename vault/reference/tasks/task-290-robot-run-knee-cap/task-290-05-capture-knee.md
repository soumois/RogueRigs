---
id: task-290-05-capture-knee
description: "Capture the heavy and chaser v2 running in -game with the new knee, compared with run-v1 shots (cpp)."
parent: task-290-robot-run-knee-cap
kind: task
status: done
---
# Capture both robots running

- goal: Rerun the task-289-06 capture (robot_shots.ps1 -Label run-knee) for heavy, chaser v2 and chaser v1, plus a close-up if possible, compared with Saved/RobotMeshes/run-v1/.
- agent: cpp
- depends_on: task-290-04-bp-chaser-v2-knee
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: task-289-06 node, robot_shots.ps1, Saved/RobotMeshes/run-v1/
- expected_output: Saved/RobotMeshes/run-knee/ shots and sheets, short description per robot
- done_when: both robots run with visible knee bend, parts joined, eyes on head; no untracked throwaway files
- editor_state: any (runs beside the owner's open editor)
- token_ceiling: 40000
- model: open
- tokens: not measured: no subagent transcript found (checked 2026-10-26)
- runs: open
- milestone: M2
- result: Cancelled before running (nothing baked or imported): the owner changed his mind on 2026-10-23, replaced by task-291-chaser-v2-v1-anims.

## Relations
- [[task-290-robot-run-knee-cap]] — parent global task.
