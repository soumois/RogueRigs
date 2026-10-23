---
id: task-300-03-capture-slam-impact
description: "Capture frames of a heavy slam impact in -game (cpp)."
parent: task-300-heavy-slam-impact-fx
kind: task
status: done
---
# Capture the slam impact

- goal: Capture in -game frames from the slam frame to 0.5 s after (ring and chips).
- agent: cpp
- depends_on: task-300-02-bp-slam-impact-wire
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md); files: vault/reference/tasks/task-289-robot-run-v1-motion/task-289-06-capture-run.md (capture recipe), Tools robot_shots.ps1
- expected_output: Saved/RobotMeshes/slam-impact/ frames and sheet, short description
- done_when: ring reaching about the slam radius and chips visible; no untracked throwaway files
- editor_state: any (runs beside the owner editor)
- token_ceiling: 30000
- model: opus
- tokens: input 44, cache write 49336, cache read 595298, output 11346
- runs: 2
- milestone: M2
- result: Run 1 (Saved/RobotMeshes/slam-impact/): ring ~0.4 R, raised, whitish, chips specks -> fix task-300-04. Run 2 (Saved/RobotMeshes/slam-impact-v2/slam_sheet.png, frames 08-17, slomo 0.1): flat ring centred at the feet, ~0.86 R at frame 14, ~0.98 R at frame 15 (~+0.3 s), gone at +0.35 s; light beige-tan semi-transparent (lighter than intended grey-brown); ~5 light-tan chips visible near the legs, short throw. Pixel estimates from the pitched camera.

## Relations
- [[task-300-heavy-slam-impact-fx]] — parent global task.
