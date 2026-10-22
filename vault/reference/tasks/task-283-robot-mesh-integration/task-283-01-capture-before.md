---
id: task-283-01-capture-before
description: "Capture the robots before integration (cpp)."
parent: task-283-robot-mesh-integration
kind: task
status: in-progress
---
# Capture the robots before integration

- goal: With the editor closed, run the game (-game) and capture the heavy, kamikaze, shooter and chaser near the miner with rr.Debug.SpawnNear and rr.Debug.Shots, as the reference for the after capture of this task.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: none
- expected_output: Saved/RobotMeshes/before/*.png (one shot per robot, same camera as the game, robot centred) and the command line in the report
- done_when: one PNG per robot exists, each robot is visible and described in the report; same command reusable for 283-11
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
