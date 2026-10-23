---
id: task-287-04-capture-shooter-aim-fire
description: "Capture the shooter aiming and firing (cpp)."
parent: task-287-shooter-aim-fire-anim
kind: task
status: done
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
- model: opus
- tokens: input 40, cache write 68674, cache read 663474, output 10013
- runs: 1
- milestone: M2
- result: -game L_ArenaM1 side capture (robot_shots.ps1 + slomo 0.2): cannon arm raised horizontal along the laser (slow 4/8/10), laser from cannon tip, shot leaves the glowing tip with recoil tilt (slow 11-14), arm back horizontal after (slow 20); before: arm hanging, laser from hip (walk/shooter_0). Shots Saved/RobotMeshes/shooter-aim/. Unverified: 0.3 s raise not caught, rest pose after fire vs walk arm, side view only. No code change.

## Relations
- [[task-287-shooter-aim-fire-anim]] — parent global task.
