---
id: task-297-03-capture-heavy-death
description: "Capture frames of a heavy death in -game to check the shrink is hidden (cpp)."
parent: task-297-heavy-death-puff
kind: task
status: done
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
- model: opus
- tokens: input 54, cache write 72479, cache read 1149395, output 15225
- runs: 2
- milestone: M2
- result: Added rr.Debug.KillRobots (b5b37c44, sys-cheats 8ea89703). Run 1: heavy head drawn over the puff at 0.04 s -> 297-01 rework f729a121 (puff +130 cm, sprites 70-190 cm). Run 2 (Saved/RobotMeshes/heavy-death/heavy_sheet.png): head, torso and arms hidden from the first puff frame (0.04 s); feet and shins still visible below the puff 0.04-0.09 s, hidden from 0.14 s; colour reads cream-yellow with orange edges. Residual feet left for owner review.

## Relations
- [[task-297-heavy-death-puff]] — parent global task.
