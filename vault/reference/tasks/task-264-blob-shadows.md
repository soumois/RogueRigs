---
id: task-264-blob-shadows
description: "Global task: Simple fake round shadows under the enemies, the miner and the drone."
parent: tasks-done-index
kind: task
status: done
---
# Blob shadows

- request: the rest is blob: a simple fake shadow under the enemies (owner, 2026-10-21)
- goal: Simple fake round shadows under the enemies, the miner and the drone.
- acceptance: M_BlobShadow exists and robots, miner and drone carry a cheap blob shadow; docs and lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-21).
- confirmed_by_owner: yes (2026-10-21)
- milestone: M1
- next_step: none
- result: Done. M_BlobShadow (unlit translucent, two sided, radial soft disc from the Plane UVs, no texture) and MI_BlobShadow (Opacity 0.45, Softness 1.6, Tint (0.02, 0, 0.04)) under /Game/RogueRigs/FX/BlobShadow/; compiles; node bp-blob-shadow-material. Translucency sort priority is not a material property in 5.8: set it on the component in 264-02. Not seen on screen.

## Subtasks
1. task-264-01-blob-shadows — bp
2. task-264-02-blob-shadows — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-264-01-blob-shadows]] — step 1, bp.
- [[task-264-02-blob-shadows]] — step 2, cpp.
