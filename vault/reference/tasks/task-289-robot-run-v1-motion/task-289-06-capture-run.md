---
id: task-289-06-capture-run
description: "Capture the heavy and chaser v2 running in -game, compared with the old walk shots and a chaser v1 run (cpp)."
parent: task-289-robot-run-v1-motion
kind: task
status: done
---
# Capture both robots running

- goal: With the editor closed, rerun the task-284-11 capture (robot_shots.ps1) for the heavy, chaser v2 and chaser v1, several frames each while they run, into a new folder, and compare with the task-284-11 walk shots.
- agent: cpp
- depends_on: task-289-05-cpp-slam-lead
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: the task-284-11 capture (robot_shots.ps1), Saved/RobotMeshes/walk/heavy_*.png, chaserv2_*.png
- expected_output: Saved/RobotMeshes/run-v1/heavy_*.png, chaserv2_*.png, chaserv1_*.png and a short description per robot
- done_when: both robots run with visible thigh and arm swing, parts joined, eyes on the head; the Orchestrator sends the shots to the owner
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 26, cache write 42955, cache read 285723, output 5136
- runs: 1
- milestone: M2
- result: -game shots Saved/RobotMeshes/run-v1/{heavy,chaserv2,chaserv1}_0-5.png + *_sheet.png (robot_shots.ps1 -Label run-v1 -NoFreeze -Distance 900 -StopSpawner, run beside the owner's open editor). Heavy: thigh and arm swing clearly larger than the old walk, joined, eye on head. Chaser v2: legs in phase, stride visible in 2 of 6 frames, swing reads weaker than v1 from the top-down camera (v1 knee bend 55 vs 15 here). Unverified: foot slide in game, slam contact timing.

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.
