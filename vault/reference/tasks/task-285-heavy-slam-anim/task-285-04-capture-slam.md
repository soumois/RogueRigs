---
id: task-285-04-capture-slam
description: "Capture the heavy slam (cpp)."
parent: task-285-heavy-slam-anim
kind: task
status: done
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
- model: opus
- tokens: input 22, cache write 62662, cache read 306072, output 6132
- runs: 1
- milestone: M2
- result: Slam shots Saved/RobotMeshes/slam/heavy_front_0-15.png (and profile heavy_0-15): telegraph fists raised above the shoulders with orange eye (front_1, front_8), crouched with fists at ground level after the strike (front_2, front_9), upright red eye between slams (front_6); ring unchanged; parts joined. Exact impact frame not captured (screenshots stall ~0.3 s).

## Relations
- [[task-285-heavy-slam-anim]] — parent global task.
