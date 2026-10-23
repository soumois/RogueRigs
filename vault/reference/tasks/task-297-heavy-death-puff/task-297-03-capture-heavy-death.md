---
id: task-297-03-capture-heavy-death
description: "Capture frames of a heavy death in -game to check the shrink is hidden (cpp)."
parent: task-297-heavy-death-puff
kind: task
status: todo
---
# Capture a heavy death

- goal: Capture in -game several frames over the first 0.4 s of a heavy death (cheat kill), compared with a heavy death before (NS_RobotDeathPuff) if frames exist, else a chaser death.
- agent: cpp
- depends_on: task-297-02-bp-heavy-death-puff-set
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/heavy-death/ frames and sheet, short description
- done_when: puff covers the heavy during the shrink on the frames; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-297-heavy-death-puff]] — parent global task.
