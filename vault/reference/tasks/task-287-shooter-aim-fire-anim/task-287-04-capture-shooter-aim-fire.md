---
id: task-287-04-capture-shooter-aim-fire
description: "Capture the shooter aiming and firing (cpp)."
parent: task-287-shooter-aim-fire-anim
kind: task
status: in-progress
---
# Capture the shooter aiming and firing

- goal: With the editor closed, capture a shooter during the laser and at the shot, compared with its 284-11 walk shots.
- agent: cpp
- depends_on: task-287-03-bp-shooter-aim-fire
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: Saved/RobotMeshes/walk/ shooter shots and the 283-01 command
- expected_output: Saved/RobotMeshes/shooter-aim/*.png and a description
- done_when: cannon arm raised along the laser, laser and shot leave from the cannon tip; the Orchestrator sends the shots to the owner
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-287-shooter-aim-fire-anim]] — parent global task.
