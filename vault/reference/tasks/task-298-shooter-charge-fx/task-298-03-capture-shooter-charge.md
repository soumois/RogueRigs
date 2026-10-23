---
id: task-298-03-capture-shooter-charge
description: "Capture the shooter charge-up during the laser in -game (cpp)."
parent: task-298-shooter-charge-fx
kind: task
status: todo
---
# Capture the shooter charge-up

- goal: Capture in -game a shooter aiming (frames over the 0.6 s laser) and right after the shot.
- agent: cpp
- depends_on: task-298-02-bp-shooter-charge-wire
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/shooter-charge/ frames and sheet, short description
- done_when: charge-up visible at the cannon tip during the laser, gone after the shot; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-298-shooter-charge-fx]] — parent global task.
