---
id: task-284-11-capture-walk
description: "Capture the robots walking (cpp)."
parent: task-284-robot-walk-knee-anim
kind: task
status: done
---
# Capture the robots walking

- goal: With the editor closed, rerun the 283-10 capture command with the robots moving (several frames per robot, chaser v2 included) and compare with the after-integration shots and the 283-01 chaser v1 shot.
- agent: cpp
- depends_on: task-284-10-bp-spawner-chaser-v2
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: Saved/RobotMeshes/after-integration/*.png and the 283-01 command
- expected_output: Saved/RobotMeshes/walk/*.png and a description per robot (legs in different phases, knee bend visible, no part detached)
- done_when: every robot shows its clip in motion (kamikaze legs sway very lightly), parts stay joined, eyes follow the head; the Orchestrator sends the shots to the owner
- editor_state: closed
- token_ceiling: 60000
- model: opus
- tokens: input 76, cache write 90002, cache read 1732749, output 25280
- runs: 2
- milestone: M2
- result: Walk shots Saved/RobotMeshes/walk/: heavy_0-3 (legs in phases, joined, visor on head), kamikaze_0-3 (light leg sway), shooter_far_0-3 (walks from 1300 uu, stride changes), chaserv2_0-3 and chaserv2_attack_0-7 (walks, crouched knee bend near the miner); robot_shots.ps1 gained -NoFreeze, -Distance, -Yaw, -Camera, -StopSpawner, -Suffix. Not confirmed: chaser v2 strike pose, clear knee bend from the top-down camera; chaser v2 missing in 2 of 8 frames (cause unknown, open).

## Relations
- [[task-284-robot-walk-knee-anim]] — parent global task.
