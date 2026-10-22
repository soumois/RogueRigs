---
id: task-285-04-capture-slam
description: "Capture the heavy slam (cpp)."
parent: task-285-heavy-slam-anim
kind: task
status: in-progress
---
# Capture the heavy slam

- goal: With the editor closed, capture a heavy near the miner during the telegraph and at the slam frame, and compare with its 284-11 walk shots.
- agent: cpp
- depends_on: task-285-03-bp-heavy-slam
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: Saved/RobotMeshes/walk/ heavy shots and the 283-01 command
- expected_output: Saved/RobotMeshes/slam/*.png and a description
- done_when: fists raised in the telegraph shot, fists on the ground in the slam shot, ground circle unchanged; the Orchestrator sends the shots to the owner
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-285-heavy-slam-anim]] — parent global task.
