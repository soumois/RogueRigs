---
id: task-289-05-capture-run
description: "Capture the heavy and chaser v2 running in -game, compared with the old walk shots and a chaser v1 run (cpp)."
parent: task-289-robot-run-v1-motion
kind: task
status: todo
---
# Capture both robots running

- goal: With the editor closed, rerun the task-284-11 capture (robot_shots.ps1) for the heavy, chaser v2 and chaser v1, several frames each while they run, into a new folder, and compare with the task-284-11 walk shots.
- agent: cpp
- depends_on: task-289-04-bp-chaser-v2-run
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: the task-284-11 capture (robot_shots.ps1), Saved/RobotMeshes/walk/heavy_*.png, chaserv2_*.png
- expected_output: Saved/RobotMeshes/run-v1/heavy_*.png, chaserv2_*.png, chaserv1_*.png and a short description per robot
- done_when: both robots run with visible thigh and arm swing, parts joined, eyes on the head; the Orchestrator sends the shots to the owner
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.
