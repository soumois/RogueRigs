---
id: task-299-02-capture-kamikaze-shake
description: "Capture frames of an armed kamikaze over its 0.65 s fuse in -game (cpp)."
parent: task-299-kamikaze-fuse-shake
kind: task
status: done
---
# Capture the kamikaze fuse

- goal: Capture in -game frames over one kamikaze fuse (shake and core growth), a short sequence rather than one still.
- agent: cpp
- depends_on: task-299-01-cpp-kamikaze-fuse-shake
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/kamikaze-shake/ frames and sheet, short description
- done_when: body offset visible between frames, core growth visible; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: opus
- tokens: input 38, cache write 45862, cache read 519938, output 11352
- runs: 1
- milestone: M2
- result: 14 -game frames at slomo 0.1 over one fuse, 0.05 s apart, plus sheet (Saved/RobotMeshes/kamikaze-shake/kamikaze_sheet.png): body offset against a fixed cross visible between frames (about 21 px travel each screen axis); core bigger at the end but mixed with its pulse. Not verified: uu amplitude, rate rise 8 to 24 Hz, look at normal speed.

## Relations
- [[task-299-kamikaze-fuse-shake]] — parent global task.
